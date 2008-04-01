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
#ifndef KMFDCOPINTERFACE_H
#define KMFDCOPINTERFACE_H

#include <dcopobject.h>

/**
	@author Petri Damsten <petri.damsten@iki.fi>
*/
class KMFDcopInterface : virtual public DCOPObject
{
    K_DCOP
  public:
    KMFDcopInterface();
    ~KMFDcopInterface();

  k_dcop:
    virtual ASYNC openProject(QString file);
    virtual ASYNC newProject();
    virtual ASYNC quit();

    virtual ASYNC setProjectTitle(QString title);
    virtual ASYNC setProjectType(QString type);
    virtual ASYNC setProjectDirectory(QString dir);
    virtual QString projectTitle();
    virtual QString projectType();
    virtual QString projectDirectory(QString subdir = QString::null);

    virtual ASYNC addMediaObject(QString xml);
    virtual ASYNC selectTemplate(QString xml);
    virtual ASYNC selectOutput(QString xml);

    virtual ASYNC start();
};

#endif
