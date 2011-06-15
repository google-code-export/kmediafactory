// **************************************************************************
//   Copyright (C) 2010 by Petri Damstén
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

#ifndef QDVDINFO_H
#define QDVDINFO_H

#include <config.h>

#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtGui/QFont>

#include <KLocale>

#undef _STDINT_H
#define __STDC_LIMIT_MACROS
#include <stdint.h>

#ifdef HAVE_LIBDVDREAD
#include <dvdread/ifo_read.h>
#endif

namespace QDVD
{

class KDE_EXPORT Languages
{
    public:
        Languages()
        {
        };
        ~Languages(){};

        static QString language(const QString &code);

        static QStringList languageIds();

    private:
        static void checkLanguages();

        static QStringList m_languageIds;
        static QMap<QString, QString> m_languages;
        static const char *LanguageStrings[][2];
};

class KDE_EXPORT Base
{
    public:
        enum {
            CELL, AUDIO, SUBTITLE, VIDEO, TITLE, INFO
        };

        Base()
        {
        };
        virtual ~Base(){};

        virtual QString toString() const = 0;

        virtual uint64_t size() const = 0;

        virtual uint rtti() const = 0;
};

class KDE_EXPORT Cell : public Base
{
    public:
        Cell() : m_cell(0), m_chapter(true), m_startSector(0), m_endSector(0)
        {
        };
        Cell(QTime start, QTime length, QString name, bool chapter = true, bool hidden =
                false, QString previewFile=QString()) : m_cell(0), m_chapter(chapter), m_hidden(hidden), m_start(start), m_length(
                                 length),
                         m_startSector(0), m_endSector(0), m_name(name), m_preview(previewFile)
        {
        };
#ifdef HAVE_LIBDVDREAD
        Cell(int cell, pgc_t * pgc);
#endif
        ~Cell(){};

        QTime start() const
        {
            return m_start;
        };
        QTime length() const
        {
            return m_length;
        };
        int startSector() const
        {
            return m_startSector;
        };
        int endSector() const
        {
            return m_endSector;
        };
        int cellNbr() const
        {
            return m_cell;
        };
        QString name() const
        {
            return m_name;
        };
        bool isChapter() const
        {
            return m_chapter;
        };
        bool isHidden() const
        {
            return m_hidden;
        };
        QString previewFile() const
        {
            return m_preview;
        }
        virtual uint64_t size() const;

        virtual QString toString() const;

        virtual uint rtti() const
        {
            return CELL;
        };

        void setLength(const QTime &length)
        {
            m_length = length;
        };
        void setStart(const QTime &start)
        {
            m_start = start;
        };
        void setName(const QString &name)
        {
            m_name = name;
        };
        void setHidden(bool h)
        {
            m_hidden = h;
        }
        void setPreviewFile(const QString &file)
        {
            m_preview = file;
        }

    private:
        int m_cell;
        bool m_chapter;
        bool m_hidden;
        QTime m_start;
        QTime m_length;
        int m_startSector;
        int m_endSector;
        QString m_name;
        QString m_preview;
};

typedef QList<Cell> CellList;

class KDE_EXPORT Track : public Base
{
    public:
        Track() : m_trackId(-1), m_position(-1), m_size(0)
        {
        };
        virtual ~Track(){};

        int trackId() const
        {
            return m_trackId;
        };
        int position() const
        {
            return m_position;
        };
        virtual uint64_t size() const
        {
            return m_size;
        };

        void setTrackId(int trackId)
        {
            m_trackId = trackId;
        };
        void setPosition(int position)
        {
            m_position = position;
        };
        void setSize(uint64_t size)
        {
            m_size = size;
        };
        virtual uint rtti() const = 0;

    private:
        int m_trackId;
        int m_position;
        uint64_t m_size;
};

class KDE_EXPORT Subtitle : public Track
{
    public:
        enum Type {
            Undefined = 0, Normal, Large, Children, Reserved1,
            Normal_CC, Large_CC, Children_CC, Reserved2,
            Forced, Reserved3, Reserved4, Reserved5, Director,
            Large_Director, Children_Director
        };

        explicit Subtitle(QString langCode = "en", QString file = QString());
#ifdef HAVE_LIBDVDREAD
        Subtitle(int pos, subp_attr_t * subp_attr,
            uint16_t subp_control, video_attr_t * video_attr);
#endif
        virtual ~Subtitle(){};

        QString languageString() const
        {
            return Languages::language(m_langCode);
        };
        QString language() const
        {
            return m_langCode;
        };
        void setLanguage(QString langCode)
        {
            m_langCode = langCode;
        };
        QString typeString() const
        {
            return i18n(subpType[m_type]);
        };
        Type type() const
        {
            return m_type;
        };
        virtual QString toString() const;

