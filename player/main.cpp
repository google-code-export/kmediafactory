/***************************************************************************
 *   Copyright (C) 2005 by Petri Damsten   *
 *   petri.damsten@iki.fi   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "kmediafactoryplayer.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kdebug.h>
#include <X11/Xlib.h>

static const char description[] =
    I18N_NOOP("KMediaFactory DVD player");

static const char version[] = VERSION;

static KCmdLineOptions options[] =
{
  { "+[URL]", I18N_NOOP("DVD to open"), 0 },
  KCmdLineLastOption
};

int main(int argc, char **argv)
{
  /*  XInitThreads() should always be the first call in multithreaded X programs
  *  but because of a bug in some xfree versions that can freeze at startup,
  *  we also call XInitThreads() in kxinewidget.cpp
  *
  *  kaffeinepart & konqueror: We call XInitThreads() in kxinewidget.cpp, so
  *  the part is not stable with buggy X implementations.
  */
  kdDebug() << "Main: XInitThreads()" << endl;
  XInitThreads();

  KAboutData about("kmediafactory_player", I18N_NOOP("KMediaFactoryPlayer"),
                   version, description, KAboutData::License_GPL,
                   "(C) 2005 Petri Damsten", 0,
                   0, "petri.damsten@iki.fi");
  about.addAuthor("Petri Damsten", 0, "petri.damsten@iki.fi");
  KCmdLineArgs::init(argc, argv, &about);
  KCmdLineArgs::addCmdLineOptions(options);
  KApplication app;

  KMediaFactoryPlayer* mainWin = 0;

  if (app.isRestored())
  {
    RESTORE(KMediaFactoryPlayer);
  }
  else
  {
    // no session.. just start up normally
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    mainWin = new KMediaFactoryPlayer();
    app.setMainWidget(mainWin);
    mainWin->show();

    args->clear();
  }
  // mainWin has WDestructiveClose flag by default, so it will delete itself.
  return app.exec();
}

