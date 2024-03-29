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

#ifndef VIDEOOBJECT_H
#define VIDEOOBJECT_H

#include <QtCore/QDateTime>
#include <QtGui/QPixmap>

#include <KProcess>
#include <KUrl>

#include <kmf_stddef.h>
#include <qdvdinfo.h>
#include <rect.h>
#include <kmediafactory/plugin.h>

class QDir;
class QMediaFile;

namespace QDVD
{
class Subtitle;
class Cell;
}
namespace KMF
{
class Time;
}

class VideoObject : public KMF::MediaObject
{
    Q_OBJECT

    public:
        enum VideoFilePrefix {
            PrefixSub = 0, PrefixMpg,
            PrefixEmpty, PrefixXml, PrefixMicroDVD
        };
        enum VideoFileStatus { StatusOk, StatusInvalidResolution, StatusNonCompataible};

        VideoObject(QObject *parent);
        virtual ~VideoObject();
        virtual void toXML(QDomElement *element) const;
        virtual bool fromXML(const QDomElement &element);
        virtual bool prepare(const QString &type);

        const QDVD::CellList& cellList() const
        {
            return m_cells;
        };
        void setCellList(const QDVD::CellList &list);
        void addCell(const QDVD::Cell &chapter)
        {
            m_cells.append(chapter);
        };

        const QDVD::AudioList& audioTracks() const
        {
            return m_audioTracks;
        };
        void setAudioTracks(const QDVD::AudioList &streams)
        {
            m_audioTracks = streams;
        };
        void addAudioTrack(const QDVD::AudioTrack &audio)
        {
            m_audioTracks.append(audio);
        };

        const QDVD::SubtitleList&subtitles() const
        {
            return m_subtitles;
        };
        void setSubtitles(const QDVD::SubtitleList &subtitles)
        {
            m_subtitles = subtitles;
        };
        void addSubtitle(const QDVD::Subtitle &subtitle)
        {
            m_subtitles.append(subtitle);
        };

        virtual QDVD::VideoTrack::AspectRatio aspect() const
        {
            return m_aspect;
        };
        void setAspect(QDVD::VideoTrack::AspectRatio aspect)
        {
            m_aspect = aspect;
        };

        virtual void actions(QList<QAction *> *actionList) const;
        virtual VideoFileStatus addFile(QString fileName);
        void setTitleFromFileName();
        const KUrl&previewUrl() const
        {
            return m_previewUrl;
        };
        void setPreviewUrl(const KUrl &previewUrl)
        {
            m_previewUrl = previewUrl;
            m_thumbnail = QPixmap();
        };
        QImage getFrame(QTime time) const;
        const QString&id() const
        {
            return m_id;
        };
        QString information() const;

        double frameRate() const;
        virtual QStringList files() const;
        QString fileName() const;
        void parseCellLengths();
        QString videoFileName(int index, VideoFilePrefix prefix);
        QString videoFileName(KMF::Time *time) const;
        void printCells();
        bool isDVDCompatible() const;

    public slots:
        virtual void slotProperties();
        virtual void slotPlayVideo();
        // Help Kross plugin declaring these as slots
        virtual QPixmap pixmap() const;
        virtual QMap<QString, QString> subTypes() const;
        virtual QImage preview(int chapter = MainPreview) const;
        virtual QString text(int chapter = MainTitle) const;
        virtual int chapters() const;
        virtual uint64_t size() const;
        virtual QTime duration() const;
        virtual QTime chapterTime(int chapter) const;
        virtual int chapterId(int chapter) const;
        // KMF::Object::call slots
        QVariant writeDvdAuthorXml(QVariantList args) const;

    private:
        QImage generatePreview(int chapter, QSize desiredSize) const;
        void checkForSubtitleFile(const QString &fileName);

    protected:
        mutable QDVD::CellList m_cells;
        QDVD::AudioList m_audioTracks;
        QDVD::SubtitleList m_subtitles;

        bool checkObjectParams();
        QTime duration(QString file) const;

    private:
        QAction *m_videoProperties;
        QAction *m_videoPlay;
        KUrl m_previewUrl;
        QDVD::VideoTrack::AspectRatio m_aspect;
        QString m_id;
        bool m_stopped;
        QString m_kmfplayer;
        static const char *m_prefixes[];
        QStringList m_files;
        KProcess *m_spumux;
        QString m_type;
        mutable QPixmap m_thumbnail;

        void generateId();
        void setCellSecs(double secs);
        bool isBlack(const QImage &img) const;
        void addCell(QDomElement &vob, const QDVD::Cell &cell, const KMF::Time &fileStart);
        const QDVD::Cell&chapter(int chap) const;

        QString videoFileFind(int index, VideoFilePrefix startPrefix = PrefixSub) const;
};

#endif // VIDEOOBJECT_H
