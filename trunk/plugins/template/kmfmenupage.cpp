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

#include "kmfmenupage.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QObject>
#include <QtCore/QRegExp>
#include <QtCore/QVariant>
#include <QtGui/QImage>
#include <QtGui/QPainter>

#include <KApplication>
#include <KDebug>
#include <KLocale>
#include <KMessageBox>
#include <KStandardDirs>

#include <sys/stat.h>
#include <sys/types.h>

#include <kmftime.h>
#include <kmftools.h>
#include <run.h>
#include <templatepluginsettings.h>
#include <kmediafactory/job.h>
#include <kmediafactory/plugininterface.h>
#include "kmfbutton.h"
#include "kmfgeometry.h"
#include "kmfmenu.h"
#include "templateobject.h"

class KMFMenuPageJob : public KMF::Job
{
    public:
        KMFMenuPageJob(const KMFMenuPage &page) : menuPage(page), m_modifiedLayers(0){};

        const KMFMenuPage &menuPage;
        QString menuDir;
        QString projectType;

        void run()
        {
            message(msgId(), KMF::Start,
                    i18n("Menu: %1", KMFTemplateBase::uiText(menuPage.objectName())));
            init();
            CHECK_IF_ABORTED();

            if (paint() == false) {
                message(msgId(), KMF::Error, i18n("Could not paint menu."));
                return;
            }

            CHECK_IF_ABORTED();

            if (writeSpumuxXml() == false) {
                message(msgId(), KMF::Error, i18n("Could not write spumux xml file."));
                return;
            }

            CHECK_IF_ABORTED();

            if (saveImages() == false) {
                message(msgId(), KMF::Error, i18n("Could not save images."));
                return;
            }

            CHECK_IF_ABORTED();

            if (makeMpeg() == false) {
                message(msgId(), KMF::Error, i18n("Could not make mpeg file."));
                return;
            }

            message(msgId(), KMF::Done);
        }

        void init()
        {
            QSize resolution = menuPage.resolution();

            m_background = QImage(resolution, QImage::Format_ARGB32);
            m_background.fill(KMF::Tools::toColor("#444444FF").rgba());
            m_background.setDotsPerMeterX(DPM);
            m_background.setDotsPerMeterY(DPM);
            m_background.setText("layer", "background");
            m_sub = QImage(resolution, QImage::Format_ARGB32);
            m_sub.fill(KMF::Tools::toColor("#00000000").rgba());
            m_sub.setDotsPerMeterX(DPM);
            m_sub.setDotsPerMeterY(DPM);
            m_sub.setText("layer", "sub");
            m_subHighlight = QImage(resolution, QImage::Format_ARGB32);
            m_subHighlight.fill(KMF::Tools::toColor("#00000000").rgba());
            m_subHighlight.setDotsPerMeterX(DPM);
            m_subHighlight.setDotsPerMeterY(DPM);
            m_subHighlight.setText("layer", "highlight");
            m_subSelect = QImage(resolution, QImage::Format_ARGB32);
            m_subSelect.fill(KMF::Tools::toColor("#00000000").rgba());
            m_subSelect.setDotsPerMeterX(DPM);
            m_subSelect.setDotsPerMeterY(DPM);
            m_subSelect.setText("layer", "select");
        }

        bool paint()
        {
            m_modifiedLayers = 0;
            return paintChildWidgets(&menuPage);
        }

