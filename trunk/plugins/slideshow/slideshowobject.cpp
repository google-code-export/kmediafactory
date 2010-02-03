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

#include "config.h"
#include "slideshowobject.h"

#include <QtCore/QDir>
#include <QtCore/QRegExp>
#include <QtCore/QSet>
#include <QtGui/QImage>
#include <QtGui/QPixmap>
#include <QtXml/QDomDocument>

#include <KAboutData>
#include <KAction>
#include <KActionCollection>
#include <KApplication>
#include <KDebug>
#include <KFileItem>
#include <KFileMetaInfo>
#include <KIcon>
#include <KLocale>
#include <KMessageBox>
#include <KMimeType>
#include <KProcess>
#include <KProgressDialog>
#include <KStandardDirs>
#ifdef HAVE_KEXIV2
#include <libkexiv2/kexiv2.h>
#endif

#include <errno.h>
#include <list>
#include <unistd.h>

#include <kmfmediafile.h>
#include <kmftime.h>
#include <kmftools.h>
#include <qdvdinfo.h>
#include <run.h>
#include <slideshowpluginsettings.h>
#include <kmediafactory/job.h>
#include "slideshowplugin.h"
#include "slideshowproperties.h"

class CopyOriginalsJob : public KMF::Job
{
    public:
        CopyOriginalsJob(){};

        QString destDir;
        QSet<QString> files;

        void run()
        {
            message(msgId(), KMF::Start, i18n("Copying slideshow originals"));
            setMaximum(msgId(), files.size());
            int i = 0;
            foreach (QString file, files) {
                if (!QFile::copy(file, destDir + QFileInfo(file).fileName())) {
                    message(msgId(), KMF::Error, i18n("Copying originals failed."));
                    return;
                }

                setValue(msgId(), ++i);
            }
            message(msgId(), KMF::Done);
        }

    private:
        QString t;
};

class SpumuxJob : public KMF::Job
{
    public:
        const SlideshowObject &slideshow;
        QString projectType;
        QString mediaDir;

        SpumuxJob(const SlideshowObject &s) : slideshow(s) {};

        void run()
        {
            message(msgId(), KMF::Start, i18n("Comments for: %1", slideshow.title()));

            QDir dir(mediaDir);
            QString output = dir.filePath(QString("%1.vob").arg(slideshow.id()));
            QString sub = dir.filePath(QString("%1.mlt.sub").arg(slideshow.id()));
            QString spumux = dir.filePath(QString("%1.mlt.spumux").arg(slideshow.id()));

            writeSpumuxXml(spumux, sub);
            CHECK_IF_ABORTED();

            KProcess *proc = process(msgId(), "INFO: \\d+ bytes of data written",
                    KProcess::OnlyStderrChannel);
            *proc << "spumux" << "-P" << spumux;
            proc->setStandardInputFile(output);
            proc->setStandardOutputFile(output + ".tmp");
            proc->setWorkingDirectory(mediaDir);

            QFileInfo info(output);
            setMaximum(msgId(), info.size() / 1024);
            lastUpdate = 0;
            half = info.size() / 200;
            proc->execute();

            if ((proc->exitCode() != QProcess::NormalExit) || (proc->exitStatus() != 0)) {
                QFile::remove(output + ".tmp");
                message(msgId(), KMF::Error, i18n("Failed to add comments."));
            } else   {
                QFile::remove(output);
                QFile::rename(output + ".tmp", output);
            }

            message(msgId(), KMF::Done);
        }

        void writeSpumuxXml(const QString &spumuxFile, const QString &subFile) const
        {
            QDomDocument doc("");
            QDomElement root = doc.createElement("subpictures");
            QDomElement stream = doc.createElement("stream");
            QDomElement textsub = doc.createElement("textsub");

            textsub.setAttribute("filename", subFile);
            textsub.setAttribute("vertical-alignment", slideshow.subtitleSettings().verticalAlign());
            textsub.setAttribute("horizontal-alignment",
                    slideshow.subtitleSettings().horizontalAlign());
            textsub.setAttribute("left-margin", 40);
            textsub.setAttribute("right-margin", 40);
            textsub.setAttribute("top-margin", 30);
            textsub.setAttribute("bottom-margin", 40);
            textsub.setAttribute("movie-width", "720");
            textsub.setAttribute("characterset", slideshow.subtitleSettings().encoding());

            if (projectType == "DVD-PAL") {
                textsub.setAttribute("movie-fps", "25");
                textsub.setAttribute("movie-height", "574");
            } else   {
                textsub.setAttribute("movie-fps", "29.97");
                textsub.setAttribute("movie-height", "478");
            }

            QFont font(slideshow.subtitleSettings().font());
            QString fontFile = KMF::Tools::fontFile(font);

            if (!fontFile.isEmpty()) {
                textsub.setAttribute("font", checkFontFile(fontFile));
            }

            if (slideshow.subtitleSettings().font().pointSize() > 0) {
                textsub.setAttribute("fontsize", slideshow.subtitleSettings().font().pointSize());
            }

            stream.appendChild(textsub);
            root.appendChild(stream);
            doc.appendChild(root);

            // Write spumux xml
            KMF::Tools::saveString2File(spumuxFile, doc.toString(), false);
        }

        void output(const QString &line)
        {
            QRegExp bytes("INFO: (\\d+) bytes of data written");

            if (bytes.indexIn(line) > -1) {
                qulonglong temp = bytes.cap(1).toULongLong();

                if (temp - lastUpdate > half) {
                    setValue(msgId(), temp / 1024);
                    lastUpdate = temp;
                }
            }
        }

