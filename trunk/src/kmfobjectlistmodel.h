//**************************************************************************
//   Copyright (C) 2004-2008 by Petri Damsten
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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//**************************************************************************

#ifndef KMFTEMPLATEICONVIEW_H
#define KMFTEMPLATEICONVIEW_H

#include <kmediafactory/object.h>
#include "kmflistmodel.h"
#include <KLocale>
#include <QMimeData>

template <class T>
class KMFObjectListModel : public KMFListModel<T>
{
  public:
    KMFObjectListModel() : m_itemsDragable(false) { }

    QVariant data(const QModelIndex &index, int role) const
    {
      if(!KMFListModel<T>::isValid(index) || !KMFListModel<T>::at(index))
        return QVariant();

      if (role == Qt::DisplayRole)
        return KMFListModel<T>::at(index)->title();
      else if (role == Qt::DecorationRole)
        return KMFListModel<T>::at(index)->pixmap();
      else if (role == Qt::UserRole)
        return KMFListModel<T>::at(index)->information();
      return QVariant();
    }

    QVariant headerData(int, Qt::Orientation, int role) const
    {
      if (role != Qt::DisplayRole)
        return QVariant();

      return ::i18n("Title");
    }

    virtual Qt::ItemFlags flags(const QModelIndex &index) const
    {
        Qt::ItemFlags result = QAbstractItemModel::flags(index);
        return m_itemsDragable ? result|Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled : result;
    }

    virtual Qt::DropActions supportedDropActions() const
    {
      Qt::DropActions result = QAbstractItemModel::supportedDropActions();
      return m_itemsDragable ? result|Qt::MoveAction : result;
    }

    QStringList mimeTypes() const
    {
        QStringList types;
        types << QLatin1String("application/x-kmf-indexlist");
        return types;
    }

    QMimeData * mimeData(const QModelIndexList &indexes) const
    {
        if (!m_itemsDragable || indexes.count() != 1 || KMFListModel<T>::rowCount()<2)
            return 0;
        QMimeData *data = new QMimeData();
        QByteArray encoded;
        QDataStream stream(&encoded, QIODevice::WriteOnly);
        stream << indexes.at(0).row();
        data->setData(mimeTypes().at(0), encoded);
        return data;
    }

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
    {
        if(!data || !data->hasFormat(mimeTypes().at(0)) || -1!=row || -1!=column)
            return false;

        if (Qt::IgnoreAction==action)
            return true;
        
        if(parent.isValid())
        {
            QByteArray encoded = data->data(mimeTypes().at(0));
            QDataStream stream(&encoded, QIODevice::ReadOnly);
            int row;
            stream >> row;
            QModelIndex to=KMFListModel<T>::index(row, 0);

            if(to.isValid())
            {
                KMFListModel<T>::swap(parent, to);
                emit KMFListModel<T>::layoutChanged();
            }
        }
        return false;
    }

    void setDragable() { m_itemsDragable=true; }

    private:

    bool m_itemsDragable;
};

#endif // KMFTEMPLATEICONVIEW_H
