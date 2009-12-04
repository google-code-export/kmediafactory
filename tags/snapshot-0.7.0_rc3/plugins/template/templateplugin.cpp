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
#include "templateplugin.h"
#include "templateobject.h"
#include "newstuffobject.h"
#include "ui_templateconfig.h"
#include "templatepluginsettings.h"
#include "kconfigxml.h"
#include "kmftools.h"
#include <kmediafactory/plugininterface.h>
#include <kdeversion.h>
#include <KGenericFactory>
#include <KStandardDirs>
#include <KFileDialog>
#include <KGlobal>
#include <KIconLoader>
#include <KAboutData>
#include <KDebug>
#include <KPluginLoader>
#include <QRegExp>
#include <QPixmap>
#include <QImage>

K_EXPORT_KMEDIAFACTORY_PLUGIN(template, TemplatePlugin)

class TemplateConfig : public QWidget, public Ui::ConfigureTemplatePlugin
{
  public:
    TemplateConfig(QWidget* parent = 0) : QWidget(parent)
    {
      setupUi(this);
    };
};

TemplatePlugin::TemplatePlugin(QObject *parent, const QVariantList&) :
  KMF::Plugin(parent)
{
  KGlobal::locale()->insertCatalog("kmediafactory_template");
  setObjectName("KMFTemplateEngine");
  // Initialize GUI
  setXMLFile("kmediafactory_templateui.rc");
}

const KMF::ConfigPage* TemplatePlugin::configPage() const
{
  KMF::ConfigPage* configPage = new KMF::ConfigPage;
  configPage->page = new TemplateConfig;
  configPage->config = TemplatePluginSettings::self();
  configPage->itemName = i18n("Template plugin");
  configPage->pixmapName = "folder-image";
  return configPage;
}

void TemplatePlugin::init(const QString &type)
{
  kDebug() << type;
  deleteChildren();
  if (type.left(3) == "DVD")
  {
    kDebug() << "Trying to find templates from: "
        << KGlobal::dirs()->resourceDirs("data");
    QStringList list =
        KMF::Tools::findAllResources("data", "kmediafactory_template/*.kmft");
    kDebug() << "Found templates: " << list;

    for(QStringList::Iterator it = list.begin(); it != list.end(); ++it)
      new TemplateObject(*it, this);
  }
  new NewStuffObject(this);
}

QStringList TemplatePlugin::supportedProjectTypes() const
{
  QStringList result;
  result << "DVD-PAL" << "DVD-NTSC";
  return result;
}

#include "templateplugin.moc"