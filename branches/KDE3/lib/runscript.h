//**************************************************************************
//   Copyright (C) 2006 by Petri Damsten
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
#ifndef RUNSCRIPT_H
#define RUNSCRIPT_H

#include <qobject.h>
#include <qstring.h>
#include <kprocess.h>

/**
	@author Petri Damsten <petri.damsten@iki.fi>
*/
class Script : QObject
{
    Q_OBJECT
  public:
    Script(QString script);
    ~Script();

    bool run(QString arguments = "");
    QString output() { return m_output; };
    int result() { return m_result; };

  public slots:
    virtual void stdout(KProcess *proc, char *buffer, int buflen);
    virtual void stderr(KProcess *proc, char *buffer, int buflen);

  private:
    QString m_script;
    QString m_output;
    int m_result;
};

#endif
