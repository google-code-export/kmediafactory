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
#include "kmfdbusinterface.h"
#include "kmfapplication.h"
#include "kmediafactory.h"
#include "outputpage.h"
#include <kdebug.h>
#include <kpagewidget.h>
#include <kfiledialog.h>
#include <kprogressdialog.h>
#include <kmessagebox.h>

KMFDbusInterface::KMFDbusInterface(QObject *parent) :
  QObject(parent), m_pdlg(0)
{
}

KMFDbusInterface::~KMFDbusInterface()
{
}

void KMFDbusInterface::openProject(QString file)
{
  kmfApp->mainWindow()->load(file);
}

void KMFDbusInterface::newProject()
{
  kmfApp->mainWindow()->fileNew();
}

void KMFDbusInterface::quit()
{
  kmfApp->mainWindow()->quit();
}

void KMFDbusInterface::setProjectTitle(const QString& title)
{
  kmfApp->project()->setTitle(title);
}

void KMFDbusInterface::setProjectType(const QString& type)
{
  kmfApp->project()->setType(type);
}

void KMFDbusInterface::setProjectDirectory(const QString& dir)
{
  kmfApp->project()->setDirectory(dir);
}

QString KMFDbusInterface::projectTitle()
{
  return kmfApp->project()->title();
}

QString KMFDbusInterface::projectType()
{
  return kmfApp->project()->type();
}

QString KMFDbusInterface::projectDirectory(const QString& subdir)
{
  return kmfApp->project()->directory(subdir);
}

void KMFDbusInterface::addMediaObject(const QString& xml)
{
  QDomDocument doc;
  doc.setContent(xml);
  kmfApp->project()->mediaObjFromXML(doc.documentElement());
}

void KMFDbusInterface::selectTemplate(const QString& xml)
{
  QDomDocument doc;

  doc.setContent(xml);
  kmfApp->project()->templateFromXML(doc.documentElement());
}

void KMFDbusInterface::selectOutput(const QString& xml)
{
  QDomDocument doc;

  doc.setContent(xml);
  kmfApp->project()->outputFromXML(doc.documentElement());
}

void KMFDbusInterface::start()
{
  kmfApp->mainWindow()->showPage(KMediaFactory::Output);
  kmfApp->mainWindow()->outputPage->start();
}

QString KMFDbusInterface::getOpenFileName(const QString &startDir,
                                          const QString &filter,
                                          const QString &caption)
{
  QString start = QString("kfiledialog:///<%1>").arg(startDir);
  kDebug() << k_funcinfo << start << endl;
  KUrl url = KFileDialog::getOpenFileName(KUrl(start), filter,
      kmfApp->mainWindow(), caption);
  return url.path();
}

void KMFDbusInterface::debug(const QString &txt)
{
  kDebug() << "SCRIPT: " << txt << endl;
}

void KMFDbusInterface::progressDialog(const QString &caption,
                                      const QString &label,
                                      int maximum)
{
  if(m_pdlg)
    delete m_pdlg;
  m_pdlg = new KProgressDialog(kmfApp->mainWindow(), caption, label);
  m_pdlg->progressBar()->setMaximum(maximum);
  m_pdlg->showCancelButton(true);
}

void KMFDbusInterface::pdlgSetMaximum(int maximum)
{
  if(m_pdlg)
    m_pdlg->progressBar()->setMaximum(maximum);
}

void KMFDbusInterface::pdlgSetValue(int value)
{
  if(m_pdlg)
    m_pdlg->progressBar()->setValue(value);
}

void KMFDbusInterface::pdlgSetLabel(const QString &label)
{
  if(m_pdlg)
    m_pdlg->setLabelText(label);
}

void KMFDbusInterface::pdlgShowCancelButton(bool show)
{
  if(m_pdlg)
    m_pdlg->showCancelButton(show);
}

bool KMFDbusInterface::pdlgWasCancelled()
{
  if(m_pdlg)
    return m_pdlg->wasCancelled();
  return false;
}

void KMFDbusInterface::pdlgClose()
{
  if(m_pdlg)
  {
    m_pdlg->close();
    delete m_pdlg;
    m_pdlg = 0;
  }
}

int KMFDbusInterface::message(const QString &caption, const QString &txt,
                              int type)
{
  if(type == KMessageBox::Information)
    KMessageBox::information(kmfApp->mainWindow(), caption, txt);
  else if(type == KMessageBox::Sorry)
    KMessageBox::sorry(kmfApp->mainWindow(), caption, txt);
  else if(type == KMessageBox::Error)
    KMessageBox::error(kmfApp->mainWindow(), caption, txt);
  return 0;
}
