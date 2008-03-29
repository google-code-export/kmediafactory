/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef DVDINFO_H
#define DVDINFO_H

#include <kmf_stddef.h>

#ifdef HAVE_LIBDVDREAD

#include "dvdinfolayout.h"
#include "qdvdinfo.h"
#include <qlistview.h>

class DVDItem : public QListViewItem
{
  public:
    DVDItem(QListView* parent, const QDVD::Base* data);
    DVDItem(QListViewItem* parent, const QDVD::Base* data);
    DVDItem(QListViewItem* parent, QListViewItem* after,
            const QDVD::Base* data);
    ~DVDItem();

    virtual QString text(int column) const;
    const QDVD::Base* data() const { return m_data; };
    void initItem();

  private:
    const QDVD::Base* m_data;
};

class DVDInfo : public DVDInfoLayout
{
    Q_OBJECT
  public:
    DVDInfo(QWidget *parent = 0, const char *name = 0,
            QString device = "/dev/dvd");
    ~DVDInfo();

  protected slots:
    virtual void open();
    virtual void itemChanged(QListViewItem*);
    virtual void configureFileDialog(KURLRequester*);

  private:
    QDVD::Info m_info;

    void analyze();
    bool isDVD();
};

#endif // HAVE_LIBDVDREAD
#endif // DVDINFO_H
