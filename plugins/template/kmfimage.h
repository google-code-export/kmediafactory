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

#ifndef KMFIMAGE_H
#define KMFIMAGE_H

#include <QtGui/QImage>

#include <KSvgRenderer>
#include <KUrl>

#include "kmfwidget.h"

class KMFImage : public KMFWidget
{
    Q_OBJECT

    public:
        KMFImage(QObject *parent = 0);
        ~KMFImage();

        const QImage&image()
        {
            return m_image;
        };
        void setImage(KUrl url);
        virtual void fromXML(const QDomElement &element);
        virtual int minimumPaintWidth() const;
        virtual int minimumPaintHeight() const;
        virtual void setProperty(const QString &name, QVariant value);
        virtual QRect paintRect(const QPoint offset = QPoint(0, 0)) const;
        virtual int paintWidth() const;
        virtual int paintHeight() const;
        virtual int paintX() const;
        virtual int paintY() const;

    protected:
        virtual void paintWidget(QImage *layer, bool shdw = false) const;
        QImage mask(const QImage &img, const QRgb &maskColor, bool oneBitMask) const;
        void setImage(const QImage &image);
        void setImage(const QByteArray &ba); // svg
        QSizeF svgSize() const;

    private:
        QImage m_image;
        mutable KSvgRenderer m_svg;
        KUrl m_url;
        bool m_scale;
        bool m_proportional;
        float m_aspectRatio;
        static QImage m_empty;
        QString m_element;
};

#endif
