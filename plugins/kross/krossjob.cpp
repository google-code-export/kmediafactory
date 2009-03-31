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

#include "krossjob.h"

KrossJob::KrossJob(QObject* parent, Kross::Object::Ptr job)
 : KMF::Job(parent), m_job(job)
{
}

KrossJob::~KrossJob()
{
}

void KrossJob::run()
{
  if (m_job) {
    m_job->callMethod("run", QVariantList() << qVariantFromValue(qobject_cast<QObject*>(this)));
  }
}

#include "krossjob.moc"
