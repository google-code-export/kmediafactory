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

#include "videoobject.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtGui/QColor>
#include <QtGui/QImage>
#include <QtGui/QPixmap>

#include <KAction>
#include <KActionCollection>
#include <KApplication>
#include <KDebug>
#include <KFileItem>
#include <KIcon>
#include <KLocale>
#include <KMimeType>
#include <KProcess>
#include <KRun>
#include <KStandardDirs>
#include <KStringHandler>
#include <KUrl>
#include <KIO/NetAccess>

#include <errno.h>
#include <unistd.h>

#include <kmfmediafile.h>
#include <kmftime.h>
#include <kmftools.h>
#include <videopluginsettings.h>
#include <kmediafactory/job.h>
#include "videooptions.h"
#include "videoplugin.h"

static const char startString[] = I18N_NOOP("Subtitles for: %1");

const char *VideoObject::m_prefixes[] = { ".sub.mpg", ".mpg", "", ".xml", ".sub"};

class ConvertSubtitlesJob : public KMF::Job
{
    public:
        QDVD::Subtitle subtitle;
        QString subtitleFile;
        QString subtitleXmlFile;
        QString videoFile;
        QString videoFileWithSubtitles;
        QString mediaDir;
        QString type;

        void run()
        {
            message(msgId(), KMF::Start, i18n("Subtitles for: %1", QFileInfo(videoFile).fileName()));

            QStringList subtitleFiles = subtitle.file().split(';');

            writeSpumuxXml();
            CHECK_IF_ABORTED();

            KProcess *spumux = process(msgId(), "INFO: \\d+ bytes of data written",
                    KProcess::OnlyStderrChannel);
            *spumux << "spumux" << "-P" << subtitleXmlFile;
            spumux->setStandardInputFile(videoFile);
            spumux->setStandardOutputFile(videoFileWithSubtitles);
            spumux->setWorkingDirectory(mediaDir);

            // kDebug() << "spumux" << "-P" << subtitleXmlFile << "<" << videoFile <<
            //                                                   ">" << videoFileWithSubtitles;
            QFileInfo info(videoFile);
            setMaximum(msgId(), info.size() / 1024);
            lastUpdate = 0;
            half = info.size() / 200;
            spumux->execute();

            if ((spumux->exitCode() != QProcess::NormalExit) || (spumux->exitStatus() != 0)) {
                QFile::remove(videoFileWithSubtitles);
                message(msgId(), KMF::Error, i18n("Conversion error."));
            }

            message(msgId(), KMF::Done);
        }

