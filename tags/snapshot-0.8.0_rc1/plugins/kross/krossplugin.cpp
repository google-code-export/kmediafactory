// **************************************************************************
//   Copyright (C) 2008 by Petri Damsten
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

#include "krossplugin.h"

#include <QtCore/QTimer>

#include <KActionCollection>
#include <KDebug>
#include <KMessageBox>
#include <KSharedPtr>
#include <KStandardDirs>
#include <kdeversion.h>

#if KDE_IS_VERSION(4, 4, 0)
#include <Kross/Action>
#include <Kross/InterpreterInfo>
#include <Kross/Manager>
#else
#include <kross/core/action.h>
#include <kross/core/interpreter.h>
#include <kross/core/manager.h>
#endif


#include <kmftools.h>
#include "krossplugininterface.h"

K_EXPORT_KMEDIAFACTORY_PLUGIN(kross, KrossPlugin)

KrossPlugin::KrossPlugin(QObject *parent, const QVariantList &args)
    : KMF::Plugin(parent)
    , m_action(0)
{
    KGlobal::locale()->insertCatalog("kmediafactory_kross");
    // KMF::Tools::spy(this);
    KService::Ptr service = args[0].value<KService::Ptr>();
    QString name = service->property("Name").toString();
    QString script = service->property("X-KMediaFactory-Script").toString();
    QString uirc = service->property("X-KMediaFactory-UiRcFile").toString();

    // kDebug() << name << script << uirc;
    setObjectName(name);
    m_action = new Kross::Action(this, script);

    script = KGlobal::dirs()->locate("appdata", "tools/bin/" + script);
    m_action->setFile(script);

    m_action->addObject(this, "kmediafactory");

    if (!uirc.isEmpty()) {
        setXMLFile(uirc);
    }

    // kDebug() << "Running" << script;
    m_action->trigger();
}

KrossPlugin::~KrossPlugin()
{
    kDebug();
}

void KrossPlugin::init(const QString &type)
{
    kDebug() << type;

    if (m_plugin) {
        // init is reserved word in ?? well in some scripting language
        m_plugin->callMethod("initPlugin", QVariantList() << type);
    }
}

QStringList KrossPlugin::supportedProjectTypes() const
{
    kDebug();

    if (m_plugin) {
        return m_plugin->callMethod("supportedProjectTypes").toStringList();
    }

    return QStringList();
}

KMF::MediaObject *KrossPlugin::createMediaObject(const QDomElement &)
{
    return 0;
}

const KMF::ConfigPage *KrossPlugin::configPage() const
{
    return 0;
}

void KrossPlugin::registerPlugin(Kross::Object::Ptr plugin)
{
    m_plugin = plugin;
}

QObject *KrossPlugin::interface()
{
    return new KrossPluginInterface(this, KMF::Plugin::interface());
}

void KrossPlugin::actionTriggered()
{
    QVariantList args = m_actionMap[sender()];

    if (args.size() == 2) {
        Kross::Object::Ptr obj = args[0].value<Kross::Object::Ptr>();
        obj->callMethod(args[1].toString());
    }
}

void KrossPlugin::addActions(QList<QAction *> *actionList, QStringList actions)
{
    foreach (const QString &act, actions) {
        QAction *a = actionCollection()->action(act);
        actionList->append(a);
    }
}

#include "krossplugin.moc"
