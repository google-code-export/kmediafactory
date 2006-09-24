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
#ifndef KMFPROGRESSLISTVIEW_H
#define KMFPROGRESSLISTVIEW_H

#include <Q3ListView>
#include <QPixmap>
#include <QResizeEvent>

/**
@author Petri Damsten
*/
class KMFProgressListView : public Q3ListView
{
  public:
    KMFProgressListView(QWidget *parent = 0, const char *name = 0);
    ~KMFProgressListView();

    void insertItem(const QPixmap &pixmap, const QString &text);

  public slots:
    void setTotalSteps(int totalSteps);
    void setProgress(int progress);

  protected:
    virtual void viewportResizeEvent(QResizeEvent* e);
};

#endif
