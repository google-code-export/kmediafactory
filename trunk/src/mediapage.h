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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//**************************************************************************
#ifndef MEDIAPAGE_H
#define MEDIAPAGE_H

#include "ui_mediapage.h"

class QPoint;
namespace KMF { class MediaObject; }

/**
 * @short Page for media selection
 * @author Petri Damsten <petri.damsten@iki.fi>
 */

class MediaPage : public QWidget, public Ui::MediaPage
{
    Q_OBJECT
  public:
    MediaPage(QWidget* parent = 0);
    virtual ~MediaPage();

  Q_SIGNALS:
    void details(const QString &details);
    void sizes(quint64 max, quint64 size);

  public Q_SLOTS:
    void contextMenuRequested(const QPoint& pos);
    void calculateSizes();
    void projectInit();
    void mediaModified();
};

#endif
