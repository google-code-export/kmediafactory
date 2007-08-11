//**************************************************************************
//   Copyright (C) 2004-2006 by Petri Damsten
//   petri.damsten@iki.fi
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the
//   Free Software Foundation, Inc.,
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************
#include "kmftoolbutton.h"
#include "kglobalsettings.h"
#include <qpainter.h>
#include <qstyle.h>
#include <QStyleOption>

KMFToolButton::KMFToolButton(QWidget* parent)
    : QToolButton(parent)
{
}


KMFToolButton::~KMFToolButton()
{
}

// from KToolBarButton
void KMFToolButton::paintEvent(QPaintEvent* event)
{
  QToolButton::paintEvent(event);

  if(QToolButton::menu())
  {
    QPainter painter(this);
    QStyleOption option;

    option.initFrom(this);
    option.rect = QRect(width()-10, height()-10, 7, 7);
    option.state |= QStyle::State_None;
    if (isDown())
      option.state |= QStyle::State_DownArrow;
    if (isEnabled())
      option.state |= QStyle::State_Enabled;

    style()->drawPrimitive(QStyle::PE_IndicatorArrowDown,
        &option, &painter, this);
  }
}


#include "kmftoolbutton.moc"
