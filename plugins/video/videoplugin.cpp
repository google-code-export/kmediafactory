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

#include "videoplugin.h"
#include "videoobject.h"
#include "videopluginsettings.h"
#include "videoconfiglayout.h"
//#include <avcodec.h>
#include <kgenericfactory.h>
#include <kdeversion.h>
#include <kaction.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kaboutdata.h>
#include <kapplication.h>
#include <qregexp.h>
#include <qpixmap.h>
#include <qcheckbox.h>
#include <kmessagebox.h>

static const char description[] =
  I18N_NOOP("Video plugin for KMediaFactory.");
static const char version[] = VERSION;
static const KAboutData about("kmediafactory_video",
                              I18N_NOOP("KMediaFactory Video"),
                              version, description, KAboutData::License_GPL,
                              "(C) 2005 Petri Damsten", 0, 0,
                              "petri.damsten@iki.fi");

typedef KGenericFactory<VideoPlugin> videoFactory;
#if KDE_IS_VERSION(3, 3, 0)
K_EXPORT_COMPONENT_FACTORY(kmediafactory_video, videoFactory(&about))
#else
K_EXPORT_COMPONENT_FACTORY(kmediafactory_video, videoFactory(about.appName()))
#endif

VideoPlugin::VideoPlugin(QObject *parent,
                         const char* name, const QStringList&) :
  KMF::Plugin(parent, name )
{
  // Initialize GUI
  setInstance(KGenericFactory<VideoPlugin>::instance());
  setXMLFile("kmediafactory_videoui.rc");
  // Add action for menu item
  addVideoAction = new KAction(i18n("Add Video"), "video", CTRL+Key_V,
                               this, SLOT(slotAddVideo()), actionCollection(),
                               "video");
}

VideoPlugin::~VideoPlugin()
{
}

const KMF::ConfigPage* VideoPlugin::configPage() const
{
  KMF::ConfigPage* configPage = new KMF::ConfigPage;
  configPage->page = new ConfigureVideoPluginLayout;
  configPage->config = VideoPluginSettings::self();
  configPage->itemName = i18n("Video plugin");
  configPage->pixmapName = "video";
  return configPage;
}

void VideoPlugin::init(const QString &type)
{
  deleteChildren();
  if (type.left(3) == "DVD")
  {
    addVideoAction->setEnabled(true);
  }
  else
  {
    addVideoAction->setEnabled(false);
  }
}

void VideoPlugin::slotAddVideo()
{
  QCheckBox* multipleFiles = new QCheckBox(0, "multipleFiles");
  KFileDialog dlg(":AddVideo",
                  "*.mpg *.mpeg *.vob *.avi *.mov|Video files\n*.*|All files",
                  kapp->mainWidget(), "filedialog", true, multipleFiles);

  multipleFiles->setText(i18n("Multiple files make multiple titles."));
  multipleFiles->setChecked(true);
  dlg.setOperationMode(KFileDialog::Opening);
  dlg.setCaption(i18n("Open"));
  dlg.setMode(KFile::Files | KFile::ExistingOnly | KFile::LocalOnly);
  dlg.exec();

  QStringList filenames = dlg.selectedFiles();
  KMF::UiInterface *m = uiInterface();

  if(m && filenames.count() > 0)
  {
    VideoObject *vob = 0;
    for(QStringList::ConstIterator filename = filenames.begin();
        filename != filenames.end(); ++filename)
    {
      QFileInfo fi(*filename);

      if(fi.isDir())
      {
        KMessageBox::error(kapp->activeWindow(),
                           i18n("Cannot add directory."));
        continue;
      }
      if(multipleFiles->isChecked() || filename == filenames.begin())
        vob = new VideoObject(this);
      if(!vob->addFile(*filename))
      {
        KMessageBox::error(kapp->activeWindow(),
                           i18n("Couldn't add file: %1").arg(*filename));
        break;
      }
      vob->setTitleFromFileName();
      if(multipleFiles->isChecked() || filename == filenames.fromLast())
        m->addMediaObject(vob);
    }
  }
}

KMF::MediaObject* VideoPlugin::createMediaObject(const QDomElement& element)
{
  KMF::MediaObject *mob = new VideoObject(this);
  if(mob)
    mob->fromXML(element);
  return mob;
}

QStringList VideoPlugin::supportedProjectTypes()
{
  QStringList result;
  result << "DVD-PAL" << "DVD-NTSC";
  return result;
}

#include "videoplugin.moc"
