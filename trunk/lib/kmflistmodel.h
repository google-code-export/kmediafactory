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
class KMFListModel : public QAbstractListModel
{
  public:
    KMFListModel(QObject *parent = 0);
    KMFListModel(const QList<T> &values, QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool insertRows(const QModelIndex &index, int count,
                    const QModelIndex &parent = QModelIndex());
    bool removeRows(const QModelIndex &index, int count,
                    const QModelIndex &parent = QModelIndex());
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    QList<T> list() const;
    void setList(const QList<T> &values);

    void clear();
    void removeAt(const QModelIndex& index);
    void removeAt(const QModelIndexList& list);
    T at(const QModelIndex &index) const;
    T at(int index) const;
    void swap(const QModelIndex &i1, const QModelIndex &i2);
    void insert(const QModelIndex &index, const T& value);
    void append(const T& value);
    void append(const QList<T>& values);

private:
    QList<T> m_lst;
};

template <class T>
KMFListModel<T>::KMFListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

template <class T>
KMFListModel<T>::KMFListModel(const QList<T>& values, QObject* parent)
    : QAbstractListModel(parent), m_lst(values)
{
}

template <class T>
int KMFListModel<T>::rowCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;

  return m_lst.count();
}

template <class T>
QVariant KMFListModel<T>::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= m_lst.size())
    return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
    return m_lst.at(index.row());

  return QVariant();
}

template <class T>
Qt::ItemFlags KMFListModel<T>::flags(const QModelIndex& index) const
{
  if (!index.isValid())
    return QAbstractItemModel::flags(index) | Qt::ItemIsDropEnabled;

  return QAbstractItemModel::flags(index) | Qt::ItemIsEditable |
      Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

template <class T>
QList<T> KMFListModel<T>::list() const
{
  return m_lst;
}

template <class T>
bool KMFListModel<T>::setData(const QModelIndex& index, const QVariant& value,
                              int role)
{
  if (index.row() >= 0 && index.row() < m_lst.size()
      && (role == Qt::EditRole || role == Qt::DisplayRole))
  {
    m_lst.replace(index.row(), value.value<T>());
    emit dataChanged(index, index);
    return true;
  }
  return false;
}

template <class T>
bool KMFListModel<T>::insertRows(const QModelIndex& index, int count,
                                 const QModelIndex& parent)
{
  int row = index.row();
  if (count < 1 || row < 0 || row > rowCount(parent))
    return false;

  beginInsertRows(QModelIndex(), row, row + count - 1);
  for (int r = 0; r < count; ++r)
      m_lst.insert(row, QString());
  endInsertRows();
  return true;
}

template <class T>
bool KMFListModel<T>::removeRows(const QModelIndex& index, int count,
                                 const QModelIndex& parent)
{
  int row = index.row();
  if(count <= 0 || row < 0 || (row + count) > rowCount(parent))
    return false;

  beginRemoveRows(QModelIndex(), row, row + count - 1);
  for (int r = 0; r < count; ++r)
    m_lst.removeAt(row);
  endRemoveRows();
  return true;
}

template <class T>
void KMFListModel<T>::sort(int, Qt::SortOrder order)
{
  emit layoutAboutToBeChanged();
  if (order == Qt::AscendingOrder)
    qSort(m_lst.begin(), m_lst.end(), qLess<T>());
  else
    qSort(m_lst.begin(), m_lst.end(), qGreater<T>());
  emit layoutChanged();
}

template <class T>
void KMFListModel<T>::setList(const QList<T> &values)
{
  m_lst = values;
  reset();
}

template <class T>
void KMFListModel<T>::clear()
{
  m_lst = QList<T>();
  reset();
}

template <class T>
void KMFListModel<T>::removeAt(const QModelIndexList& list)
{
  for(int i; i < list.count(); ++i)
  {
    removeAt(list[i]);
  }
}

template <class T>
void KMFListModel<T>::removeAt(const QModelIndex &index)
{
  removeRows(index, 1);
}

template <class T>
T KMFListModel<T>::at(const QModelIndex &index) const
{
  return at(index.row());
}

template <class T>
T KMFListModel<T>::at(int index) const
{
  return m_lst.at(index);
}

template <class T>
void KMFListModel<T>::swap(const QModelIndex &i1, const QModelIndex &i2)
{
  T s1 = at(i1);
  T s2 = at(i2);

  setData(i1, s2);
  setData(i2, s1);
}

template <class T>
void KMFListModel<T>::insert(const QModelIndex &index, const T& value)
{
  insertRows(index, 1);
  setData(index, value);
}

template <class T>
void KMFListModel<T>::append(const T& value)
{
  QModelIndex i = index(rowCount(), 0, QModelIndex());
  insert(i, value);
}

template <class T>
void KMFListModel<T>::append(const QList<T>& values)
{
  QList<T> lst = list();
  lst << values;
  setList(lst);
}

#endif // KMFLISTMODEL_H
