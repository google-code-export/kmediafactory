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
#include "kmfprojectinterface.h"
#include "kmediafactory.h"
#include "templatepage.h"
#include "outputpage.h"
#include "kmficonview.h"
#include "kmfapplication.h"
#include "kmfproject.h"
#include <kdebug.h>

KMFProjectInterface::KMFProjectInterface(QObject *parent) :
  KMF::ProjectInterface(parent)
{
}

KMFProjectInterface::~KMFProjectInterface()
{
}

QString KMFProjectInterface::title()
{
  if (kmfApp->project()) {
    return kmfApp->project()->title();
  }
  return QString();
}

void KMFProjectInterface::setTitle(QString title)
{
  if (kmfApp->project()) {
    kmfApp->project()->setTitle(title);
  }
}

QList<KMF::MediaObject*> KMFProjectInterface::mediaObjects()
{
  if (kmfApp->project()) {
    return kmfApp->project()->mediaObjects()->list();
  }
  return QList<KMF::MediaObject*>();
}

KMF::TemplateObject* KMFProjectInterface::templateObject()
{
  if (kmfApp->project()) {
    return kmfApp->project()->templateObj();
  }
  return 0;
}

KMF::OutputObject* KMFProjectInterface::outputObject()
{
  if (kmfApp->project()) {
    return kmfApp->project()->output();
  }
  return 0;
}

QString KMFProjectInterface::projectDir(const QString& subDir)
{
  if (kmfApp->project()) {
    return kmfApp->project()->directory(subDir);
  }
  return QString();
}

void KMFProjectInterface::cleanFiles(const QString& subDir,
                                     const QStringList& files)
{
  if (kmfApp->project()) {
    kmfApp->project()->cleanFiles(subDir, files);
  }
}

void KMFProjectInterface::setDirty(KMF::ProjectInterface::DirtyType type)
{
  if (kmfApp->project()) {
    kmfApp->project()->setDirty(type);
  }
}

QString KMFProjectInterface::type()
{
  if (kmfApp->project()) {
    return kmfApp->project()->type();
  }
  return QString();
}

QString KMFProjectInterface::lastSubType()
{
  if (kmfApp->project()) {
    return kmfApp->project()->lastSubType();
  }
  return QString();
}

QDateTime KMFProjectInterface::lastModified(
    KMF::ProjectInterface::DirtyType type)
{
  if (kmfApp->project()) {
    return kmfApp->project()->lastModified(type);
  }
  return QDateTime();
}

int KMFProjectInterface::serial()
{
  if (kmfApp->project()) {
    return kmfApp->project()->serial();
  }
  return -1;
}

#include "kmfprojectinterface.moc"