        void writeSpumuxXml()
        {
            QDomDocument doc("");
            QDomElement root = doc.createElement("subpictures");
            QDomElement stream = doc.createElement("stream");
            QDomElement textsub = doc.createElement("textsub");

            textsub.setAttribute("filename", subtitleFile);
            textsub.setAttribute("vertical-alignment", subtitle.verticalAlign());
            textsub.setAttribute("horizontal-alignment", subtitle.horizontalAlign());
            textsub.setAttribute("left-margin", subtitle.leftMargin());
            textsub.setAttribute("right-margin", subtitle.rightMargin());
            textsub.setAttribute("top-margin", subtitle.topMargin());
            textsub.setAttribute("bottom-margin", subtitle.bottomMargin());
            textsub.setAttribute("movie-width", "720");
            textsub.setAttribute("characterset", subtitle.encoding());

            if (type == "DVD-PAL") {
                textsub.setAttribute("movie-fps", "25");
                textsub.setAttribute("movie-height", "574");
            } else   {
                textsub.setAttribute("movie-fps", "29.97");
                textsub.setAttribute("movie-height", "478");
            }

            QFont font(subtitle.font());
            QString fontFile = KMF::Tools::fontFile(font);

            if (!fontFile.isEmpty()) {
                textsub.setAttribute("font", checkFontFile(fontFile));
            }

            if (subtitle.font().pointSize() > 0) {
                textsub.setAttribute("fontsize", subtitle.font().pointSize());
            }

            stream.appendChild(textsub);
            root.appendChild(stream);
            doc.appendChild(root);

            // Write spumux xml
            KMF::Tools::saveString2File(subtitleXmlFile, doc.toString(), false);
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

        QString checkFontFile(const QString &file)
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

VideoObject::VideoObject(QObject *parent)
    : MediaObject(parent)
    , m_videoPlay(0)
    , m_aspect(QDVD::VideoTrack::Aspect_Unknown)
    , m_spumux(0)
{
    setObjectName("video");
    m_videoProperties = new KAction(KIcon("document-properties"), i18n("Properties"), this);
    m_videoProperties->setProperty("hover-action", true);
    plugin()->actionCollection()->addAction("video", m_videoProperties);
    connect(m_videoProperties, SIGNAL(triggered()), SLOT(slotProperties()));

    m_kmfplayer = KStandardDirs::findExe("kmediafactoryplayer");

    if (!m_kmfplayer.isEmpty()) {
        m_videoPlay = new KAction(KIcon("media-playback-start"),
                i18n("Play Video"), this);
        m_videoPlay->setShortcut(Qt::CTRL + Qt::Key_P);
        plugin()->actionCollection()->addAction("mob_play", m_videoPlay);
        connect(m_videoPlay, SIGNAL(triggered()), SLOT(slotPlayVideo()));
    }
}

VideoObject::~VideoObject()
{
}

QString VideoObject::information() const
{
    int numChapters(0);
    QStringList audios;
    QStringList subs;

    foreach (const QDVD::Cell& cell, m_cells) {
        if (cell.isChapter()) {
            numChapters++;
        }
    }
    foreach (const QDVD::AudioTrack& audio, m_audioTracks) {
        audios.append(audio.language());
    }
    foreach (const QDVD::Subtitle &sub, m_subtitles) {
        subs.append(sub.language());
    }
    QSize res = KMFMediaFile::mediaFile(m_files[0]).resolution();
    QString s;
    s = "<html><body><table><tr><td>";
    s += i18n("Resolution:");
    s += "</td><td>";
    s += QString("%1x%2").arg(res.width()).arg(res.height());
    s += "</td><td width=\"20\"></td><td>";
    s += i18n("Chapters:");
    s += "</td><td>";
    s += QString::number(numChapters);
    s += "</td><td width=\"20\"></td><td>";
    s += i18n("Audio Tracks:");
    s += "</td><td>";
    s += audios.join(", ");
    s += "</td></tr><tr><td>";
    s += i18n("Aspect Ratio:");
    s += "</td><td>";
    s += QDVD::VideoTrack::aspectRatioString(aspect());
    s += "</td><td width=\"20\"></td><td>";
    s += i18n("Length:");
    s += "</td><td>";
    s += KGlobal::locale()->formatTime(duration(), true, true);
    s += "</td><td width=\"20\"></td><td>";
    s += i18n("Subtitles:");
    s += "</td><td>";
    s += subs.join(", ");
    s += "</td></tr></table></body></html>";
    return s;
}

double VideoObject::frameRate() const
{
    return KMFMediaFile::mediaFile(m_files[0]).frameRate();
}

QTime VideoObject::duration() const
{
    KMF::Time result;

    for (QStringList::ConstIterator it = m_files.begin();
         it != m_files.end(); ++it)
    {
        result += duration(*it);
    }

    return result;
}

QTime VideoObject::duration(QString file) const
{
    return KMFMediaFile::mediaFile(file).duration();
}

QTime VideoObject::chapterTime(int chap) const
{
    return chapter(chap).start();
}

int VideoObject::chapterId(int chapter) const
{
    int i = 0;
    int adjust = 0;

    for (QDVD::CellList::ConstIterator it = m_cells.begin(); it != m_cells.end(); ++it) {
        if ((*it).isChapter()) {
            if ((*it).isHidden()) {
                adjust++;
            } else {
                i++;
            }
        }

        if (i == chapter) {
            break;
        }
    }

    return chapter + adjust;
}

QStringList VideoObject::files() const
{
    return m_files;
}

QString VideoObject::fileName() const
{
    return m_files.first();
}

void VideoObject::actions(QList<QAction *> *actionList) const
{
    if (m_videoPlay) {
        actionList->append(m_videoPlay);
    }

    actionList->append(m_videoProperties);
}

bool VideoObject::fromXML(const QDomElement &element)
{
    m_cells.clear();
    m_audioTracks.clear();
    QDomNode n = element.firstChild();
    bool parseLengths = false;

    while (!n.isNull()) {
        QDomElement e = n.toElement();

        if (!e.isNull()) {
            if (e.tagName() == "video") {
                m_id = e.attribute("id");
                setTitle(e.attribute("title"));
                setPreviewUrl(e.attribute("custom_preview"));

                if (e.hasAttribute("auto_chapters")) {
                    setCellSecs(e.attribute("auto_chapters").toDouble());
                }

                setAspect((QDVD::VideoTrack::AspectRatio)
                        e.attribute("aspect", "3").toInt());

                QDomNode m = e.firstChild();

                while (!m.isNull()) {
                    QDomElement e2 = m.toElement();

                    if (!e2.isNull()) {
                        if (e2.tagName() == "file") {
                            m_files.append(e2.attribute("path"));
                        } else if (e2.tagName() == "cell")    {
                            QDVD::Cell cell;
                            KMF::Time start(e2.attribute("start"));
                            KMF::Time length(e2.attribute("length"));
                            QString name = e2.attribute("name");
                            QString previewFile = e2.attribute("preview");
                            bool chapter = (e2.attribute("chapter") == "1");
                            bool hidden = (e2.attribute("hidden") == "1");

                            if (!e2.hasAttribute("length")) {
                                parseLengths = true;
                            }

                            int file = e2.attribute("file", 0).toInt();

                            if (file > 1) {
                                for (int i = 0; i < file - 1 && i < m_files.count(); ++i) {
                                    start += duration(m_files[i]);
                                }
                            }

                            // kDebug() << file  << ", " << start
                            //    << ", " << chapter;
                            if (e2.hasAttribute("name")) {
                                name = e2.attribute("name");
                            } else {
                                name = start.toString("h:mm:ss");
                            }

                            addCell(QDVD::Cell(start, length, name, chapter, hidden, previewFile));
                        } else if (e2.tagName() == "audio")    {
                            QDVD::AudioTrack a(e2.attribute("language",
                                                       VideoPluginSettings::defaultAudioLanguage()));
                            addAudioTrack(a);
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
                            s.setFile(e2.attribute("file"));
                            QString encoding = e2.attribute("encoding");

                            if (!encoding.isEmpty()) {
                                s.setEncoding(encoding);
                            }

                            s.setLanguage(e2.attribute("language",
                                            VideoPluginSettings::defaultSubtitleLanguage()));
                            QString a = e2.attribute("align",
                                    QString("%1").arg(Qt::AlignHCenter | Qt::AlignBottom));
                            s.setAlignment(QFlags<Qt::AlignmentFlag>(a.toInt()));
                            s.setFont(font);
                            if (e2.hasAttribute("margins")) {
                                QStringList margins=e2.attribute("margins").split(',', QString::SkipEmptyParts);

                                static const int constMarginMin=10;
                                static const int constMarginMax=250;
                                if(4==margins.count()) {
                                    int left=margins[0].toInt(),
                                        right=margins[1].toInt(),
                                        top=margins[2].toInt(),
                                        bottom=margins[3].toInt();

                                    if(left>=constMarginMin && left<=constMarginMax &&
                                       right>=constMarginMin && right<=constMarginMax &&
                                       top>=constMarginMin && top<=constMarginMax &&
                                       bottom>=constMarginMin && bottom<=constMarginMax) {
                                        s.setMargins(left, right, top, bottom);
                                    }
                                }
                            }
                            addSubtitle(s);
                        }
                    }

                    m = m.nextSibling();
                }
            }
        }

        n = n.nextSibling();
    }

    if (parseLengths) {
        parseCellLengths();
    }

    return checkObjectParams();
}

bool VideoObject::checkObjectParams()
{
    // kDebug() << VideoPluginSettings::defaultAudioLanguage();
    if (m_files.count() > 0) {
        const KMFMediaFile &media = KMFMediaFile::mediaFile(m_files[0]);

        while (m_audioTracks.count() < (int)media.audioStreams()) {
            addAudioTrack(
                    QDVD::AudioTrack(VideoPluginSettings::defaultAudioLanguage()));
        }

        if (m_cells.count() < 1) {
            setCellSecs(900.0);
        }

        if (m_id.isEmpty()) {
            generateId();
        }

        if (title().isEmpty()) {
            setTitleFromFileName();
        }

        if (m_aspect == QDVD::VideoTrack::Aspect_Unknown) {
            m_aspect = media.aspectRatio();
        }

        return true;
    }

    return false;
}

void VideoObject::toXML(QDomElement *element) const
{
    QDomDocument doc = element->ownerDocument();
    QDomElement video = doc.createElement("video");

    video.setAttribute("title", title());
    video.setAttribute("aspect", (int)m_aspect);
    video.setAttribute("id", m_id);

    if (m_previewUrl.isValid()) {
        video.setAttribute("custom_preview", m_previewUrl.prettyUrl());
    }

    foreach (const QString& file, m_files) {
        QDomElement e = doc.createElement("file");
        e.setAttribute("path", file);
        video.appendChild(e);
    }

    foreach (const QDVD::Cell& cell, m_cells) {
        QDomElement e = doc.createElement("cell");
        e.setAttribute("name", cell.name());
        e.setAttribute("start", KMF::Time(cell.start()).toString());
        e.setAttribute("length", KMF::Time(cell.length()).toString());
        e.setAttribute("chapter", cell.isChapter());
        e.setAttribute("hidden", cell.isHidden());
        if(!cell.previewFile().isEmpty())
            e.setAttribute("preview", cell.previewFile());
        video.appendChild(e);
    }

    foreach (const QDVD::AudioTrack& audio, m_audioTracks) {
        QDomElement e = doc.createElement("audio");
        e.setAttribute("language", audio.language());
        video.appendChild(e);
    }

    foreach (const QDVD::Subtitle& sub, m_subtitles) {
        QDomElement e = doc.createElement("subtitle");
        e.setAttribute("language", sub.language());
        e.setAttribute("encoding", sub.encoding());
        e.setAttribute("file", sub.file());
        e.setAttribute("align", (int)sub.alignment());
        QDomElement e2 = doc.createElement("font");
        KMF::Tools::fontToXML(sub.font(), &e2);
        e.appendChild(e2);
        video.appendChild(e);
    }

    element->appendChild(video);
}

QVariant VideoObject::writeDvdAuthorXml(QVariantList args) const
{
    QDomDocument doc;
    QString preferredLanguage = args[0].toString();
    int titleset = args[1].toInt();

    QDir dir(interface()->projectDir("media"));
    int audioTrack = 0; // First audio track
    int subTrack = 62; // Let player decide
    bool audioFound = false;
    bool subFound = false;
    int i;

    QDomElement titles = doc.createElement("titles");
    QDomElement video = doc.createElement("video");

    video.setAttribute("aspect", QDVD::VideoTrack::aspectRatioString(m_aspect));

    if (QDVD::VideoTrack::Aspect_16_9 == m_aspect) {
        video.setAttribute("widescreen", "nopanscan");
    }

    titles.appendChild(video);

    i = 0;

    foreach (const QDVD::AudioTrack& audio, m_audioTracks) {
        QDomElement audioElem = doc.createElement("audio");

        if ((audio.language() == preferredLanguage) && (audioFound == false)) {
            audioTrack = i;
            audioFound = true;
        }

        audioElem.setAttribute("lang", audio.language());
        titles.appendChild(audioElem);
        ++i;
    }

    i = 0;

    foreach (const QDVD::Subtitle& sub, m_subtitles) {
        QString lang = sub.language();

        if (lang.isEmpty()) {
            lang = "xx";
        }

        QDomElement sub = doc.createElement("subpicture");

        if ((lang == preferredLanguage) && (subFound == false)) {
            subFound = true;
            subTrack = i;
        }

        sub.setAttribute("lang", lang);
        titles.appendChild(sub);
        ++i;
    }

    QDomElement pgc = doc.createElement("pgc");
    QDomElement pre = doc.createElement("pre");
    QDomText text = doc.createTextNode("");
    QString commands = "";

    commands += QString("audio=%1; ").arg(audioTrack);

    if (!audioFound && (subTrack < 32)) {
        subTrack += 64; // All subtitles, not just forced ones
    }

    commands += QString("subtitle=%1; ").arg(subTrack);
    text.setData(QString(" { %1} ").arg(commands));
    pre.appendChild(text);
    pgc.appendChild(pre);

    // Add cells
    KMF::Time pos;
    bool open = false;
    QDomElement vob;
    QDVD::Cell cell;
    int file = 0;

    for (i = 0; i <= m_cells.count(); ++i) {
        if (!open) {
            vob = doc.createElement("vob");

            if (m_type != "dummy") {
                vob.setAttribute("file", videoFileFind(i));
            } else   {
                vob.setAttribute("file", dir.filePath("dummy.mpg"));
            }

            open = true;
        }

        if ((KMF::Time(cell.start()) > pos + duration(m_files[file])) ||
            (i >= m_cells.count()))
        {
            pos += duration(m_files[file]);
            pgc.appendChild(vob);

            if ((i >= m_files.count()) || (i >= m_cells.count())) {
                break;
            }

            open = false;
            ++file;
        }

        cell = m_cells[i];
        KMF::Time start(cell.start());
        KMF::Time end(cell.start());

        start -= pos;
        QDomElement c = vob.ownerDocument().createElement("cell");

        c.setAttribute("start", KMF::Time(start).toString());

        if (cell.length() == KMF::Time()) {
            c.setAttribute("end", "-1");
        } else {
            end -= pos;
            end += cell.length();
            c.setAttribute("end", end.toString());
        }

        c.setAttribute("chapter", cell.isChapter());
        vob.appendChild(c);
        // kDebug() << "Cell: " << start << ", " << end;
    }

    QString postString;
    int mobCount = interface()->mediaObjects().count();

    if ((titleset < mobCount) &&
        interface()->templateObject()->call("continueToNextTitle").toBool()) {
        postString = QString(" g3 = %1 ; ").arg(titleset + 1);
    }

    postString += " call vmgm menu 1 ; ";
    QDomElement post = doc.createElement("post");
    text = doc.createTextNode(postString);
    post.appendChild(text);
    pgc.appendChild(post);

    titles.appendChild(pgc);

    QVariant result;
    result.setValue(titles);
    return result;
}

QString VideoObject::videoFileName(int index, VideoFilePrefix prefix)
{
    QDir dir(interface()->projectDir("media"));
    QString file = QFileInfo(m_files[index]).fileName();

    /*
     * kDebug() << dir.filePath(QString("%1_%2")
     *  .arg(QString::number(index+1).rightJustify(3, '0'))
     *  .arg(file) + m_prefixes[prefix]);
     */
    return dir.filePath(QString("%1_%2")
            .arg(QString::number(index + 1).rightJustified(3, '0'))
            .arg(file) + m_prefixes[prefix]);
}

QString VideoObject::videoFileFind(int index, VideoFilePrefix prefixStart) const
{
    QDir dir(interface()->projectDir("media"));
    QString file = QFileInfo(m_files[index]).fileName();

    for (int i = prefixStart; i < PrefixEmpty; ++i) {
        QString s = dir.filePath(QString("%1_%2")
                .arg(QString::number(index + 1).rightJustified(3, '0'))
                .arg(file) + m_prefixes[i]);
        QFileInfo fi(s);

        if (fi.exists()) {
            // kDebug() << s;
            return s;
        }
    }

    // kDebug() << m_files()[index].fileName();
    return m_files[index];
}

bool VideoObject::prepare(const QString &type)
{
    interface()->message(msgId(), KMF::Start, i18n("Video: %1", title()));
    QString fileName;

    m_type = type;

    if (type != "dummy") {
        foreach (const QDVD::Subtitle & subtitle, m_subtitles) {
            if (!subtitle.file().isEmpty()) {
                QStringList subtitleFiles = subtitle.file().split(';');

                for (int i = 0; i < m_files.count(); ++i) {
                    if (i >= subtitleFiles.count()) {
                        break;
                    }

                    QFileInfo subtitleFile(subtitleFiles[i]);
                    QFileInfo videoFile(videoFileFind(i, PrefixMpg));
                    QFileInfo videoFileWithSubtitles(videoFileName(i, PrefixSub));

                    if (!videoFileWithSubtitles.exists() ||
                        (videoFile.lastModified() > videoFileWithSubtitles.lastModified()) ||
                        (subtitleFile.lastModified() > videoFileWithSubtitles.lastModified()))
                    {
                        ConvertSubtitlesJob *job = new ConvertSubtitlesJob();
                        job->subtitle = subtitle;
                        job->subtitleFile = subtitleFile.filePath();
                        job->subtitleXmlFile = videoFileName(i, PrefixXml);
                        job->videoFile = videoFile.filePath();
                        job->videoFileWithSubtitles = videoFileWithSubtitles.filePath();
                        job->mediaDir = interface()->projectDir("media");
                        job->type = interface()->projectType();
                        interface()->setModified(KMF::Media);
                        interface()->addJob(job);
                    } else   {
                        interface()->message(msgId(), KMF::Info,
                                i18n("Subtitle conversion seems to be up to date for %1",
                                        videoFile.fileName()));
                    }
                }
            }
        }
    }

    interface()->message(msgId(), KMF::Done);
    return true;
}

void VideoObject::slotProperties()
{
    QPointer<VideoOptions> dlg = new VideoOptions(kapp->activeWindow());

    dlg->setData(*this);

    if (dlg->exec()) {
        dlg->getData(*this);
        interface()->setDirty(KMF::Media);
    }
}

QPixmap VideoObject::pixmap() const
{
    if (m_thumbnail.isNull()) {
        m_thumbnail =
            QPixmap::fromImage(generatePreview(MainPreview, QSize(constIconSize, constIconSize))
                    .scaled(constIconSize, constIconSize, Qt::KeepAspectRatio));
    }

    if (m_thumbnail.isNull()) {
        m_thumbnail = KIO::pixmapForUrl(fileName());
    }

    return m_thumbnail;
}

#define PERCENT 1
#define LIGHT 50

bool VideoObject::isBlack(const QImage &img) const
{
    int pixelLimit = (img.height() * img.width() * PERCENT) / 100;
    int lightLimit = (255 * LIGHT) / 100;
    int pixel = 0;

    for (int y = 0; y < img.height(); y++) {
        for (int x = 0; x < img.width(); x++) {
            QRgb pix = img.pixel(x, y);

            if ((qRed(pix) > lightLimit) || (qGreen(pix) > lightLimit) ||
                (qBlue(pix) > lightLimit))
            {
                ++pixel;
            }

            if (pixel > pixelLimit) {
                return false;
            }
        }
    }

    return true;
}

QString VideoObject::videoFileName(KMF::Time *time) const
{
    foreach (const QString &file, m_files) {
        // kDebug() << *it;
        const KMFMediaFile &media = KMFMediaFile::mediaFile(file);

        if (*time <= KMF::Time(media.duration())) {
            return file;
        } else   {
            *time -= media.duration();
        }
    }
    return QString();
}

QImage VideoObject::getFrame(QTime time, QString frameFile) const
{
    bool ok = false;
    KMF::Time t = time;

    foreach (const QString &file, m_files) {
        // kDebug() << *it;
        const KMFMediaFile &media = KMFMediaFile::mediaFile(file);

        if (t <= KMF::Time(media.duration())) {
            media.frame(t, frameFile);
            ok = true;
            break;
        } else   {
            t -= media.duration();
        }
    }

    if (ok) {
        return QImage(frameFile);
    } else {
        return QImage();
    }
}

QImage VideoObject::preview(int chap) const
{
    return generatePreview(chap, QSize(0, 0));
}

QImage VideoObject::generatePreview(int chap, QSize desiredSize) const
{
    bool black = true;
    int counter;
    QImage img;
    QString cacheFile;

    if (chap == MainPreview) {
        if (m_previewUrl.isValid()) {
            img.load(m_previewUrl.path());
            return img;
        }
    }

    cacheFile=chapter(chap).previewFile();
    
    KMF::Time t;

    if(cacheFile.isEmpty()) {
        QDir dir(interface()->projectDir("media"));
        QString s;
        t = chapter(chap).start();
        cacheFile = dir.filePath(s.sprintf("%s_%s.pnm",
                        (const char *)m_id.toLocal8Bit(),
                        (const char *)t.toString().toLocal8Bit()));
    }

    if (img.load(cacheFile)) {
        return img;
    }

    counter = 0;

    while (black && counter < 60) {
        img = getFrame(t, cacheFile);
        black = isBlack(img);

        if (black) {
            kDebug() << "Black frame: " << t.toString();
        }

        t += VideoPluginSettings::blackFrameJump();
        ++counter;
    }
    QSize templateRatio = desiredSize.width() > 0
                          ? QSize(1, 1)
                          : (interface()->aspectRatio() == QDVD::VideoTrack::Aspect_4_3)
                          ? QSize(4, 3) : QSize(16, 9);
    QSize videoRatio = (aspect() == QDVD::VideoTrack::Aspect_4_3) ?
                       QSize(4, 3) : QSize(16, 9);
    QSize imageRatio = KMF::Tools::guessRatio(img.size(), videoRatio);
    QSize templateSize = desiredSize.width() > 0 ? desiredSize : KMF::Tools::maxResolution(
            interface()->projectType());
    QSize imageSize = img.size();
    QSize res = KMF::Tools::resolution(imageSize, imageRatio,
            templateSize, templateRatio);

    img = img.scaled(res, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    /*
     * kDebug() << "Template size: " << templateSize;
     * kDebug() << "Template ratio:" << templateRatio;
     * kDebug() << "Video ratio:" << videoRatio;
     * kDebug() << "Image size:" << imageSize;
     * kDebug() << "Image ratio:" << imageRatio;
     * kDebug() << "Final resolution:" << res;
     */
    if (!VideoPluginSettings::usePreviewCache()) {
        QFile::remove(cacheFile);
    }

    return img;
}

void VideoObject::checkForSubtitleFile(const QString &fileName)
{
    const char *constExt[] = { "srt", "sub", 0L }; // TODO: Subtitle extensions?

    for (int i = 0; constExt[i]; ++i) {
        QString subName(KMF::Tools::changeExt(fileName, constExt[i]));

        if (QFileInfo(subName).exists()) {
            QDVD::Subtitle s;
            s.setFile(subName);
            s.setLanguage(VideoPluginSettings::defaultSubtitleLanguage());
            s.setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
            s.setFont(QFont());
            addSubtitle(s);
        }
    }
}

QString VideoObject::text(int chap) const
{
    if (chap == MainTitle) {
        return title();
    } else {
        return chapter(chap).name();
    }
}

void VideoObject::setCellSecs(double secs)
{
    double d = KMF::Time(duration());
    int chapters = (int)(d / secs) + 1;

    // Do not put chapter too close to end
    if ((((double)chapters * secs) + 30.0 > d) && (chapters > 1)) {
        --chapters;
    }

    m_cells.clear();

    for (int i = 0; i < chapters; ++i) {
        KMF::Time length(secs);

        if (i == chapters - 1) {
            length = 0;
        }

        QDVD::Cell c(KMF::Time(i * secs), length,
                     i18n("Chapter %1", i + 1));
        addCell(c);
    }
}

int VideoObject::chapters() const
{
    int i = 0;

    foreach (const QDVD::Cell& cell, m_cells) {
        if (cell.isChapter() && !cell.isHidden()) {
            ++i;
        }
    }
    return i;
}

const QDVD::Cell &VideoObject::chapter(int chap) const
{
    int i = 0;

    foreach (const QDVD::Cell& cell, m_cells) {
        if (cell.isChapter() && !cell.isHidden()) {
            ++i;
        }
        if (i == chap) {
            return cell;
        }
    }
    return m_cells.first();
}

void VideoObject::generateId()
{
    int serial = interface()->serial();
    QString name = KMF::Tools::simpleBaseName(m_files.first());

    m_id.sprintf("%3.3d_%s", serial, (const char *)name.toLocal8Bit());
}

VideoObject::VideoFileStatus VideoObject::addFile(QString fileName)
{
    VideoFileStatus status =
            KMFMediaFile::mediaFile(fileName).dvdCompatible()
                ? KMF::Tools::isVideoResolution(KMFMediaFile::mediaFile(fileName).resolution())
                             ? StatusOk
                             : StatusInvalidResolution
                : StatusNonCompataible;

    if (StatusOk == status) {
        m_files.append(fileName);
        checkObjectParams();
        checkForSubtitleFile(fileName);
    }

    return status;
}

void VideoObject::setTitleFromFileName()
{
    QString name = KUrl(fileName()).fileName();

    setTitle(KMF::Tools::simple2Title(name));
}

void VideoObject::slotPlayVideo()
{
    KRun::runUrl(KUrl(fileName()), "video/mpeg", qApp->activeWindow());
}

void VideoObject::printCells()
{
    KMF::Time next;
    int i = 1;

    foreach (const QDVD::Cell& cell, m_cells) {
        kDebug() << i << ": " << cell.start() << " / " << cell.length();
    }
}

void VideoObject::parseCellLengths()
{
    KMF::Time next;

    for (QDVD::CellList::Iterator it = m_cells.begin(); it != m_cells.end(); ++it) {
        // kDebug() << (*it).start();
        ++it;
        if (it != m_cells.end()) {
            next = (*it).start();
        } else {
            next = QTime();
        }
        --it;
        if (!next.isNull()) {
            (*it).setLength(next - (*it).start());
        } else {
            (*it).setLength(QTime());
        }
    }
}

void VideoObject::setCellList(const QDVD::CellList &list)
{
    m_cells = list;

    if (m_cells.count() == 0) {
        m_cells.append(QDVD::Cell(QTime(), QTime(), QString("Chapter 1")));
    }
}

uint64_t VideoObject::size() const
{
    uint64_t total = 0;

    foreach (const QString& file, m_files) {
        KFileItem finfo(KFileItem::Unknown, KFileItem::Unknown, KUrl(file));
        total += finfo.size();
    }
    return total;
}

bool VideoObject::isDVDCompatible() const
{
    return KMFMediaFile::mediaFile(m_files[0]).dvdCompatible();
}

QMap<QString, QString> VideoObject::subTypes() const
{
    QMap<QString, QString> result;

    result[""] = i18n("Make DVD");
    result["dummy"] = i18n("Make preview DVD with dummy videos");
    return result;
}

#include "videoobject.moc"
