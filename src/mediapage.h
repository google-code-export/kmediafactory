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
#ifndef MEDIAPAGE_H
#define MEDIAPAGE_H

#include "ui_mediapage.h"
#include <kpagewidgetmodel.h>

class Q3IconViewItem;
class QPoint;

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
  public slots:
    void contextMenuRequested(Q3IconViewItem* item, const QPoint& pos);
    void calculateSizes();
};

#endif
