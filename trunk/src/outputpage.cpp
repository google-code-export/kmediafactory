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
#include "kmfprogressitemdelegate.h"
#include "kmfplugininterface.h"
#include "kmftoolbutton.h"
#include "logview.h"
#include <kmftools.h>
#include <kcursor.h>
#include <kpushbutton.h>
#include <klocale.h>
#include <kdebug.h>
#include <kxmlguifactory.h>
#include <kpagedialog.h>
#include <QToolButton>
#include <QTimer>
#include <QStandardItemModel>
#include <threadweaver/ThreadWeaver.h>
#include <KMessageBox>

OutputPage::OutputPage(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);
  outputs->setSpacing(5);
  outputs->setItemDelegate(new KMFItemDelegate());
  connect(outputs, SIGNAL(customContextMenuRequested(const QPoint&)),
          this, SLOT(contextMenuRequested(const QPoint&)));
  connect(&m_startPopup, SIGNAL(triggered(QAction*)),
           this, SLOT(start(QAction*)));
  connect(ThreadWeaver::Weaver::instance(), SIGNAL(finished()), this, SLOT(finished()));
  m_model = new QStandardItemModel();
  progressListView->setModel(m_model);
  progressListView->setItemDelegate(new KMFProgressItemDelegate());
}

OutputPage::~OutputPage()
{
}

void OutputPage::projectInit()
{
  outputs->setModel(kmfApp->project()->outputObjects());
  connect(outputs->selectionModel(),
          SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
          this, SLOT(currentChanged(const QModelIndex&, const QModelIndex&)));
  outputs->blockSignals(true);
  KMF::OutputObject* obj = kmfApp->project()->output();
  QModelIndex i = kmfApp->project()->outputObjects()->indexOf(obj);
  if(i == QModelIndex())
    i = kmfApp->project()->outputObjects()->index(0);
  outputs->setCurrentIndex(i);
  outputs->blockSignals(false);
}

void OutputPage::currentChanged(const QModelIndex& index, const QModelIndex&)
{
  if(kmfApp->project())
  {
    KMF::OutputObject* ob =
        kmfApp->project()->outputObjects()->at(index);
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

  QList<QAction*> actions;
  ob->actions(&actions);
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
  KUrl url = kmfApp->project()->directory() + "kmf_log.html";
  dlg.setData(url);
  dlg.exec();
}

void OutputPage::stop()
{
  stopPushBtn->setEnabled(false);
  // TODO stop threads
  kmfApp->interface()->setStopped(true);
  kmfApp->interface()->message(KMF::Error, i18n("User cancelled."));
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
  // TODO check if running ?
  ThreadWeaver::Weaver::instance()->suspend();
  kmfApp->mainWindow()->enableUi(false);
  showLogPushBtn->setEnabled(false);
  stopPushBtn->setEnabled(true);
  startButton->setEnabled(false);
  kmfApp->interface()->setUseMessageBox(false);
  kmfApp->interface()->setStopped(false);
  m_items.clear();
  m_model->clear();
  // TODO
  //progressBar->setRange(0, kmfApp->project()->timeEstimate());
  progressBar->setValue(0);
  kmfApp->interface()->message(KMF::Info, i18n("Preparing files..."));
  if (kmfApp->project()->prepare(m_type))
  {
    // Run jobs
    kmfApp->interface()->message(KMF::Info, i18n("Making files..."));
    ThreadWeaver::Weaver::instance()->resume();
  }
}

void OutputPage::finished()
{
  kmfApp->project()->finished();
  m_type = "";
  // TODO
  //kmfApp->logger().save(kmfApp->project()->directory() + "kmf_log.html");
  showLogPushBtn->setEnabled(true);
  stopPushBtn->setEnabled(false);
  startButton->setEnabled(true);
  kmfApp->mainWindow()->enableUi(true);
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
  // Arranges icon in a nice row. Otherwise icons are arranged in one column
  outputs->setViewMode(QListView::IconMode);
}

void OutputPage::message(KMF::MsgType type, const QString& txt, const QString& submsg)
{
  QString icon;
  QColor color;
  KMessageBox::DialogType dlgType = KMessageBox::Information;
  QStandardItem *item = 0;

  /*
  if (m_items.keys().contains(txt))
    item = m_items[txt];
  else
    item = m_model->invisibleRootItem();
  */
  switch(type)
  {
    case KMF::Start:
      break;
    case KMF::Info:
      icon = "dialog-information";
      color = QColor("darkGreen");
      dlgType = KMessageBox::Information;
      break;
    case KMF::Warning:
      icon = "dialog-warning";
      color = QColor(211, 183, 98);
      dlgType = KMessageBox::Sorry;
      break;
    case KMF::Error:
      icon = "dialog-error";
      color = QColor("red");
      dlgType = KMessageBox::Error;
      break;
    case KMF::OK:
      icon = "dialog-ok";
      color = QColor("darkGreen");
      dlgType = KMessageBox::Information;
      break;
  }

  item = new QStandardItem(KIcon(icon), txt);
  m_model->appendRow(item);
  /*
  setItemTotalSteps(0);
  QListView* lv = mainWindow->outputPage->progressListView;
  KMFProgressItemModel* model = static_cast<KMFProgressItemModel*>(lv->model());
  KMFProgressItem item;
  item.text = msg;
  item.pixmap = pixmap;
  model->append(item);
  lv->scrollTo(model->lastIndex());

  kmfApp->logger().message(msg, color);
  if(m_useMessageBox)
    KMessageBox::messageBox(mainWindow, dlgType, msg);
  */
  kmfApp->processEvents(QEventLoop::AllEvents);
}

void OutputPage::setMaximum(int maximum, const QString& txt)
{
}

void OutputPage::setValue(int value, const QString& txt)
{
}

void OutputPage::log(const QString& logtxt, const QString& txt)
{
}

#include "outputpage.moc"
