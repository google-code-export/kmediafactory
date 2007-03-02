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
  KProcess process;
  //process.setWorkingDirectory();
  process << m_command;
  connect(&process, SIGNAL(receivedStdout(KProcess*, char*, int)),
          this, SLOT(stdout(KProcess*, char*, int)));
  connect(&process, SIGNAL(receivedStderr(KProcess*, char*, int)),
          this, SLOT(stderr(KProcess*, char*, int)));
  process.start(KProcess::Block, KProcess::AllOutput);
  m_result = process.exitStatus();
  if(!process.normalExit() || process.exitStatus() != 0)
  {
    return false;
  }
  return true;
}

void Run::stdout(KProcess*, char* buffer, int buflen)
{
  m_output += QString::fromLatin1(buffer, buflen);
}

void Run::stderr(KProcess* proc, char* buffer, int buflen)
{
  stdout(proc, buffer, buflen);
}

#include "run.moc"
