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
#include "kmfimageview.h"
#include <kdebug.h>
#include <qpainter.h>
#include <qscrollbar.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qtimer.h>

KMFImageView::KMFImageView(QWidget *parent, const char *name)
  : QScrollView(parent, name, WStaticContents | WNoAutoErase), m_label(0),
    m_box(0), m_scaled(false)
{
}

KMFImageView::~KMFImageView()
{
  delete m_label;
  delete m_box;
}

void KMFImageView::setImage(const QImage& image)
{
  m_image = image;
  newImage();
}

void KMFImageView::clear()
{
  m_image = QPixmap();
  newImage();
}

void KMFImageView::resizeEvent(QResizeEvent* re)
{
  QScrollView::resizeEvent(re);

  if(m_label && m_box)
  {
    updateImage();
  }
}

void KMFImageView::updateImage()
{
  if(m_scaled)
  {
    m_box->resize(viewport()->width(), viewport()->height());
    m_label->resize(viewport()->width(), viewport()->height());
  }
  else
  {
    int x = 0, y = 0;

    if(viewport()->width() > m_box->width())
      x = (viewport()->width() - m_box->width()) / 2;
    if(viewport()->height() > m_box->height())
      y = (viewport()->height() - m_box->height()) / 2;
    moveChild(m_box, x, y);
  }
}

void KMFImageView::newImage()
{
  if(!m_box)
  {
    m_box = new QVBox(viewport());
    addChild(m_box);
  }
  if(!m_label)
    m_label = new QLabel("Image", m_box);

  m_label->setPixmap(m_image);

  if(m_scaled)
  {
    m_label->setScaledContents(true);
    setHScrollBarMode(AlwaysOff);
    setVScrollBarMode(AlwaysOff);
  }
  else
  {
    setHScrollBarMode(Auto);
    setVScrollBarMode(Auto);
  }
  // Somehow this works better
  QTimer::singleShot(0, this, SLOT(updateImage()));
  updateContents();
}

// From QTable
void KMFImageView::contentsContextMenuEvent(QContextMenuEvent* e)
{
  if(!receivers(SIGNAL(contextMenuRequested(const QPoint &))))
  {
    e->ignore();
    return;
  }
  if (e->reason() == QContextMenuEvent::Keyboard)
  {
    emit contextMenuRequested(viewport()->mapToGlobal(
        contentsToViewport(rect().center())));
  }
  else
  {
    emit contextMenuRequested(e->globalPos());
  }
}

#include "kmfimageview.moc"
