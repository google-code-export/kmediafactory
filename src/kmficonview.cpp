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
#include "kmficonview.h"
#include <kdebug.h>
#include <qpainter.h>

#define MARGIN 3

void KMFIconViewItem::paintItem(QPainter* p, const QColorGroup& cg)
{
  if(iconView()->currentItem() == this)
  {
    QRect rc = rect();

    p->setBrush(QBrush(cg.highlight()));
    p->setPen(Qt::NoPen);
    p->drawRoundRect(rc, 20, 20);
  }
  KIconViewItem::paintItem(p, cg);
}

void KMFIconViewItem::paintFocus(QPainter*, const QColorGroup&)
{
  // No focus rectangle
}

void KMFIconViewItem::calcRect(const QString& text_)
{
  KIconViewItem::calcRect(text_);
  QRect itemIconRect = pixmapRect();
  QRect itemTextRect = textRect();
  QRect itemRect = rect();

  itemRect.addCoords(0, 0, 2*MARGIN, 2*MARGIN);
  itemIconRect.moveBy(MARGIN, MARGIN);
  itemTextRect.moveBy(MARGIN, MARGIN);

  setPixmapRect(itemIconRect);
  setTextRect(itemTextRect);
  setItemRect(itemRect);
}

KMFIconView::KMFIconView(QWidget *parent, const char *name) :
    KIconView(parent, name), m_after(0)
{
  setSelectionMode(QIconView::NoSelection);
}

KMFIconView::~KMFIconView()
{
}

void KMFIconView::init(const QString&)
{
  clear();
}

KMFIconViewItem* KMFIconView::newItem(KMF::Object *ob)
{
  KMFIconViewItem* item;
  if(m_after)
    item = new KMFIconViewItem(this, m_after, ob->title(), ob->pixmap());
  else
    item = new KMFIconViewItem(this, ob->title(), ob->pixmap());
  m_obs.insert(ob, item);
  item->setOb(ob);
  if(count() == 1)
    setCurrentItem(ob);
  return item;
}

void KMFIconView::itemRemoved(KMF::Object *ob)
{
  const KIconViewItem *item = m_obs.find(ob).data();
  if(item == m_after)
    m_after = 0;
  delete item;
  m_obs.remove(ob);
}

void KMFIconView::setCurrentItem(KMF::Object* ob)
{
  if(ob)
  {
    KIconViewItem *item = m_obs[ob];
    if(item)
      KIconView::setCurrentItem(item);
  }
}

void KMFIconView::setSelected(KMF::Object* ob, bool s, bool cb)
{
  if(ob)
  {
    KIconViewItem *item = m_obs.find(ob).data();
    if(item)
      KIconView::setSelected(item, s, cb);
  }
}

void KMFIconView::clear()
{
  m_after = 0;
  KIconView::clear();
  m_obs.clear();
}

#include "kmficonview.moc"