        bool paintChildWidgets(const QObject *parent)
        {
            foreach (const QObject * obj, parent->children()) {
                if (obj->inherits("KMFWidget")) {
                    const KMFWidget *widget = static_cast<const KMFWidget *>(obj);

                    if (widget->isHidden()) {
                        continue;
                    }

                    if (TemplatePluginSettings::widgetDebug()) {
                        QRect paintRC = widget->paintRect();
                        QRect rc = widget->rect();
                        QString type = widget->metaObject()->className();
                        QColor color;

                        if (TemplatePluginSettings::widgetDebugInfo()) {
                            kDebug() << type << " (" << widget->objectName() << "): " << rc <<
                            " - " << paintRC;
                        }

                        if ((type == "KMFImage") &&
                            TemplatePluginSettings::widgetDebugImage())
                        {
                            color.setNamedColor("blue");
                        } else if (type == "KMFLabel" &&
                                   TemplatePluginSettings::widgetDebugLabel())
                        {
                            color.setNamedColor("red");
                        } else if (type == "KMFFrame" &&
                                   TemplatePluginSettings::widgetDebugFrame())
                        {
                            color.setNamedColor("green");
                        } else if (type == "KMFButton" &&
                                   TemplatePluginSettings::widgetDebugButton())
                        {
                            color.setNamedColor("yellow");
                        } else if ((type == "KMFVBox" || type == "KMFHBox")    &&
                                   TemplatePluginSettings::widgetDebugBox())
                        {
                            color.setNamedColor("white");
                        } else if (type == "KMWidget" &&
                                   TemplatePluginSettings::widgetDebugWidget())
                        {
                            color.setNamedColor("gray");
                        }

                        if (color.isValid()) {
                            QPainter p(&m_background);

                            p.setPen(QPen(QBrush(color), 1));

                            if (TemplatePluginSettings::withMargin()) {
                                color.setAlphaF(0.1);
                                p.setBrush(QBrush(color));
                                p.drawRect(rc);
                            }

                            color.setAlphaF(0.3);
                            p.setBrush(QBrush(color));
                            p.drawRect(rc);
                        }
                    }

                    widget->paint(layer(widget->layer()), widget->layer() == KMFWidget::Background);
                    m_modifiedLayers |= widget->layer();

                    if (paintChildWidgets(obj) == false) {
                        return false;
                    }
                }
            }
            return true;
        }

        bool writeSpumuxXml()
        {
            QDomDocument doc("");
            QDomElement root = doc.createElement("subpictures");
            QDomElement stream = doc.createElement("stream");
            QDomElement spu = doc.createElement("spu");

            spu.setAttribute("end", "00:00:00.00");

            if (m_modifiedLayers & KMFWidget::Sub) {
                spu.setAttribute("image", QString("%1_sub.png").arg(menuPage.objectName()));
            }

            if (m_modifiedLayers & KMFWidget::Highlight) {
                spu.setAttribute("highlight",
                        QString("%1_highlight.png").arg(menuPage.objectName()));
            }

            if (m_modifiedLayers & KMFWidget::Select) {
                spu.setAttribute("select",
                        QString("%1_select.png").arg(menuPage.objectName()));
            }

            spu.setAttribute("force", "yes");

            // We can't search and loop in the same list concurrently
            // QList<KMFButton*> temp = *m_buttons;
            // foreach(KMFButton *btn, temp)
            // TODO What search?
            foreach (const KMFButton * btn, menuPage.buttons()) {
                if (btn->isHidden()) {
                    continue;
                }

                /*QDomComment comment =
                 *  doc.createComment(QString("Button: %1").arg(btn->objectName()));
                 * spu.appendChild(comment);*/
                QDomElement button = doc.createElement("button");
                button.setAttribute("name", btn->objectName());
                QRect rc = btn->paintRect();
                // kDebug() << rc;
                button.setAttribute("x0", rc.left());
                button.setAttribute("y0", rc.top());
                button.setAttribute("x1", rc.right());
                button.setAttribute("y1", rc.bottom());

                if (btn->down()) {
                    button.setAttribute("down", btn->down()->objectName());
                }

                if (btn->up()) {
                    button.setAttribute("up", btn->up()->objectName());
                }

                if (btn->right()) {
                    button.setAttribute("right", btn->right()->objectName());
                }

                if (btn->left()) {
                    button.setAttribute("left", btn->left()->objectName());
                }

                spu.appendChild(button);
            }
            stream.appendChild(spu);
            root.appendChild(stream);
            doc.appendChild(root);

            // Write spumux xml
            return KMF::Tools::saveString2File(menuDir + menuPage.objectName() + ".xml",
                    doc.toString(), false);
        }

