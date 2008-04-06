//**************************************************************************
//   Copyright (C) 2008 by Petri Damsten <damu@iki.fi>
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

#include "krossprojectinterface.h"
#include <KDebug>

KrossProjectInterface::KrossProjectInterface(QObject *parent, KMF::ProjectInterface* projectIf)
 : QObject(parent), m_projectIf(projectIf)
{
}

KrossProjectInterface::~KrossProjectInterface()
{
}

QList<MediaObject*> KrossProjectInterface::mediaObjects()
{
  return m_projectIf->mediaObjects();
}

QString KrossProjectInterface::title()
{
  return m_projectIf->title();
}

void KrossProjectInterface::setTitle(QString title)
{
  m_projectIf->setTitle(title);
}

QString KrossProjectInterface::projectDir(const QString& subDir)
{
  return m_projectIf->projectDir(subDir);
}

void KrossProjectInterface::cleanFiles(const QString& subDir, const QStringList& files)
{
  return m_projectIf->cleanFiles(subDir, files);
}

void KrossProjectInterface::setDirty(KMF::ProjectInterface::DirtyType type)
{
  return m_projectIf->setDirty(type);
}

QString KrossProjectInterface::type()
{
  return m_projectIf->type();
}

QString KrossProjectInterface::lastSubType()
{
  return m_projectIf->lastSubType();
}

QDateTime KrossProjectInterface::lastModified(KMF::ProjectInterface::DirtyType type)
{
  return m_projectIf->lastModified(type);
}

int KrossProjectInterface::serial()
{
  return m_projectIf->serial();
}

#include "krossprojectinterface.moc"
