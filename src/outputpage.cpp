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
#include "outputpage.h"
#include "kmficonview.h"
#include "kmediafactory.h"
#include "kmfapplication.h"
#include "kmfprogresslistview.h"
#include "kmfprogressitem.h"
#include "kmfuiinterface.h"
#include "kmftoolbutton.h"
#include "logview.h"
#include <kcursor.h>
#include <kpushbutton.h>
#include <kprogress.h>
#include <klistview.h>
#include <klocale.h>
#include <kdebug.h>
#include <qpopupmenu.h>
#include <qheader.h>
#include <qtoolbutton.h>
#include <qtimer.h>

OutputPage::OutputPage(QWidget *parent, const char *name) :
  OutputPageLayout(parent, name)
{
  connect(outputs, SIGNAL(currentChanged(QIconViewItem*)),
          this, SLOT(currentChanged(QIconViewItem*)));
  connect(outputs,
          SIGNAL(contextMenuRequested(QIconViewItem*, const QPoint&)),
          this, SLOT(contextMenuRequested(QIconViewItem*, const QPoint&)));

  startButton->setPopupDelay(QApplication::startDragTime());
  connect(&m_startPopup, SIGNAL(activated(int)), this, SLOT(start(int)));
}

OutputPage::~OutputPage()
{
}

void OutputPage::currentChanged(QIconViewItem* item)
{
  if(item && kmfApp->project())
  {
    KMFIconViewItem* it = static_cast<KMFIconViewItem*>(item);
    KMF::OutputObject* ob = static_cast<KMF::OutputObject*>(it->ob());

    kmfApp->project()->setOutput(ob);
  }
}

void OutputPage::contextMenuRequested(QIconViewItem *item, const QPoint &pos)
{
  if(item)
  {
    KMediaFactory* mainWindow =
        static_cast<KMediaFactory*>(kmfApp->mainWidget());
    KXMLGUIFactory* factory = mainWindow->factory();
    KMF::Object* ob = (static_cast<KMFIconViewItem*>(item))->ob();

    QPtrList<KAction> actions;
    ob->actions(actions);
    factory->plugActionList(mainWindow,
        QString::fromLatin1("output_actionlist"), actions);
    QWidget *w = factory->container("output_popup", mainWindow);
    if(w)
    {
      QPopupMenu *popup = static_cast<QPopupMenu*>(w);
      if(popup->count() > 0)
        popup->exec(pos);
    }
    factory->unplugActionList(mainWindow, "output_actionlist");
  }
}

void OutputPage::showLog()
{
  LogView dlg(this);
  KURL url = "file://" + kmfApp->project()->directory() + "kmf_log.html";
  dlg.setData(url);
  dlg.exec();
}

void OutputPage::stop()
{
  stopPushBtn->setEnabled(false);
  kmfApp->uiInterface()->setStopped(true);
  kmfApp->uiInterface()->message(KMF::Error, i18n("User cancelled."));
  kmfApp->uiInterface()->setItemProgress(-1);
}

void OutputPage::start(int type)
{
  if(m_types.contains(type))
  {
    m_type = m_types[type];
    QTimer::singleShot(0, this, SLOT(start()));
  }
}

void OutputPage::start()
{
#if RECORD_TIMINGS
  stopWatch.start();
#endif
  kmfApp->mainWindow()->enableUi(false);
  showLogPushBtn->setEnabled(false);
  stopPushBtn->setEnabled(true);
  startButton->setEnabled(false);
  //kmfApp->setOverrideCursor(KCursor::waitCursor());
  kmfApp->uiInterface()->setUseMessageBox(false);
  kmfApp->uiInterface()->setStopped(false);
  progressBar->setTotalSteps(kmfApp->project()->timeEstimate());
  progressBar->setValue(0);
  progressListView->clear();
  kmfApp->logger().start();
  if(kmfApp->project()->make(m_type) == false)
    if(!kmfApp->project()->error().isEmpty())
      kmfApp->uiInterface()->message(KMF::Error, kmfApp->project()->error());
  m_type = "";
  kmfApp->logger().stop();
  kmfApp->logger().save(kmfApp->project()->directory() + "kmf_log.html");
  //kmfApp->restoreOverrideCursor();
  showLogPushBtn->setEnabled(true);
  stopPushBtn->setEnabled(false);
  startButton->setEnabled(true);
  kmfApp->mainWindow()->enableUi(true);
#if RECORD_TIMINGS
  stopWatch.stop();
#endif
}

void OutputPage::aboutToShowPage(QWidget* page)
{
  QMap<QString, QString> types = kmfApp->project()->subTypes();
  m_startPopup.clear();
  m_types.clear();
  if(types.count() > 0)
  {
    for (QMap<QString, QString>::ConstIterator it = types.begin();
         it != types.end(); ++it)
    {
      m_types[m_startPopup.insertItem(it.data())] = it.key();
    }
    startButton->setPopup(&m_startPopup);
  }
  else
    startButton->setPopup(0);

  if (parent() == page)
  {
    startButton->setEnabled(kmfApp->project()->mediaObjects()->count() > 0);
    progressBar->setValue(0);
  }
}

#include "outputpage.moc"
