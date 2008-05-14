//**************************************************************************
//   Copyright (C) 2004, 2005 by Petri Damst�n
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

KMFToolButton::KMFToolButton(QWidget* parent, const char* name)
    : QToolButton(parent, name)
{
}


KMFToolButton::~KMFToolButton()
{
}

// from KToolBarButton
void KMFToolButton::drawButton(QPainter* _painter)
{
  QToolButton::drawButton(_painter);

  if (QToolButton::popup())
  {
    QStyle::SFlags arrowFlags = QStyle::Style_Default;

    if (isDown())
      arrowFlags |= QStyle::Style_Down;
    if (isEnabled())
      arrowFlags |= QStyle::Style_Enabled;

    style().drawPrimitive(QStyle::PE_ArrowDown, _painter,
        QRect(width()-10, height()-10, 7, 7), colorGroup(),
        arrowFlags, QStyleOption() );
  }
}

#include "kmftoolbutton.moc"