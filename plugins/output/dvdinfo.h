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

#include <kmflistmodel.h>
#include <qdvdinfo.h>
#include <ui_dvdinfo.h>

class DVDModel : public KMFListModel<QDVD::Base*>
{
  virtual int columnCount(const QModelIndex&) const;
  virtual QVariant data(const QModelIndex &index, int role) const;
  virtual QVariant headerData(int column, Qt::Orientation, int role) const;
};

Q_DECLARE_METATYPE(QDVD::Base*);

class DVDInfo : public KDialog, public Ui::DVDInfo
{
    Q_OBJECT
  public:
    DVDInfo(QWidget *parent = 0, QString device = "/dev/dvd");
    ~DVDInfo();

  protected slots:
    void open();
    void currentChanged(const QModelIndex&, const QModelIndex&);
    void configureFileDialog(KUrlRequester*);

  private:
    QDVD::Info m_info;
    DVDModel m_model;

    void analyze();
    bool isDVD();
};

#endif // HAVE_LIBDVDREAD
#endif // DVDINFO_H
