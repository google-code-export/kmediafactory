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
#include <kdebug.h>
#include <kicon.h>
#include <QPainter>
#include <QModelIndex>

int KMFProgressItemModel::rowCount(const QModelIndex&) const
{
  return m_data.count();
}

QVariant KMFProgressItemModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (index.row() >= rowCount(index))
    return QVariant();

  if (role == Qt::DisplayRole)
    return m_data.at(index.row()).text;
  if (role == Qt::DecorationRole)
    return KIcon(m_data.at(index.row()).pixmap);
  return QVariant();
}

#define BARW 100

void KMFProgressItemDelegate::paint(QPainter* painter,
                                    const QStyleOptionViewItem& option,
                                    const QModelIndex& index) const
{
  painter->save();
  QItemDelegate::paint(painter, option, index);
  QRect rc = option.rect;
  const KMFProgressItem& item = m_model->data()->at(index.row());

  // If value < max
  // Paint gradient
  // Paint progress bar
  //kDebug() << k_funcinfo << rc << endl;
  QProgressBar bar;
  bar.setRange(0, item.max);
  bar.setValue(item.value);
  bar.resize(BARW, rc.height());
  QPixmap barPixmap = QPixmap::grabWidget(&bar, QRect(0, 0, BARW, rc.height()));
  painter->drawPixmap(rc.width() - BARW, rc.y(), barPixmap);
  painter->restore();
}

#include "kmfprogresslistview.moc"
