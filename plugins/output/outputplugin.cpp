//**************************************************************************
//   Copyright (C) 2004 by Petri Damstén
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
#include "config.h"
#include "outputplugin.h"
#include "dvdauthorobject.h"
#include "dvddirectoryobject.h"
#ifdef HAVE_LIBDVDREAD
#include "dvdinfo.h"
#endif
#include "k3bobject.h"
#include <kdeversion.h>
#include <kapplication.h>
#include <kgenericfactory.h>
#include <kaction.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kaboutdata.h>
#include <kstandarddirs.h>
#include <qregexp.h>
#include <qpixmap.h>

static const char description[] =
  I18N_NOOP("Output plugin for KMediaFactory.");
static const char version[] = VERSION;
static const KAboutData about("kmediafactory_output",
                              I18N_NOOP("KMediaFactory Output"),
                              version, description, KAboutData::License_GPL,
                              "(C) 2005 Petri Damsten", 0, 0,
                              "petri.damsten@iki.fi");

typedef KGenericFactory<OutputPlugin> outputFactory;
#if KDE_IS_VERSION(3, 3, 0)
K_EXPORT_COMPONENT_FACTORY(kmediafactory_output, outputFactory(&about))
#else
K_EXPORT_COMPONENT_FACTORY(kmediafactory_output, outputFactory(about.appName()))
#endif

OutputPlugin::OutputPlugin(QObject *parent,
                           const char* name, const QStringList&) :
  KMF::Plugin(parent, name ), addPreviewDVD(0),
  addPreviewDVDXine(0), addPreviewDVDKaffeine(0)
{
  // Initialize GUI
  setInstance(KGenericFactory<OutputPlugin>::instance());
  setXMLFile("kmediafactory_outputui.rc");

  m_kmfplayer = KStandardDirs::findExe("kmediafactoryplayer");
  m_xine = KStandardDirs::findExe("xine");
  m_kaffeine = KStandardDirs::findExe("kaffeine");

#ifdef HAVE_LIBDVDREAD
  dvdInfo = new KAction(i18n("DVD Info"), "viewmag",
                        CTRL+Key_I,
                        this, SLOT(slotDVDInfo()),
                        actionCollection(),
                        "dvd_info");
#endif
  if(!m_kmfplayer.isEmpty())
    addPreviewDVD = new KAction(i18n("Preview DVD"), "viewmag",
                                CTRL+Key_P,
                                this, SLOT(slotPreviewDVD()),
                                actionCollection(),
                                "preview_dvd");
  if(!m_xine.isEmpty())
    addPreviewDVDXine = new KAction(i18n("Preview DVD in Xine"), "xine",
                                    CTRL+Key_X,
                                    this, SLOT(slotPreviewDVDXine()),
                                    actionCollection(),
                                    "preview_dvd_xine");
  if(!m_kaffeine.isEmpty())
    addPreviewDVDKaffeine = new KAction(i18n("Preview DVD in Kaffeine"),
                                        "kaffeine",
                                        CTRL+Key_K,
                                        this, SLOT(slotPreviewDVDKaffeine()),
                                        actionCollection(),
                                        "preview_dvd_kaffeine");
}

void OutputPlugin::init(const QString &type)
{
  deleteChildren();
  if (type.left(3) == "DVD")
  {
    KMF::UiInterface *ui = uiInterface();
    if(ui)
    {
      DvdAuthorObject *daob = new DvdAuthorObject(this);
      ui->addOutputObject(daob);
      DvdDirectoryObject *ddob = new DvdDirectoryObject(this);
      ui->addOutputObject(ddob);
      DvdAuthorObject *k3bob = new K3bObject(this);
      ui->addOutputObject(k3bob);
    }
  }
}

void OutputPlugin::play(const QString& player)
{
  QString bin;
  QString projectDir = projectInterface()->projectDir();
  KProcess process;

  if(player.isEmpty())
  {
    if(!m_kmfplayer.isEmpty())
      bin = m_kmfplayer;
    else if(!m_xine.isEmpty())
      bin = m_xine;
    else if(!m_kaffeine.isEmpty())
      bin = m_kaffeine;
    else
      return;
  }
  else
    bin = player;
  process << bin << "dvd:/" + projectDir + "DVD/VIDEO_TS";
  process.start(KProcess::DontCare);
}

void OutputPlugin::slotPreviewDVDXine()
{
  play(m_xine);
}

void OutputPlugin::slotPreviewDVDKaffeine()
{
  play(m_kaffeine);
}

void OutputPlugin::slotPreviewDVD()
{
  play(m_kmfplayer);
}

void OutputPlugin::slotDVDInfo()
{
#ifdef HAVE_LIBDVDREAD
  QString projectDir = projectInterface()->projectDir();
  DVDInfo dlg(kapp->activeWindow(), "", projectDir + "/DVD/");

  dlg.exec();
#endif
}

QStringList OutputPlugin::supportedProjectTypes()
{
  QStringList result;
  result << "DVD-PAL" << "DVD-NTSC";
  return result;
}

#include "outputplugin.moc"
