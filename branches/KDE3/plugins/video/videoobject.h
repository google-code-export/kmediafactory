//**************************************************************************
//   Copyright (C) 2004 by Petri Damst�
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
#ifndef VIDEOOBJECT_H
#define VIDEOOBJECT_H

#include <kmf_stddef.h>
#include <kmediafactory/plugin.h>
#include "rect.h"
#include "font.h"
#include <kurl.h>
#include <kprocess.h>
#include <qdatetime.h>
#include <qptrlist.h>
#include <qdatetime.h>
#include <qdvdinfo.h>

class QDir;

namespace QDVD { class Subtitle; class Cell; };
namespace KMF { class Time; };

/**
*/
class VideoObject : public KMF::MediaObject
{
    Q_OBJECT
  public:
    enum { TotalPoints = 100 };
    enum VideoFilePrefix { PrefixSub = 0, PrefixMpg,
        PrefixEmpty, PrefixXml, PrefixMicroDVD };

    VideoObject(QObject* parent);
    virtual ~VideoObject();
    virtual void toXML(QDomElement& element) const;
    virtual void fromXML(const QDomElement& element);
    virtual void writeDvdAuthorXml(QDomElement& element,
                                   QString preferredLanguage,
                                   QString post, QString type);
    virtual bool make(QString type);
    virtual QMap<QString, QString> subTypes() const;
    virtual int timeEstimate() const;
    virtual QPixmap pixmap() const;
    virtual QImage preview(int chapter = MainPreview) const;
    virtual QString text(int chapter = MainTitle) const;
    virtual int chapters() const;
    virtual uint64_t size() const;

    const QDVD::CellList& cellList() const { return m_cells; };
    void setCellList(const QDVD::CellList& list);
    void addCell(const QDVD::Cell& chapter)
        { m_cells.append(chapter); };

    const QDVD::AudioList& audioTracks() const { return m_audioTracks; };
    void setAudioTracks(const QDVD::AudioList& streams)
        { m_audioTracks = streams; };
    void addAudioTrack(const QDVD::AudioTrack& audio)
        { m_audioTracks.append(audio); };

    const QDVD::SubtitleList& subtitles() const { return m_subtitles; };
    void setSubtitles(const QDVD::SubtitleList& subtitles)
        { m_subtitles = subtitles; };
    void addSubtitle(const QDVD::Subtitle& subtitle)
        { m_subtitles.append(subtitle); };

    virtual QDVD::VideoTrack::AspectRatio aspect() const { return m_aspect; };
    void setAspect(QDVD::VideoTrack::AspectRatio aspect) { m_aspect = aspect; };

    virtual void actions(QPtrList<KAction>& actionList) const;
    virtual bool addFile(QString fileName);
    void setTitleFromFileName();
    const KURL& previewURL() const { return m_previewURL; };
    void setPreviewURL(const KURL& previewURL) { m_previewURL = previewURL; };
    QImage getFrame(double time) const;
    const QString& id() const { return m_id; };

    double frameRate() const;
    virtual QTime duration() const;
    virtual QTime duration(QString file) const;
    virtual QTime chapterTime(int chapter) const;
    virtual QStringList files() const;
    QString fileName() const;
    void parseCellLengths();
    QString videoFileName(int index, VideoFilePrefix prefix);
    void printCells();
    bool isDVDCompatible() const;

  public slots:
    virtual void slotProperties();
    virtual void slotPlayVideo();
    virtual void clean() { };

  private slots:
    void output(KProcess* proc, char* buffer, int buflen);

  protected:
    mutable QDVD::CellList m_cells;
    QDVD::AudioList m_audioTracks;
    QDVD::SubtitleList m_subtitles;

    void checkObjectParams();

  private:
    QStringList m_files;
    KAction* m_videoProperties;
    KAction* m_videoPlay;
    KURL m_previewURL;
    QDVD::VideoTrack::AspectRatio m_aspect;
    QString m_id;
    QString m_buffer;
    QTime m_lastUpdate;
    bool m_stopped;
    QString m_kmfplayer;
    static char* m_prefixes[];

    void generateId();
    void setCellSecs(double secs);
    bool isBlack(const QImage& img) const;
    bool convertToDVD();
    bool convertSubtitles(const QDVD::Subtitle& subtitle);
    bool writeSpumuxXml(QDomDocument& doc, const QString& subFile,
                        const QDVD::Subtitle& subtitle);
    bool writeSpumuxXml(const QString& fileName, const QString& subFile,
                        const QDVD::Subtitle& subtitle);
    QString checkFontFile(const QString& file);
    void addCell(QDomElement& vob, const QDVD::Cell& cell,
                 const KMF::Time& fileStart);
    const QDVD::Cell& chapter(int chap) const;

    QString videoFileFind(int index,
                          VideoFilePrefix startPrefix = PrefixSub) const;
};

#endif // VIDEOOBJECT_H