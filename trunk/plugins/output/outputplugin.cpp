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
#include "config.h"
#include "outputplugin.h"
#include "dvddirectoryobject.h"
#include "dvdauthorobject.h"
#include "k3bobject.h"
#include <kactioncollection.h>
#include <kdeversion.h>
#include <kapplication.h>
#include <kgenericfactory.h>
#include <kaction.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kaboutdata.h>
#include <kstandarddirs.h>
#include <kicon.h>
#include <QRegExp>
#include <QPixmap>

#ifdef HAVE_LIBDVDREAD
#include "dvdinfo.h"
#endif

static const char description[] =
  I18N_NOOP("Output plugin for KMediaFactory.");
static const char version[] = VERSION;
static const KAboutData about("kmediafactory_output",
                              I18N_NOOP("KMediaFactory Output"),
                              version, description, KAboutData::License_GPL,
                              "(C) 2005 Petri Damsten", 0, 0,
                              "petri.damsten@iki.fi");

typedef KGenericFactory<OutputPlugin> outputFactory;
K_EXPORT_COMPONENT_FACTORY(kmediafactory_output, outputFactory(&about))

OutputPlugin::OutputPlugin(QObject *parent, const QStringList&) :
  KMF::Plugin(parent), addPreviewDVD(0),
  addPreviewDVDXine(0), addPreviewDVDKaffeine(0)
{
  setObjectName("KMFOutput");
  // Initialize GUI
#warning TODO
  //setInstance(KGenericFactory<OutputPlugin>::instance());
  setXMLFile("kmediafactory_outputui.rc");

  m_kmfplayer = KStandardDirs::findExe("kmediafactoryplayer");
  m_xine = KStandardDirs::findExe("xine");
  m_kaffeine = KStandardDirs::findExe("kaffeine");

#ifdef HAVE_LIBDVDREAD
  dvdInfo =new KAction(KIcon("viewmag"), i18n("DVD Info"),this);
  dvdInfo->setShortcut(Qt::CTRL + Qt::Key_I);
  actionCollection()->addAction("dvd_info", dvdInfo);
  connect(dvdInfo, SIGNAL(triggered()), SLOT(slotDVDInfo()));
#endif
  if(!m_kmfplayer.isEmpty())
  {
    addPreviewDVD =new KAction(KIcon("viewmag"), i18n("Preview DVD"),this);
    addPreviewDVD->setShortcut(Qt::CTRL + Qt::Key_P);
    actionCollection()->addAction("preview_dvd", addPreviewDVD);
    connect(addPreviewDVD, SIGNAL(triggered()), SLOT(slotPreviewDVD()));
  }
  if(!m_xine.isEmpty())
  {
    addPreviewDVDXine =new KAction(KIcon("xine"),
                                   i18n("Preview DVD in Xine"),this);
    addPreviewDVDXine->setShortcut(Qt::CTRL + Qt::Key_X);
    actionCollection()->addAction("preview_dvd_xine", addPreviewDVDXine);
    connect(addPreviewDVDXine, SIGNAL(triggered()), SLOT(slotPreviewDVDXine()));
  }
  if(!m_kaffeine.isEmpty())
  {
    addPreviewDVDKaffeine =new KAction(KIcon("xine"),
                                   i18n("Preview DVD in Kaffeine"),this);
    addPreviewDVDKaffeine->setShortcut(Qt::CTRL + Qt::Key_K);
    actionCollection()->addAction("preview_dvd_kaffeine",
                                  addPreviewDVDKaffeine);
    connect(addPreviewDVDKaffeine, SIGNAL(triggered()),
            SLOT(slotPreviewDVDKaffeine()));
  }
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
