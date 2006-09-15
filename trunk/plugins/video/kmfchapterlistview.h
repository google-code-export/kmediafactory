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
#ifndef KMFCHAPTERLISTVIEW_H
#define KMFCHAPTERLISTVIEW_H

#include <Q3ListView>
#include <kmftime.h>

class KMFChapterListView;

/**
@author Petri Damsten
*/
class KMFChapterListViewItem : public Q3ListViewItem
{
  public:
    KMFChapterListViewItem(Q3ListView *parent, Q3ListViewItem *after);
    KMFChapterListViewItem(Q3ListView *parent, Q3ListViewItem *after,
                           const QString& name, QTime pos);
    ~KMFChapterListViewItem();

    virtual QString text(int column) const;
    KMFChapterListView* listView() const;
    KMFChapterListViewItem* nextSibling() const;

    QTime pos() const { return m_pos; };
    QTime length() const;
    void setPos(QTime pos) { m_pos = pos; };
    QString name() const { return text(0); };
    void setName(const QString& name) { setText(0, name); };

  private:
    KMF::Time m_pos;
};

class KMFChapterListView : public Q3ListView
{
    Q_OBJECT
  public:
    KMFChapterListView(QWidget *parent = 0, const char *name = 0);
    ~KMFChapterListView();

    KMFChapterListViewItem* selectedItem() const;
    KMFChapterListViewItem* currentItem() const;

    const double& duration() const { return m_duration; };
    void setDuration(const double& duration) { m_duration = duration; };

  private:
    double m_duration;
};

#endif
