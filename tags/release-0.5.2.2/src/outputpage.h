//**************************************************************************
//   Copyright (C) 2004 by Petri Damstén
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
#ifndef OUTPUTPAGE_H
#define OUTPUTPAGE_H

#include <outputpagelayout.h>
#include <qpopupmenu.h>

class QIconViewItem;

/**
*/
class OutputPage : public OutputPageLayout
{
    Q_OBJECT
  public:
    OutputPage(QWidget *parent = 0, const char *name = 0);
    ~OutputPage();

  public slots:
    void aboutToShowPage(QWidget* page);
    void start();

  protected slots:
    void currentChanged(QIconViewItem* item);
    void contextMenuRequested(QIconViewItem *item, const QPoint &pos);
    void stop();
    void showLog();
    void start(int type);

  private:
    QPopupMenu m_startPopup;
    QMap<int, QString> m_types;
    QString m_type;
};

#endif
