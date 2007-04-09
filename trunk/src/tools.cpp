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
#include <kio/deletejob.h>
#include <QImage>
#include <QFileInfo>
#include <QPixmap>

// From kmenuedit - treeview.cpp
static QPixmap appIcon(const QString &iconName)
{
  QPixmap normal = KIconLoader::global()->loadIcon(iconName, K3Icon::Small, 0,
  K3Icon::DefaultState, 0L, true);
    // make sure they are not larger than 20x20
  if (normal.width() > 20 || normal.height() > 20)
  {
    QImage tmp = normal.toImage();
    tmp = tmp.scaled(20, 20, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    normal.fromImage(tmp);
  }
  return normal;
}

int ToolListModel::columnCount(const QModelIndex&) const
{
  return 2;
};

QVariant ToolListModel::data(const QModelIndex &index, int role) const
{
  int i = index.row();

  if (!isValid(i))
    return QVariant();

  if (role == Qt::DisplayRole)
  {
    switch(index.column())
    {
      case 0:
        return at(i).name;
      case 1:
        return at(i).description;
    }
  }
  if (role == Qt::DecorationRole)
  {
    return appIcon(at(i).icon);
  }
  return QVariant();
};

QVariant ToolListModel::headerData(int column, Qt::Orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  switch(column)
  {
    case 0:
      return i18n("Name");
    case 1:
      return i18n("Description");
  }
  return "";
};

Tools::Tools(QWidget* parent) :
    QWidget(parent)
{
  setupUi(this);
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
    ToolItem* item = new ToolItem();
    dlg.getData(item);
#warning TODO add to treeview model
  }
  enableButtons();
}

void Tools::propertiesClicked()
{
#warning TODO get item data
  ToolItem* item = 0;

  if(item)
  {
    ToolProperties dlg(kapp->activeWindow());
    dlg.setData(*item);
    dlg.setReadOnly(!writableItem(item));
    if (dlg.exec())
    {
      dlg.getData(item);
#warning TODO
//      KMF::Tools::updateView(toolsListView);
    }
  }
}

void Tools::removeClicked()
{
#warning TODO get item data
  ToolItem* item = 0;

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

  ToolItem* item;
  QFileInfo file;
  QString name;

#warning TODO save data
/*
  for(Q3ListViewItemIterator it(toolsListView); *it != 0; ++it)
  {
    item = static_cast<ToolItem*>(*it);
    if(!writableItem(item))
      continue;

    name = KMF::Tools::simpleName(item->name);

    if(item->desktopFile.isEmpty())
    {
      int i = 0;

      file = KStandardDirs::locateLocal("appdata",
                                        "tools/" + name + ".desktop");
      while(file.exists())
      {
        file = KStandardDirs::locateLocal("appdata", "tools/" +
            QString("%1_%2").arg(name).arg(++i) + ".desktop");
      }
    }
    else
      file = item->desktopFile;
    KDesktopFile df(file.absoluteFilePath());
    df.writeEntry("Name", item->name);
    df.writeEntry("Type", "Application");
    df.writeEntry("Icon", item->icon);
    df.writeEntry("GenericName", item->description);
    df.writeEntry("Exec", item->command);
    df.writeEntry("Path", item->workPath);
    df.writeEntry("Terminal", item->runInTerminal);
    df.writeEntry("X-KMF-MediaMenu", item->mediaMenu);
  }
  */
}

void Tools::load()
{
  QStringList files =
      KGlobal::dirs()->findAllResources("appdata", "tools/*.desktop");
  ToolItem* item;

  for(QStringList::ConstIterator it = files.begin();
      it != files.end(); ++it)
  {
    KDesktopFile df(*it);

    if(df.readType() != "Application")
      continue;

    item = new ToolItem();
    item->name = df.readName();
    item->icon = df.readIcon();
    item->description = df.readGenericName();
    item->command = df.readEntry("Exec");
    item->workPath = df.readPath();
    item->mediaMenu = (df.readEntry("X-KMF-MediaMenu") == "true");
    item->runInTerminal = (df.readEntry("Terminal") == "true");
    item->desktopFile = *it;
#warning TODO add to treeview model
  }
  enableButtons();
}

bool Tools::writableItem(ToolItem* item)
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
#warning TODO get item data
  ToolItem* item = 0;
  if(item)
  {
    bool writable = writableItem(item);

    propertiesButton->setEnabled(item != 0L);
    removeButton->setEnabled(item != 0L & writable);
  }
}

#include "tools.moc"
