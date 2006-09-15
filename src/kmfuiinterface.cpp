//**************************************************************************
//   Copyright (C) 2004 by Petri Damst�
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
#include "kmfprogressitem.h"
#include "kmfprogresslistview.h"
#include "kmediafactory.h"
#include "mediapage.h"
#include "templatepage.h"
#include "kmficonview.h"
#include "outputpage.h"
#include <kprogressbar.h>
#include <kdebug.h>
#include <klistbox.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <QToolButton>
#include <QEventLoop>
#include <QPixmap>
#include <Q3Frame>

#if RECORD_TIMINGS
  StopWatch stopWatch;
#endif

KMFUiInterface::KMFUiInterface(QObject *parent) :
  KMF::UiInterface(parent), m_useMessageBox(false)
{
}

KMFUiInterface::~KMFUiInterface()
{
}

bool KMFUiInterface::addMediaObject(KMF::MediaObject *media) const
{
  kmfApp->project()->addItem(media);
  return true;
}

bool KMFUiInterface::addTemplateObject(KMF::TemplateObject* tob)
{
  KMediaFactory* mainWindow = kmfApp->mainWindow();
  mainWindow->templatePage->templates->newItem(tob);
  return true;
}

bool KMFUiInterface::addOutputObject(KMF::OutputObject* oob)
{
  KMediaFactory* mainWindow = kmfApp->mainWindow();
  mainWindow->outputPage->outputs->newItem(oob);
  return true;
}

bool KMFUiInterface::removeMediaObject(KMF::MediaObject *media) const
{
  kmfApp->project()->removeItem(media);
  return true;
}

bool KMFUiInterface::removeTemplateObject(KMF::TemplateObject* tob)
{
  KMediaFactory* mainWindow = kmfApp->mainWindow();
  if(mainWindow)
    mainWindow->templatePage->templates->itemRemoved(tob);
  return true;
}

bool KMFUiInterface::removeOutputObject(KMF::OutputObject* oob)
{
  KMediaFactory* mainWindow = kmfApp->mainWindow();
  if(mainWindow)
    mainWindow->outputPage->outputs->itemRemoved(oob);
  return true;
}

bool KMFUiInterface::message(KMF::MsgType type, const QString& msg)
{
  KMediaFactory* mainWindow = kmfApp->mainWindow();
  QPixmap pixmap;
  QColor color;
  KMessageBox::DialogType dlgType = KMessageBox::Information;

#if RECORD_TIMINGS
  stopWatch.message(msg);
#endif

  switch(type)
  {
    case KMF::None:
    case KMF::Info:
      pixmap = KGlobal::iconLoader()->loadIcon("info", K3Icon::Small);
      color = QColor("darkGreen");
      dlgType = KMessageBox::Information;
      break;
    case KMF::Warning:
      pixmap = KGlobal::iconLoader()->loadIcon("flag", K3Icon::Small);
      color = QColor(211, 183, 98);
      dlgType = KMessageBox::Sorry;
      break;
    case KMF::Error:
      pixmap = KGlobal::iconLoader()->loadIcon("cancel", K3Icon::Small);
      color = QColor("red");
      dlgType = KMessageBox::Error;
      break;
    case KMF::OK:
      pixmap = KGlobal::iconLoader()->loadIcon("ok", K3Icon::Small);
      color = QColor("darkGreen");
      dlgType = KMessageBox::Information;
      break;
  }
  KMFProgressListView* lv = mainWindow->outputPage->progressListView;
  lv->insertItem(pixmap, msg);
  kmfApp->logger().message(msg, color);
  if(m_useMessageBox)
    KMessageBox::messageBox(mainWindow, dlgType, msg);
  //mainWindow->outputPage->progressListbox->repaint();
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
    kdDebug() << k_funcinfo
        << mainWindow->outputPage->progressBar->progress() << " + " << advance
        << " / " << mainWindow->outputPage->progressBar->totalSteps()
        << endl;
    */
    mainWindow->outputPage->progressBar->advance(advance);
  }
#if RECORD_TIMINGS
  stopWatch.progress(advance);
#endif
  kmfApp->processEvents(QEventLoop::AllEvents);
  return m_stopped;
}

bool KMFUiInterface::setItemTotalSteps(int totalSteps)
{
  KMediaFactory* mainWindow =
      kmfApp->mainWindow();
  KMFProgressListView* lv = mainWindow->outputPage->progressListView;
  lv->setTotalSteps(totalSteps);
  kmfApp->processEvents(QEventLoop::AllEvents);
  return m_stopped;
}

bool KMFUiInterface::setItemProgress(int progress)
{
  KMediaFactory* mainWindow =
      kmfApp->mainWindow();
  KMFProgressListView* lv = mainWindow->outputPage->progressListView;
  lv->setProgress(progress);
  kmfApp->processEvents(QEventLoop::AllEvents);
  return m_stopped;
}

#include "kmfuiinterface.moc"
