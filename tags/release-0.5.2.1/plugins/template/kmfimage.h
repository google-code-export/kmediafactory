//**************************************************************************
//   Copyright (C) 2004 by Petri Damstén
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
#ifndef KMFIMAGE_H
#define KMFIMAGE_H

#include "kmfwidget.h"
#include <kurl.h>
#include <qimage.h>

/**
@author Petri Damsten
*/
class KMFImage : public KMFWidget
{
    Q_OBJECT
  public:
    KMFImage(QObject *parent = 0, const char *name = 0);
    ~KMFImage();

    const QImage& image() { return m_image; };
    void setImage(const QImage& image);
    void setImage(KURL url);
    virtual void fromXML(const QDomElement& element);
    virtual int minimumPaintWidth() const;
    virtual int minimumPaintHeight() const;
    virtual void setProperty(const QString& name, QVariant value);
    virtual QRect paintRect(const QPoint offset = QPoint(0,0)) const;
    virtual int paintWidth() const;
    virtual int paintHeight() const;
    virtual int paintX() const;
    virtual int paintY() const;

  protected:
    virtual void paintWidget(Magick::Image& layer, bool shdw = false);

  private:
    QImage m_image;
    bool m_scale;
    bool m_proportional;
    float m_aspectRatio;
    static QImage m_empty;
};

#endif