        QString checkFontFile(const QString &file) const
        {
            QFileInfo fi(file);
            QDir dir(QDir::home().filePath(".spumux"));
            QFileInfo link(dir.absoluteFilePath(fi.fileName()));

            if (!dir.exists()) {
                dir.mkdir(dir.path());
            }

            // kDebug() << link.filePath() << " -> " << file;
            if (!link.exists()) {
                if (symlink(file.toLocal8Bit(), link.filePath().toLocal8Bit()) < 0) {
                    kDebug() << strerror(errno);
                }
            }

            return fi.fileName();
        }

    private:
        qulonglong lastUpdate;
        qulonglong half;
};

class SlideshowJob : public KMF::Job
{
    public:
        SlideshowJob(const SlideshowObject &s) : slideshow(s){};

        QString mediaDir;
        QString projectType;
        QString app;
        SlideshowPlugin::EBackend backend;
        bool wideScreen;
        const SlideshowObject &slideshow;

        void run()
        {
            message(msgId(), KMF::Start, i18n("Slideshow: %1", slideshow.title()));
            QDir dir(mediaDir);
            QString output = dir.filePath(QString("%1.vob").arg(slideshow.id()));

            if (writeSlideshowFile()) {
                CHECK_IF_ABORTED();
                KProcess *dvdslideshow = process(msgId(), "INFO: \\d+ bytes of data written");

                *dvdslideshow << app;

                if (SlideshowPlugin::BACKEND_MELT == backend) {
                    QString format = QLatin1String("dv_") +
                            QLatin1String("DVD-PAL" == projectType ? "pal" : "ntsc") +
                            QLatin1String(wideScreen ? "_wide" : "");
                    *dvdslideshow <<
                            "-progress" <<
                            dir.filePath(QString("%1.mlt").arg(slideshow.id())) <<
                            "-profile" <<
                            format <<
                            "-consumer" <<
                            QLatin1String("avformat:") + output <<
                            "progress=1" <<
                            "f=dvd" <<
                            "vcodec=mpeg2video" <<
                            QLatin1String(SlideshowPluginSettings::audioType() == 0 ?
                                    "acodec=mp2" : "acodec=ac3") <<
                            "b=5000k" <<
                            "maxrate=8000k" <<
                            "minrate=0" <<
                            "bufsize=1835008" <<
                            "mux_packet_s=2048" <<
                            "mux_rate=10080000" <<
                            "ab=192k" <<
                            "ar=48000" <<
                            QLatin1String("DVD-PAL" == projectType ? "s=720x576" : "s=720x480") <<
                            "g=15" <<
                            QLatin1String("profile=") + format;
                    // melt only ever seems to get up to 99%
                    setMaximum(msgId(), 99);
                    // Filter out melt's progress lines from the log...
                    setFilter(QLatin1String(
                            "^\\s*Current Frame:\\s+\\d+,\\s+percentage:\\s+\\d+\\s*$"));
                } else {
                    if (SlideshowPluginSettings::audioType() == 0) {
                        *dvdslideshow << "-mp2";
                    }

                    *dvdslideshow <<
                            "-o" << mediaDir <<
                            "-n" << slideshow.id() <<
                            "-f" << dir.filePath(QString("%1.slideshow").arg(slideshow.id()));

                    if (projectType == "DVD-PAL") {
                        *dvdslideshow << "-p";
                    }

                    foreach (const QString &audio, slideshow.audioFiles()) {
                        *dvdslideshow << "-a" << audio;
                    }
                }

                dvdslideshow->setWorkingDirectory(mediaDir);

                int exitCode = dvdslideshow->execute();

                switch (exitCode) {
                    case - 2:
                        message(msgId(), KMF::Error,
                            i18n("Failed to start dvd-slideshow application."));
                        break;

                    case - 1:
                        message(msgId(), KMF::Error, i18n("dvd-slideshow application crashed."));
                        break;

                    default:
                        if ((dvdslideshow->exitCode() != QProcess::NormalExit) || (exitCode != 0)) {
                            message(msgId(), KMF::Error,
                                    i18n("Slideshow error (%1).").arg(exitCode));
                        }
                }
            } else {
                message(msgId(), KMF::Error, i18n("Can't write slideshow file."));
            }

            message(msgId(), KMF::Done);
        }

        static inline int endFrame(int frame)
        {
            return frame - 1;
        }

