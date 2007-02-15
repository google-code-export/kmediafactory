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
#include "uiinterface.h"
#include <kdebug.h>
#include <kprocess.h>

KMF::MediaObject::MediaObject(QObject* parent) :
  Object(parent)
{
}

KMF::TemplateObject::TemplateObject(QObject *parent) :
  Object(parent)
{
}

KMF::OutputObject::OutputObject(QObject *parent) :
  Object(parent)
{
}

KMF::UiInterface::UiInterface(QObject* parent) :
  QObject(parent)
{
  setObjectName("KMF::UiInterface");
}

void KMF::Logger::connectProcess(KProcess *proc, const QString& filter,
                                 KProcess::Communication comm)
{
  if(comm && KProcess::Stdout)
    connect(proc, SIGNAL(receivedStdout(KProcess*, char*, int)),
            this, SLOT(stdout(KProcess*, char*, int)));
  if(comm && KProcess::Stderr)
    connect(proc, SIGNAL(receivedStderr(KProcess*, char*, int)),
            this, SLOT(stderr(KProcess*, char*, int)));
  setFilter(filter);
}

#include "uiinterface.moc"