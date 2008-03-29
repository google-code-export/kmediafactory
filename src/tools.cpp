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
#include "tools.h"
#include "toolproperties.h"
#include "kmftools.h"
#include <kapplication.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kio/job.h>
#include <kstandarddirs.h>
#include <kdesktopfile.h>
#include <kpushbutton.h>
#include <qimage.h>
#include <qfileinfo.h>

// From kmenuedit - treeview.cpp
static QPixmap appIcon(const QString &iconName)
{
  QPixmap normal = KGlobal::iconLoader()->loadIcon(iconName, KIcon::Small, 0,
  KIcon::DefaultState, 0L, true);
    // make sure they are not larger than 20x20
  if (normal.width() > 20 || normal.height() > 20)
  {
    QImage tmp = normal.convertToImage();
    tmp = tmp.smoothScale(20, 20);
    normal.convertFromImage(tmp);
  }
  return normal;
}

Tools::Tools(QWidget* parent, const char* name, WFlags fl) :
  ToolsLayout(parent, name, fl)
{
  connect(parent, SIGNAL(okClicked()), this, SLOT(save()));
  load();
}

Tools::~Tools()
{
}

void Tools::addClicked()
{
  ToolProperties dlg(kapp->activeWindow());

  if (dlg.exec())
  {
    QToolListItem* item = new QToolListItem(toolsListView);
    dlg.getData(item);
    item->setText(0, item->name);
    item->setText(1, item->description);
    item->setPixmap(0, appIcon(item->icon));
  }
  enableButtons();
}

void Tools::propertiesClicked()
{
  QToolListItem* item =
      static_cast<QToolListItem*>(toolsListView->selectedItem());

  if(item)
  {
    ToolProperties dlg(kapp->activeWindow());
    dlg.setData(*item);
    dlg.setReadOnly(!writableItem(item));
    if (dlg.exec())
    {
      dlg.getData(item);
      item->setText(0, item->name);
      item->setText(1, item->description);
      item->setPixmap(0, appIcon(item->icon));
    }
  }
}

void Tools::removeClicked()
{
  QToolListItem* item =
      static_cast<QToolListItem*>(toolsListView->selectedItem());

  if(item)
  {
    if(!item->desktopFile.isEmpty())
      m_remove.append(item->desktopFile);
    delete item;
  }
  enableButtons();
}

void Tools::save()
{
  if(!m_remove.isEmpty())
    KIO::del(m_remove);

  QToolListItem* item;
  QFileInfo file;
  QString name;

  for(QListViewItemIterator it(toolsListView); *it != 0; ++it)
  {
    item = static_cast<QToolListItem*>(*it);
    if(!writableItem(item))
      continue;

    name = KMF::Tools::simpleName(item->name);

    if(item->desktopFile.isEmpty())
    {
      int i = 0;

      file = locateLocal("appdata", "tools/" + name + ".desktop");
      while(file.exists())
      {
        file = locateLocal("appdata", "tools/" +
            QString("%1_%2").arg(name).arg(++i) + ".desktop");
      }
    }
    else
      file = item->desktopFile;
    KDesktopFile df(file.absFilePath());
    df.writeEntry("Name", item->name);
    df.writeEntry("Type", "Application");
    df.writeEntry("Icon", item->icon);
    df.writeEntry("GenericName", item->description);
    df.writeEntry("Exec", item->command);
    df.writeEntry("Path", item->workPath);
    df.writeEntry("Terminal", item->runInTerminal);
    df.writeEntry("X-KMF-MediaMenu", item->mediaMenu);
  }
}

void Tools::load()
{
  QStringList files =
      KGlobal::dirs()->findAllResources("appdata", "tools/*.desktop");
  QToolListItem* item;

  for(QStringList::ConstIterator it = files.begin();
      it != files.end(); ++it)
  {
    KDesktopFile df(*it);

    if(df.readType() != "Application")
      continue;

    item = new QToolListItem(toolsListView);
    item->name = df.readName();
    item->icon = df.readIcon();
    item->description = df.readGenericName();
    item->command = df.readEntry("Exec");
    item->workPath = df.readPath();
    item->mediaMenu = (df.readEntry("X-KMF-MediaMenu") == "true");
    item->runInTerminal = (df.readEntry("Terminal") == "true");
    item->desktopFile = *it;

    item->setText(0, item->name);
    item->setText(1, item->description);
    item->setPixmap(0, appIcon(item->icon));
  }
  enableButtons();
}

bool Tools::writableItem(QToolListItem* item)
{
  if(item)
  {
    if(!item->desktopFile.isEmpty())
    {
      QFileInfo fi(item->desktopFile);

      return fi.isWritable();
    }
    else
      return true;
  }
  return false;
}

void Tools::enableButtons()
{
  QToolListItem* item =
      static_cast<QToolListItem*>(toolsListView->selectedItem());
  bool writable = writableItem(item);

  propertiesButton->setEnabled(item != 0L);
  removeButton->setEnabled(item != 0L & writable);
}

#include "tools.moc"
