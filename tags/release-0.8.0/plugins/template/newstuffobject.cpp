// **************************************************************************
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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
// **************************************************************************

#include "newstuffobject.h"

#include <KApplication>
#include <KIconLoader>
#include <KLocale>
#if KDE_IS_VERSION(4, 3, 90)
#include <KNS3/DownloadDialog>
#else
#include <KNS/Engine>
#endif

#include <templatepluginsettings.h>
#include "templateobject.h"
#include "templateplugin.h"

NewStuffObject::NewStuffObject(QObject *parent) : KMF::TemplateObject(parent), m_newstuff(0)
{
    setObjectName("newstuff");
    setTitle(i18n("Get More..."));
    interface()->addTemplateObject(this);
}

NewStuffObject::~NewStuffObject()
{
    KMF::PluginInterface *ui = interface();

    if (ui) {
        interface()->removeTemplateObject(this);
    }
}

bool NewStuffObject::clicked()
{
    // Remove uninstalled
    QList< ::TemplateObject* > templates = parent()->findChildren< ::TemplateObject* >();
    foreach (::TemplateObject *temp, templates) {
        if (!temp->fileExists()) {
            delete temp;
        }
    }

#if KDE_IS_VERSION(4, 3, 90)
    QPointer<KNS3::DownloadDialog> dialog = new KNS3::DownloadDialog("kmediafactory_template.knsrc",
                                                                     kapp->activeWindow());
    dialog->exec();
    KNS3::Entry::List entries = dialog->changedEntries();
    // Add installed
    foreach (const KNS3::Entry& entry, entries) {
        foreach (QString file, entry.installedFiles()) {
            new ::TemplateObject(file, parent());
        }
    }
    delete dialog;
#else
    KNS::Engine *engine = new KNS::Engine();
    engine->init("kmediafactory_template.knsrc");
    KNS::Entry::List entries = engine->downloadDialogModal(kapp->activeWindow());
    delete engine;
    // Add installed
    foreach (const KNS::Entry* entry, entries) {
        foreach (QString file, entry->installedFiles()) {
            new ::TemplateObject(file, parent());
        }
    }
#endif
    return true;
}

QPixmap NewStuffObject::pixmap() const
{
    return KIconLoader::global()->loadIcon("get-hot-new-stuff",
            KIconLoader::NoGroup, KIconLoader::SizeLarge);
}

#include "newstuffobject.moc"