        bool saveImages()
        {
            QString file;

            // Save subpicture files
            if (m_modifiedLayers & KMFWidget::Sub) {
                file = menuDir + QString("%1_sub.png").arg(menuPage.objectName());
                m_sub.save(file);
            }

            if (m_modifiedLayers & KMFWidget::Highlight) {
                file = menuDir + QString("%1_highlight.png").arg(menuPage.objectName());
                m_subHighlight.save(file);
            }

            if (m_modifiedLayers & KMFWidget::Select) {
                file = menuDir + QString("%1_select.png").arg(menuPage.objectName());
                m_subSelect.save(file);
            }

            file = menuDir + QString("%1.pnm").arg(menuPage.objectName());
            // PNM P6 256
            return m_background.save(file, "PPM");
        }

        bool makeMpeg()
        {
            QString sound = menuPage.sound();

            if (sound.isEmpty()) {
                sound = KStandardDirs::locate("data", "kmediafactory/media/silence.mp2");
            }

            KProcess *kmf_make_mpeg = process(msgId());
            kmf_make_mpeg->setWorkingDirectory(menuDir);
            QString exe = KGlobal::dirs()->findResource("data",
                    "kmediafactory/scripts/kmf_make_mpeg");
            *kmf_make_mpeg << exe << projectType <<
            QString(KMF::Tools::frames(projectType)) << menuPage.objectName() << sound;
            kmf_make_mpeg->execute();

            if (kmf_make_mpeg->exitStatus() != 0) {
                return false;
            }

            return true;
        }

        QImage*layer(KMFWidget::Layer l)
        {
            switch (l) {
                    case KMFWidget::Sub:
                    return &m_sub;

                    case KMFWidget::Highlight:
                    return &m_subHighlight;

                    case KMFWidget::Select:
                    return &m_subSelect;

                    case KMFWidget::None:
                    case KMFWidget::Background:
                    default:
                    return &m_background;
            }

            return &m_background;
        }

    private:
        int m_modifiedLayers;
        QImage m_background;
        QImage m_sub;
        QImage m_subHighlight;
        QImage m_subSelect;
        QString m_sound;
};

KMFMenuPage::KMFMenuPage(QObject *parent) : KMFWidget(parent), m_mob(0), m_language("en"),
                                            m_titles(0), m_chapters(0), m_titleStart(0),
                                            m_chapterStart(0), m_index(0), m_titleset(0),
                                            m_titlesetCount(0), m_count(0), m_vmgm(false),
                                            m_directPlay(false), m_directChapterPlay(false),
                                            m_continueToNextTitle(true)
{
    m_buttons = new QList<KMFButton *>;
    setResolution(KMF::Tools::maxResolution(m_interface->projectType()));
}

KMFMenuPage::~KMFMenuPage()
{
    qDeleteAll(*m_buttons);
    delete m_buttons;
}

void KMFMenuPage::fromXML(const QDomElement &element)
{
    KMFWidget::fromXML(element);
    m_titles = element.attribute("titles", "0").toInt();
    m_chapters = element.attribute("chapters", "0").toInt();
    QString s;

    if (m_titles > 0) {
        s = QString("%1").arg((m_titleStart / m_titles) + 1);
    } else if (m_chapters > 0) {
        s = QString("%1_%2").arg(m_titleStart + 1)
            .arg((m_chapterStart / m_chapters) + 1);
    } else if (m_chapters > 0) {
        s = QString("%1_%2").arg(m_titleStart + 1)
            .arg(m_chapterStart + 1);
    }

    QString n = QString("%1_%2").arg(objectName()).arg(s);
    setObjectName(n);
}

void KMFMenuPage::setResolution(QSize resolution)
{
    KMFUnit::setMaxRes(resolution);
    m_resolution = resolution;
    geometry().left().set(0, KMFUnit::Absolute);
    geometry().top().set(0, KMFUnit::Absolute);
    geometry().width().set(resolution.width(), KMFUnit::Absolute);
    geometry().height().set(resolution.height(), KMFUnit::Absolute);
}

