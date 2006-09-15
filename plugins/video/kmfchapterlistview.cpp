//**************************************************************************
//   Copyright (C) 2004, 2005 by Petri Damst�
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
#include "kmfchapterlistview.h"
#include <klocale.h>

KMFChapterListViewItem::KMFChapterListViewItem(Q3ListView *parent,
                                               Q3ListViewItem *after)
  : Q3ListViewItem(parent, after)
{
}

KMFChapterListViewItem::KMFChapterListViewItem(Q3ListView *parent,
                                               Q3ListViewItem *after,
                                               const QString& name,
                                               QTime pos)
  : Q3ListViewItem(parent, after), m_pos(pos)
{
  setText(0, name);
}

KMFChapterListViewItem::~KMFChapterListViewItem()
{
}

KMFChapterListView* KMFChapterListViewItem::listView() const
{
  return static_cast<KMFChapterListView*>(Q3ListViewItem::listView());
}

KMFChapterListViewItem* KMFChapterListViewItem::nextSibling() const
{
  return static_cast<KMFChapterListViewItem*>(Q3ListViewItem::nextSibling());
}

QTime KMFChapterListViewItem::length() const
{
  KMF::Time length;
  if(nextSibling() != 0)
    length = nextSibling()->pos();
  else
    length = listView()->duration();
  length -= m_pos;
  return length;
}

QString KMFChapterListViewItem::text(int column) const
{
  switch(column)
  {
    case 0:
      return Q3ListViewItem::text(0);
      break;
    case 1:
      return m_pos.toString();
      break;
    case 2:
    default:
      KMF::Time l = length();
      return l.toString();
      break;
  }
}

KMFChapterListView::KMFChapterListView(QWidget *parent, const char *name)
  : Q3ListView(parent, name), m_duration(0.0)
{
  addColumn(i18n("Name"), -1);
  addColumn(i18n("Start"));
  addColumn(i18n("Length"));
  setSorting(-1);
  setAllColumnsShowFocus(true);
#warning TODO
#if 0
  setItemsRenameable(true);
  setItemsMovable(false);
  setFocusPolicy(QWidget::StrongFocus);
#endif
  setDefaultRenameAction(Accept);
}

KMFChapterListView::~KMFChapterListView()
{
}

KMFChapterListViewItem* KMFChapterListView::selectedItem() const
{
  return static_cast<KMFChapterListViewItem*>(Q3ListView::selectedItem());
}

KMFChapterListViewItem* KMFChapterListView::currentItem() const
{
  return static_cast<KMFChapterListViewItem*>(Q3ListView::currentItem());
}

#include "kmfchapterlistview.moc"
