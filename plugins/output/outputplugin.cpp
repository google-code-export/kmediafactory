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
#include <krun.h>
#include <QRegExp>
#include <QPixmap>

#ifdef HAVE_LIBDVDREAD
#include "dvdinfo.h"
#endif

K_EXPORT_KMEDIAFACTORY_PLUGIN(output, OutputPlugin);

OutputPlugin::OutputPlugin(QObject *parent, const QVariantList&) :
  KMF::Plugin(parent), addPreviewDVDXine(0), addPreviewDVDKaffeine(0)
{
  setObjectName("KMFOutput");
  // Initialize GUI
  setXMLFile("kmediafactory_outputui.rc");

  m_xine = KStandardDirs::findExe("xine");
  m_kaffeine = KStandardDirs::findExe("kaffeine");

#ifdef HAVE_LIBDVDREAD
  dvdInfo =new KAction(KIcon("zoom-original"), i18n("DVD Info"), parent);
  dvdInfo->setShortcut(Qt::CTRL + Qt::Key_I);
  actionCollection()->addAction("dvd_info", dvdInfo);
  connect(dvdInfo, SIGNAL(triggered()), SLOT(slotDVDInfo()));
#endif
  if(!m_xine.isEmpty())
  {
    addPreviewDVDXine =new KAction(KIcon("xine"),
                                   i18n("Preview DVD in Xine"), parent);
    addPreviewDVDXine->setShortcut(Qt::CTRL + Qt::Key_X);
    actionCollection()->addAction("preview_dvd_xine", addPreviewDVDXine);
    connect(addPreviewDVDXine, SIGNAL(triggered()), SLOT(slotPreviewDVDXine()));
  }
  if(!m_kaffeine.isEmpty())
  {
    addPreviewDVDKaffeine =new KAction(KIcon("xine"),
                                   i18n("Preview DVD in Kaffeine"), parent);
    addPreviewDVDKaffeine->setShortcut(Qt::CTRL + Qt::Key_K);
    actionCollection()->addAction("preview_dvd_kaffeine",
                                  addPreviewDVDKaffeine);
    connect(addPreviewDVDKaffeine, SIGNAL(triggered()),
            SLOT(slotPreviewDVDKaffeine()));
  }
}

void OutputPlugin::init(const QString &type)
{
  kDebug() << type;
  deleteChildren();
  if (type.left(3) == "DVD")
  {
    KMF::PluginInterface *ui = interface();
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
  QString cmd;
  QString projectDir = interface()->projectDir();

  if(player.isEmpty())
  {
    if(!m_xine.isEmpty())
      cmd = m_xine;
    else if(!m_kaffeine.isEmpty())
      cmd = m_kaffeine;
    else
      return;
  }
  else
    cmd = player;
  cmd +=  " dvd:/" + projectDir + "DVD/VIDEO_TS";
  KRun::runCommand(cmd, kapp->activeWindow());
}

void OutputPlugin::slotPreviewDVDXine()
{
  play(m_xine);
}

void OutputPlugin::slotPreviewDVDKaffeine()
{
  play(m_kaffeine);
}

void OutputPlugin::slotDVDInfo()
{
#ifdef HAVE_LIBDVDREAD
  QString projectDir = interface()->projectDir();
  DVDInfo dlg(kapp->activeWindow(), projectDir + "DVD/");

  dlg.exec();
#endif
}

QStringList OutputPlugin::supportedProjectTypes() const
{
  QStringList result;
  result << "DVD-PAL" << "DVD-NTSC";
  return result;
}

#include "outputplugin.moc"
