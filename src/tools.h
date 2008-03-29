//**************************************************************************
//   Copyright (C) 2004, 2005 by Petri Damstén
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
#ifndef TOOLS_H
#define TOOLS_H

#include "toolslayout.h"
#include <kurl.h>
#include <qlistview.h>

/**
	@author Petri Damsten <petri.damsten@iki.fi>
*/

class QToolListItem : public QListViewItem
{
  public:
    QToolListItem(QListView* listbox) :
      QListViewItem(listbox), mediaMenu(false), runInTerminal(false) {};

    QString name;
    QString description;
    QString command;
    QString icon;
    QString desktopFile;
    QString workPath;
    bool mediaMenu;
    bool runInTerminal;
};

class Tools : public ToolsLayout
{
    Q_OBJECT
  public:
    Tools(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    virtual ~Tools();

  public slots:
    virtual void save();
    virtual void load();

  protected slots:
    virtual void addClicked();
    virtual void propertiesClicked();
    virtual void removeClicked();
    virtual void enableButtons();

  protected:
    bool writableItem(QToolListItem* item);

  private:
    KURL::List m_remove;
};

#endif
