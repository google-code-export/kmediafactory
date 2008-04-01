/***************************************************************************
                          positionslider.h
                             -------------------
    begin                : Wed Feb 4 2004
    revision             : $Revision: 1.2 $
    last modified        : $Date: 2005/02/02 10:08:27 $ by $Author: juergenk $
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

#ifndef POSITIONSLIDER_H
#define POSITIONSLIDER_H

#include <qslider.h>

class PositionSlider : public QSlider
{
  Q_OBJECT

 public:
  PositionSlider(QWidget * parent = 0, const char* name = 0);
  virtual ~PositionSlider();

  void setPosition(int, bool);

signals:
  void signalStartSeeking();
  void signalStopSeeking();

 protected:
   virtual void wheelEvent(QWheelEvent* e);
   bool eventFilter(QObject *obj, QEvent *ev);

 private slots:
   void slotSliderPressed();
   void slotSliderReleased();

 private:
   bool m_userChange;
};


#endif

