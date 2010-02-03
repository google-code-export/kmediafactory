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

#include "kmfmenu.h"

#include <QtCore/QFileInfo>
#include <QtGui/QImage>

#include <KAboutData>
#include <KComponentData>
#include <KDebug>
#include <KLocale>

#include <kmftime.h>
#include <kmediafactory/job.h>
#include <kstore/KoStore.h>
#include "kmfwidgetfactory.h"

KMFMenu::KMFMenu(const QString &tmplate, QObject *parent)
    : KMFTemplateBase(parent)
    , m_template(tmplate)
    , m_templateXML("kmf_project")
{
    m_templateXML.setContent(m_template.readFile("template.xml"));
    QDomElement element = m_templateXML.documentElement();
    m_id = element.attribute("id");
    m_title = element.attribute("name");

    QDomNodeList list = m_templateXML.elementsByTagName("page");

    for (int i = 0; i < list.count(); ++i) {
        m_menus.append(list.item(i).toElement().attribute("name"));
    }
}

KMFMenu::~KMFMenu()
{
    foreach (QList<KMFMenuPage *> pages, m_pages) {
        qDeleteAll(pages);
    }
}

QDomElement KMFMenu::writeDvdAuthorXml(const QString &type, int titleset) const
{
    if (pages() > 0) {
        QDomDocument doc;
        QDomElement result = doc.createElement("menus");
        QDomElement video = doc.createElement("video");
        QList<KMF::MediaObject *> mobs = m_interface->mediaObjects();
        video.setAttribute("aspect", QDVD::VideoTrack::aspectRatioString(m_interface->aspectRatio()));

        if (QDVD::VideoTrack::Aspect_16_9 == m_interface->aspectRatio()) {
            video.setAttribute("widescreen", "nopanscan");
        }

        result.appendChild(video);

        if (titleset == 0) {
            int j = 1;
            foreach (KMFMenuPage * ob, m_pages[0]) {
                ob->setIndex(0, mobs.count(), j, m_pages[0].count());
                ob->writeDvdAuthorXml(result, type);
                ++j;
            }
            return result;
        } else {
            if ((titleset < m_pages.count()) && (m_pages[titleset].count() != 0)) {
                int j = 1;
                foreach (KMFMenuPage * ob, m_pages[titleset]) {
                    ob->setIndex(titleset, mobs.count(), j, m_pages[titleset].count());
                    ob->writeDvdAuthorXml(result, type);
                    ++j;
                }
            }

            if (!result.hasChildNodes() && (m_pages[0].count() > 0)) {
                m_pages[0][0]->writeDvdAuthorXmlNoMenu(result);
            }

            return result;
        }
    } else   {
        return QDomElement();
    }
}

bool KMFMenu::addMenuMpegJobs(const QString &type)
{
    for (int i = 0; i < m_pages.count(); ++i) {
        if (m_pages[i].count() > 0) {
            foreach (KMFMenuPage * ob, m_pages[i]) {
                KMF::Job *job = ob->job(type);

                if (job) {
                    m_interface->addJob(job);
                }
            }
        }
    }

    return true;
}

QList<KMFMenuPage *> *KMFMenu::titlePages(int title)
{
    while (m_pages.count() <= title) {
        m_pages.append(QList<KMFMenuPage *>());
    }
    return &m_pages[title];
}

bool KMFMenu::addPage(const QDomElement &element, int pageSet, int title, int chapter,
                      const KMF::MediaObject *mob)
{
    bool result = false;
    KMFMenuPage *menuPage = static_cast<KMFMenuPage *>
                            (KMFWidgetFactory::createPage(element, this, title, chapter));

    progress(m_pagePoints);

    if (menuPage) {
        uint msgid = KMF::PluginInterface::messageId();
        m_interface->message(msgid, KMF::Start, i18n("Menu: %1", uiText(menuPage->objectName())));
        menuPage->setMediaObject(mob);

        if (pageSet == 0) {
            menuPage->setVmgm(true);
        }

        titlePages(pageSet)->append(menuPage);
        result = menuPage->parseButtons();
        m_interface->message(msgid, KMF::Done);
    }

    return result;
}

