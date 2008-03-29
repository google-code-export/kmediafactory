//**************************************************************************
//   Copyright (C) 2004, 2005 by Petri Damstén
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
#include "kmfdcopinterface.h"
#include "kmfapplication.h"
#include "kmediafactory.h"
#include "outputpage.h"
#include <kdebug.h>
#include <kjanuswidget.h>

KMFDcopInterface::KMFDcopInterface()
  : DCOPObject("KMediaFactoryIface")
{
}

KMFDcopInterface::~KMFDcopInterface()
{
}

void KMFDcopInterface::openProject(QString file)
{
  kmfApp->mainWindow()->load(file);
}

void KMFDcopInterface::newProject()
{
  kmfApp->mainWindow()->fileNew();
}

void KMFDcopInterface::quit()
{
  kmfApp->mainWindow()->quit();
}

void KMFDcopInterface::setProjectTitle(QString title)
{
  kmfApp->project()->setTitle(title);
}

void KMFDcopInterface::setProjectType(QString type)
{
  kmfApp->project()->setType(type);
}

void KMFDcopInterface::setProjectDirectory(QString dir)
{
  kmfApp->project()->setDirectory(dir);
}

QString KMFDcopInterface::projectTitle()
{
  return kmfApp->project()->title();
}

QString KMFDcopInterface::projectType()
{
  return kmfApp->project()->type();
}

QString KMFDcopInterface::projectDirectory(QString subdir)
{
  return kmfApp->project()->directory(subdir);
}

void KMFDcopInterface::addMediaObject(QString xml)
{
  QDomDocument doc;

  doc.setContent(xml);
  kmfApp->project()->mediaObjFromXML(doc.documentElement());
}

void KMFDcopInterface::selectTemplate(QString xml)
{
  QDomDocument doc;

  doc.setContent(xml);
  kmfApp->project()->templateFromXML(doc.documentElement());
}

void KMFDcopInterface::selectOutput(QString xml)
{
  QDomDocument doc;

  doc.setContent(xml);
  kmfApp->project()->outputFromXML(doc.documentElement());
}

void KMFDcopInterface::start()
{
  kmfApp->mainWindow()->showPage(KMediaFactory::Output);
  kmfApp->mainWindow()->outputPage->start();
}
