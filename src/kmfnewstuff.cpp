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

#include "kmfnewstuff.h"
#include <kdebug.h>
#include <kapplication.h>
#include <kstandarddirs.h>
#include <ktar.h>
#include <kdesktopfile.h>
#include <qfile.h>
#include <qstringlist.h>

#warning TODO KMFNewStuff

#if 0

KMFNewStuff::KMFNewStuff(const QString& provider, QWidget* parentWidget) :
#warning why this is an error
  //QObject(parentWidget),
  KNewStuff("kmediafactory/tools", provider, parentWidget)
{
}

bool KMFNewStuff::install(const QString& fileName)
{
  //KTar tar("/home/damu/debug.tar.bz2");
  KTar tar(fileName);
  QString destinationPath =
      KStandardDirs::locateLocal("appdata", "tools/", true);
  const KArchiveDirectory* dir;
  QStringList list;

  tar.open(QIODevice::ReadOnly);
  dir = tar.directory();
  list = dir->entries();
  dir->copyTo(destinationPath);
  tar.close();
  QFile::remove(fileName);

  for(QStringList::Iterator it = list.begin(); it != list.end(); ++it)
  {
    kDebug() << k_funcinfo << "File: " << destinationPath << *it << endl;

    if((*it).endsWith(".desktop"))
    {
      KDesktopFile df(destinationPath + *it);
      QString exec = destinationPath + df.readEntry("Exec");
      QString icon = destinationPath + df.readEntry("Icon");

      if(QFile::exists(exec))
      {
        chmod((const char*)exec.toLocal8Bit(),
              S_IRUSR|S_IWRITE|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH);
        df.writeEntry("Exec", exec);
        kDebug() << k_funcinfo << "Exec: " << exec << endl;
      }
      if(QFile::exists(icon))
      {
        df.writeEntry("Icon", icon);
        kDebug() << k_funcinfo << "Icon: " << icon << endl;
      }
    }
  }
  emit scriptInstalled();
  return true;
}

bool KMFNewStuff::createUploadFile(const QString&)
{
  return false;
}

#endif

#include "kmfnewstuff.moc"
