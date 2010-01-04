//**************************************************************************
//   Copyright (C) 2004-2008 by Petri Damsten
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

#include "config.h"
#include "slideshowplugin.h"
#include "ui_slideshowconfig.h"
#include "slideshowpluginsettings.h"
#include "slideshowobject.h"
#include <kmftools.h>
#include <KAction>
#include <KActionCollection>
#include <KLocale>
#include <KAboutData>
#include <KPluginFactory>
#include <KStandardDirs>
#include <KApplication>
#include <KFileDialog>
#include <KIcon>
#include <KPluginLoader>

K_EXPORT_KMEDIAFACTORY_PLUGIN(slideshow, SlideshowPlugin)

class SlideshowConfig : public QWidget, public Ui::SlideshowConfig
{
  public:
    SlideshowConfig(QWidget* parent = 0) : QWidget(parent)
    {
      setupUi(this);
    };
};

SlideshowPlugin::SlideshowPlugin(QObject *parent, const QVariantList&) :
  KMF::Plugin(parent)
{
  KGlobal::locale()->insertCatalog("kmediafactory_slideshow");
  setObjectName("KMFSlideshow");
  setupActions();
}

void SlideshowPlugin::setupActions()
{
  setXMLFile("kmediafactory_slideshowui.rc");

  // Add action for menu item
  QAction* addSlideshowAction = new KAction(KIcon("kuickshow"),
                                            i18n("Add Slideshow"), parent());
  addSlideshowAction->setShortcut(Qt::CTRL + Qt::Key_W);
  actionCollection()->addAction("slideshow", addSlideshowAction);
  connect(addSlideshowAction, SIGNAL(triggered()), SLOT(slotAddSlideshow()));
  interface()->addMediaAction(addSlideshowAction);
}

void SlideshowPlugin::init(const QString &type)
{
  kDebug() << type;
  deleteChildren();

  QAction* action = actionCollection()->action("slideshow");
  if(!action)
    return;

  if (type.left(3) == "DVD")
  {

    m_dvdslideshow = KStandardDirs::findExe("dvd-slideshow");
    if(m_dvdslideshow.isEmpty())
    {
      action->setEnabled(false);
    }
    else
    {
      action->setEnabled(true);
    }
  }
  else
  {
    action->setEnabled(false);
  }
}

void SlideshowPlugin::slotAddSlideshow()
{
  QWidget *parent=kapp->activeWindow();
  QStringList pics = KFileDialog::getOpenFileNames(
      KUrl("kfiledialog:///<AddSlideshow>"),
      "*.jpg *.png *.pdf *.odp *.odt *.ods *.odx *.sxw *.sxc *.sxi \
       *.ppt *.xls *.doc|Pictures, Presentations\n*.*|All files",
      parent);

  if(pics.count() > 0)
  {
    KMF::PluginInterface *m = interface();
    SlideshowObject *sob;

    sob = new SlideshowObject(this);
    QFileInfo fi(pics[0]);
    QDir dir(fi.absolutePath());

    if(pics.count() == 1)
      sob->setTitle(KMF::Tools::simple2Title(fi.baseName()));
    else if(!dir.dirName().isEmpty())
      sob->setTitle(KMF::Tools::simple2Title(dir.dirName()));
    else
      sob->setTitle(i18n("Slideshow"));
    sob->addPics(pics, parent);
    if(sob->slides().count() > 0)
      m->addMediaObject(sob);
  }
}

KMF::MediaObject* SlideshowPlugin::createMediaObject(const QDomElement& element)
{
  KMF::MediaObject *mob = new SlideshowObject(this);
  if(mob)
    mob->fromXML(element);
  return mob;
}

QStringList SlideshowPlugin::supportedProjectTypes() const
{
  QStringList result;
  result << "DVD-PAL" << "DVD-NTSC";
  return result;
}

const KMF::ConfigPage* SlideshowPlugin::configPage() const
{
  KMF::ConfigPage* configPage = new KMF::ConfigPage;
  configPage->page = new SlideshowConfig;
  configPage->config = SlideshowPluginSettings::self();
  configPage->itemName = i18n("Slideshow plugin");
  configPage->pixmapName = "kuickshow";
  return configPage;
}

#include "slideshowplugin.moc"
