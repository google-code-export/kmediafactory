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
#include "config.h"
#include "templateplugin.h"
#include "templateobject.h"
#include "newstuffobject.h"
#include "templateconfiglayout.h"
#include "templatepluginsettings.h"
#include "kconfigxml.h"
#include "kmftools.h"
#include <kmediafactory/uiinterface.h>
#include <kdeversion.h>
#include <kgenericfactory.h>
#include <kstandarddirs.h>
#include <kaction.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kaboutdata.h>
#include <kdebug.h>
#include <qregexp.h>
#include <qpixmap.h>
#include <qdom.h>
#include <qimage.h>
#include <qsqlpropertymap.h>
//#include <avcodec.h>

static const char description[] =
  I18N_NOOP("Template plugin for KMediaFactory.");
static const char version[] = VERSION;
static const KAboutData about("kmediafactory_template",
                              I18N_NOOP("KMediaFactory Template"),
                              version, description, KAboutData::License_GPL,
                              "(C) 2005 Petri Damsten", 0, 0,
                              "petri.damsten@iki.fi");

typedef KGenericFactory<TemplatePlugin> templateFactory;
#if KDE_IS_VERSION(3, 3, 0)
K_EXPORT_COMPONENT_FACTORY(kmediafactory_template, templateFactory(&about))
#else
K_EXPORT_COMPONENT_FACTORY(kmediafactory_template, templateFactory(about.appName()))
#endif

TemplatePlugin::TemplatePlugin(QObject *parent,
                               const char* name, const QStringList&) :
  KMF::Plugin(parent, name )
{
  // Initialize GUI
  setInstance(templateFactory::instance());
  setXMLFile("kmediafactory_templateui.rc");
}

const KMF::ConfigPage* TemplatePlugin::configPage() const
{
  KMF::ConfigPage* configPage = new KMF::ConfigPage;
  configPage->page = new ConfigureTemplatePluginLayout;
  configPage->config = TemplatePluginSettings::self();
  configPage->itemName = i18n("Template plugin");
  configPage->pixmapName = "kmultiple";
  return configPage;
}

void TemplatePlugin::init(const QString &type)
{
  deleteChildren();
  if (type.left(3) == "DVD")
  {
    kdDebug() << "Trying to find templates from: "
        << KGlobal::dirs()->resourceDirs("data") << endl;
    QStringList list =
        KMF::Tools::findAllResources("data", "kmediafactory_template/*.kmft");
    kdDebug() << "Found templates: " << list << endl;

    for(QStringList::Iterator it = list.begin(); it != list.end(); ++it)
      new TemplateObject(*it, this);
  }
  new NewStuffObject(this);
}

QStringList TemplatePlugin::supportedProjectTypes()
{
  QStringList result;
  result << "DVD-PAL" << "DVD-NTSC";
  return result;
}

#include "templateplugin.moc"