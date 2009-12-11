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

class ProjectOptions : public KDialog, public Ui::ProjectOptions
{
    Q_OBJECT
  public:
    ProjectOptions(QWidget* parent = 0);
    ~ProjectOptions();
    void getData(KMFProject& project) const;
    void setData(const KMFProject& project);

  protected slots:
    void textChanged(const QString&);
    void slotOk();
    virtual void slotButtonClicked(int button);

  private:
    bool titleChangesPath;
    QString m_type;
    QString m_saved;
    int m_count;
};

#endif