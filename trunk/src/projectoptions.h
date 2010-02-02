//**************************************************************************
//   Copyright (C) 2004-2008 by Petri Damsten
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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//**************************************************************************

#ifndef PROJECTOPTIONS_H
#define PROJECTOPTIONS_H

#include "ui_projectoptions.h"
#include "kmfproject.h"

class ProjectOptions : public QWidget, public Ui::ProjectOptions
{
    Q_OBJECT
  public:
    ProjectOptions(QWidget* parent = 0);
    ~ProjectOptions();
    void init();
    void setData(const KMFProject& project);

    void setProjectTitle(const QString& title);
    void setProjectType(const QString& type);
    void setProjectAspectRatio(const QString& aspect);
    void setProjectDirectory(const QString& dir);

  protected slots:
    void titleChanged(const QString&);
    void directoryChanged(const QString&);
    void typeChanged(int);
    void aspectChanged(int idx);

  private:
    void paintEvent(QPaintEvent *);

  private:
    bool titleChangesPath;
    QString m_type;
    QString m_saved;
    int m_count;
};

#endif