        bool writeSlideshowFile() const
        {
            QDir dir(mediaDir);
            QString output =
                dir.filePath(QString(SlideshowPlugin::BACKEND_MELT == backend ? "%1.mlt"
                                : "%1.slideshow").arg(slideshow.id()));
            QFile file(output),
            subFile(output + ".sub");
            double duration = slideshow.calculatedSlideDuration();

            if (file.open(QIODevice::WriteOnly | QIODevice::Truncate) &&
                ((SlideshowPlugin::BACKEND_MELT != backend) ||
                 subFile.open(QIODevice::WriteOnly | QIODevice::Truncate)))
            {
                QTextStream ts(&file);

                if (SlideshowPlugin::BACKEND_MELT == backend) {
                    static const double constAudioFade = 3.0;
                    const double constPictureFade = duration > 4.0 ? 1.0 : duration / 3.0;

                    double fps = "DVD-PAL" == projectType ? 25.0 : (30000.0 / 1001.0),
                           totalDuration =
                        (slideshow.slides().count() * duration) + constPictureFade;
                    int    totalFrames = totalDuration * fps,
                           pictureFrames = duration * fps,
                           audioFadeFrames = constAudioFade * fps,
                           pictureFadeFrames = constPictureFade * fps;
                    bool   haveCaptions(false);

                    ts <<
                            "<?xml version=\'1.0\' encoding=\'utf-8\'?>" << endl <<
                            "<!--" << endl <<
                            "This file was made with " <<
                            KGlobal::mainComponent().aboutData()->programName() <<
                            " - " << KGlobal::mainComponent().aboutData()->version() << endl <<
                            "http://www.iki.fi/damu/software/kmediafactory/" << endl <<
                            "-->" << endl <<
                            "<mlt title=\"Anonymous Submission\" root=\"" <<
                            mediaDir << "\" >" << endl;

                    // Add producer that creates black background
                    ts <<
                        "  <producer in=\"0\" out=\"" << endFrame(totalFrames) <<
                        "\" id=\"black\" >" << endl <<
                        "    <property name=\"mlt_type\" >producer</property>" << endl <<
                        "    <property name=\"aspect_ratio\" >0</property>" << endl <<
                        "    <property name=\"resource\" >black</property>" << endl <<
                        "    <property name=\"mlt_service\" >colour</property>" << endl <<
                        "  </producer>" << endl <<
                        "  <playlist id=\"black_track\" >" << endl <<
                        "    <entry in=\"0\" out=\"" << endFrame(totalFrames) <<
                        "\" producer=\"black\" />" << endl <<
                        "  </playlist>" << endl;

                    if (slideshow.audioFiles().count()) {
                        int file = 0,
                            totalUsedAFrames = 0;
                        QString audioFiles, audioPlaylist;
                        QTextStream files(&audioFiles), playlist(&audioPlaylist);

                        playlist << "  <playlist id=\"audioPlaylist\" >" << endl;
                        foreach (const QString &audio, slideshow.audioFiles()) {
                            int  aFrames =
                                (int)(((double)KMF::Time(KMFMediaFile::mediaFile(audio).duration()))
                                        * fps),
                                 framesLeft = totalFrames - totalUsedAFrames,
                                 useFrames = aFrames > framesLeft ? framesLeft : aFrames;
                            bool last = (slideshow.audioFiles().count() - 1) == file ||
                                        ((totalUsedAFrames + aFrames) >= totalFrames);

                            // Note: Fade out this track if it is the last one, or its
                            // duration+fade would be at the end.
                            bool fade = last || (framesLeft < (aFrames + audioFadeFrames));

                            files <<
                                "  <producer in=\"0\" out=\"" << endFrame(useFrames) <<
                                "\" id=\"a_" << file << "\" >" << endl <<
                                "    <property name=\"mlt_type\" >producer</property>" << endl <<
                                "    <property name=\"aspect_ratio\" >0.000000</property>" <<
                                    endl <<
                                "    <property name=\"resource\" >" << audio << "</property>" <<
                                    endl <<
                                "    <property name=\"seekable\" >1</property>" << endl <<
                                "    <property name=\"av_bypass\" >0</property>" << endl <<
                                "    <property name=\"audio_index\" >0</property>" << endl <<
                                "    <property name=\"video_index\" >-1</property>" << endl <<
                                "    <property name=\"mlt_service\" >avformat</property>" <<
                                    endl <<
                                "  </producer>" << endl;

                            playlist << "    <entry in=\"0\" out=\"" << endFrame(useFrames) <<
                                    "\" producer=\"a_" << file << "\" >" << endl;

                            if (fade) {
                                // Fade out...
                                playlist <<
                                    "      <filter in=\"" <<
                                        (useFrames > audioFadeFrames ?
                                        useFrames - audioFadeFrames : 0) <<
                                    "\" out=\"" << endFrame(useFrames) <<
                                    "\" id=\"fadeout\" >" << endl <<
                                    "        <property name=\"track\" >0</property>" << endl <<
                                    "        <property name=\"window\" >75</property>" << endl <<
                                    "        <property name=\"max_gain\" >20dB</property>" <<
                                        endl <<
                                    "        <property name=\"mlt_type\" >filter</property>" <<
                                        endl <<
                                    "        <property name=\"mlt_service\" >volume</property>" <<
                                        endl <<
                                    "        <property name=\"tag\" >volume</property>" << endl <<
                                    "        <property name=\"gain\" >1</property>" << endl <<
                                    "        <property name=\"end\" >0</property>" << endl <<
                                    "      </filter>" << endl;
                            }

                            playlist << "    </entry>" << endl;
                            totalUsedAFrames += useFrames;

                            if ((totalUsedAFrames >= totalFrames) || fade) {
                                break;
                            }

                            file++;
                        }
                        ts << audioFiles << audioPlaylist << "  </playlist>" << endl;
                    }

                    // Pictures...
                    //     Pics 1, 3, 5, etc are placed on one track, and 2, 4, 6, etc on
                    // another...
                    int pic = 0;
                    QString track1Pics, track2Pics, track1Playlist, track2Playlist;
                    QTextStream t1pics(&track1Pics), t2pics(&track2Pics), t1playlist(
                            &track1Playlist), t2playlist(&track2Playlist),
                    sub(&subFile);
                    t1playlist << "  <playlist id=\"imagePlayList2\" >" << endl;
                    t2playlist << "  <playlist id=\"imagePlayList1\" >" << endl;
                    foreach (Slide slide, slideshow.slides()) {
                        QTextStream &pics = 0 == pic % 2 ? t1pics : t2pics,
                        &currentList = 0 == pic % 2 ? t1playlist : t2playlist,
                        &otherList = 0 == pic % 2 ? t2playlist : t1playlist;
                        bool        first = 0 == pic, last = (slideshow.slides().count() - 1) ==
                                                             pic;

                        if (!slide.comment.isEmpty()) {
                            QString comment(slide.comment);

                            comment.replace("<", "(");
                            comment.replace(">", ")");
                            comment.replace("{", "(");
                            comment.replace("}", ")");
                            comment.replace("\n", "|");
                            sub << '{' << (pic * pictureFrames) + pictureFadeFrames << "}{" <<
                            endFrame((pic + 1) * pictureFrames) << '}' <<
                            comment << endl;
                            haveCaptions = true;
                        }

                        pics << "  <producer in=\"0\" out=\"" <<
                            endFrame( pictureFrames + pictureFadeFrames) <<
                            "\" id=\"" << pic + 1 << "\" >" << endl <<
                            "    <property name=\"mlt_type\" >producer</property>" << endl <<
                            "    <property name=\"aspect_ratio\" >1</property>" << endl <<
                            "    <property name=\"resource\" >" << slide.picture << "</property>" <<
                                endl <<
                            "    <property name=\"progressive\" >1</property>" << endl <<
                            "    <property name=\"mlt_service\" >pixbuf</property>" << endl <<
                            "  </producer>" << endl;

                        currentList <<
                            "    <entry in=\"0\" out=\"" <<
                            endFrame(pictureFrames + pictureFadeFrames) <<
                            "\" producer=\"" << pic + 1 << "\" >" << endl;

                        if (first) {
                            // Fade in from black...
                            currentList <<
                                "       <filter in=\"0\" out=\"" <<
                                endFrame(pictureFadeFrames) <<
                                "\" id=\"fade_from_black\" >" << endl <<
                                "         <property name=\"track\" >0</property>" << endl <<
                                "         <property name=\"start\" >0</property>" << endl <<
                                "         <property name=\"mlt_type\" >filter</property>" << endl <<
                                "         <property name=\"mlt_service\" >brightness</property>" <<
                                    endl <<
                                "         <property name=\"tag\" >brightness</property>" << endl <<
                                "         <property name=\"end\" >1</property>" << endl <<
                                "       </filter>" << endl;
                        }

                        if (last) {
                            // Fade out to black...
                            currentList <<
                                "       <filter in=\"" << pictureFrames << " \" out=\"" <<
                                endFrame(pictureFrames + pictureFadeFrames) <<
                                "\" id=\"fade_to_black\" >" << endl <<
                                "         <property name=\"track\" >0</property>" << endl <<
                                "         <property name=\"start\" >1</property>" << endl <<
                                "         <property name=\"mlt_type\" >filter</property>" << endl <<
                                "         <property name=\"mlt_service\" >brightness</property>" <<
                                endl <<
                                "         <property name=\"tag\" >brightness</property>" << endl <<
                                "         <property name=\"end\" >0</property>" << endl <<
                                "       </filter>" << endl;
                        }

                        currentList << "    </entry>" << endl;
                        otherList   << "    <blank length=\"" <<
                            (first ? pictureFrames : pictureFrames - pictureFadeFrames) <<
                            "\" /> " << endl;
                        pic++;
                    }

                    ts << track2Pics <<
                        track2Playlist <<
                        "  </playlist>" << endl <<
                        track1Pics <<
                        track1Playlist <<
                        "  </playlist>" << endl;

                    // Add transitions between picures...
                    ts <<
                        "  <tractor title=\"Anonymous Submission\" " <<
                        "global_feed=\"1\" in=\"0\" out=\"" <<
                        endFrame(totalFrames) << "\" id=\"maintractor\" >" << endl <<
                        "    <track producer=\"black_track\" /> " << endl;
                    int v1 = 1, v2 = 2;

                    if (slideshow.audioFiles().count()) {
                        ts << "    <track hide=\"video\" producer=\"audioPlaylist\" /> " << endl;
                        v1++, v2++;
                    }

                    ts << "    <track producer=\"imagePlayList1\" /> " << endl <<
                        "    <track producer=\"imagePlayList2\" /> " << endl;

                    for (int i = 1; i < slideshow.slides().count(); ++i) {
                        ts <<
                            "    <transition in=\"" <<
                            (i * pictureFrames) << "\" out=\"" << endFrame(
                                    (i * pictureFrames) + pictureFadeFrames) <<
                            "\" id=\"transition" << i - 1 << "\" >" << endl <<
                            "      <property name=\"a_track\" >" << v1 << "</property>" << endl <<
                            "      <property name=\"b_track\" >" << v2 << "</property>" << endl <<
                            "      <property name=\"factory\" >loader</property>" << endl <<
                            "      <property name=\"mlt_type\" >transition</property>" << endl <<
                            "      <property name=\"mlt_service\" >luma</property>" << endl <<
                            "      <property name=\"automatic\" >1</property>" << endl <<
                            "      <property name=\"reverse\" >" <<
                            (i % 2 ? 1 : 0) << "</property>" << endl <<
                            "    </transition>" << endl;
                    }

                    ts << "  </tractor>" << endl <<
                    "</mlt>" << endl;
                } else   {
                    ts << QString(
                            "#**************************************************************\n"
                            "#\n"
                            "# This file was made with %1 - %2\n"
                            "# http://www.iki.fi/damu/software/kmediafactory/\n"
                            "# \n"
                            "#**************************************************************\n")
                    .arg(KGlobal::mainComponent().aboutData()->programName())
                    .arg(KGlobal::mainComponent().aboutData()->version());

                    ts << "background:0::black\n";
                    ts << "fadein:1\n";
                    foreach(Slide slide, slideshow.slides())
                    {
                        QString comment = slide.comment;

                        comment.replace(":", "\\:");
                        comment.replace("\n", " ");
                        ts << slide.picture << ":" << QString::number(duration, 'f', 2) <<
                        ":" << comment << "\n";

                        if (slide.picture != slideshow.slides().last().picture) {
                            ts << "crossfade:1\n";
                        }
                    }
                    ts << "fadeout:1\n";
                }

                file.close();

                return true;
            }

            return false;
        }

