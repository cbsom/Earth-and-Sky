// SPDX-License-Identifier: GPL-3.0-or-later
// Observer calculations shared by the location panel and built-in tests.
// All daily events use the computer's civil date and time zone, not longitude.
#pragma once
#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QTimeZone>
#include <QVariant>
#include <QtMath>
#include <algorithm>
#include <array>
#include <limits>
#include <utility>
#include <vector>
#include "vendor/astronomy.h"

inline constexpr std::array<astro_body_t,7> bodies={BODY_SUN,BODY_MOON,BODY_MERCURY,BODY_VENUS,BODY_MARS,BODY_JUPITER,BODY_SATURN};
inline constexpr std::array<const char*,7> bodyNames={"Sun","Moon","Mercury","Venus","Mars","Jupiter","Saturn"};

inline astro_time_t astronomyTime(const QDateTime& instant) {
    const auto utc=instant.toUTC();const auto d=utc.date();const auto t=utc.time();
    return Astronomy_MakeTime(d.year(),d.month(),d.day(),t.hour(),t.minute(),t.second()+t.msec()/1000.0);
}
inline QDateTime eventInstant(const astro_time_t& time) {
    // Astronomy Engine UT is measured in days from 2000-01-01 12:00 UTC.
    return QDateTime::fromMSecsSinceEpoch(946728000000LL+qRound64(time.ut*86400000.0),QTimeZone::utc());
}
inline double bodyAltitude(astro_body_t body,astro_time_t time,astro_observer_t observer,bool refracted) {
    const auto eq=Astronomy_Equator(body,&time,observer,EQUATOR_OF_DATE,ABERRATION);
    if(eq.status!=ASTRO_SUCCESS)return std::numeric_limits<double>::quiet_NaN();
    return Astronomy_Horizon(&time,observer,eq.ra,eq.dec,refracted?REFRACTION_NORMAL:REFRACTION_NONE).altitude;
}
inline QVariantMap locationDay(QDate date,const QTimeZone& zone,double latitude,double longitude) {
    const auto start=date.startOfDay(zone),end=date.addDays(1).startOfDay(zone);
    QVariantMap result{{"date",date.toString("dd MMM yyyy")},{"zone",QString::fromUtf8(zone.id())}};
    if(!start.isValid() || !end.isValid())return {{"error","This civil date is not available in the computer's time zone."}};
    const auto startTime=astronomyTime(start);
    const double days=start.msecsTo(end)/86400000.0;
    const astro_observer_t observer{latitude,longitude,0};
    auto findEvent=[&](astro_body_t body,astro_direction_t direction,double altitude,bool riseSet) {
        const auto event=riseSet?Astronomy_SearchRiseSet(body,observer,direction,startTime,days):
            Astronomy_SearchAltitude(body,observer,direction,startTime,days,altitude);
        if(event.status!=ASTRO_SUCCESS) return QVariantMap{{"text",event.status==ASTRO_SEARCH_FAILURE?"None":"Unavailable"}};
        const auto instant=eventInstant(event.time);
        if(instant<start || instant>=end)return QVariantMap{{"text","None"}};
        return QVariantMap{{"text",instant.toTimeZone(zone).toString("HH:mm")},{"instant",instant}};
    };
    QVariantList rows;
    QDateTime sunrise,sunset;
    for(int i=0;i<int(bodies.size());++i) {
        const auto rise=findEvent(bodies[i],DIRECTION_RISE,0,true);
        const auto set=findEvent(bodies[i],DIRECTION_SET,0,true);
        rows.append(QVariantMap{{"name",QString::fromLatin1(bodyNames[i])},{"rise",rise["text"]},{"set",set["text"]},
            {"riseInstant",rise.value("instant")},{"setInstant",set.value("instant")}});
        if(i==0){sunrise=rise.value("instant").toDateTime();sunset=set.value("instant").toDateTime();}
    }
    result["bodies"]=rows;
    // Integrate the daylight portions of this civil day, including polar days,
    // midnight sun and days on which only one crossing occurs.
    const bool sunUp=bodyAltitude(BODY_SUN,startTime,observer,false)>-0.833;
    std::vector<std::pair<QDateTime,bool>> crossings;
    if(sunrise.isValid())crossings.push_back({sunrise,true});
    if(sunset.isValid())crossings.push_back({sunset,false});
    // A 25-hour civil day can contain a second crossing of the same kind,
    // especially when the observer is far from the computer's time zone.
    for(const auto direction:{DIRECTION_RISE,DIRECTION_SET}) {
        const auto first=direction==DIRECTION_RISE?sunrise:sunset;
        if(!first.isValid())continue;
        const auto after=first.addSecs(1);
        if(after>=end)continue;
        const auto next=Astronomy_SearchRiseSet(BODY_SUN,observer,direction,astronomyTime(after),after.msecsTo(end)/86400000.0);
        if(next.status==ASTRO_SUCCESS) {
            const auto instant=eventInstant(next.time);
            if(instant>=after && instant<end)crossings.push_back({instant,direction==DIRECTION_RISE});
        }
    }
    std::sort(crossings.begin(),crossings.end(),[](const auto& a,const auto& b){return a.first<b.first;});
    bool up=crossings.empty()?sunUp:!crossings.front().second;auto cursor=start;qint64 daylight=0;
    for(const auto& crossing:crossings){if(up)daylight+=cursor.msecsTo(crossing.first);cursor=crossing.first;up=crossing.second;}
    if(up)daylight+=cursor.msecsTo(end);
    const auto minutes=qRound64(daylight/60000.0);
    const auto sun=rows[0].toMap();
    result["daylightMilliseconds"]=daylight;
    result["daylight"]=(sun["rise"]=="Unavailable" || sun["set"]=="Unavailable")?QString("Unavailable"):
        QString("%1 h %2 min").arg(minutes/60).arg(minutes%60,2,10,QLatin1Char('0'));
    QVariantList twilight;
    for(int i=0;i<3;++i){
        const auto dawn=findEvent(BODY_SUN,DIRECTION_RISE,-6.0*(i+1),false);
        const auto dusk=findEvent(BODY_SUN,DIRECTION_SET,-6.0*(i+1),false);
        twilight.append(QVariantMap{{"name",QStringList{"Civil","Nautical","Astronomical"}[i]},
            {"dawn",dawn["text"]},{"dusk",dusk["text"]}});
    }
    result["twilight"]=twilight;
    return result;
}
