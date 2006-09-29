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
#include "kmfprogressitem.h"
#include <kprogressbar.h>
#include <QPainter>
#include <QPixmap>

KMFProgressItem::KMFProgressItem()
  : QListWidgetItem(), m_bar(0)
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
    m_bar = new QProgressBar();
    m_bar->resize(ProgressWidth - 2 * Margin, sizeHint().height());
  }
}

void KMFProgressItem::setTotalSteps(int totalSteps)
{
  checkBar();
  m_bar->setRange(0, totalSteps);
  listWidget()->update();
}

void KMFProgressItem::setProgress(int progress)
{
  checkBar();
  m_bar->setValue(progress);
  listWidget()->update();
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
  listWidget()->update();
}

// Taken from Autopackage statuslistitems.cpp
void KMFProgressItem::paintCell(QPainter* p, int column,
                                int width, int align)
{
  //p->fillRect(0, 0, width, sizeHint().height(), cg.base());

  if(column != 1)
  {
#warning TODO
    // QListWidgetItem::paintCell(p, cg, column, width, align);
    // Must be done with QItemDelegate since it's dynamic content???
    return;
  }

  if(m_bar && m_bar->value() >= 0 && m_bar->value() < m_bar->maximum())
  {
    QPixmap barPixmap = QPixmap::grabWidget(m_bar, 0, 0,
                                            ProgressWidth, sizeHint().height());
    p->drawPixmap(Margin, 0, barPixmap);
  }
}
