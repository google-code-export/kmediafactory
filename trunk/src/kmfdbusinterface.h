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
#ifndef KMFDCOPINTERFACE_H
#define KMFDCOPINTERFACE_H

#include <QObject>

/**
	@author Petri Damsten <petri.damsten@iki.fi>
*/
class KMFDbusInterface : public QObject
{
  public:
    KMFDbusInterface(QObject* parent);
    virtual ~KMFDbusInterface();

    virtual void openProject(QString file);
    virtual void newProject();
    virtual void quit();

    virtual void setProjectTitle(const QString& title);
    virtual void setProjectType(const QString& type);
    virtual void setProjectDirectory(const QString& dir);
    virtual QString projectTitle();
    virtual QString projectType();
    virtual QString projectDirectory(const QString& subdir);

    virtual void addMediaObject(const QString& xml);
    virtual void selectTemplate(const QString& xml);
    virtual void selectOutput(const QString& xml);

    virtual void start();

    // Script helpers
    virtual QString getOpenFileName (const QString &startDir,
                                     const QString &filter,
                                     const QString &caption);
    virtual void debug(const QString &txt);
};

#endif
