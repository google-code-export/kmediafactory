/***************************************************************************
                          positionslider.cpp
                             -------------------
    begin                : Wed Feb 4 2004
    revision             : $Revision: 1.4 $
    last modified        : $Date: 2005/02/24 17:09:36 $ by $Author: gillata $
    copyright            : (C) 2004-2005 by Giorgos Gousios, Jürgen Kofler, Miguel Freitas
    email                : kaffeine@gmx.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "positionslider.h"
#include <qapplication.h>
//Added by qt3to4:
#include <QWheelEvent>
#include <QEvent>
#include <QMouseEvent>
#include <math.h>

PositionSlider::PositionSlider(QWidget *parent, const char* name) :
           QSlider(Qt::Horizontal, parent, name), m_userChange(false)
{
    connect(this, SIGNAL(sliderPressed()), this ,SLOT(slotSliderPressed()));
    connect(this, SIGNAL(sliderReleased()), this, SLOT(slotSliderReleased()));
    installEventFilter(this);
}

PositionSlider::~PositionSlider() {}

void PositionSlider::setPosition(int val, bool changePosition)
{
   if(!m_userChange)
     setValue(val);
   if(changePosition)
   {
     setValue(val);
     emit sliderMoved(val);
   }
}

void PositionSlider::slotSliderPressed()
{
   m_userChange = true;
   emit signalStartSeeking();
}

void PositionSlider::slotSliderReleased()
{
   emit signalStopSeeking();
   emit sliderMoved(this->value());
   m_userChange = false;
}

void PositionSlider::wheelEvent(QWheelEvent* e)
{
  float offset = log10( QABS(e->delta()) ) / 0.002;
  int newVal = 0;
  if (e->delta()>0)
     newVal = value() + int(offset);
  else
     newVal = value() - int(offset);
  if (newVal < 0) newVal = 0;
  setPosition(newVal, true);
  e->accept();
}

bool PositionSlider::eventFilter(QObject *obj, QEvent *ev)
{
  if( obj == this && (ev->type() == QEvent::MouseButtonPress ||
                      ev->type() == QEvent::MouseButtonDblClick) )
  {
     QMouseEvent *e = (QMouseEvent *)ev;
     QRect r = sliderRect();

     if( r.contains( e->pos() ) || e->button() != LeftButton )
        return FALSE;

     int range = maxValue() - minValue();
     int pos = (orientation() == Horizontal) ? e->pos().x() : e->pos().y();
     int maxpos = (orientation() == Horizontal) ? width() : height();
     int value = pos * range / maxpos + minValue();

     if (QApplication::reverseLayout())
       value = maxValue() - (value - minValue());

     setPosition(value, true);
     return TRUE;
  }
  else
  {
     return FALSE;
  }
}

#include "positionslider.moc"
