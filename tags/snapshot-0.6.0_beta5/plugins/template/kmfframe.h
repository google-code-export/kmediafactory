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
#ifndef KMFFRAME_H
#define KMFFRAME_H

#include "kmfwidget.h"

class KMFFrame : public KMFWidget
{
    Q_OBJECT
  public:
    KMFFrame(QObject *parent = 0);
    ~KMFFrame();

    const int& lineWidth() const { return m_lineWidth; };
    void setLineWidth(const int& lineWidth) { m_lineWidth = lineWidth; };
    void fromXML(const QDomElement& element);

  protected:
    virtual void paintWidget(QImage& layer, bool shdw = false);

  private:
    int m_lineWidth;
    QColor m_fillColor;
    int m_rounded;
};

#endif
