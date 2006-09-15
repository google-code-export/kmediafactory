//**************************************************************************
//   Copyright (C) 2004 by Petri Damst�
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
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kdebug.h>

static const char description[] =
  I18N_NOOP("Template based DVD authoring software.");

static const char version[] = VERSION;

static KCmdLineOptions options[] =
{
  { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
  KCmdLineLastOption
};

int main(int argc, char **argv)
{
  KAboutData about(PACKAGE, I18N_NOOP("KMediaFactory"), version,
    description, KAboutData::License_GPL, "(C) 2005 Petri Damsten", 0,
    0, "petri.damsten@iki.fi");
  about.addAuthor("Petri Damsten", 0, "petri.damsten@iki.fi");
  KCmdLineArgs::init(argc, argv, &about);
  KCmdLineArgs::addCmdLineOptions(options);
  KMFApplication app;

  KGlobal::locale()->insertCatalog("libkmf");
  if (qApp->isSessionRestored())
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

