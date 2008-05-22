//**************************************************************************
//   Copyright (C) 2004-2008 by Petri Damsten
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
#include "kmediafactory.h"
#include "kmfapplication.h"
#include "kmfprogressitemdelegate.h"
#include "kmfplugininterface.h"
#include "kmftoolbutton.h"
#include "logview.h"
#include <kmftools.h>
#include <threadweaver/ThreadWeaver.h>
#include <KMessageBox>
#include <KCursor>
#include <KPushButton>
#include <KLocale>
#include <KDebug>
#include <KFileItemDelegate>
#include <kxmlguifactory.h>
#include <KPageDialog>
#include <QToolButton>
#include <QTimer>
#include <QStandardItemModel>
#include <QTextDocument>

OutputPage::OutputPage(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);
  outputs->setSpacing(5);
  outputs->setItemDelegate(new KFileItemDelegate(this));
  outputs->setIconSize(QSize(KIconLoader::SizeHuge, KIconLoader::SizeHuge));
  connect(outputs, SIGNAL(customContextMenuRequested(const QPoint&)),
          this, SLOT(contextMenuRequested(const QPoint&)));
  connect(&m_startPopup, SIGNAL(triggered(QAction*)),
           this, SLOT(start(QAction*)));
  connect(ThreadWeaver::Weaver::instance(), SIGNAL(finished()), this, SLOT(finished()));
  connect(ThreadWeaver::Weaver::instance(), SIGNAL(jobDone(ThreadWeaver::Job *)), 
          this, SLOT(jobDone(ThreadWeaver::Job *)));
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
  kmfApp->interface()->message(KMF::Root, KMF::Error, i18n("User cancelled."));
  kmfApp->interface()->setStopped(true);
  ThreadWeaver::Weaver::instance()->requestAbort();
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
  ThreadWeaver::Weaver::instance()->dequeue();
  ThreadWeaver::Weaver::instance()->suspend();
  kmfApp->mainWindow()->enableUi(false);
  showLogPushBtn->setEnabled(false);
  stopPushBtn->setEnabled(true);
  startButton->setEnabled(false);
  kmfApp->interface()->setUseMessageBox(false);
  kmfApp->interface()->setStopped(false);
  kmfApp->interface()->clearJobs();
  m_items.clear();
  m_model->clear();
  progressBar->setValue(0);
  message(KMF::Root, KMF::Info, i18n("Preparing files..."));
  if (kmfApp->project()->prepare(m_type))
  {
    int jobs = kmfApp->interface()->jobCount();
    progressBar->setMaximum(jobs + 1);
    progressBar->setValue(1);
    // Run jobs
    message(KMF::Root, KMF::Info, i18n("Making files..."));
    ThreadWeaver::Weaver::instance()->resume();
  }
  else
  {
    finished();
  }
}

void OutputPage::jobDone(ThreadWeaver::Job *)
{
  CHECK_IF_STOPPED();
  progressBar->setValue(progressBar->value() + 1);
}

void OutputPage::finished()
{
  ThreadWeaver::Weaver::instance()->dequeue();
  kmfApp->interface()->clearJobs();
  kmfApp->project()->finished();
  m_type = "";
  makeLog();
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

void OutputPage::message(uint id, KMF::MsgType type, const QString& msg)
{
  CHECK_IF_STOPPED();
  QString icon;
  QColor color;
  KMessageBox::DialogType dlgType = KMessageBox::Information;
  QStandardItem *item = 0;
  QStandardItem *parent = 0;
  uint parentId = KMF::PluginInterface::parent(id);

  kDebug() << parentId << id << type << msg;
  if (id == KMF::Root)
    parent = m_model->invisibleRootItem();
  else if (m_items.keys().contains(id))
    parent = m_items[id];
  else if (parentId != 0 && m_items.keys().contains(parentId))
    parent = m_items[parentId];
  else if (m_items.keys().contains(KMF::Root))
    parent = m_items[KMF::Root];
  else
    parent = m_model->invisibleRootItem();

  switch(type)
  {
    case KMF::Start:
      icon = "application-x-executable";
      color = QColor("darkGreen");
      break;
    case KMF::Info:
      icon = "help-about";
      color = QColor("darkGreen");
      break;
    case KMF::Warning:
      icon = "flag-yellow";
      parent->setData(icon, KMFProgressItemDelegate::ResultRole);
      color = QColor(211, 183, 98);
      dlgType = KMessageBox::Sorry;
      break;
    case KMF::Error:
      icon = "process-stop";
      parent->setData(icon, KMFProgressItemDelegate::ResultRole);
      color = QColor("red");
      dlgType = KMessageBox::Error;
      break;
    case KMF::OK:
      icon = "dialog-ok";
      color = QColor("darkGreen");
      break;
    case KMF::Done:
      parent->setData(0, KMFProgressItemDelegate::MaxRole);
      parent->setData(0, KMFProgressItemDelegate::ValueRole);
      parent->setData(KIcon(parent->data(KMFProgressItemDelegate::ResultRole).toString()), 
                      Qt::DecorationRole);
      return; // We are done here
    case KMF::Root:
      return;
  }

  if(kmfApp->interface()->useMessageBox())
  {
    KMessageBox::messageBox(this, dlgType, msg);
  }
  else
  {
    item = new QStandardItem(KIcon(icon), msg);
    item->setData("dialog-ok", KMFProgressItemDelegate::ResultRole);
    item->setData(color, KMFProgressItemDelegate::ColorRole);
    parent->appendRow(item);
  
    if (id == KMF::Root)
    {
      m_items.clear();
      m_items[id] = item;
    }
    if (type == KMF::Start)
    {
      m_items[id] = item;
    }
    progressListView->scrollTo(m_model->indexFromItem(item));
    kmfApp->processEvents(QEventLoop::AllEvents);
  }
}

void OutputPage::setMaximum(uint id, int maximum)
{
  if (m_items.keys().contains(id))
  {
    QStandardItem *item = m_items[id];
    item->setData(maximum, KMFProgressItemDelegate::MaxRole);
  }
}

void OutputPage::setValue(uint id, int value)
{
  if (m_items.keys().contains(id))
  {
    QStandardItem *item = m_items[id];
    item->setData(value, KMFProgressItemDelegate::ValueRole);
  }
}

void OutputPage::log(uint id, const QString& msg)
{
  if (m_items.keys().contains(id))
  {
    QStandardItem *item = m_items[id];
    item->setData(msg, KMFProgressItemDelegate::LogRole);
  }
}

void OutputPage::makeLog()
{
  QString s = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n" \
              "<html><pre style=\"font-size: 9pt;\">";
  QString tmp;

  foreach (const QStandardItem* item, 
           m_model->findItems("*", Qt::MatchWildcard | Qt::MatchRecursive)) 
  {
    tmp = item->text();
    QColor color = item->data(KMFProgressItemDelegate::ColorRole).value<QColor>();
    if(color != QColor("black"))
      s += QString("<font color=%1>").arg(color.name());
    s += Qt::escape(tmp);
    if(color != QColor("black"))
      s += "</font>";
    s += "\n";

    tmp = item->data(KMFProgressItemDelegate::LogRole).toString().trimmed();
    if(!tmp.isEmpty())
    {
      tmp = Qt::escape(tmp) + "\n";
      tmp.replace("\n", "<br />");
      s += tmp;
    }
  }
  s += "</pre></html>";

  KMF::Tools::saveString2File(kmfApp->project()->directory() + "kmf_log.html", s);
}

#include "outputpage.moc"
