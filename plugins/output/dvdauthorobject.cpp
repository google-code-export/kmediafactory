// **************************************************************************
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
// **************************************************************************

#include "dvdauthorobject.h"

#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtGui/QPixmap>
#include <QtXml/QDomElement>

#include <KAboutData>
#include <KDebug>
#include <KIconLoader>
#include <KLocale>
#include <KMimeType>
#include <KAction>
#include <KActionCollection>

#include <kmftools.h>
#include <kmediafactory/job.h>
#include "outputplugin.h"

static const char startString[] = I18N_NOOP("DVD Author XML file");

class WriteDVDAuthorXMLJob : public KMF::Job
{
    public:
        KMF::TemplateObject *tempObj;
        QList<KMF::MediaObject *> mobs;
        QString title;
        QString projectDir;

        void run()
        {
            message(msgId(), KMF::Start, i18n(startString));

            QDomDocument doc("");
            doc.appendChild(doc.createProcessingInstruction("xml",
                            "version=\"1.0\" encoding=\"UTF-8\""));
            QDomElement root = doc.createElement("dvdauthor");
            int i;
            QString tmp;
            QString header =
                    "\n"
                    " *********************************************************************\n"
                    " %1\n"
                    " *********************************************************************\n";
            QString comment = i18n(
                    "\n"
                    "**********************************************************************\n"
                    "\n"
                    "This file was made with %1 - %2\n"
                    "http://www.iki.fi/damu/software/kmediafactory/\n"
                    "\n"
                    "**********************************************************************\n"
                    "\n"
                    "Register usage:\n"
                    "================\n"
                    "g0: Playing chapter saved here.\n"
                    "g1: Play this chapter when entering titleset menu. 0 == show menu.\n"
                    "    Also used by Back button.\n"
                    "g2: Used vmgm button is saved here\n"
                    "g3: Jump to this titleset. Also used for direct play.\n"
                    "g4: Used titleset button is saved here\n"
                    "g5: vmgm page is saved here\n"
                    "g6: titleset page is saved here\n"
                    "g7: temporary variable\n"
                    "\n"
                    "**********************************************************************\n",
                    KGlobal::mainComponent().aboutData()->programName(),
                    KGlobal::mainComponent().aboutData()->version());

            doc.appendChild(doc.createComment(comment));
            doc.appendChild(doc.createTextNode("\n"));

            root.setAttribute("dest", "./DVD");
            doc.appendChild(root);

            root.appendChild(doc.createTextNode("\n "));
            tmp = i18n("Main menu for %1", title);
            root.appendChild(doc.createComment(header.arg(tmp)));
            root.appendChild(doc.createTextNode("\n "));

            QDomElement vmgm = doc.createElement("vmgm");

            QDomElement fpc = doc.createElement("fpc");

            int titleset = 0;

            if (tempObj->call("directPlay").toBool()) {
                titleset = 1;
            }

            fpc.appendChild(doc.createTextNode(QString(
                                    " { g0 = 0; g1 = 0; g2 = 0; g3 = %1; g4 = 0; g5 = 0; g6 = 0;"
                                    " jump menu 1; }").arg(titleset)));
            vmgm.appendChild(fpc);

            QDomElement menus = toElement(tempObj->call("writeDvdAuthorXml", QVariantList() << 0));

            if (menus.hasChildNodes()) {
                vmgm.appendChild(menus);
            }

            root.appendChild(vmgm);
            CHECK_IF_ABORTED();

            i = 1;
            foreach(KMF::MediaObject * mob, mobs)
            {
                root.appendChild(doc.createTextNode("\n "));
                root.appendChild(doc.createComment(header.arg(mob->text())));
                root.appendChild(doc.createTextNode("\n "));

                QDomElement titleset = doc.createElement("titleset");

                menus = toElement(tempObj->call("writeDvdAuthorXml", QVariantList() << i));
                titleset.appendChild(menus);

                QDomElement titles = toElement(mob->call("writeDvdAuthorXml",
                                QVariantList() << tempObj->call("language") << i));

                titleset.appendChild(titles);
                root.appendChild(titleset);
                ++i;
                CHECK_IF_ABORTED();
            }

            if (!KMF::Tools::saveString2File(projectDir + "dvdauthor.xml", doc.toString(),
                        false)) {
                message(msgId(), KMF::Error, i18n("Error in saving dvdauthor.xml"));
            }

            message(msgId(), KMF::Done);
        }

        QDomElement toElement(const QVariant &element)
        {
            if (element.canConvert<QDomElement>()) {
                return element.value<QDomElement>();
            } else   {
                return KMF::Tools::string2XmlElement(element.toString());
            }
        }
};

DvdAuthorObject::DvdAuthorObject(QObject *parent)
    : OutputObject(parent)
{
    setObjectName("dvdauthor");
    setTitle(i18n("DVDAuthor Project"));
    
    cleanFile = new KAction(KIcon("edit-delete"), i18n("Clean"), this);
    plugin()->actionCollection()->addAction("dvda_remove", cleanFile);
    connect(cleanFile, SIGNAL(triggered()), SLOT(clean()));
}

DvdAuthorObject::~DvdAuthorObject()
{
    KMF::PluginInterface *pi = interface();

    if (pi) {
        pi->removeOutputObject(this);
    }
}

void DvdAuthorObject::actions(QList<QAction *> *actionList) const
{
    actionList->append(cleanFile);
}

bool DvdAuthorObject::fromXML(const QDomElement &)
{
    return true;
}

bool DvdAuthorObject::isUpToDate(QString type)
{
    if (type != interface()->lastSubType()) {
        return false;
    }

    QDateTime lastModified = interface()->lastModified(KMF::Any);
    QFileInfo fi(interface()->projectDir() + "dvdauthor.xml");

    if ((fi.exists() == false) || (lastModified > fi.lastModified())) {
        return false;
    }

    return true;
}

bool DvdAuthorObject::prepare(const QString &type)
{
    interface()->message(msgId(), KMF::Start, i18n(startString));

    if (isUpToDate(type)) {
        interface()->message(msgId(), KMF::Info, i18n("DVD Author XML file is up to date"));
    } else   {
        WriteDVDAuthorXMLJob *job = new WriteDVDAuthorXMLJob();
        job->tempObj = interface()->templateObject();
        job->mobs =  interface()->mediaObjects();
        job->title = interface()->title();
        job->projectDir = interface()->projectDir();
        interface()->addJob(job);
    }

    interface()->message(msgId(), KMF::Done);
    return true;
}

void DvdAuthorObject::toXML(QDomElement *) const
{
}

QPixmap DvdAuthorObject::pixmap() const
{
    return KIconLoader::global()->loadIcon("media-optical", KIconLoader::NoGroup,
            KIconLoader::SizeLarge);
}

void DvdAuthorObject::clean()
{
    QFile::remove(interface()->projectDir()+"dvdauthor.xml");
}

#include "dvdauthorobject.moc"
