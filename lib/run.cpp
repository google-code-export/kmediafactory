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
#include "run.h"
#include <kdebug.h>
#include <kstandarddirs.h>
#include <qstringlist.h>
#include <qfileinfo.h>

Run::Run(QString command)
{
  m_command = command;
  if(!command.isEmpty())
    run();
}

Run::~Run()
{
}

bool Run::run()
{
  K3Process process;
  //process.setWorkingDirectory();
  QString c = m_command.split(" ")[0];
  if(c[0] != '/')
  {
    QFileInfo file;
    file = KStandardDirs::locate("data", "kmediafactory/scripts/" + c);
    if(file.exists())
    {
      process.setUseShell(true);
      m_command = m_command.replace(0, c.length(), file.filePath());
    }
  }
  //kdDebug() << "Running: " << m_command << endl;
  process << m_command;
  connect(&process, SIGNAL(receivedStdout(K3Process*, char*, int)),
          this, SLOT(stdout(K3Process*, char*, int)));
  connect(&process, SIGNAL(receivedStderr(K3Process*, char*, int)),
          this, SLOT(stderr(K3Process*, char*, int)));
  process.start(K3Process::Block, K3Process::AllOutput);
  m_result = process.exitStatus();
  //kdDebug() << "Output: " << m_output << endl;
  if(!process.normalExit() || process.exitStatus() != 0)
  {
    return false;
  }
  return true;
}

void Run::stdout(K3Process*, char* buffer, int buflen)
{
  m_output += QString::fromLatin1(buffer, buflen);
}

void Run::stderr(K3Process* proc, char* buffer, int buflen)
{
  stdout(proc, buffer, buflen);
}

#include "run.moc"
