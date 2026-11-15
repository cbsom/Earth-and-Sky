// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <QFontMetricsF>
#include <QVector>
#include <limits>
#include <algorithm>

// Prefer nearby labels and move crowded labels away from bodies and other names.
inline QRectF placeBodyLabel(QPointF center,double diameter,QSizeF size,QRectF bounds,const QVector<QRectF>& obstacles) {
    QRectF best;double bestScore=std::numeric_limits<double>::infinity();
    for(int level=0;level<8;++level)for(int side=0;side<4;++side) {
        const double gap=diameter/2+4+level*(size.height()+3);
        QPointF point=side==0?QPointF(center.x()-size.width()/2,center.y()+gap):
            side==1?QPointF(center.x()-size.width()/2,center.y()-gap-size.height()):
            side==2?QPointF(center.x()+gap,center.y()-size.height()/2):
                    QPointF(center.x()-gap-size.width(),center.y()-size.height()/2);
        point.setX(std::clamp(point.x(),bounds.left(),std::max(bounds.left(),bounds.right()-size.width())));
        point.setY(std::clamp(point.y(),bounds.top(),std::max(bounds.top(),bounds.bottom()-size.height())));
        QRectF candidate(point,size);double overlap=0;
        for(const auto& obstacle:obstacles){auto intersection=candidate.adjusted(-2,-2,2,2).intersected(obstacle);overlap+=intersection.width()*intersection.height();}
        if(overlap==0)return candidate;
        if(overlap<bestScore){bestScore=overlap;best=candidate;}
    }
    return best;
}