        void output(const QString &line)
        {
            if (SlideshowPlugin::BACKEND_MELT == backend) {
                QString indicator("percentage:");
                int pos = line.indexOf(indicator);

                if (pos > -1) {
                    setValue(msgId(), line.mid(pos + indicator.length() + 1).toInt());
                }
            } else   {
                QRegExp re2(" (\\d+)\\/(\\d+) ");
                int pos = re2.indexIn(line);

                if (pos > -1) {
                    // Maximum is eg. 6/5
                    // setMaximum(re2.cap(2).toInt() + 1);
                    // setValue(re2.cap(1).toInt() - 1);
                }
            }
        }

    private:
        QString t;
};

Slide::Slide()
    : chapter(true)
{
}

SlideshowObject::SlideshowObject(QObject *parent)
    : MediaObject(parent)
    , m_loop(false)
    , m_includeOriginals(true)
{
    setObjectName("slideshow");
    m_slideshowProperties = new KAction(KIcon("document-properties"),
            i18n("Properties"), this);
    m_slideshowProperties->setProperty("hover-action", true);
    m_slideshowProperties->setShortcut(Qt::CTRL + Qt::Key_W);
    plugin()->actionCollection()->addAction("mob_properties",
            m_slideshowProperties);
    connect(m_slideshowProperties, SIGNAL(triggered()), SLOT(slotProperties()));
    m_duration = SlideshowPluginSettings::slideDuration();
}

