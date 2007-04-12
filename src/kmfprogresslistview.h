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

#include <kmflistmodel.h>
#include <QItemDelegate>

class KMFProgressItem
{
  public:
    KMFProgressItem() : max(0), value(0) {};
    QString pixmap;
    QString text;
    int max;
    int value;

    bool operator <(const KMFProgressItem &t) const { return (text < t.text); }
};

Q_DECLARE_METATYPE(KMFProgressItem);

class KMFProgressItemModel : public KMFListModel<KMFProgressItem>
{
  virtual QVariant data(const QModelIndex &index, int role) const;
};

class KMFProgressItemDelegate : public QItemDelegate
{
    Q_OBJECT
  public:
    KMFProgressItemDelegate(KMFProgressItemModel* model, QObject* parent = 0) :
        QItemDelegate(parent), m_model(model) {};
    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option,
                       const QModelIndex& index) const;
  private:
    KMFProgressItemModel* m_model;
};

#endif
