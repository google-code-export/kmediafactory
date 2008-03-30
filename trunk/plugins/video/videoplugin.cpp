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

K_EXPORT_KMEDIAFACTORY_PLUGIN(video, VideoPlugin);

class VideoConfig : public QWidget, public Ui::VideoConfig
{
  public:
    VideoConfig(QWidget* parent = 0) : QWidget(parent)
    {
      setupUi(this);
    };
};

VideoPlugin::VideoPlugin(QObject *parent, const QVariantList&) :
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
  configPage->pixmapName = "video-mpeg";
  return configPage;
}

QAction* VideoPlugin::setupActions()
{
  // Add action for menu item
  QAction* addVideoAction = new KAction(KIcon("video-mpeg"), i18n("Add Video"),
                                        parent());
  addVideoAction->setShortcut(Qt::CTRL + Qt::Key_V);
  actionCollection()->addAction("video", addVideoAction);
  connect(addVideoAction, SIGNAL(triggered()), SLOT(slotAddVideo()));

  setXMLFile("kmediafactory_videoui.rc");

  uiInterface()->addMediaAction(addVideoAction);

  return addVideoAction;
}

void VideoPlugin::init(const QString &type)
{
  kDebug() << type;
  deleteChildren();

  QAction* action = actionCollection()->action("video");
  if(!action)
    return;

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

  kDebug() << m << filenames;
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
  {
    if(!mob->fromXML(element))
    {
      delete mob;
      return 0;
    }
  }
  return mob;
}

QStringList VideoPlugin::supportedProjectTypes() const
{
  QStringList result;
  result << "DVD-PAL" << "DVD-NTSC";
  return result;
}

#include "videoplugin.moc"
