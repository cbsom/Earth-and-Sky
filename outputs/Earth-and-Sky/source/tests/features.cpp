// SPDX-License-Identifier: GPL-3.0-or-later
#include <QSettings>
#include <QTemporaryDir>
#include <cstdio>
static QString settingsFile;
class IsolatedSettings : public QSettings {
public: IsolatedSettings(const char*,const char*) : QSettings(settingsFile,QSettings::IniFormat) {}
};
#define QSettings IsolatedSettings
#define main earthSkyApplicationMain
#include "../main.cpp"
#undef main
#undef QSettings
#include <QtTest/QTest>
#include <QQmlExpression>
#include <QQuickItem>

int main(int argc,char** argv) {
    QGuiApplication app(argc,argv);
    app.setApplicationVersion(QStringLiteral(EARTH_SKY_VERSION));
    QTemporaryDir temporary;settingsFile=temporary.filePath("preferences.ini");
    int warnings=0;
    static int* warningCount=&warnings;
    qInstallMessageHandler([](QtMsgType type,const QMessageLogContext&,const QString& message){
        fprintf(stderr,"%s\n",qPrintable(message));fflush(stderr);
        if(type==QtWarningMsg || type==QtCriticalMsg)++*warningCount;
        if(type==QtFatalMsg)abort();
    });
    if(!selfTest())return 1;
    qmlRegisterType<GlobeView>("EarthSky.Globe",1,0,"GlobeView");
    qmlRegisterType<FlatSkyView>("EarthSky.Globe",1,0,"FlatSkyView");
    Sky sky;sky.refresh();
    QQmlApplicationEngine engine;engine.rootContext()->setContextProperty("sky",&sky);
    engine.addImageProvider("earth",new Images(&sky));
    engine.loadFromModule("EarthSky","Main");
    if(engine.rootObjects().isEmpty())return 2;
    auto window=qobject_cast<QQuickWindow*>(engine.rootObjects().first());
    auto check=[](bool condition,const char* label){if(!condition)qFatal("FAIL: %s",label);};
    auto eval=[&](const QString& code){QQmlExpression e(qmlContext(window),window,code);auto r=e.evaluate();check(!e.hasError(),qPrintable(e.error().toString()));return r;};
    auto find=[&](const char* name){auto item=window->findChild<QQuickItem*>(name);check(item,name);return item;};
    auto click=[&](const char* name){auto item=find(name);QTest::mouseClick(window,Qt::LeftButton,Qt::NoModifier,item->mapToScene(QPointF(item->width()/2,item->height()/2)).toPoint());QCoreApplication::processEvents();};
    auto settle=[&](){QTest::qWait(600);window->grabWindow();};
    auto snapshot=[&](const QString& name){
        const int index=app.arguments().indexOf("--snapshots");
        if(index>=0 && index+1<app.arguments().size())window->grabWindow().save(app.arguments()[index+1]+"/"+name+".png");
    };
    settle();
    check(find("timeStatusLabel")->property("text")=="LIVE","initial live status");
    click("dateTimeButton");
    find("dateInput")->setProperty("text","2026-02-30");find("timeInput")->setProperty("text","12:00:00");
    const auto held=sky.displayedUtc();click("applyDateTimeButton");
    check(find("dateTimeDialog")->isVisible() && sky.displayedUtc()==held,"invalid date keeps dialog and instant");
    check(!find("dateTimeDialog")->property("error").toString().isEmpty(),"visible validation error");
    find("dateInput")->setProperty("text","2026-03-20");click("utcTimeChoice");click("applyDateTimeButton");
    check(!find("dateTimeDialog")->isVisible() && sky.displayedUtc()==QDateTime(QDate(2026,3,20),QTime(12,0),QTimeZone::utc()),"apply UTC date through UI");
    check(find("timeStatusLabel")->property("text")=="SELECTED","selected-time status");
    eval("planetsToggle.checked = true; allConstellationsToggle.checked = true; mapViewport.setObserver(35.22,31.78); globe.centerOn(35.22,31.78)");settle();
    check(find("locationInfoPanel")->isVisible(),"location selection opens panel");
    auto info=find("locationInfoPanel")->property("info").toMap();
    check(info["bodies"].toList().size()==7 && info["twilight"].toList().size()==3,"location has seven bodies and twilight");
    check(eval("mapViewport.horizonActive").toBool(),"horizon selection shared");
    snapshot("location-panel-wide");
    click("nowButton");
    check(sky.live() && eval("mapViewport.horizonActive").toBool(),"Now preserves selected location");
    click("playForwardButton");check(find("timeStatusLabel")->property("text")=="PLAYING","playing status");
    click("dateTimeButton");check(sky.animationDirection()==0,"opening date editor pauses playback");
    snapshot("date-time-dialog");
    QTest::keyClick(window,Qt::Key_Escape);QCoreApplication::processEvents();check(!find("dateTimeDialog")->isVisible(),"Escape closes dialog");
    click("dateTimeButton");find("dateInput")->forceActiveFocus();
    QTest::keyClick(window,Qt::Key_Tab);check(find("timeInput")->hasActiveFocus(),"date input tab order");
    QTest::keyClick(window,Qt::Key_Escape);QCoreApplication::processEvents();
    click("locationInfoButton");check(!find("locationInfoPanel")->isVisible(),"panel can close without clearing location");
    click("locationInfoButton");
    eval("mapViewport.setObserver(0,89)");sky.selectDateTime("2026-06-21","12:00:00",true);settle();
    check(find("locationInfoPanel")->property("info").toMap()["daylight"]=="24 h 00 min","polar result reaches panel");
    sky.selectDateTime("2026-12-21","12:00:00",true);settle();
    check(find("locationInfoPanel")->property("info").toMap()["daylight"]=="0 h 00 min","panel invalidates daily cache");
    eval("mapViewport.setObserver(-74,40.71)");sky.selectDateTime("2026-03-20","22:00:00",true);
    for(const auto size:{QSize(1280,752),QSize(1000,650),QSize(720,472)}) {
        window->resize(size);settle();
        check(eval("timeReadout.x >= 0 && timeReadout.x + timeReadout.width <= window.width && togglesRow.x >= 0 && coordinatesLabel.x >= 0 && coordinatesLabel.x + coordinatesLabel.width <= window.width").toBool(),"footer fits window");
        check(eval("locationPanel.y + locationPanel.height <= footer.y && locationPanel.height > 100").toBool(),"scrollable panel fits map area");
        snapshot("location-panel-"+QString::number(size.width()));
    }
    click("dateTimeButton");snapshot("date-time-small");QTest::keyClick(window,Qt::Key_Escape);QCoreApplication::processEvents();
    window->resize(1280,752);click("flatMapViewButton");settle();snapshot("location-flat-map");
    check(eval("mapViewport.bodyLabels.length === 7").toBool(),"flat-map label placement");
    // At maximum zoom the overlay must remain viewport-sized, rather than
    // stretching a capped image or allocating an 8x full-world texture.
    eval("mapViewport.fitMap(); mapViewport.zoom = 8; mapViewport.centerX = 0.5; mapViewport.centerY = 0.5");settle();
    auto overlay=find("flatSkyOverlay");
    check(overlay->width()==find("mapViewport")->width() && overlay->height()==find("mapViewport")->height(),"zoomed sky uses viewport-sized paint surface");
    check(overlay->property("worldSize").toSizeF().width()>overlay->width()*7.9,"sky geometry follows maximum zoom");
    snapshot("constellations-zoom-8");
    eval("mapViewport.centerX = 0.999");settle();snapshot("constellations-zoom-seam");
    click("globeViewButton");settle();snapshot("location-globe");
    click("fullMapButton");check(!eval("mapViewport.horizonActive").toBool(),"full view clears observer");
    click("resetButton");check(sky.live(),"reset restores live time");
    click("screenshotButton");check(find("screenshotDialog")->isVisible() && !sky.live(),"export options pause displayed time");
    snapshot("screenshot-export-dialog");
    // Exercise the file-picker import and cancellation using Qt's testable
    // fallback; production keeps the native platform picker.
    eval("exportFileDialog.options = FileDialog.DontUseNativeDialog");
    click("chooseScreenshotFileButton");settle();
    check(eval("exportFileDialog.visible").toBool(),"save-file picker opens");
    eval("exportFileDialog.close(); exportFileDialog.rejected()");
    check(find("screenshotDialog")->isVisible(),"canceling file picker retains export options");

    QTest::keyClick(window,Qt::Key_Escape);QCoreApplication::processEvents();
    check(!find("screenshotDialog")->isVisible(),"cancel export with Escape");
    auto exportTo=[&](const QString& path){
        click("screenshotButton");
        check(QMetaObject::invokeMethod(window,"performExport",Q_ARG(QVariant,QVariant(QUrl::fromLocalFile(path)))),"start image export");
        QElapsedTimer wait;wait.start();
        while(window->property("exporting").toBool() && wait.elapsed()<10000){window->grabWindow();QTest::qWait(25);}
        check(!window->property("exporting").toBool(),"asynchronous capture completed");
    };
    eval("mapViewport.setObserver(35.22,31.78); globe.centerOn(35.22,31.78)");
    sky.selectDateTime("2026-03-20","12:00:00",true);settle();
    const auto png=temporary.filePath(QString::fromUtf8("scene with spaces-\xe6\x98\x9f.png"));
    exportTo(png);QImage annotated(png);
    check(!annotated.isNull() && annotated.width()==find("globeView")->width() && annotated.height()>find("globeView")->height(),"PNG contains map plus small credits footer");
    check(!annotated.text("Description").contains("Displayed time:") && !annotated.text("Description").contains("My Location:") && !annotated.text("Description").contains("Layers:") && !annotated.text("Description").contains("Earth & Sky"),"export contains only image credits");
    check(annotated.height()-find("globeView")->height()<48,"credits footer remains compact");
    check(!window->property("exportMessage").toString().isEmpty(),"export confirmation shown");
    click("dateTimeButton");check(window->property("exportMessage").toString().isEmpty() && find("dateTimeDialog")->isVisible(),"any click clears confirmation without consuming action");
    QTest::keyClick(window,Qt::Key_Escape);QCoreApplication::processEvents();
    check(annotated.text("Description").contains("CC BY-SA 3.0"),"image attribution retained");
    const int shotIndex=app.arguments().indexOf("--snapshots");
    if(shotIndex>=0 && shotIndex+1<app.arguments().size())annotated.save(app.arguments()[shotIndex+1]+"/exported-globe.png");
    click("flatMapViewButton");settle();
    const auto jpg=temporary.filePath("scene.jpg");exportTo(jpg);QImage plain(jpg);
    check(!plain.isNull() && plain.width()==find("mapViewport")->width() && plain.height()-find("mapViewport")->height()<48,"JPEG export with small credits footer");
    if(shotIndex>=0 && shotIndex+1<app.arguments().size())plain.save(app.arguments()[shotIndex+1]+"/exported-flat.jpg");
    check(!window->property("exportMessage").toString().isEmpty(),"JPEG confirmation shown");
    QTest::qWait(5100);check(window->property("exportMessage").toString().isEmpty(),"confirmation disappears after five seconds");
    const auto minimal=temporary.filePath("minimal.png");exportTo(minimal);QImage noCaptions(minimal);
    check(!noCaptions.text("Description").contains("Displayed time:") && !noCaptions.text("Description").contains("My Location:") && !noCaptions.text("Description").contains("Layers:") && noCaptions.text("Description").contains("Gregory"),"only credit metadata retained");
    const auto invalid=temporary.filePath("unsupported.gif");exportTo(invalid);
    check(find("screenshotDialog")->isVisible() && !QFileInfo::exists(invalid) && !find("screenshotDialog")->property("error").toString().isEmpty(),"unsupported format produces recoverable error");
    QTest::keyClick(window,Qt::Key_Escape);QCoreApplication::processEvents();
    exportTo(temporary.filePath("missing-folder/scene.png"));
    check(find("screenshotDialog")->isVisible() && !find("screenshotDialog")->property("error").toString().isEmpty(),"write failure stays in export dialog");
    QTest::keyClick(window,Qt::Key_Escape);QCoreApplication::processEvents();
    // Invalid output must leave an existing file untouched.
    QFile existing(invalid);check(existing.open(QIODevice::WriteOnly),"create preservation fixture");existing.write("original");existing.close();
    check(!writeScreenshot(QImage(10,10,QImage::Format_RGB32),QUrl::fromLocalFile(invalid)).isEmpty(),"reject unsupported extension before opening output");
    check(existing.open(QIODevice::ReadOnly) && existing.readAll()=="original","failed export preserves existing file");
    check(warnings==0,"no QML binding or runtime warnings");
    qInfo()<<"PASS: date entry, location panel, playback, responsive layouts, sharp zoom, PNG/JPEG export, compact credits, confirmation dismissal, file picker and error recovery";
    return 0;
}