        virtual uint rtti() const
        {
            return SUBTITLE;
        };
        void setFile(QString file)
        {
            m_file = file;
        };
        QString file() const
        {
            return m_file;
        };
        void setFont(QFont font)
        {
            m_font = font;
        };
        QFont font() const
        {
            return m_font;
        };
        void setEncoding(const QString &encoding)
        {
            m_encoding = encoding;
        };
        QString encoding() const
        {
            return m_encoding;
        };
        void setAlignment(Qt::Alignment align)
        {
            m_align = align;
        };
        Qt::Alignment alignment() const
        {
            return m_align;
        };
        QString verticalAlign() const;

        QString horizontalAlign() const;

        int leftMargin() const
        {
            return m_leftMargin;
        }
        int rightMargin() const
        {
            return m_rightMargin;
        }
        int topMargin() const
        {
            return m_topMargin;
        }
        int bottomMargin() const
        {
            return m_bottomMargin;
        }
        void setMargins(int left, int right, int top, int bottom)
        {
            m_leftMargin=left;
            m_rightMargin=right;
            m_topMargin=top;
            m_bottomMargin=bottom;
        }
    protected:
        static const char *subpType[16];
        static const char *hAlign[4];
        static const char *vAlign[4];

    private:
        QString m_langCode;
        Type    m_type;
        QString m_file;
        QFont   m_font;
        Qt::Alignment m_align;
        QString m_encoding;
        int     m_leftMargin,
                m_rightMargin,
                m_topMargin,
                m_bottomMargin;
};

class KDE_EXPORT AudioTrack : public Track
{
    public:
        enum Type {
            Undefined = 0, Normal, Impaired, Comments1, Comments2
        };

        AudioTrack(QString langCode = "en");
#ifdef HAVE_LIBDVDREAD
        AudioTrack(audio_attr_t * audio_attr, uint16_t audio_control);
#endif
        virtual ~AudioTrack(){};

        QString languageString() const
        {
            return Languages::language(m_langCode);
        };
        QString language() const
        {
            return m_langCode;
        };
        QString formatString() const
        {
            return AudioFormat[m_format];
        };
        int sampleFreq() const
        {
            return m_sampleFreq;
        };
        QString quantizationString() const
        {
            return Quantization[m_quantization];
        };
        int channels() const
        {
            return m_channels;
        };
        int apMode() const
        {
            return m_apMode;
        };
        QString typeString() const
        {
            return i18n(AudioType[m_type]);
        };
        Type type() const
        {
            return m_type;
        };
        int bitrate() const
        {
            return m_bitrate;
        };
        virtual QString toString() const;

        virtual uint rtti() const
        {
            return AUDIO;
        };

        void setBitrate(int bitrate)
        {
            m_bitrate = bitrate;
        };
        void setLanguage(QString language)
        {
            m_langCode = language;
        };

    protected:
        static const char *AudioFormat[7];
        static const int SampleFreq[2];
        static const char *Quantization[4];
        static const char *AudioType[5];

    private:
        QString m_langCode;
        int m_format;
        int m_sampleFreq;
        int m_quantization;
        int m_channels;
        int m_apMode;
        Type m_type;
        int m_bitrate;
};

class KDE_EXPORT VideoTrack : public Track
{
    friend class Title;

    public:
        enum AspectRatio {
            Aspect_4_3 = 0, Aspect_16_9 = 1, Aspect_Unknown = 3
        };
        enum Format {
            NTSC = 0, PAL
        };

        VideoTrack() : m_fps(25.0), m_format(PAL), m_aspect(Aspect_16_9), m_width(720), m_height(
                               576), m_permittedDf(0)
        {
        };
#ifdef HAVE_LIBDVDREAD
        VideoTrack(pgc_t * pgc, video_attr_t * video_attr);
#endif
        virtual ~VideoTrack(){};

        double fps() const
        {
            return m_fps;
        };
        QString formatString() const
        {
            return VideoFormat[m_format];
        };
        QString aspectRatioString() const
        {
            return aspectRatioString(m_aspect);
        };
        AspectRatio aspectRatio() const
        {
            return ((m_aspect % 2 == 0) ? Aspect_4_3 : Aspect_16_9);
        };
        int width() const
        {
            return m_width;
        };
        int height() const
        {
            return m_height;
        };
        QString permittedDfString() const
        {
            return i18n(PermittedDf[m_permittedDf]);
        };
        virtual QString toString() const;

        QString aspectRatioString()
        {
            return AspectRatioString[m_aspect];
        };
        static QString aspectRatioString(AspectRatio aspect)
        {
            return AspectRatioString[aspect];
        };
        static QString aspectRatioString(uint aspect)
        {
            return AspectRatioString[aspect<4 ? aspect : 0];
        };
        virtual uint rtti() const
        {
            return VIDEO;
        };

    protected:
        static const double FramesPerS[4];
        static const char *VideoFormat[2];
        static const char *AspectRatioString[4];
        static const int VideoHeight[4];
        static const int VideoWidth[4];
        static const char *PermittedDf[4];

