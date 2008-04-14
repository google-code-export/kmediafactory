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

#include "kmfuiinterface.h"
#include "kmfapplication.h"
#include "kmfproject.h"
#include "kmfprogresslistview.h"
#include "kmediafactory.h"
#include "mediapage.h"
#include "templatepage.h"
#include "kmficonview.h"
#include "kmftools.h"
#include "outputpage.h"
#include <KDebug>
#include <KIconLoader>
#include <KMessageBox>
#include <KFileDialog>
#include <QToolButton>
#include <QEventLoop>
#include <QPixmap>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QTextStream>

KMFProgressDialog::KMFProgressDialog(QWidget* parent) : KMF::ProgressDialog(parent), m_pdlg(parent)
{
    m_pdlg.setAutoClose(false);
}

void KMFProgressDialog::setMaximum(int maximum)
{
    m_pdlg.progressBar()->setMaximum(maximum);
}

void KMFProgressDialog::setValue(int value)
{
    m_pdlg.progressBar()->setValue(value);
}

void KMFProgressDialog::setCaption(const QString &caption)
{
    m_pdlg.setCaption(caption);
}

void KMFProgressDialog::setLabel(const QString &label)
{
    m_pdlg.setLabelText(label);
}

void KMFProgressDialog::showCancelButton(bool show)
{
    m_pdlg.showCancelButton(show);
}

bool KMFProgressDialog::wasCancelled()
{
    return m_pdlg.wasCancelled();
}

void KMFProgressDialog::close()
{
    m_pdlg.close();
    deleteLater();
}

KMFUiInterface::KMFUiInterface(QObject *parent) :
  KMF::UiInterface(parent), m_useMessageBox(false)
{
}

KMFUiInterface::~KMFUiInterface()
{
}

bool KMFUiInterface::addMediaAction(QAction* action,
                                    const QString& group) const
{
  kmfApp->mainWindow()->mediaPage->mediaButtons->add(action, group);
  return true;
}

bool KMFUiInterface::addMediaObject(KMF::MediaObject *media) const
{
  KMFProject* project = kmfApp->project();

  if(project)
    project->addItem(media);
  return true;
}

bool KMFUiInterface::addTemplateObject(KMF::TemplateObject* tob)
{
  KMFProject* project = kmfApp->project();

  if(project)
    project->templateObjects()->append(tob);
  return true;
}

bool KMFUiInterface::addOutputObject(KMF::OutputObject* oob)
{
  KMFProject* project = kmfApp->project();

  if(project)
    project->outputObjects()->append(oob);
  return true;
}

bool KMFUiInterface::removeMediaObject(KMF::MediaObject *media) const
{
  KMFProject* project = kmfApp->project();

  if(project)
    project->removeItem(media);
  return true;
}

bool KMFUiInterface::removeTemplateObject(KMF::TemplateObject* tob)
{
  KMFProject* project = kmfApp->project();

  if(project)
    project->templateObjects()->removeAll(tob);
  return true;
}

bool KMFUiInterface::removeOutputObject(KMF::OutputObject* oob)
{
  KMFProject* project = kmfApp->project();

  if(project)
    project->outputObjects()->removeAll(oob);
  return true;
}

void KMFUiInterface::addMediaObject(const QString& xml)
{
  QDomDocument doc;
  doc.setContent(xml);
  kmfApp->project()->mediaObjFromXML(doc.documentElement());
}

void KMFUiInterface::selectTemplate(const QString& xml)
{
  QDomDocument doc;

  doc.setContent(xml);
  kmfApp->project()->templateFromXML(doc.documentElement());
}

void KMFUiInterface::selectOutput(const QString& xml)
{
  QDomDocument doc;

  doc.setContent(xml);
  kmfApp->project()->outputFromXML(doc.documentElement());
}