SlideshowObject::~SlideshowObject()
{
}

QString SlideshowObject::information() const
{
    return i18np("%1 Image (%2)", "%1 Images (%2)", m_slides.count(),
            KGlobal::locale()->formatTime(duration(), true, true));
}

SlideList SlideshowObject::slideList(QStringList list, QWidget *parent) const
{
    SlideList result;
    KProgressDialog dlg(parent);

    dlg.progressBar()->setMinimum(0);
    dlg.progressBar()->setMaximum(list.count());
    dlg.show();

    foreach (QString file, list) {
        KFileMetaInfo minfo(file, QString::null, KFileMetaInfo::ContentInfo);
        QString mime;
        QFileInfo fi(file);
        QDir dir(interface()->projectDir("media"));
        KMimeType::Ptr type = KMimeType::findByUrl(QUrl::fromLocalFile(file));

        dlg.setLabelText(i18n("Processing %1...", file));
        kapp->processEvents();

        if (fi.isDir()) {
            KMessageBox::error(kapp->activeWindow(),
                    i18n("Cannot add folder."));
            dlg.progressBar()->setValue(dlg.progressBar()->value() + 1);
            continue;
        }

        if (type) {
            mime = type->name();
        }

        kDebug() << mime;

        if (mime.startsWith("application/vnd.oasis.opendocument") ||
            mime.startsWith("application/vnd.sun.xml") ||
            (mime == "application/msexcel") ||
            (mime == "application/msword") ||
            (mime == "application/mspowerpoint"))
        {
            QString output = QString("%1.pdf").arg(id());
            QDir dir(interface()->projectDir("media"));
            output = dir.filePath(output);
            Run run(QString("kmf_oo2pdf \"%1\" \"%2\"").arg(file).arg(output));

            kDebug() << file << "->" << output;

            if (run.exitCode() == 0) {
                mime = "application/pdf";
                minfo = KFileMetaInfo(file, QString::null,
                        KFileMetaInfo::ContentInfo);
                file = output;
            }
        }

        if (mime == "application/pdf") {
            QString output = id() + "_%d.png";
            QDir dir(interface()->projectDir("media"));
            output = dir.filePath(output);
            Run run(QString("kmf_pdf2png \"%1\" \"%2\"").arg(file).arg(output));

            kDebug() << file << "->" << output;

            for (int i = 1; true; ++i) {
                Slide slide;
                QString fileNameTemplate = id() + "_%1.png";
                QString file = dir.filePath(QString(fileNameTemplate).arg(i));
                QFileInfo fi(file);

                if (fi.exists()) {
                    kDebug() << "Slide: " << i;
                    slide.comment = i18n("Page %1", i);
                    slide.picture = file;
                    result.append(slide);
                } else   {
                    break;
                }
            }
        } else   {
            Slide slide;

            kDebug() << minfo.keys();

            if (minfo.keys().contains("Comment") &&
                !minfo.item("Comment").value().toString().isEmpty())
            {
                slide.comment = minfo.item("Comment").value().toString();
            } else   {
                if (minfo.keys().contains("CreationDate")) {
                    slide.comment = minfo.item("CreationDate").value().toString();
                }

                if (minfo.keys().contains("CreationTime")) {
                    slide.comment += " " + minfo.item("CreationTime").value().toString();
                }
            }

            if (slide.comment.isEmpty()) {
#ifdef HAVE_KEXIV2
                QDateTime dt = KExiv2Iface::KExiv2(file).getImageDateTime();

                slide.comment =
                    dt.isNull() ? QFileInfo(file).fileName() : KGlobal::locale()->formatDateTime(
                            dt, KLocale::ShortDate, true);
#else
                Run run(QString("kmf_comment \"%1\"").arg(file));

                if (run.exitStatus() == 0) {
                    slide.comment = run.output();
                }
#endif
            }

            slide.comment = slide.comment.trimmed();
            slide.picture = file;
            result.append(slide);
            kapp->processEvents();
            dlg.progressBar()->setValue(dlg.progressBar()->value() + 1);
        }
    }
    int chapter = ((result.count() - 1) / 12) + 1;
    int i = 0;

    for (SlideList::Iterator it = result.begin(); it != result.end(); ++it) {
        (*it).chapter = (((i++) % chapter) == 0);
    }

    dlg.close();
    return result;
}

