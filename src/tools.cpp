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
    switch(index.column())
    {
      case 0:
        return appIcon(at(i).icon);
    }
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
  toolsListView->setModel(&m_model);
  load();
  connect(toolsListView->selectionModel(),
      SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
      this, SLOT(enableButtons()));
}

Tools::~Tools()
{
}

void Tools::addClicked()
{
  ToolProperties dlg(kapp->activeWindow());

  if (dlg.exec())
  {
    ToolItem item;
    dlg.getData(&item);
    m_model.append(item);
  }
  enableButtons();
}

void Tools::propertiesClicked()
{
  QModelIndexList list = toolsListView->selectionModel()->selectedIndexes();

  if(list.size() > 0)
  {
    ToolItem item = m_model.at(list.first());
    ToolProperties dlg(kapp->activeWindow());
    dlg.setData(item);
    dlg.setReadOnly(!writableItem(&item));
    if (dlg.exec())
    {
      dlg.getData(&item);
      m_model.replace(list.first(), item);
    }
  }
}

void Tools::removeClicked()
{
  QModelIndexList list = toolsListView->selectionModel()->selectedIndexes();

  if(list.size() > 0)
  {
    ToolItem item = m_model.at(list.first());

    if(!item.desktopFile.isEmpty())
      m_remove.append(item.desktopFile);
    m_model.removeAt(list.first());
    enableButtons();
  }
}

void Tools::save()
{
  if(!m_remove.isEmpty())
    KIO::del(m_remove);

  QList<ToolItem> items = m_model.list();
  QFileInfo file;
  QString name;

  foreach(ToolItem item, items)
  {
    if(!writableItem(&item))
      continue;

    name = KMF::Tools::simpleName(item.name);

    if(item.desktopFile.isEmpty())
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
      file = item.desktopFile;
    KDesktopFile df(file.absoluteFilePath());
    KConfigGroup group = df.group("Desktop Entry");
    group.writeEntry("Name", item.name);
    group.writeEntry("Type", "Application");
    group.writeEntry("Icon", item.icon);
    group.writeEntry("GenericName", item.description);
    group.writeEntry("Exec", item.command);
    group.writeEntry("Path", item.workPath);
    group.writeEntry("Terminal", item.runInTerminal);
    group.writeEntry("X-KMF-MediaMenu", item.mediaMenu);
  }
}

void Tools::load()
{
  QStringList files =
      KGlobal::dirs()->findAllResources("appdata", "tools/*.desktop");

  for(QStringList::ConstIterator it = files.begin();
      it != files.end(); ++it)
  {
    KDesktopFile df(*it);

    if(df.readType() != "Application")
      continue;

    ToolItem item;
    item.name = df.readName();
    item.icon = df.readIcon();
    item.description = df.readGenericName();
    item.workPath = df.readPath();
    KConfigGroup group = df.group("Desktop Entry");
    item.command = group.readEntry("Exec");
    item.mediaMenu = (group.readEntry("X-KMF-MediaMenu") == "true");
    item.runInTerminal = (group.readEntry("Terminal") == "true");
    item.desktopFile = *it;
    m_model.append(item);
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
  QModelIndexList list = toolsListView->selectionModel()->selectedIndexes();
  bool writable = false;

  if(list.size() > 0)
  {
    ToolItem item = m_model.at(list.first());
    writable = writableItem(&item);
  }
  propertiesButton->setEnabled(list.size() > 0);
  removeButton->setEnabled(list.size() > 0 && writable);
}

#include "tools.moc"
