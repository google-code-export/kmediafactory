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

#include "videoplugin.h"
#include "videoobject.h"
#include "videopluginsettings.h"
#include "ui_videoconfig.h"
#include <kstandarddirs.h>
#include <kgenericfactory.h>
#include <kactioncollection.h>
#include <kdeversion.h>
#include <kaction.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kicon.h>
#include <kaboutdata.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <QRegExp>
#include <QPixmap>
#include <QCheckBox>

static const char description[] =
  I18N_NOOP("Video plugin for KMediaFactory.");
static const char version[] = VERSION;
static const KAboutData about("kmediafactory_video",
                              I18N_NOOP("KMediaFactory Video"),
                              version, description, KAboutData::License_GPL,
                              "(C) 2005-2007 Petri Damsten", 0, 0,
                              "petri.damsten@iki.fi");

typedef KGenericFactory<VideoPlugin> VideoFactory;
K_EXPORT_COMPONENT_FACTORY(kmediafactory_video, VideoFactory(&about));

class VideoConfig : public QWidget, public Ui::VideoConfig
{
  public:
    VideoConfig(QWidget* parent = 0) : QWidget(parent)
    {
      setupUi(this);
    };
};

VideoPlugin::VideoPlugin(QObject *parent, const QStringList&) :
  KMF::Plugin(parent)
{
  setObjectName("KMFImportVideo");
  setupActions();
}

VideoPlugin::~VideoPlugin()
{
}

const KMF::ConfigPage* VideoPlugin::configPage() const
{
  KMF::ConfigPage* configPage = new KMF::ConfigPage;
  configPage->page = new VideoConfig;
  configPage->config = VideoPluginSettings::self();
  configPage->itemName = i18n("Video plugin");
  configPage->pixmapName = "video";
  return configPage;
}

QAction* VideoPlugin::setupActions()
{
  setComponentData(VideoFactory::componentData());

  // Add action for menu item
  QAction* addVideoAction = new KAction(KIcon("video"), i18n("Add Video"),
                                        parent());
  addVideoAction->setShortcut(Qt::CTRL + Qt::Key_V);
  actionCollection()->addAction("video", addVideoAction);
  connect(addVideoAction, SIGNAL(triggered()), SLOT(slotAddVideo()));

  setXMLFile("kmediafactory_videoui.rc");

  //uiInterface()->addMediaAction(addVideoAction);

  return addVideoAction;
}

void VideoPlugin::init(const QString &type)
{
  kDebug() << k_funcinfo << type << endl;
  deleteChildren();

  QAction* action = actionCollection()->action("video");
  if(!action)
    action = setupActions();

  if (type.left(3) == "DVD")
  {
    action->setEnabled(true);
  }
  else
  {
    action->setEnabled(false);
  }
}

void VideoPlugin::slotAddVideo()
{
  QCheckBox* multipleFiles = new QCheckBox(0);
  KFileDialog dlg(KUrl("kfiledialog:///<AddVideo>"),
                  "*.mpg *.mpeg *.vob *.avi *.mov|Video files\n*.*|All files",
                  0, multipleFiles);

  multipleFiles->setText(i18n("Multiple files make multiple titles."));
  multipleFiles->setChecked(true);
  dlg.setOperationMode(KFileDialog::Opening);
  dlg.setCaption(i18n("Open"));
  dlg.setMode(KFile::Files | KFile::ExistingOnly | KFile::LocalOnly);
  dlg.exec();

  QStringList filenames = dlg.selectedFiles();
  KMF::UiInterface *m = uiInterface();

  kDebug() << k_funcinfo << m << filenames << endl;
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
      if(multipleFiles->isChecked() || filename == filenames.end())
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
