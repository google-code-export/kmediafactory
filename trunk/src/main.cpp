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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//**************************************************************************

#include "kmediafactory.h"
#include "kmfapplication.h"
#include "svnrev.h"
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <KDebug>
#include <KMessageBox>
#include <QMap>

int main(int argc, char **argv)
{
  KAboutData about(PACKAGE, 0,
                   ki18n("KMediaFactory"), VERSION SVNREV,
                   ki18n("Template based DVD authoring software."),
                   KAboutData::License_GPL,
                   ki18n(COPYRIGHT), KLocalizedString(),
                   HOMEPAGE, BUG_EMAIL);
  about.addAuthor(ki18n("Petri Damsten"), ki18n("Developer"),
                  "petri.damsten@iki.fi");
  about.addAuthor(ki18n("Craig Drummond"), ki18n("Developer"),
                  "craig.p.drummond@googlemail.com");
  about.setOrganizationDomain("kde.org");

  KCmdLineOptions options;
  options.add("+[URL]", ki18n("Document to open"));
  KCmdLineArgs::init(argc, argv, &about);
  KCmdLineArgs::addCmdLineOptions(options);
  KMFApplication app;

  QMap<QString, QStringList> requiredApps,
                             missingApps;
 
  requiredApps[i18n("DVD Author")] << "dvdauthor" << "spumux";
  requiredApps[i18n("FFmpeg")] << "ffmpeg";
  requiredApps[i18n("MJPEG Tools")] << "mplex" << "ppmtoy4m" << "mpeg2enc";

  QMap<QString, QStringList>::iterator it(requiredApps.begin()),
                                       end(requiredApps.end());

  for(; it!=end; ++it)
  {
    QStringList::iterator app(it.value().begin()),
                          aEnd(it.value().end());

    for(; app!=aEnd; ++app)
      if(KStandardDirs::findExe(*app).isEmpty())
        missingApps[it.key()].append(*app);
  }

  if(!missingApps.isEmpty())
  {
    QStringList apps;
    it=missingApps.begin();
    end=missingApps.end();

    for(; it!=end; ++it)
      if(it.value().count()==1 && 0==(*(it.value().begin())).compare(QString(it.key()).replace(" ", ""), Qt::CaseInsensitive))
        apps.append(it.key());
      else
        apps.append(it.key()+" ("+it.value().join(", ")+')');

    if(apps.count()>1)
      KMessageBox::errorList(0, i18n("<p>KMediaFactory uses the following packages, and these have not been "
                                     "found on your system.</p>"
                                     "<p>You must install them before KMediaFactory can continue.</p>"), apps);
    else
      KMessageBox::error(0, i18n("<p>KMediaFactory uses \"%1\", and this has not been found on your system.</p><p>"
                                 "You must install this before KMediaFactory can continue.</p>", *(apps.begin())));
    return -1;
  }

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

