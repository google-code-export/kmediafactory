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
  QObject(parent)
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
  kmfApp->uiInterface()->addMediaObject(xml);
}

void KMFDbusInterface::selectTemplate(const QString& xml)
{
  kmfApp->uiInterface()->selectTemplate(xml);
}

void KMFDbusInterface::selectOutput(const QString& xml)
{
  kmfApp->uiInterface()->selectOutput(xml);
}

void KMFDbusInterface::start()
{
  kmfApp->mainWindow()->showPage(KMediaFactory::Output);
  kmfApp->mainWindow()->outputPage->start();
}

QStringList KMFDbusInterface::getOpenFileNames(const QString &startDir,
                                               const QString &filter,
                                               const QString &caption)
{
  return kmfApp->uiInterface()->getOpenFileNames(startDir, filter, caption);
}

void KMFDbusInterface::debug(const QString &txt)
{
  kmfApp->uiInterface()->debug(txt);
}

void KMFDbusInterface::progressDialog(const QString &caption,
                                      const QString &label,
                                      int maximum)
{
  kmfApp->uiInterface()->progressDialog(caption, label, maximum);
}

void KMFDbusInterface::pdlgSetMaximum(int maximum)
{
  kmfApp->uiInterface()->progressDialog()->setMaximum(maximum);
}

void KMFDbusInterface::pdlgSetValue(int value)
{
  kmfApp->uiInterface()->progressDialog()->setValue(value);
}

void KMFDbusInterface::pdlgSetLabel(const QString &label)
{
  kmfApp->uiInterface()->progressDialog()->setLabel(label);
}

void KMFDbusInterface::pdlgShowCancelButton(bool show)
{
  kmfApp->uiInterface()->progressDialog()->showCancelButton(show);
}

bool KMFDbusInterface::pdlgWasCancelled()
{
  return kmfApp->uiInterface()->progressDialog()->wasCancelled();
}

void KMFDbusInterface::pdlgClose()
{
  kmfApp->uiInterface()->progressDialog()->close();
}

int KMFDbusInterface::message(const QString &caption, const QString &txt,
                              int type)
{
  return kmfApp->uiInterface()->messageBox(caption, txt, type);
}
