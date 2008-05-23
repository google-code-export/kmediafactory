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

#include "kmediafactory.h"
#include "kmfapplication.h"
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <KDebug>

int main(int argc, char **argv)
{
  KAboutData about(PACKAGE, 0,
                   ki18n("KMediaFactory"), VERSION,
                   ki18n("Template based DVD authoring software."),
                   KAboutData::License_GPL,
                   ki18n(COPYRIGHT), KLocalizedString(),
                   HOMEPAGE, BUG_EMAIL);
  about.addAuthor(ki18n("Petri Damsten"), ki18n("Developer"),
                  "petri.damsten@iki.fi");
  about.setOrganizationDomain("kde.org");

  KCmdLineOptions options;
  options.add("+[URL]", ki18n("Document to open"));
  KCmdLineArgs::init(argc, argv, &about);
  KCmdLineArgs::addCmdLineOptions(options);
  KMFApplication app;

  // Add catalog for translations
  KGlobal::locale()->insertCatalog("libkmf");
  KGlobal::locale()->insertCatalog("kmediafactory_kstore");
  // Add resource dirs
  QStringList tools = KGlobal::dirs()->findDirs("data", "kmediafactory/tools");
  foreach(QString tooldir, tools)
  {
    kDebug() << "Adding resource dir: " << tooldir;
    // For icons in tool scripts
    KGlobal::dirs()->addResourceDir("icon", tooldir);
    // For full script plugins
    KGlobal::dirs()->addResourceDir("data", tooldir);
  }
  //KIconLoader::global()->reconfigure(about.appName(), KGlobal::dirs());

  if (app.isSessionRestored())
  {
    RESTORE(KMediaFactory);
  }
  else
  {
    // no session.. just start up normally
    app.init();
  }
  // mainWin has WDestructiveClose flag by default, so it will delete itself.
  return app.exec();
}

