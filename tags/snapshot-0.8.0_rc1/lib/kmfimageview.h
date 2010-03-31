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

#ifndef KMFIMAGEVIEW_H
#define KMFIMAGEVIEW_H

#include <QtGui/QGraphicsPixmapItem>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QImage>

#include <kdemacros.h>

#include "rect.h"

class KDE_EXPORT KMFImageView : public QGraphicsView
{
    Q_OBJECT

    public:
        KMFImageView(QWidget * parent = 0);
        virtual ~KMFImageView();

        void setImage(const QImage &image);
        const QImage&image()
        {
            return m_image;
        };
        void clear();
        bool scaled()
        {
            return m_scaled;
        };
        void setScaled(bool scaled);

    protected:
        void newImage();
        void scale();
        virtual void contextMenuEvent(QContextMenuEvent *e);
        virtual void resizeEvent(QResizeEvent *e);

    signals:
        void contextMenuRequested(const QPoint &pos);

    private:
        QImage m_image;
        QGraphicsPixmapItem *m_imageItem;
        QGraphicsScene m_scene;
        bool m_scaled;
};

#endif