bool KMFMenuPage::parseButtons(bool addPages)
{
    // kDebug();
    foreach (KMFButton * btn, *m_buttons) {
        if (btn->parseJump(addPages) == false) {
            return false;
        }
    }
    foreach (KMFButton * btn, *m_buttons) {
        btn->parseDirections();
    }
    return true;
}

void KMFMenuPage::writeDvdAuthorXml(QDomElement &element, QString type) const
{
    QDomDocument doc = element.ownerDocument();
    QDomElement pgc = doc.createElement("pgc");
    QDomElement pre = doc.createElement("pre");
    QDomText text = doc.createTextNode("");

    checkDummyVideo();

    if (isVmgm()) {
        if (isFirst()) {
            pgc.setAttribute("entry", "title");
        }

        QString s = "\n    {\n";

        // Direct play
        if (isFirst()) {
            s += "      if (g3 gt 0)\n"
                 "      {\n"
                 "        g7 = g3;\n"
                 "        g3 = 0;\n"
                 "        g1 = 1;\n";

            for (int i = 1; i <= m_titlesetCount; ++i) {
                s += QString("        if(g7 eq %1)\n").arg(i);
                s += "        {\n";
                s += QString("          jump titleset %1 menu; \n").arg(i);
                s += "        }\n";
            }

            s += "      }\n";
        }

        // Jump to saved page if necessary
        if (isFirst()) {
            for (int i = 2; i <= m_count; ++i) {
                s += QString("      if (g5 eq %1)\n"
                             "        jump menu %2;\n").arg(i).arg(i);
            }
        }

        // Highlight saved button
        s += "      if (g2 lt 1024)\n"
             "        button = 1024;\n"
             "      else\n"
             "        button = g2;\n";
        // Reset titleset page index
        s += QString("      g6 = 0;\n");
        // Reset titleset page button
        s += QString("      g4 = 0;\n");
        // Reset played chapter
        s += QString("      g0 = 0;\n");
        // Close code
        s += "    }\n    ";
        text.setData(s);
    } else   {
        if (isFirst()) {
            pgc.setAttribute("entry", "root");
        }

        QString s = "\n    {\n";

        // If chapter specified jump there
        if (isFirst()) {
            s += "      if (g1 gt 0)\n"
                 "      {\n"
                 "        g0 = g1;\n"
                 "        g1 = 0;\n"
                 "        jump title 1 chapter 1;\n"
                 "      }\n";
        }

        // Jump to saved page if necessary
        if (isFirst()) {
            for (int i = 2; i <= m_count; ++i) {
                s += QString("      if (g6 eq %1)\n"
                             "        jump menu %2;\n").arg(i).arg(i);
            }
        }

        // Highlight saved button
        s += "      if (g4 lt 1024)\n"
             "        button = 1024;\n"
             "      else\n"
             "        button = g4;\n";
        // Close code
        s += "    }\n    ";
        text.setData(s);
    }

    pre.appendChild(text);
    pgc.appendChild(pre);

    pgc.setAttribute("pause", "0");
    QDomElement vob = doc.createElement("vob");
    QString file = QString("./menus/%1.mpg").arg(objectName());
    vob.setAttribute("file", file);
    vob.setAttribute("pause", "inf");
    pgc.appendChild(vob);

    foreach(KMFButton * btn, *m_buttons) {
        if (btn->isHidden()) {
            continue;
        }

        QDomElement button = doc.createElement("button");
        button.setAttribute("name", btn->objectName());
        QDomText text = doc.createTextNode("");
        btn->writeDvdAuthorXml(text, type, m_mob);
        button.appendChild(text);
        pgc.appendChild(button);
    }
    element.appendChild(pgc);
}

