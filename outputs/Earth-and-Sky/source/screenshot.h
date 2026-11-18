// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <QColor>
#include <QFileInfo>
#include <QFont>
#include <QFontMetrics>
#include <QImage>
#include <QImageWriter>
#include <QIODevice>
#include <QPainter>
#include <QPoint>
#include <QRect>
#include <QSaveFile>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QVector>
#include <QtMath>
#include <algorithm>

// Write atomically: an encoding/write error must not truncate an existing image.
inline QString writeScreenshot(QImage image,const QUrl& target,qreal scale=1) {
    if(image.isNull())return "The view could not be captured. Try again with the window visible.";
    if(!target.isLocalFile())return "Choose a local file for the exported image.";
    const auto path=target.toLocalFile();const auto suffix=QFileInfo(path).suffix().toLower();
    if(suffix!="png" && suffix!="jpg" && suffix!="jpeg")return "Use a .png, .jpg or .jpeg filename.";
    scale=std::clamp(scale,1.0,4.0);image.setDevicePixelRatio(1);
    const int margin=qRound(6*scale),gap=0;
    QFont font("Segoe UI");font.setPixelSize(qRound(8*scale));
    const QFontMetrics metrics(font);const int textWidth=std::max(1,image.width()-2*margin);
    const QStringList lines{QString::fromUtf8("Earth: NASA Blue Marble · Moon: Gregory H. Revera (cropped, resized, phase-shaded), CC BY-SA 3.0 · https://creativecommons.org/licenses/by-sa/3.0/")};
    const int flags=Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignTop;
    QVector<int> heights;int footer=margin*2;
    for(const auto& line:lines){const int h=metrics.boundingRect(QRect(0,0,textWidth,100000),flags,line).height();heights.append(h);footer+=h+gap;}
    footer-=gap;
    QImage output(image.width(),image.height()+footer,QImage::Format_RGB32);output.fill(QColor(8,15,24));
    {
        QPainter painter(&output);painter.drawImage(QPoint(0,0),image);painter.setFont(font);painter.setPen(QColor(105,115,125));
        int y=image.height()+margin;
        for(int i=0;i<lines.size();++i){painter.drawText(QRect(margin,y,textWidth,heights[i]),flags,lines[i]);y+=heights[i]+gap;}
    }
    QSaveFile file(path);
    if(!file.open(QIODevice::WriteOnly))return "Could not open the destination: " + file.errorString();
    QImageWriter writer(&file,suffix=="png"?"png":"jpeg");writer.setQuality(95);
    writer.setText("Description",lines.join("\n"));writer.setText("Software","Earth & Sky");
    if(!writer.write(output)){file.cancelWriting();return "Could not encode the image: " + writer.errorString();}
    if(!file.commit())return "Could not save the image: " + file.errorString();
    return {};
}
