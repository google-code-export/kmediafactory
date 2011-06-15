// **************************************************************************
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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
// **************************************************************************
#include "rect.h"

#include <KDebug>

KMF::Rect::Rect()
    : QRect()
{
}

KMF::Rect::Rect(int left, int top, int width, int height)
    : QRect(left, top, width, height)
{
}

KMF::Rect::Rect(const QPoint &topLeft, const QSize &size)
    : QRect(topLeft, size)
{
}

KMF::Rect::Rect(const QRect &rc) : QRect(rc)
{
}

KMF::Rect::~Rect()
{
}

void KMF::Rect::set(const QRect &maxRect)
{
    set(maxRect, (double)width() / (double)height());
}

void KMF::Rect::set(const QRect &maxRect, double aspectRatio)
{
    // kDebug() << maxRect;
    if (maxRect.height() * aspectRatio > maxRect.width()) {
        setWidth(maxRect.width());
        setHeight((int)(maxRect.width() / aspectRatio));
    } else   {
        setWidth((int)(maxRect.height() * aspectRatio));
        setHeight(maxRect.height());
    }

    // kDebug() << *this;
}

void KMF::Rect::align(const QRect &parentRect, HAlign halign, VAlign valign)
{
    int x = 0, y = 0;

    switch (halign) {
    case Left:
        x = parentRect.x();
        break;
    case HDefault:
    case Center:
        x = parentRect.x() + ((parentRect.width() - width()) / 2);
        break;
    case Right:
        x = parentRect.x() + (parentRect.width() - width());
        break;
    }

    switch (valign) {
    case Top:
        y = parentRect.y();
        break;
    case VDefault:
    case Middle:
        y = parentRect.y() + ((parentRect.height() - height()) / 2);
        break;
    case Bottom:
        y = parentRect.y() + (parentRect.height() - height());
        break;
    }

    translate(x, y);
    // kDebug() << *this;
}