void KMFMenuPage::checkDummyVideo() const
{
    QFileInfo fi(m_interface->projectDir("media") + "dummy.mpg");

    if (!fi.exists()) {
        QImage temp;
        QString size;

        if (m_interface->projectType() == "DVD-PAL") {
            temp = QImage(720, 576, QImage::Format_RGB32);
        } else {
            temp = QImage(720, 480, QImage::Format_RGB32);
        }

        temp.fill(QColor("black").rgba());
        // PNM P6 256
        temp.save(m_interface->projectDir("media") + "dummy.pnm", "PPM");
        QString sound = KStandardDirs::locate("data", "kmediafactory/media/silence.mp2");
        Run run(QString("kmf_make_mpeg %1 %2 %3 %4")
                .arg(m_interface->projectType())
                .arg(KMF::Tools::frames(m_interface->projectType()))
                .arg("dummy")
                .arg(sound), m_interface->projectDir("media"));
    }
}

void KMFMenuPage::writeDvdAuthorXmlNoMenu(QDomElement &element) const
{
    QDomDocument doc = element.ownerDocument();
    QDomElement pgc = doc.createElement("pgc");
    QDomElement pre = doc.createElement("pre");
    QDomText text = doc.createTextNode("");

    checkDummyVideo();
    pgc.setAttribute("entry", "root");

    QString s = "\n    {\n";
    // If chapter specified jump there
    s += "      if (g1 gt 0)\n"
         "      {\n"
         "        g1 = 0;\n"
         "        jump title 1 chapter 1;\n"
         "      }\n"
         "      else\n"
         "      {\n"
         "        jump vmgm menu ;\n"
         "      }\n";
    // Close code
    s += "    }\n    ";

    text.setData(s);
    pre.appendChild(text);
    pgc.appendChild(pre);

    pgc.setAttribute("pause", "0");
    QDomElement vob = doc.createElement("vob");
    QString file = "./media/dummy.mpg";
    vob.setAttribute("file", file);
    vob.setAttribute("pause", "inf");
    pgc.appendChild(vob);
    element.appendChild(pgc);
}

KMFButton *KMFMenuPage::button(const QString &name)
{
    // We can't search and loop in the same list concurrently
    QList<KMFButton *> temp = *m_buttons;
    foreach (KMFButton * btn, temp) {
        if (!btn->isHidden() && (btn->objectName() == name)) {
            return btn;
        }
    }
    return 0;
}

void KMFMenuPage::setProperty(const QString &name, QVariant value)
{
    KMFWidget::setProperty(name, value);

    if (name == "language") {
        // kDebug() << "Setting language";
        m_language = value.toString();
        menu()->setLanguage("template", m_language);
    } else if (name == "direct_play")    {
        m_directPlay = value.toBool();
    } else if (name == "direct_chapter_play") {
        m_directChapterPlay = value.toBool();
    } else if (name == "sound") {
        m_sound = value.toString();
        kDebug() << "***************************************" << m_sound;
    } else if (name == "continue_to_next_title") {
        m_continueToNextTitle = value.toInt();
    }
}

bool KMFMenuPage::isUpToDate(const QString &type) const
{
    if (type != m_interface->lastSubType()) {
        return false;
    }

    QDateTime lastModified = m_interface->lastModified(KMF::Any);
    QFileInfo fi(m_interface->projectDir("menus") + objectName() + ".mpg");

    if ((fi.exists() == false) || (lastModified > fi.lastModified())) {
        return false;
    }

    return true;
}

KMF::Job *KMFMenuPage::job(const QString &type) const
{
    if (!isUpToDate(type)) {
        KMFMenuPageJob *job = new KMFMenuPageJob(*this);
        job->menuDir = m_interface->projectDir("menus");
        job->projectType = m_interface->projectType();
        m_interface->setModified(KMF::Template);
        return job;
    }

    return 0;
}

QImage KMFMenuPage::preview()
{
    parseButtons(false);
    KMFMenuPageJob *j = static_cast<KMFMenuPageJob *>(job("preview"));
    j->init();
    j->paint();
    QImage img = *j->layer(KMFWidget::Background);
    delete j;
    return img;
}

#include "kmfmenupage.moc"