    private:
        double m_fps;
        int m_format;
        AspectRatio m_aspect;
        int m_width;
        int m_height;
        int m_permittedDf;
};

typedef QList<AudioTrack> AudioList;
typedef QList<Subtitle> SubtitleList;

class KDE_EXPORT Title : public Base
{
    friend class Info;

#ifdef HAVE_LIBDVDREAD
    public:
        Title(int titleNbr, pgc_t * pgc, int pgcNbr, title_info_t * title,
            vtsi_mat_t * vtsi_mat);

        static QTime dvdTime2QTime(dvd_time_t *dt);

        static int dvdTime2ms(dvd_time_t *dt);

    protected:
        bool parseTrackLengths(dvd_reader_t *dvd);

        bool parseAudioBitrates(dvd_reader_t *dvd);
#endif


    public:
        enum StreamType {
            UNKNOWN = -1, MPEG_VIDEO = 0, MPEG_AUDIO, AC3_AUDIO,
            DTS_AUDIO, LPCM_AUDIO, SUBPICTURE
        };

        Title() : m_length(QTime(0, 0)), m_vts(0), m_ttn(0), m_angles(0), m_titleNbr(0), m_pgc(0)
        {
        };
        virtual ~Title(){};

        QTime length() const
        {
            return m_length;
        };
        QString vtsId() const
        {
            return m_vtsId;
        };
        int vts() const
        {
            return m_vts;
        };
        int ttn() const
        {
            return m_ttn;
        };
        int pgcNbr() const
        {
            return m_pgc;
        };
        int angles() const
        {
            return m_angles;
        };
        int titleNbr() const
        {
            return m_titleNbr;
        };
        AudioTrack*audioById(int id);

        const AudioList&audioTracks() const
        {
            return m_audios;
        };
        const CellList&cells() const
        {
            return m_cells;
        };
        const SubtitleList&subtitles() const
        {
            return m_subtitles;
        };
        const VideoTrack&videoTrack() const
        {
            return m_video;
        };
        virtual QString toString() const;

        virtual uint64_t size() const;

        virtual uint rtti() const
        {
            return TITLE;
        };

        void setVideo(const VideoTrack &video)
        {
            m_video = video;
        };
        void addAudio(const AudioTrack &audio)
        {
            m_audios.append(audio);
        };
        void addCell(const Cell &cell)
        {
            m_cells.append(cell);
        };
        void addSubtitle(const Subtitle &subtitle)
        {
            m_subtitles.append(subtitle);
        };
        int startSector() const
        {
            return m_cells.first().startSector();
        };
        int endSector() const
        {
            return m_cells.last().endSector();
        };
        uint64_t overheadSize() const;

    protected:
        static const unsigned int ac3BitrateIndex[32];
        static const unsigned int dtsBitrateIndex[32];
        static const unsigned int mpaBitrateIndex[2][4][16];

        StreamType packetType(unsigned char *buffer);

        int MPEGBitrate(unsigned char *buffer);

        int AC3Bitrate(unsigned char *buffer);

        int DTSBitrate(unsigned char *buffer);

    private:
        QTime m_length;
        QString m_vtsId;
        int m_vts;
        int m_ttn;
        int m_angles;
        int m_titleNbr;
        int m_pgc;

        VideoTrack m_video;
        AudioList m_audios;
        SubtitleList m_subtitles;
        CellList m_cells;
};

typedef QList<Title> TitleList;

class KDE_EXPORT Info : public QObject, public Base
{
    Q_OBJECT
#ifdef HAVE_LIBDVDREAD
    public:
        explicit Info(const QString &device, QObject * parent = 0);

        bool parseDVD(const QString &device = "/dev/dvd");

    private:
        static bool getTitleName(const char *dvd_device, QString &title);
#endif
    public:
        Info(QObject * parent = 0) : QObject(parent)
        {
        };
        ~Info();

        void addTitle(const Title &title)
        {
            m_titles.append(title);
        };
        const TitleList&titles() const
        {
            return m_titles;
        };
        const QString&device() const
        {
            return m_device;
        };
        const QString&vmgIdentifier() const
        {
            return m_vmgIdentifier;
        };
        int longestTitle() const
        {
            return m_longestTitle;
        };
        QString providerIdentifier() const
        {
            return m_providerIdentifier;
        };
        QTime longestTitleLength() const
        {
            return m_longestTitleLength;
        };
        const QString&title() const
        {
            return m_title;
        };
        void clear();

        virtual uint64_t size() const;

        virtual QString toString() const;

        virtual uint rtti() const
        {
            return INFO;
        };

    signals:
        void titles(int);
        void title(int);

    private:
        QString m_title;
        QString m_device;
        QString m_vmgIdentifier;
        QString m_providerIdentifier;
        QTime   m_longestTitleLength;
        int     m_longestTitle;
        TitleList m_titles;
};

}

Q_DECLARE_METATYPE(const QDVD::Base *)

#endif
