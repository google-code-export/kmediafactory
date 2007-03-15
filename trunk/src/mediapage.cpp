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
#include "mediapage.h"
#include "kmediafactory.h"
#include "kmfapplication.h"
#include "kmficonview.h"
#include <kmftools.h>
#include "sizewidget.h"
#include <kxmlguifactory.h>
#include <kactioncollection.h>
#include <QPoint>
#include <QMenu>
#include <QTimer>

MediaPage::MediaPage(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);

  mediaFiles->setSpacing(5);
  mediaFiles->setItemDelegate(new KMFItemDelegate());
  connect(mediaFiles, SIGNAL(customContextMenuRequested(const QPoint&)),
          this, SLOT(contextMenuRequested(const QPoint&)));
}

MediaPage::~MediaPage()
{
}

void MediaPage::projectInit()
{
  calculateSizes();

  QList<KMF::MediaObject*>* mobs = kmfApp->project()->mediaObjects();
  m_model.setData(mobs);
  mediaFiles->setModel(&m_model);
}

void MediaPage::mediaModified()
{
  kDebug() << k_funcinfo << endl;
  calculateSizes();
  KMF::Tools::updateView(mediaFiles);
}

void MediaPage::contextMenuRequested(const QPoint &pos)
{
  QModelIndex i = mediaFiles->indexAt(pos);

  if(i.row() < 0 || i.row() > kmfApp->project()->mediaObjects()->count())
    return;

  KMF::MediaObject* ob = kmfApp->project()->mediaObjects()->at(i.row());
  KMediaFactory* mainWindow = kmfApp->mainWindow();
  KXMLGUIFactory* factory = mainWindow->factory();

  QList<QAction*> actions;
  ob->actions(actions);
  factory->plugActionList(mainWindow,
      QString::fromLatin1("media_file_actionlist"), actions);
  QWidget *w = factory->container("media_file_popup", mainWindow);
  if(w)
  {
    QMenu* popup = static_cast<QMenu*>(w);
    popup->exec(mediaFiles->viewport()->mapToGlobal(pos));
  }
  factory->unplugActionList(mainWindow, "media_file_actionlist");
}

void MediaPage::calculateSizes()
{
  uint64_t max = 4700372992LL;
  uint64_t size = 0;

  if(kmfApp->project())
  {
    QList<KMF::MediaObject*>* mobs = kmfApp->project()->mediaObjects();
    KMF::MediaObject *mob;

    foreach(mob, *mobs)
      size += mob->size();
    // size += TODO: MENU SIZE
  }
  sizeWidget->setMax(max);
  sizeWidget->setSize(size);
}

#include "mediapage.moc"
