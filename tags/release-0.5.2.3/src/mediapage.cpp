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
#include "mediapage.h"
#include "kmediafactory.h"
#include "kmfapplication.h"
#include "kmficonview.h"
#include "sizewidget.h"
#include <qpopupmenu.h>
#include <qpoint.h>
#include <qiconview.h>

MediaPage::MediaPage(QWidget *parent, const char *name, WFlags fl)
 : MediaPageLayout(parent, name, fl)
{
  connect(mediaFiles,
          SIGNAL(contextMenuRequested(QIconViewItem*, const QPoint&)),
          this, SLOT(contextMenuRequested(QIconViewItem*, const QPoint&)));
  mediaFiles->setItemsMovable(true);
  mediaFiles->setAutoArrange(true);
  calculateSizes();
}

MediaPage::~MediaPage()
{
}

void MediaPage::contextMenuRequested(QIconViewItem *item, const QPoint &pos)
{
  if(item)
  {
    KMediaFactory* mainWindow =
        static_cast<KMediaFactory*>(kmfApp->mainWidget());
    KXMLGUIFactory* factory = mainWindow->factory();
    KMF::Object* ob = (static_cast<KMFIconViewItem*>(item))->ob();
    QIconView* iconView = item->iconView();

    QPtrList<KAction> actions;
    ob->actions(actions);
    factory->plugActionList(mainWindow,
        QString::fromLatin1("media_file_actionlist"), actions);
    QWidget *w = factory->container("media_file_popup", mainWindow);
    if(w)
    {
      QPopupMenu *popup = static_cast<QPopupMenu*>(w);
      popup->exec(pos);
      // TODO: Should object update itself?
      if(iconView->index(item) != -1)
      {
        item->setText(ob->title());
        item->setPixmap(ob->pixmap());
        calculateSizes();
      }
    }
    factory->unplugActionList(mainWindow, "media_file_actionlist");
  }
}

void MediaPage::calculateSizes()
{
  uint64_t max = 4700372992LL;
  uint64_t size = 0;

  if(kmfApp->project())
  {
    QPtrList<KMF::MediaObject>* mobs = kmfApp->project()->mediaObjects();
    KMF::MediaObject *mob;

    for (mob = mobs->first(); mob; mob = mobs->next())
      size += mob->size();
    // size += TODO: MENU SIZE
  }
  sizeWidget->setMax(max);
  sizeWidget->setSize(size);
}

#include "mediapage.moc"