void SlideshowObject::addPics(QStringList lst, QWidget *parent)
{
    if (m_id.isEmpty()) {
        generateId();
    }

    m_slides = slideList(lst, parent);
}

void SlideshowObject::generateId()
{
    int serial = interface()->serial();
    QString name = KMF::Tools::simpleName(title());

    m_id.sprintf("%3.3d_%s", serial, (const char *)name.toLocal8Bit());
}

void SlideshowObject::toXML(QDomElement *element) const
{
    QDomDocument doc = element->ownerDocument();
    QDomElement slideshow = doc.createElement("slideshow");

    slideshow.setAttribute("title", title());
    slideshow.setAttribute("id", m_id);
    slideshow.setAttribute("duration", m_duration);
    slideshow.setAttribute("loop", m_loop);
    slideshow.setAttribute("include_originals", m_includeOriginals);

    for (QStringList::ConstIterator it = m_audioFiles.begin(); it != m_audioFiles.end(); ++it) {
        QDomElement e = doc.createElement("audio");
        e.setAttribute("path", *it);
        slideshow.appendChild(e);
    }

    for (SlideList::ConstIterator it = m_slides.begin(); it != m_slides.end(); ++it) {
        QDomElement e = doc.createElement("file");
        e.setAttribute("path", (*it).picture);
        e.setAttribute("comment", (*it).comment);
        e.setAttribute("chapter", (*it).chapter);
        slideshow.appendChild(e);
    }

    QDomElement e = doc.createElement("subtitle");
    e.setAttribute("language", m_subtitleSettings.language());
    e.setAttribute("encoding", m_subtitleSettings.encoding());
    e.setAttribute("align", (int)m_subtitleSettings.alignment());
    QDomElement e2 = doc.createElement("font");
    KMF::Tools::fontToXML(m_subtitleSettings.font(), &e2);
    e.appendChild(e2);
    slideshow.appendChild(e);
    element->appendChild(slideshow);
}

bool SlideshowObject::fromXML(const QDomElement &element)
{
    QDomNode n = element.firstChild();

    if (n.isNull()) {
        return false;
    }

    while (!n.isNull()) {
        QDomElement e = n.toElement();

        if (!e.isNull()) {
            if (e.tagName() == "slideshow") {
                m_id = e.attribute("id");
                setTitle(e.attribute("title"));
                m_duration = e.attribute("duration",
                        QString::number(SlideshowPluginSettings::slideDuration()))
                             .toDouble();
                m_loop = e.attribute("loop", "0").toInt();
                m_includeOriginals = e.attribute("include_originals", "1").toInt();
                QDomNode m = e.firstChild();

                while (!m.isNull()) {
                    QDomElement e2 = m.toElement();

                    if (!e2.isNull()) {
                        if (e2.tagName() == "file") {
                            Slide slide;

                            slide.picture = e2.attribute("path");
                            slide.comment = e2.attribute("comment");
                            slide.chapter = e2.attribute("chapter", "1").toInt();
                            m_slides.append(slide);
                        } else if (e2.tagName() == "audio")    {
                            m_audioFiles.append(e2.attribute("path"));
                        } else if (e2.tagName() == "subtitle")    {
                            QFont font;

                            QDomNode m2 = e2.firstChild();

                            while (!m2.isNull()) {
                                QDomElement e3 = m2.toElement();

                                if (!e3.isNull()) {
                                    if (e3.tagName() == "font") {
                                        font = KMF::Tools::fontFromXML(e3);
                                    }
                                }

                                m2 = m2.nextSibling();
                            }
                            QDVD::Subtitle s;
                            QString encoding = e2.attribute("encoding");

                            if (!encoding.isEmpty()) {
                                m_subtitleSettings.setEncoding(encoding);
                            }

                            m_subtitleSettings.setLanguage(e2.attribute("language",
                                            SlideshowPluginSettings::defaultSubtitleLanguage()));
                            QString a = e2.attribute("align",
                                    QString("%1").arg(Qt::AlignHCenter | Qt::AlignBottom));
                            m_subtitleSettings.setAlignment(QFlags<Qt::AlignmentFlag>(a.toInt()));
                            m_subtitleSettings.setFont(font);
                        }
                    }

                    m = m.nextSibling();
                }
            }
        }

        n = n.nextSibling();
    }

    if (m_id.isEmpty()) {
        generateId();
    }

    return true;
}

