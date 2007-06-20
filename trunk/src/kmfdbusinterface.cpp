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
  KUrl url = KFileDialog::getOpenFileName(
      KUrl(QString("kfiledialog:///<%1>").arg(startDir)), filter,
      kmfApp->mainWindow(), caption);
  return url.path();
}

QString KMFDbusInterface::debug(const QString &txt)
{
  kDebug() << "SCRIPT: " << txt << endl;
}
