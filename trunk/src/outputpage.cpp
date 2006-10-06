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
#include "outputpage.h"
#include "kmficonview.h"
#include "kmediafactory.h"
#include "kmfapplication.h"
#include "kmfprogresslistview.h"
#include "kmfuiinterface.h"
#include "kmftoolbutton.h"
#include "logview.h"
#include <kmftools.h>
#include <kcursor.h>
#include <kpushbutton.h>
#include <kprogressbar.h>
#include <klocale.h>
#include <kdebug.h>
#include <kxmlguifactory.h>
#include <kpagedialog.h>
#include <QToolButton>
#include <QTimer>

OutputPage::OutputPage(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);
  connect(outputs, SIGNAL(customContextMenuRequested(const QPoint&)),
          this, SLOT(contextMenuRequested(const QPoint&)));
  connect(&m_startPopup, SIGNAL(triggered(QAction*)),
           this, SLOT(start(QAction*)));
}

OutputPage::~OutputPage()
{
}

void OutputPage::projectInit()
{
  QList<KMF::OutputObject*>* oobs = kmfApp->project()->outputObjects();
  m_model.setData(oobs);
  outputs->setModel(&m_model);
  connect(outputs->selectionModel(),
          SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
          this, SLOT(currentChanged(const QModelIndex&, const QModelIndex&)));
  outputs->blockSignals(true);
  outputs->setCurrentIndex(m_model.index(kmfApp->project()->output()));
  outputs->blockSignals(false);
}

void OutputPage::outputsModified()
{
  KMF::Tools::updateView(outputs);
}

void OutputPage::currentChanged(const QModelIndex& index, const QModelIndex&)
{
  if(kmfApp->project())
  {
    KMF::OutputObject* ob =
        kmfApp->project()->outputObjects()->at(index.row());
    kmfApp->project()->setOutput(ob);
  }
}

void OutputPage::contextMenuRequested(const QPoint &pos)
{
  QModelIndex i = outputs->indexAt(pos);

  if(i.row() < 0 || i.row() > kmfApp->project()->outputObjects()->count())
    return;

  KMF::OutputObject* ob = kmfApp->project()->outputObjects()->at(i.row());
  KMediaFactory* mainWindow = kmfApp->mainWindow();
  KXMLGUIFactory* factory = mainWindow->factory();

  QList<KAction*> actions;
  ob->actions(actions);
  factory->plugActionList(mainWindow,
      QString::fromLatin1("output_actionlist"), actions);
  QWidget *w = factory->container("output_popup", mainWindow);
  if(w)
  {
    QMenu* popup = static_cast<QMenu*>(w);
    if(popup->actions().count() > 0)
      popup->exec(pos);
  }
  factory->unplugActionList(mainWindow, "output_actionlist");
}

void OutputPage::showLog()
{
  LogView dlg(this);
  KUrl url = "file://" + kmfApp->project()->directory() + "kmf_log.html";
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

void OutputPage::start(QAction* type)
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
  progressBar->setRange(0, kmfApp->project()->timeEstimate());
  progressBar->setValue(0);
#warning TODO
  //progressListView->clear();
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

void OutputPage::currentPageChanged(KPageWidgetItem* current, KPageWidgetItem*)
{
  QMap<QString, QString> types = kmfApp->project()->subTypes();

  m_startPopup.clear();
  m_types.clear();
  if(types.count() > 0)
  {
    for (QMap<QString, QString>::ConstIterator it = types.begin();
         it != types.end(); ++it)
    {
      QAction* action = new QAction(it.value(), this);
      m_types[action] = it.key();
      m_startPopup.addAction(action);
    }
    startButton->setMenu(&m_startPopup);
  }
  else
    startButton->setMenu(0);

  if (current->parent() == this)
  {
    startButton->setEnabled(kmfApp->project()->mediaObjects()->count() > 0);
    progressBar->setValue(0);
  }
}

#include "outputpage.moc"