QPixmap SlideshowObject::pixmap() const
{
    if (m_slides.count() > 0) {
        if (m_thumbnail.isNull()) {
            m_thumbnail =
                QPixmap::fromImage(generatePreview(MainPreview, QSize(constIconSize, constIconSize))
                        .scaled(constIconSize, constIconSize, Qt::KeepAspectRatio));
        }

        return m_thumbnail;
    }

    return KIO::pixmapForUrl(KUrl(""));
}

void SlideshowObject::actions(QList<QAction *> *actionList) const
{
    actionList->append(m_slideshowProperties);
}

bool SlideshowObject::prepare(const QString &type)
{
    interface()->message(msgId(), KMF::Start, i18n("Slideshow: %1", title()));
    m_type = type;

    if (type != "dummy") {
        if (m_includeOriginals) {
            QSet<QString> files;
            QString       dest(interface()->projectDir("DVD/PICTURES"));
            QDir          destDir(dest);

            foreach (const Slide &slide, m_slides) {
                // Only use if it does not already exist in the destinaiton...
                if (QFileInfo(destDir.filePath(QFileInfo(slide.picture).fileName())).exists()) {
                    files.insert(slide.picture);
                }
            }

            if (files.count() > 0) {
                CopyOriginalsJob *job = new CopyOriginalsJob;
                job->destDir = dest;
                job->files = files;
                interface()->addJob(job);
            }
        }

        QDir dir(interface()->projectDir("media"));
        QString output = dir.filePath(QString("%1.vob").arg(id()));
        QFileInfo fio(output);

        bool uptodate = true;

        if (fio.exists()) {
            foreach (const Slide &slide, m_slides) {
                QFileInfo fi(slide.picture);

                if (fi.lastModified() > fio.lastModified()) {
                    uptodate = false;
                    break;
                }
            }
        } else   {
            uptodate = false;
        }

        if (!uptodate) {
            SlideshowJob *job = new SlideshowJob(*this);
            job->mediaDir = interface()->projectDir("media");
            job->projectType = interface()->projectType();
            job->app = static_cast<SlideshowPlugin *>(plugin())->app();
            job->backend = static_cast<SlideshowPlugin *>(plugin())->backend();
            job->wideScreen = QDVD::VideoTrack::Aspect_16_9 == interface()->aspectRatio();
            interface()->setModified(KMF::Media);
            interface()->addJob(job);

            if (SlideshowPlugin::BACKEND_MELT == job->backend) {
                foreach (Slide slide, m_slides) {
                    if (!slide.comment.isEmpty()) {
                        SpumuxJob *spumux = new SpumuxJob(*this);

                        spumux->mediaDir = interface()->projectDir("media");
                        spumux->projectType = interface()->projectType();
                        interface()->addJob(spumux, job);
                        break;
                    }
                }
            }
        } else   {
            interface()->message(msgId(), KMF::Info,
                    i18n("Slideshow \"%1\" seems to be up to date", title()));
        }
    }

    interface()->message(msgId(), KMF::Done);
    return true;
}

void SlideshowObject::clean()
{
    QStringList list;
    QString name = QString("%1").arg(m_id);

    list.append(name + ".vob");
    list.append(name + ".xml");

    if (SlideshowPlugin::BACKEND_MELT == static_cast<SlideshowPlugin *>(plugin())->backend()) {
        list.append(name + ".mlt");
        list.append(name + ".mlt.spumux");
        list.append(name + ".mlt.sub");
    } else   {
        list.append(name + ".slideshow");
    }

    list.append("dvd-slideshow.log");
    KMF::Tools::cleanFiles(interface()->projectDir("media"), list);
}

QVariant SlideshowObject::writeDvdAuthorXml(QVariantList args) const
{
    QDomDocument doc;
    QDir dir(interface()->projectDir("media"));
    QDomElement titles = doc.createElement("titles");
    QDomElement video = doc.createElement("video");
    QString preferredLanguage = args[0].toString();

    video.setAttribute("aspect",
            QDVD::VideoTrack::aspectRatioString(interface()->aspectRatio()));

    if (QDVD::VideoTrack::Aspect_16_9 == interface()->aspectRatio()) {
        video.setAttribute("widescreen", "nopanscan");
    }

    titles.appendChild(video);

    QDomElement audioElem = doc.createElement("audio");
    audioElem.setAttribute("lang", preferredLanguage);
    titles.appendChild(audioElem);

    QDomElement sub = doc.createElement("subpicture");

    QString subLang = SlideshowPlugin::BACKEND_MELT ==
                      static_cast<SlideshowPlugin *>(plugin())->backend()
                      ? m_subtitleSettings.language()
                      : preferredLanguage;

    if (subLang.isEmpty()) {
        subLang = preferredLanguage;
    }

    sub.setAttribute("lang", subLang);
    titles.appendChild(sub);

    QDomElement pgc = doc.createElement("pgc");
    QDomElement pre = doc.createElement("pre");
    QDomText text = doc.createTextNode("");
    QString commands = "";

    QDomElement vob = doc.createElement("vob");

    if (m_type != "dummy") {
        vob.setAttribute("file", dir.filePath(QString("%1.vob").arg(m_id)));

        QList<double> chapters;
        double start = 0.0;
        double duration = calculatedSlideDuration();

        foreach (Slide slide, m_slides) {
            if (slide.chapter) {
                chapters.append(start);
            }

            // Forward over first fade in (0.520 is added to be sure we are out
            // of fade)
            if (start == 0.0) {
                start = 0.520;
            }

            start += duration;
        }

        for (int i = 0; i < chapters.count(); ++i) {
            QDomElement c = vob.ownerDocument().createElement("cell");

            c.setAttribute("start", KMF::Time(chapters[i]).toString());

            if (i == chapters.count() - 1) {
                c.setAttribute("end", "-1");
            } else {
                c.setAttribute("end", KMF::Time(chapters[i + 1]).toString());
            }

            c.setAttribute("chapter", 1);
            vob.appendChild(c);
        }
    } else   {
        vob.setAttribute("file", dir.filePath("dummy.mpg"));
        QDomElement c = vob.ownerDocument().createElement("cell");

        c.setAttribute("start", KMF::Time().toString());
        c.setAttribute("end", "-1");
        c.setAttribute("chapter", 1);
        vob.appendChild(c);
    }

    pgc.appendChild(vob);

    if (m_loop) {
        QDomElement postElem = doc.createElement("post");
        QDomText text2;
        text2 = doc.createTextNode(" jump chapter 1 ; ");
        postElem.appendChild(text2);
        pgc.appendChild(postElem);
    }

    titles.appendChild(pgc);

    QVariant result;
    result.setValue(titles);
    return result;
}

