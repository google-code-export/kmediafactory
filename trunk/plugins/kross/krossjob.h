//**************************************************************************
//   Copyright (C) 2008 Petri Damsten <damu@iki.fi>
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

#ifndef KROSSJOBOBJECT_H
#define KROSSJOBOBJECT_H

#include <kmediafactory/plugininterface.h>
#include <kross/core/object.h>
#include "objectmapper.h"

class KrossJob : public KMF::Job, public ObjectMapper
{
  Q_OBJECT
  public:
    KrossJob(QObject *parent, Kross::Object::Ptr job);
    ~KrossJob();

    virtual void run();

  private:
    Kross::Object::Ptr m_job;
};

#endif // KROSSJOBOBJECT_H
