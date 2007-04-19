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
#include <Q3PtrList>

KMFProjectInterface::KMFProjectInterface(QObject *parent) :
  KMF::ProjectInterface(parent)
{
}

KMFProjectInterface::~KMFProjectInterface()
{
}

QString KMFProjectInterface::title()
{
  return kmfApp->project()->title();
}

void KMFProjectInterface::setTitle(QString title)
{
  return kmfApp->project()->setTitle(title);
}

QList<KMF::MediaObject*> KMFProjectInterface::mediaObjects()
{
  return kmfApp->project()->mediaObjects()->list();
}

QString KMFProjectInterface::projectDir(const QString& subDir)
{
  return kmfApp->project()->directory(subDir);
}

void KMFProjectInterface::cleanFiles(const QString& subDir,
                                     const QStringList& files)
{
  kmfApp->project()->cleanFiles(subDir, files);
}

void KMFProjectInterface::setDirty(KMF::ProjectInterface::DirtyType type)
{
  kmfApp->project()->setDirty(type);
}

QString KMFProjectInterface::type()
{
  return kmfApp->project()->type();
}

QString KMFProjectInterface::lastSubType()
{
  return kmfApp->project()->lastSubType();
}

QDateTime KMFProjectInterface::lastModified(
    KMF::ProjectInterface::DirtyType type)
{
  return kmfApp->project()->lastModified(type);
}

int KMFProjectInterface::serial()
{
  return kmfApp->project()->serial();
}

#include "kmfprojectinterface.moc"
