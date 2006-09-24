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
#include "kmfprogresslistview.h"
#include "kmfprogressitem.h"
#include <q3header.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QPixmap>
#include <kdebug.h>

KMFProgressListView::KMFProgressListView(QWidget *parent, const char *name)
  : Q3ListView(parent, name)
{
  addColumn("", 10);
  addColumn("", KMFProgressItem::ProgressWidth);
  header()->hide();
  setSorting(-1);
  setFocusPolicy(Qt::NoFocus);
}

KMFProgressListView::~KMFProgressListView()
{
}

void KMFProgressListView::viewportResizeEvent(QResizeEvent* e)
{
  setColumnWidth(0, e->size().width() - KMFProgressItem::ProgressWidth);
}

void KMFProgressListView::insertItem(const QPixmap &pixmap,
                                     const QString &text)
{
  KMFProgressItem* li = static_cast<KMFProgressItem*>(lastItem());
  if(li)
  {
    li->showProgressBar(false);
    updateContents();
  }
  li = new KMFProgressItem(this, li);
  li->setPixmap(0, pixmap);
  li->setText(0, text);
  ensureItemVisible(li);
}

void KMFProgressListView::setTotalSteps(int totalSteps)
{
  //kdDebug() << k_funcinfo << totalSteps << endl;
  static_cast<KMFProgressItem*>(lastItem())->setTotalSteps(totalSteps);
  repaintItem(lastItem());
}

void KMFProgressListView::setProgress(int progress)
{
  //kdDebug() << k_funcinfo << progress << endl;
  static_cast<KMFProgressItem*>(lastItem())->setProgress(progress);
  repaintItem(lastItem());
}
