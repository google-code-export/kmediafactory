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
#ifndef LOGVIEW_H
#define LOGVIEW_H

#include "ui_logview.h"
#include <kmediafactory/logger.h>
#include <QVBoxLayout>

class QVBoxLayout;
class KHTMLPart;

/**
@author Petri Damsten
*/
class LogView : public KDialog, public Ui::LogView
{
    Q_OBJECT
  public:
    LogView(QWidget *parent = 0);
    virtual ~LogView();
    void setData(const KUrl& log);

protected:
    void closeEvent(QCloseEvent *);

  private:
    QVBoxLayout* m_layout;
    KHTMLPart* m_htmlPart;
};

#endif