bool KMFUiInterface::message(KMF::MsgType type, const QString& msg)
{
  KMediaFactory* mainWindow = kmfApp->mainWindow();
  QString pixmap;
  QColor color;
  KMessageBox::DialogType dlgType = KMessageBox::Information;

  switch(type)
  {
    case KMF::None:
    case KMF::Info:
      pixmap = "dialog-information";
      color = QColor("darkGreen");
      dlgType = KMessageBox::Information;
      break;
    case KMF::Warning:
      pixmap = "dialog-warning";
      color = QColor(211, 183, 98);
      dlgType = KMessageBox::Sorry;
      break;
    case KMF::Error:
      pixmap = "dialog-error";
      color = QColor("red");
      dlgType = KMessageBox::Error;
      break;
    case KMF::OK:
      pixmap = "dialog-ok";
      color = QColor("darkGreen");
      dlgType = KMessageBox::Information;
      break;
  }
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
  kmfApp->processEvents(QEventLoop::AllEvents);
  return m_stopped;
}

KMF::Logger* KMFUiInterface::logger()
{
  return &kmfApp->logger();
}

bool KMFUiInterface::progress(int advance)
{
  KMediaFactory* mainWindow =
      kmfApp->mainWindow();

  if(advance)
  {
    /*
    kDebug()
        << mainWindow->outputPage->progressBar->value() << " + " << advance
        << " / " << mainWindow->outputPage->progressBar->maximum()
       ;
    */
    mainWindow->outputPage->progressBar->setValue(
        mainWindow->outputPage->progressBar->value()+advance);
  }
  kmfApp->processEvents(QEventLoop::AllEvents);
  return m_stopped;
}

bool KMFUiInterface::setItemTotalSteps(int totalSteps)
{
  QListView* lv = kmfApp->mainWindow()->outputPage->progressListView;
  KMFProgressItemModel* model = static_cast<KMFProgressItemModel*>(lv->model());
  int last = model->rowCount() - 1;
  KMFProgressItem item = model->at(last);
  if(item.max != totalSteps)
  {
    item.max = totalSteps;
    model->replace(last, item);
  }
  return m_stopped;
}

bool KMFUiInterface::setItemProgress(int progress)
{
  QListView* lv = kmfApp->mainWindow()->outputPage->progressListView;
  KMFProgressItemModel* model = static_cast<KMFProgressItemModel*>(lv->model());
  int last = model->rowCount() - 1;
  KMFProgressItem item = model->at(last);
  if(item.value != progress)
  {
    item.value = progress;
    model->replace(last, item);
  }
  return m_stopped;
}

void KMFUiInterface::progressDialogDestroyed()
{
  m_pdlg = 0;
}

KMF::ProgressDialog* KMFUiInterface::progressDialog(const QString &caption, const QString &label,
                                                    int maximum)
{
  if(m_pdlg)
    delete m_pdlg;
  m_pdlg = new KMFProgressDialog(kmfApp->mainWindow());
  connect(m_pdlg, SIGNAL(destroyed()), this, SLOT(progressDialogDestroyed));
  m_pdlg->setCaption(caption);
  m_pdlg->setLabel(label);
  m_pdlg->setMaximum(maximum);
  m_pdlg->showCancelButton(true);
  return m_pdlg;
}

KMF::ProgressDialog* KMFUiInterface::progressDialog()
{
  if(m_pdlg)
    return m_pdlg;
  return progressDialog("KMediaFactory", i18n("Progress:"), 100);
}

int KMFUiInterface::messageBox(const QString &caption, const QString &txt,
                               int type)
{
  kDebug() << caption << txt << type <<KMessageBox::Error ;
  if(type == KMessageBox::Information)
    KMessageBox::information(kmfApp->mainWindow(), txt, caption);
  else if(type == KMessageBox::Sorry)
    KMessageBox::sorry(kmfApp->mainWindow(), txt, caption);
  else if(type == KMessageBox::Error)
    KMessageBox::error(kmfApp->mainWindow(), txt, caption);
  return 0;
}

QStringList KMFUiInterface::getOpenFileNames(const QString &startDir,
                                             const QString &filter,
                                             const QString &caption)
{
  QString start = QString("kfiledialog:///<%1>").arg(startDir);
  //kDebug() << start;
  return KFileDialog::getOpenFileNames(KUrl(start), filter,
                                       kmfApp->mainWindow(), caption);
}

void KMFUiInterface::debug(const QString &txt)
{
  kDebug() << txt;
}

#include "kmfuiinterface.moc"
