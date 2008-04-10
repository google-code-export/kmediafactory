//**************************************************************************
//   Copyright (C) 2004 by Petri Damst�n
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
#include "kmfprogressitem.h"
#include <kprogress.h>
#include <qpainter.h>

KMFProgressItem::KMFProgressItem(QListView *parent, QListViewItem *after)
  : KListViewItem(parent, after), m_bar(0)
{
}

KMFProgressItem::~KMFProgressItem()
{
  delete m_bar;
}

void KMFProgressItem::checkBar()
{
  if(!m_bar)
  {
    m_bar = new KProgress();
    m_bar->resize(ProgressWidth - 2 * Margin, height());
  }
}

void KMFProgressItem::setTotalSteps(int totalSteps)
{
  checkBar();
  m_bar->setTotalSteps(totalSteps);
  repaint();
}

void KMFProgressItem::setProgress(int progress)
{
  checkBar();
  m_bar->setProgress(progress);
  repaint();
}

void KMFProgressItem::showProgressBar(bool showProgressBar)
{
  if(showProgressBar == false && m_bar == 0)
    return;
  if(showProgressBar)
    checkBar();
  else
  {
    delete m_bar;
    m_bar = 0;
  }
  repaint();
}

// Taken from Autopackage statuslistitems.cpp
void KMFProgressItem::paintCell(QPainter* p, const QColorGroup& cg, int column,
                                int width, int align)
{
  p->fillRect(0, 0, width, height(), cg.base());
        
  if(column != 1)
  {
    KListViewItem::paintCell(p, cg, column, width, align);
    return;
  }
        
  if(m_bar && m_bar->progress() >= 0 && m_bar->progress() < m_bar->totalSteps())
  {
    QPixmap barPixmap = QPixmap::grabWidget(m_bar, 0, 0,
                                            ProgressWidth, height());
    p->drawPixmap(Margin, 0, barPixmap);
  }
}