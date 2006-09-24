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
#ifndef KMFTEMPLATEICONVIEW_H
#define KMFTEMPLATEICONVIEW_H

#include <kmediafactory/projectinterface.h>
#include <klocale.h>
#include <kdebug.h>
#include <QAbstractListModel>

template <class T>
class KMFObjectListModel : public QAbstractListModel
{
  public:
    KMFObjectListModel() : m_data(0) {};
    virtual ~KMFObjectListModel() {};

    void setData(const QList<T*>* data)
    {
      m_data = data;
      kDebug() << k_funcinfo << rowCount(QModelIndex()) << endl;
    }

    int rowCount(const QModelIndex&) const
    {
      if(m_data)
        return m_data->count();
      return 0;
    }

    QVariant data(const QModelIndex &index, int role) const
    {
      if (!m_data || !index.isValid() || index.row() >= rowCount(index))
        return QVariant();

      if (role == Qt::DisplayRole)
        return m_data->at(index.row())->title();
      else if (role == Qt::DecorationRole)
        return m_data->at(index.row())->pixmap();
      return QVariant();
    }

    QVariant headerData(int, Qt::Orientation, int role) const
    {
      if (role != Qt::DisplayRole)
        return QVariant();

      return i18n("Title");
    }

    void changed()
    {
      emit dataChanged(index(0, 0), index(m_data.count(), 0));
    }

  private:
    const QList<T*>* m_data;
};

#endif // KMFTEMPLATEICONVIEW_H