bool KMFMenu::addPage(const QString &name, int title, int chapter)
{
    QDomElement element = m_templateXML.documentElement();
    QDomElement pageElement = getPage(element.firstChild(), name);

    QList<KMF::MediaObject *> mobs = m_interface->mediaObjects();
    const KMF::MediaObject *mob = title <= mobs.count() ? mobs.at(title - 1) : 0;
    KMFMenuPage temp(this);

    temp.fromXML(pageElement);

    if (temp.titles() > 0) {
        for (int i = 0; i < ((mobs.count() - 1) / temp.titles()) + 1; ++i) {
            if (addPage(pageElement, title, i * temp.titles(), chapter, mob) == false) {
                return false;
            }
        }
    } else if (temp.chapters() > 0)    {
        if (title <= mobs.count()) {
            for (int i = 0;
                 i < ((mediaObjChapterCount(title - 1) - 1) / temp.chapters()) + 1;
                 ++i)
            {
                if (addPage(pageElement, title, title - 1, i * temp.chapters(), mob) == false) {
                    return false;
                }
            }
        }
    } else   {
        return addPage(pageElement, title, title, chapter, mob);
    }

    return true;
}

QDomElement KMFMenu::getPage(const QDomNode &node, const QString &name)
{
    QDomNode n = node;

    while (!n.isNull()) {
        QDomElement e = n.toElement();

        if (!e.isNull()) {
            if ((e.tagName() == "page") && (e.attribute("name") == name)) {
                return e;
            }
        }

        n = n.nextSibling();
    }
    return QDomElement();
}

QImage KMFMenu::templateImage(const QString &image) const
{
    QImage img;

    img.loadFromData(m_template.readFile(image));
    return img;
}

QImage KMFMenu::makeMenuPreview(QString page)
{
    clear();

    if (page.isEmpty()) {
        return templateImage("preview.jpg");
    } else   {
        QDomElement element = m_templateXML.documentElement();
        QDomElement e = getPage(element.firstChild(), page);
        KMFMenuPage *p =
            static_cast<KMFMenuPage *>(KMFWidgetFactory::createPage(e, this));

        if (p) {
            return p->preview();
        }
    }

    return QImage();
}

int KMFMenu::pages() const
{
    int result = 0;

    for (int i = 0; i < m_pages.count(); ++i) {
        for (int j = 0; j < m_pages[i].count(); ++j) {
            ++result;
        }
    }

    return result;
}

void KMFMenu::progress(int points)
{
    if (points > m_points) {
        points = m_points;
    }

    m_points -= points;
    // m_interface->progress(points);
}

bool KMFMenu::makeMenu(const QString &type)
{
    clear();
    m_points = TotalPoints / 4;
    m_pagePoints = m_points / ((m_interface->mediaObjects().count() * 2) + 1);
    QDomElement element = m_templateXML.documentElement();
    QString page = element.attribute("first_page");

    if (addPage(page, 0, 0)) {
        int p = pages();
        progress(TotalPoints);
        m_points = TotalPoints - (TotalPoints / 4);

        if (p > 0) {
            m_pagePoints = m_points / p;
        }

        return addMenuMpegJobs(type);
    }

    return false;
}

int KMFMenu::mediaObjCount()
{
    return m_interface->mediaObjects().count();
}

int KMFMenu::mediaObjChapterCount(int title)
{
    QList<KMF::MediaObject *> mobs = m_interface->mediaObjects();
    KMF::MediaObject *mob = mobs.at(title);
    int result = mob->chapters();
    KMF::Time chapter = mob->chapterTime(result);

    // Don't put chapter selection for chapter really close to end
    // kDebug() << mob->duration() << ", " << chapter;
    if ((chapter + 3.0 > mob->duration()) && (result > 1)) {
        --result;
    }

    return result;
}

#include "kmfmenu.moc"
