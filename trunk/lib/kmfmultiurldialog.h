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
#ifndef KMFMULTIURLDIALOG_H
#define KMFMULTIURLDIALOG_H

#include "ui_kmfmultiurldialog.h"
#include <QStringListModel>

/**
	@author Petri Damsten <petri.damsten@iki.fi>
*/

class QStringList;

class KMFMultiURLDialog : public KDialog, public Ui::KMFMultiURLDialog
{
    Q_OBJECT
  public:
    KMFMultiURLDialog(const QString& startDir, const QString& filter,
                      QWidget* parent, const QString& title);
    ~KMFMultiURLDialog();

  public:
    void addFiles(const QStringList& files);
    QStringList files();

  protected slots:
    virtual void moveUp();
    virtual void moveDown();
    virtual void add();
    virtual void remove();

  private:
    QString m_dir;
    QString m_filter;
    QStringListModel m_model;
};

#endif
