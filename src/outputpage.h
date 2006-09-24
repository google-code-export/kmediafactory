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
#ifndef OUTPUTPAGE_H
#define OUTPUTPAGE_H

#include <ui_outputpage.h>
#include <QMenu>

class Q3IconViewItem;
class KPageWidgetItem;
/**
*/
class OutputPage : public QWidget, public Ui::OutputPage
{
    Q_OBJECT
  public:
    OutputPage(QWidget *parent = 0);
    virtual ~OutputPage();

  public slots:
    void currentPageChanged(KPageWidgetItem*, KPageWidgetItem*);
    void start();

  protected slots:
    void currentChanged(Q3IconViewItem* item);
    void contextMenuRequested(Q3IconViewItem *item, const QPoint &pos);
    void stop();
    void showLog();
    void start(QAction* type);

  private:
    QMenu m_startPopup;
    QMap<QAction*, QString> m_types;
    QString m_type;
};

#endif
