/***************************************************************************
                          k3bdvdfilldisplay.cpp  -  description
                             -------------------
    begin                : Sun Mar 24 2002
    copyright            : (C) 2002 by Sebastian Trueg
    email                : trueg@informatik.uni-freiburg.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "k3bdvdfilldisplay.h"

#include <qpainter.h>
#include <qrect.h>
#include <qstring.h>

#include <klocale.h>

K3bDvdFillDisplay::K3bDvdFillDisplay(QWidget *parent, const char *name ) : QFrame(parent,name) {
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Preferred ) );
    setFrameStyle( Panel | Sunken );
    m_size = 0;
    m_available = 0;
    m_used = 0;
}

K3bDvdFillDisplay::~K3bDvdFillDisplay(){
}

void K3bDvdFillDisplay::setupGui(){

}

void K3bDvdFillDisplay::drawContents( QPainter* p ){
    QColor color( Qt::green );
    QRect rect( contentsRect() );
    int maxWidth = rect.width();
    unsigned long used = m_used + m_dvd;
    float full = (float) m_used / (float) m_size;
    m_dvdFull = (float) ( m_used+m_dvd ) / (float) m_size;
    if( m_dvdFull > 1.0 ) m_dvdFull = 1.0;
    m_aviFull = (float) ( m_used+m_dvd + 700000 ) / m_size;
    if( m_aviFull > 1.0 ) m_aviFull = 1.0;
    if( m_dvdFull > 0.95 ){
        rect.setWidth( maxWidth *.95 );
        QRect rectY( (int) maxWidth *.95, 0, (int)maxWidth *(m_dvdFull-0.95), rect.height() );
        p->fillRect( rectY, Qt::red );
    } else {
        rect.setWidth ((int)maxWidth * full );
        //qDebug("full %f", full );
        // dvd size
        QRect rectB( (int) maxWidth *full, 0, (int)maxWidth * (m_dvdFull - full) , rect.height() );
        p->fillRect( rectB, Qt::blue );
        // avi Cds size
        //qDebug("full %f", m_aviFull );
        QRect rectO( (int)maxWidth *m_dvdFull, 0, (int)maxWidth * ( m_aviFull-m_dvdFull), rect.height() );
        p->fillRect( rectO, Qt::yellow );
    }
    p->fillRect( rect, color );
}

QString K3bDvdFillDisplay::freeWithDvdAvi(){
    return QString().sprintf( "%.2f %% ",  (m_aviFull*100) ) + i18n("(full)");
}
QString K3bDvdFillDisplay::freeWithDvd(){
    return QString().sprintf( "%.2f %% ",  (m_dvdFull*100) )+ i18n("(full)");
}