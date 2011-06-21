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

#include <config.h>
#include "outputplugin.h"

#include <QtCore/QRegExp>
#include <QtGui/QPixmap>

#include <KAboutData>
#include <KAction>
#include <KActionCollection>
#include <KApplication>
#include <KFileDialog>
#include <KGenericFactory>
#include <KGlobal>
#include <KIcon>
#include <KIconLoader>
#include <KRun>
#include <KStandardDirs>
#include <kdeversion.h>

#include "dvdauthorobject.h"
#include "dvddirectoryobject.h"
#include "k3bobject.h"
#include "isoobject.h"

#include <outputpluginsettings.h>
#include <ui_outputconfig.h>

#ifdef HAVE_LIBDVDREAD
#include "dvdinfo.h"
#endif

#define NO_PROTOCOL "NoProtocol"

K_EXPORT_KMEDIAFACTORY_PLUGIN(output, OutputPlugin)

class OutputConfig : public QWidget, public Ui::OutputConfig
{
    public:
        OutputConfig(QWidget *parent = 0) : QWidget(parent)
        {
            setupUi(this);
        };
};

OutputPlugin::OutputPlugin(QObject *parent, const QVariantList &)
    : KMF::Plugin(parent)
{
    KGlobal::locale()->insertCatalog("kmediafactory_output");
    setObjectName("KMFOutput");
    // Initialize GUI
    setXMLFile("kmediafactory_outputui.rc");

#ifdef HAVE_LIBDVDREAD
    dvdInfo = new KAction(KIcon("zoom-original"), i18n("DVD Info"), parent);
    dvdInfo->setShortcut(Qt::CTRL + Qt::Key_I);
    actionCollection()->addAction("dvd_info", dvdInfo);
    connect(dvdInfo, SIGNAL(triggered()), SLOT(slotDVDInfo()));
#endif

    addPreviewAction("xine", Qt::CTRL + Qt::Key_X);
    addPreviewAction("kaffeine", Qt::CTRL + Qt::Key_K);
    addPreviewAction("dragon", Qt::CTRL + Qt::Key_D, false, "dragonplayer");
}

void OutputPlugin::addPreviewAction(const QString &app, QKeySequence shortcut, bool useDvdProtocol, const QString &icon)
{
    QString exe = KStandardDirs::findExe(app);

    if (!exe.isEmpty()) {
        QAction *action = new KAction(KIcon(icon.isEmpty() ? app : icon),
                i18n("Preview DVD in %1", app[0].toUpper()+app.mid(1)), parent());
        action->setData(exe);
        action->setShortcut(shortcut);
        actionCollection()->addAction("preview_dvd_"+app, action);
        connect(action, SIGNAL(triggered()), SLOT(slotPreviewDVD()));
        if(!useDvdProtocol) { 
            action->setProperty(NO_PROTOCOL, !useDvdProtocol);
        }
    }
}

void OutputPlugin::init(const QString &type)
{
    kDebug() << type;
    deleteChildren();

    if (type.left(3) == "DVD") {
        KMF::PluginInterface *ui = interface();

        if (ui) {
            QList<KMF::OutputObject *> objects;
            
            objects << new DvdAuthorObject(this)
                    << new DvdDirectoryObject(this)
                    << new K3bObject(this)
                    << new IsoObject(this);

            QList<KMF::OutputObject *>::ConstIterator it(objects.constBegin()),
                                                      end(objects.constEnd());

            for(; it!=end; ++it) {
                if((*it)->isValid())
                    ui->addOutputObject(*it);
                else
                    delete *it;
            }
        }
    }
}

void OutputPlugin::slotPreviewDVD()
{
    QAction *action=qobject_cast<QAction *>(sender());

    if(!action || action->data().toString().isEmpty()) {
        return;
    }

    QString projectDir = interface()->projectDir(),
            isoFile    = projectDir+interface()->title().replace("/", ".")+".iso",
            dvd        = QFile::exists(isoFile)
                            ? isoFile
                            : QFile::exists(projectDir+"DVD/VIDEO_TS/VTS_01_0.VOB")
                                ? projectDir+"DVD/"
                                : QString();
    
    if(dvd.isEmpty()) {
        return;
    }

    bool useProtocol=action->property(NO_PROTOCOL).isNull();

    KRun::runCommand(action->data().toString()+" \"" + (useProtocol ? "dvd://" : "") + dvd + '\"', kapp->activeWindow());
}

void OutputPlugin::slotDVDInfo()
{
#ifdef HAVE_LIBDVDREAD
    QString projectDir = interface()->projectDir();
    QPointer<DVDInfo> dlg = new DVDInfo(kapp->activeWindow(), projectDir + "DVD/");

    dlg->exec();
    delete dlg;
#endif
}

const KMF::ConfigPage* OutputPlugin::configPage() const
{
    KMF::ConfigPage *configPage = new KMF::ConfigPage;

    configPage->page = new OutputConfig;
    configPage->config = OutputPluginSettings::self();
    configPage->itemName = i18n("Output");
    configPage->itemDescription = i18n("Output Settings");
    configPage->pixmapName = "media-optical";
    return configPage;

}

QStringList OutputPlugin::supportedProjectTypes() const
{
    QStringList result;

    result << "DVD-PAL" << "DVD-NTSC";
    return result;
}

#include "outputplugin.moc"
