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
#include <k3process.h>

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

void KMF::Logger::connectProcess(K3Process *proc, const QString& filter,
                                 K3Process::Communication comm)
{
  if(comm && K3Process::Stdout)
    connect(proc, SIGNAL(receivedStdout(K3Process*, char*, int)),
            this, SLOT(stdout(K3Process*, char*, int)));
  if(comm && K3Process::Stderr)
    connect(proc, SIGNAL(receivedStderr(K3Process*, char*, int)),
            this, SLOT(stderr(K3Process*, char*, int)));
  setFilter(filter);
}

#include "uiinterface.moc"