QImage SlideshowObject::preview(int chap) const
{
    return generatePreview(chap, QSize(0, 0));
}

QImage SlideshowObject::generatePreview(int chap, QSize desiredSize) const
{
    QImage img(chapter(chap).picture);
    QSize templateRatio = desiredSize.width() > 0
                          ? QSize(1, 1)
                          : (interface()->aspectRatio() == QDVD::VideoTrack::Aspect_4_3)
                          ? QSize(4, 3) : QSize(16, 9);
    QSize imageRatio = KMF::Tools::guessRatio(img.size(), templateRatio);
    QSize templateSize = desiredSize.width() > 0 ? desiredSize : KMF::Tools::maxResolution(
            interface()->projectType());
    QSize imageSize = img.size();
    QSize res = KMF::Tools::resolution(imageSize, imageRatio,
            templateSize, templateRatio);

    kDebug() << res;
    img = img.scaled(res, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    return img;
}

QString SlideshowObject::text(int chap) const
{
    if (chap == MainTitle) {
        return title();
    } else {
        return chapter(chap).comment;
    }
}

int SlideshowObject::chapters() const
{
    int i = 0;

    foreach (Slide slide, m_slides) {
        if (slide.chapter) {
            ++i;
        }
    }
    return i;
}

const Slide &SlideshowObject::chapter(int chap) const
{
    int i = 0;

    foreach (const Slide &slide, m_slides) {
        if (slide.chapter) {
            ++i;
        }

        if (i == chap) {
            return slide;
        }
    }
    return m_slides.first();
}

uint64_t SlideshowObject::size() const
{
    QDir dir(interface()->projectDir("media"));
    QString output = dir.filePath(QString("%1.vob").arg(m_id));
    KFileItem finfo(KFileItem::Unknown, KFileItem::Unknown, KUrl(output));
    uint64_t size = finfo.size();

    if (size == 0) {
        if (SlideshowPlugin::BACKEND_MELT == static_cast<SlideshowPlugin *>(plugin())->backend()) {
            // A 1hr 25min and 30 second PAL slideshow was 1225431040bytes
            static const double constBytesPerSecond = 1225431040 / ((85 * 60) + 30);

            size = constBytesPerSecond * (double)KMF::Time(duration());
        } else   {
            double d = (double)KMF::Time(duration());
            size = (uint64_t)(d * 655415.35);
        }
    }

    if (m_includeOriginals) {
        // Include size of image files...

        // Store a set of the files that we've already processed, just in case there are
        // duplicates in the list!
        QSet<QString> processed;
        foreach (const Slide &slide, m_slides) {
            if (!processed.contains(slide.picture)) {
                KFileItem finfo(KFileItem::Unknown, KFileItem::Unknown, KUrl(slide.picture));

                size += finfo.size();
                processed.insert(slide.picture);
            }
        }
    }

    return size;
}

QTime SlideshowObject::audioDuration() const
{
    KMF::Time audioDuration = 0.0;

    foreach (QString file, m_audioFiles) {
        KMFMediaFile audio = KMFMediaFile::mediaFile(file);

        audioDuration += audio.duration();
    }
    return audioDuration;
}

QTime SlideshowObject::duration() const
{
    KMF::Time total;

    if (m_duration < 1.0) {
        total = audioDuration();
    } else {
        total = m_duration * m_slides.count();
    }

    return total;
}

QTime SlideshowObject::chapterTime(int chap) const
{
    KMF::Time total;
    int i = 0, n = 0;

    foreach (const Slide &slide, m_slides) {
        if (slide.chapter) {
            ++i;
        }

        ++n;

        if (chap == i) {
            break;
        }
    }
    total += calculatedSlideDuration() * n;
    return total;
}

void SlideshowObject::slotProperties()
{
    SlideshowProperties dlg(kapp->activeWindow(), SlideshowPlugin::BACKEND_MELT ==
                            static_cast<SlideshowPlugin *>(plugin())->backend());

    dlg.setData(*this);

    if (dlg.exec()) {
        clean();
        dlg.getData(*this);
        interface()->setDirty(KMF::Media);
    }
}

double SlideshowObject::calculatedSlideDuration() const
{
    double duration = slideDuration();

    if (duration < 1.0) {
        duration = KMF::Time(audioDuration());
        duration = (((duration - 1.0) / slides().count()) - 1.0);
    }

    return duration;
}

#include "slideshowobject.moc"
