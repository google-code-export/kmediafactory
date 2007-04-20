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
#include <kapplication.h>
#include <kstandarddirs.h>
#include <QStringList>
#include <QFileInfo>
#include <QWidget>

Run::Run(QString command, QString dir)
{
  setCommand(command);
  setWorkingDirectory(dir);
  if(!command.isEmpty())
    run();
}

Run::~Run()
{
}
void Run::setCommand(QString command)
{
  m_command = command;
}

bool Run::run()
{
  QString c = m_command.split(" ")[0];
  if(c[0] != '/')
  {
    QFileInfo file;
    file = KStandardDirs::locate("data", "kmediafactory/scripts/" + c);
    if(file.exists())
    {
      m_command = m_command.replace(0, c.length(), file.filePath());
    }
  }
  setUseShell(true);
  //kDebug() << "Running: " << m_command << endl;
  *this << m_command;
  connect(this, SIGNAL(receivedStdout(K3Process*, char*, int)),
          this, SLOT(stdout(K3Process*, char*, int)));
  connect(this, SIGNAL(receivedStderr(K3Process*, char*, int)),
          this, SLOT(stdout(K3Process*, char*, int)));
  connect(this, SIGNAL(processExited(K3Process*)),
          this, SLOT(exit(K3Process*)));
  setEnvironment("KMF_DBUS",
      QString("org.kde.kmediafactory_%1/KMediaFactory").arg(getpid()));
  setEnvironment("KMF_WINID",
      QString("%1").arg(QApplication::topLevelWidgets()[0]->winId()));

  m_outputIndex = 0;
  start(K3Process::Block, K3Process::AllOutput);
  //kDebug() << "Output: " << m_output << endl;
  if(!normalExit() || exitStatus() != 0)
  {
    return false;
  }
  return true;
}

void Run::stdout(K3Process*, char* buffer, int buflen)
{
  int found;
  QRegExp re("[\n\r]");

  m_output += QString::fromLocal8Bit(buffer, buflen);
  while((found = m_output.indexOf(re, m_outputIndex)) != -1)
  {
    emit line(m_output.mid(m_outputIndex, found - m_outputIndex));
    m_outputIndex = found + 1;
  }
}

void Run::exit(K3Process*)
{
  if(m_outputIndex < m_output.length())
    emit line(m_output.mid(m_outputIndex));

}

#include "run.moc"
