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

#ifndef KMFLISTMODEL_H
#define KMFLISTMODEL_H

#include <kdemacros.h>
#include <QList>
#include <QAbstractItemView>

// Based on QStringListModel from qt4 GPL-2

template <class T>
class KDE_EXPORT KMFListModel : public QAbstractListModel
{
    Q_OBJECT
  public:
    KMFListModel(QObject *parent = 0);
    KMFListModel(const QList<T> &values, QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    T data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const T &value,
                 int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool insertRows(const QModelIndex &index, int count,
                    const QModelIndex &parent = QModelIndex());
    bool removeRows(const QModelIndex &index, int count,
                    const QModelIndex &parent = QModelIndex());
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    QList<T> list() const;
    void setList(const QList<T> &values);

private:
    Q_DISABLE_COPY(KMFListModel)
    QList<T> m_lst;
};

#endif // KMFLISTMODEL_H
