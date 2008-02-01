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
#include <kdebug.h>
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
    virtual bool setData(const QModelIndex &index, const QVariant &value,
                         int role = Qt::EditRole);
    virtual bool insertRows(int row, int count,
                            const QModelIndex &parent = QModelIndex());
    virtual bool removeRows(int row, int count,
                            const QModelIndex &parent = QModelIndex());
    bool insertRows(const QModelIndex &index, int count,
                    const QModelIndex &parent = QModelIndex());
    bool removeRows(const QModelIndex &index, int count,
                    const QModelIndex &parent = QModelIndex());
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

    void removeAt(int index);
    void removeAt(QList<int> list);
    T at(int index) const;
    void replace(int index, const T& value);
    void swap(int i1, int i2);
    void insert(int index, const T& value);
    void insert(int index, const QList<T>& values);
    bool isValid(int index) const;

    void removeAt(const QModelIndex& index);
    void removeAt(const QModelIndexList& list);
    T at(const QModelIndex &index) const;
    void replace(const QModelIndex &index, const T& value);
    void swap(const QModelIndex &i1, const QModelIndex &i2);
    void insert(const QModelIndex &index, const T& value);
    void insert(const QModelIndex &index, const QList<T>& values);
    bool isValid(const QModelIndex &index) const;
    QModelIndex indexOf(const T& value, int from = 0) const;
    QModelIndex lastIndex() const { return index(count() - 1); };

    void clear();
    void append(const T& value);
    void append(const QList<T>& values);
    int  removeAll(const T& value);
    QList<T> list() const;
    void setList(const QList<T> &values);
    int  count() const;

  protected:
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
    return qVariantFromValue(m_lst.at(index.row()));

  return QVariant();
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
  return insertRows(index.row(), count, parent);
}

template <class T>
bool KMFListModel<T>::insertRows(int row, int count,
                                 const QModelIndex& parent)
{
  if (count < 1 || row < 0 || row > rowCount(parent))
    return false;

  beginInsertRows(QModelIndex(), row, row + count - 1);
  for (int r = 0; r < count; ++r)
      m_lst.insert(row, T());
  endInsertRows();
  return true;
}

template <class T>
bool KMFListModel<T>::removeRows(const QModelIndex& index, int count,
                                 const QModelIndex& parent)
{
  return removeRows(index.row(), count, parent);
}

template <class T>
bool KMFListModel<T>::removeRows(int row, int count,
                                 const QModelIndex& parent)
{
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
bool KMFListModel<T>::isValid(const QModelIndex &index) const
{
  return isValid(index.row());
}

template <class T>
QModelIndex KMFListModel<T>::indexOf(const T& value, int from) const
{
  int i = m_lst.indexOf(value, from);

  if(i < 0)
    return QModelIndex();
  else
    return index(i);
}

// Index base functions

template <class T>
bool KMFListModel<T>::isValid(int index) const
{
  if(index >= 0 && index < m_lst.count())
    return true;
  return false;
}

template <class T>
void KMFListModel<T>::removeAt(int index)
{
  if(!isValid(index))
    return;
  beginRemoveRows(QModelIndex(), index, index);
  m_lst.removeAt(index);
  endRemoveRows();
}

template <class T>
void KMFListModel<T>::removeAt(QList<int> list)
{
  // Remove from end to start
  qSort(list.begin(), list.end(), qGreater<int>());
  foreach(int index, list)
    removeAt(index);
}

template <class T>
T KMFListModel<T>::at(int index) const
{
  if(!isValid(index))
    return T();
  return m_lst.at(index);
}

template <class T>
void KMFListModel<T>::replace(int i, const T& value)
{
  if(!isValid(i))
    return;
  m_lst[i] = value;
  emit dataChanged(index(i), index(i));
}

template <class T>
void KMFListModel<T>::swap(int i1, int i2)
{
  if(!isValid(i1) or !isValid(i1))
    return;

  T s1 = at(i1);
  replace(i1, at(i2));
  replace(i2, s1);
}

template <class T>
void KMFListModel<T>::insert(int index, const T& value)
{
  if(!isValid(index))
  {
    append(value);
    return;
  }
  beginInsertRows(QModelIndex(), index, index);
  m_lst.insert(index, value);
  endInsertRows();
}

template <class T>
void KMFListModel<T>::insert(int index, const QList<T>& values)
{
  if(values.count() < 1)
    return;
  if(!isValid(index))
  {
    append(values);
    return;
  }
  beginInsertRows(QModelIndex(), index, index + values.count() - 1);
  foreach(T value, values)
    m_lst.insert(index, value);
  endInsertRows();
}

// QModelIndex based functions

template <class T>
void KMFListModel<T>::removeAt(const QModelIndexList& list)
{
  QList<int> l;

  foreach(QModelIndex index, list)
    l.append(index.row());
  removeAt(l);
}

template <class T>
void KMFListModel<T>::removeAt(const QModelIndex &index)
{
  removeAt(index.row());
}

template <class T>
T KMFListModel<T>::at(const QModelIndex &index) const
{
  return at(index.row());
}

template <class T>
void KMFListModel<T>::replace(const QModelIndex &index, const T& value)
{
  replace(index.row(), value);
}

template <class T>
void KMFListModel<T>::swap(const QModelIndex &i1, const QModelIndex &i2)
{
  swap(i1.row(), i2.row());
}

template <class T>
void KMFListModel<T>::insert(const QModelIndex &index, const T& value)
{
  insert(index.row(), value);
}

template <class T>
void KMFListModel<T>::insert(const QModelIndex &index, const QList<T>& values)
{
  insert(index.row(), values);
}

// Others

template <class T>
void KMFListModel<T>::append(const T& value)
{
  beginInsertRows(QModelIndex(), m_lst.count(), m_lst.count());
  m_lst.append(value);
  endInsertRows();
}

template <class T>
void KMFListModel<T>::append(const QList<T>& values)
{
  if(values.count() < 1)
    return;
  beginInsertRows(QModelIndex(), m_lst.count(),
                  m_lst.count() + values.count() - 1);
  m_lst << values;
  endInsertRows();
}

template <class T>
int KMFListModel<T>::removeAll(const T& value)
{
  QList<int> l;
  int i = 0;

  foreach(T item, m_lst)
  {
    if(item == value)
      l.append(i);
    ++i;
  }
  removeAt(l);
  return l.count();
}

template <class T>
void KMFListModel<T>::clear()
{
  m_lst.clear();
  reset();
}

template <class T>
QList<T> KMFListModel<T>::list() const
{
  return m_lst;
}

template <class T>
void KMFListModel<T>::setList(const QList<T> &values)
{
  m_lst = values;
  reset();
}

template <class T>
int KMFListModel<T>::count() const
{
  return rowCount();
}

#endif // KMFLISTMODEL_H
