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
#include "slideshowplugin.h"
#include "slideshowpluginsettings.h"
#include "slideshowobject.h"
#include <kmftools.h>
#include <kactioncollection.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kgenericfactory.h>
#include <kdeversion.h>
#include <kstandarddirs.h>
#include <kapplication.h>
#include <kfiledialog.h>
#include <kicon.h>

static const char description[] =
  I18N_NOOP("Slideshow plugin for KMediaFactory.");
static const char version[] = VERSION;
static const KAboutData about("kmediafactory_slideshow",
                              I18N_NOOP("KMediaFactory Slideshow"),
                              version, description, KAboutData::License_GPL,
                              "(C) 2005 Petri Damsten", 0, 0,
                              "petri.damsten@iki.fi");

typedef KGenericFactory<SlideshowPlugin> slideshowFactory;
K_EXPORT_COMPONENT_FACTORY(kmediafactory_slideshow, slideshowFactory(&about))

SlideshowPlugin::SlideshowPlugin(QObject *parent, const QStringList&) :
  KMF::Plugin(parent)
{
  kDebug() << k_funcinfo << endl;
  setObjectName("KMFSlideshow");
  // Initialize GUI
  setComponentData(slideshowFactory::componentData());
  setXMLFile("kmediafactory_slideshowui.rc");
  // Add action for menu item
  QAction* addSlideshowAction = new KAction(KIcon("icons"),
                                     i18n("Add Slideshow"),this);
  addSlideshowAction->setShortcut(Qt::CTRL + Qt::Key_W);
  actionCollection()->addAction("slideshow", addSlideshowAction);
  connect(addSlideshowAction, SIGNAL(triggered()), SLOT(slotAddSlideshow()));
}

void SlideshowPlugin::init(const QString &type)
{
  kDebug() << k_funcinfo << type << endl;
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
  QStringList pics = KFileDialog::getOpenFileNames(
      KUrl("kfiledialog:///<AddSlideshow>"),
      "*.jpg *.png *.pdf *.odp *.odt *.ods *.odx *.sxw *.sxc *.sxi \
       *.ppt *.xls *.doc|Pictures, Presentations\n*.*|All files");

  if(pics.count() > 0)
  {
    KMF::UiInterface *m = uiInterface();
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
    sob->addPics(pics);
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

QStringList SlideshowPlugin::supportedProjectTypes()
{
  QStringList result;
  result << "DVD-PAL" << "DVD-NTSC";
  return result;
}

#include "slideshowplugin.moc"
