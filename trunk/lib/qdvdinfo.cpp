// **************************************************************************
//   Copyright (C) 2010 by Petri Damstén
//   petri.damsten@iki.fi
//
//   This file is based on lsdvd.c, streamanalyze.c, dvdread.c
//
//   lsdvd:
//   2003  by Chris Phillips
//   2003-04-19  Cleanups get_title_name, added dvdtime2msec, added helper
//               macros, output info structures in form of a Perl module,
//               by Henk Vergonet.
//
//   streamanalyze:
//   Some parts of this source code were taken from the following projects:
//   transcode    by Dr. Thomas Oestreich
//   lsdvd        by Chris Phillips
//   mplex        by mjpeg-tools developers group
//
//   dvdread.c : DvdRead input module for vlc
//   Authors: St hane Borel <stef@via.ecp.fr>
//            Gildas Bazin <gbazin@videolan.org>
//
//   Copyright (C) 2001-2004 VideoLAN <videolan@videolan.org>
//   Copyright (C) 2003 EFF <information@eff.org>
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
//
// **************************************************************************

#include "qdvdinfo.h"

#include <QtCore/QFileInfo>

#include <KApplication>
#include <KDebug>
#include <KLocale>

#include <malloc.h>

#include "kmf_stddef.h"

namespace QDVD
{
const char *Languages::LanguageStrings[][2] = {
    { I18N_NOOP("Not Specified"), "  "},
    { I18N_NOOP("Unknown"), "xx"},
    { I18N_NOOP("Not Specified"), "\0"},
    { I18N_NOOP("Abkhazian"), "ab"},
    { I18N_NOOP("Afan (Oromo)"), "om"},
    { I18N_NOOP("Afar"), "aa"},
    { I18N_NOOP("Afrikaans"), "af"},
    { I18N_NOOP("Albanian"), "sq"},
    { I18N_NOOP("Amharic"), "am"},
    { I18N_NOOP("Arabic"), "ar"},
    { I18N_NOOP("Armenian"), "hy"},
    { I18N_NOOP("Assamese"), "as"},
    { I18N_NOOP("Aymara"), "ay"},
    { I18N_NOOP("Azerbaijani"), "az"},
    { I18N_NOOP("Bashkir"), "ba"},
    { I18N_NOOP("Basque"), "eu"},
    { I18N_NOOP("Bengali;Bangla"), "bn"},
    { I18N_NOOP("Bhutani"), "dz"},
    { I18N_NOOP("Bihari"), "bh"},
    { I18N_NOOP("Bislama"), "bi"},
    { I18N_NOOP("Breton"), "br"},
    { I18N_NOOP("Bulgarian"), "bg"},
    { I18N_NOOP("Burmese"), "my"},
    { I18N_NOOP("Byelorussian"), "be"},
    { I18N_NOOP("Cambodian"), "km"},
    { I18N_NOOP("Catalan"), "ca"},
    { I18N_NOOP("Chinese"), "zh"},
    { I18N_NOOP("Corsican"), "co"},
    { I18N_NOOP("Croatian"), "hr"},
    { I18N_NOOP("Czech"), "cs"},
    { I18N_NOOP("Danish"), "da"},
    { I18N_NOOP("Dutch"), "nl"},
    { I18N_NOOP("English"), "en"},
    { I18N_NOOP("Esperanto"), "eo"},
    { I18N_NOOP("Estonian"), "et"},
    { I18N_NOOP("Faroese"), "fo"},
    { I18N_NOOP("Fiji"), "fj"},
    { I18N_NOOP("Finnish"), "fi"},
    { I18N_NOOP("French"), "fr"},
    { I18N_NOOP("Frisian"), "fy"},
    { I18N_NOOP("Galician"), "gl"},
    { I18N_NOOP("Georgian"), "ka"},
    { I18N_NOOP("German"), "de"},
    { I18N_NOOP("Greek"), "el"},
    { I18N_NOOP("Greenlandic"), "kl"},
    { I18N_NOOP("Guarani"), "gn"},
    { I18N_NOOP("Gujarati"), "gu"},
    { I18N_NOOP("Hausa"), "ha"},
    { I18N_NOOP("Hebrew"), "iw"},
    { I18N_NOOP("Hindi"), "hi"},
    { I18N_NOOP("Hungarian"), "hu"},
    { I18N_NOOP("Icelandic"), "is"},
    { I18N_NOOP("Indonesian"), "in"},
    { I18N_NOOP("Interlingua"), "ia"},
    { I18N_NOOP("Interlingue"), "ie"},
    { I18N_NOOP("Inupiak"), "ik"},
    { I18N_NOOP("Irish"), "ga"},
    { I18N_NOOP("Italian"), "it"},
    { I18N_NOOP("Japanese"), "ja"},
    { I18N_NOOP("Javanese"), "jv"},
    { I18N_NOOP("Kannada"), "kn"},
    { I18N_NOOP("Kashmiri"), "ks"},
    { I18N_NOOP("Kazakh"), "kk"},
    { I18N_NOOP("Kinyarwanda"), "rw"},
    { I18N_NOOP("Kirghiz"), "ky"},
    { I18N_NOOP("Kurundi"), "rn"},
    { I18N_NOOP("Korean"), "ko"},
    { I18N_NOOP("Kurdish"), "ku"},
    { I18N_NOOP("Laothian"), "lo"},
    { I18N_NOOP("Latin"), "la"},
    { I18N_NOOP("Latvian;Lettish"), "lv"},
    { I18N_NOOP("Lingala"), "ln"},
    { I18N_NOOP("Lithuanian"), "lt"},
    { I18N_NOOP("Macedonian"), "mk"},
    { I18N_NOOP("Malagasy"), "mg"},
    { I18N_NOOP("Malay"), "ms"},
    { I18N_NOOP("Malayalam"), "ml"},
    { I18N_NOOP("Maltese"), "mt"},
    { I18N_NOOP("Maori"), "mi"},
    { I18N_NOOP("Marathi"), "mr"},
    { I18N_NOOP("Moldavian"), "mo"},
    { I18N_NOOP("Mongolian"), "mn"},
    { I18N_NOOP("Nauru"), "na"},
    { I18N_NOOP("Nepali"), "ne"},
    { I18N_NOOP("Norwegian"), "no"},
    { I18N_NOOP("Occitan"), "oc"},
    { I18N_NOOP("Oriya"), "or"},
    { I18N_NOOP("Pashto;Pushto"), "ps"},
    { I18N_NOOP("Persian (Farsi)"), "fa"},
    { I18N_NOOP("Polish"), "pl"},
    { I18N_NOOP("Portuguese"), "pt"},
    { I18N_NOOP("Punjabi"), "pa"},
    { I18N_NOOP("Quechua"), "qu"},
    { I18N_NOOP("Rhaeto-Romance"), "rm"},
    { I18N_NOOP("Romanian"), "ro"},
    { I18N_NOOP("Russian"), "ru"},
    { I18N_NOOP("Samoan"), "sm"},
    { I18N_NOOP("Sangho"), "sg"},
    { I18N_NOOP("Sanskrit"), "sa"},
    { I18N_NOOP("Scots Gaelic"), "gd"},
    { I18N_NOOP("Serbian"), "sr"},
    { I18N_NOOP("Serbo-Croatian"), "sh"},
    { I18N_NOOP("Sesotho"), "st"},
    { I18N_NOOP("Setswana"), "tn"},
    { I18N_NOOP("Shona"), "sn"},
    { I18N_NOOP("Sindhi"), "sd"},
    { I18N_NOOP("Singhalese"), "si"},
    { I18N_NOOP("Siswati"), "ss"},
    { I18N_NOOP("Slovak"), "sk"},
    { I18N_NOOP("Slovenian"), "sl"},
    { I18N_NOOP("Somali"), "so"},
    { I18N_NOOP("Spanish"), "es"},
    { I18N_NOOP("Sundanese"), "su"},
    { I18N_NOOP("Swahili"), "sw"},
    { I18N_NOOP("Swedish"), "sv"},
    { I18N_NOOP("Tagalog"), "tl"},
    { I18N_NOOP("Tajik"), "tg"},
    { I18N_NOOP("Tamil"), "ta"},
    { I18N_NOOP("Tatar"), "tt"},
    { I18N_NOOP("Telugu"), "te"},
    { I18N_NOOP("Thai"), "th"},
    { I18N_NOOP("Tibetan"), "bo"},
    { I18N_NOOP("Tigrinya"), "ti"},
    { I18N_NOOP("Tonga"), "to"},
    { I18N_NOOP("Tsonga"), "ts"},
    { I18N_NOOP("Turkish"), "tr"},
    { I18N_NOOP("Turkmen"), "tk"},
    { I18N_NOOP("Twi"), "tw"},
    { I18N_NOOP("Ukrainian"), "uk"},
    { I18N_NOOP("Urdu"), "ur"},
    { I18N_NOOP("Uzbek"), "uz"},
    { I18N_NOOP("Vietnamese"), "vi"},
    { I18N_NOOP("Volapuk"), "vo"},
    { I18N_NOOP("Welsh"), "cy"},
    { I18N_NOOP("Wolof"), "wo"},
    { I18N_NOOP("Xhosa"), "xh"},
    { I18N_NOOP("Yiddish"), "ji"},
    { I18N_NOOP("Yoruba"), "yo"},
    { I18N_NOOP("Zulu"), "zu"},
    { "", ""}
};
QMap<QString, QString> Languages::m_languages;
QStringList Languages::m_languageIds;

const char *Subtitle::subpType[16] = {
    I18N_NOOP("Undefined"), I18N_NOOP("Normal"), I18N_NOOP("Large"),
    I18N_NOOP("Children"), I18N_NOOP("reserved"), I18N_NOOP("Normal Captions"),
    I18N_NOOP("Large Captions"), I18N_NOOP("Children Captions"),
    I18N_NOOP("reserved"),
    I18N_NOOP("Forced"), I18N_NOOP("reserved"), I18N_NOOP("reserved"),
    I18N_NOOP("reserved"), I18N_NOOP("Director"), I18N_NOOP("Large Director"),
    I18N_NOOP("Children Director")
};
const char *Subtitle::hAlign[] = { "default", "left", "right", "center"};
const char *Subtitle::vAlign[] = { "default", "top", "bottom", "center"};

const char *AudioTrack::AudioFormat[7] =
{ "ac3", "?", "mpeg1", "mpeg2", "lpcm", "sdds ", "dts"};
// 28.9.2003: Chicken run again, it has frequency index of 1.
// According to dvd::rip the frequency is 48000
const int AudioTrack::SampleFreq[2] = { 48000, 48000};
const char *AudioTrack::Quantization[4] = { "16bit", "20bit", "24bit", "drc"};
const char *AudioTrack::AudioType[5] = {
    I18N_NOOP("Undefined"), I18N_NOOP("Normal"), I18N_NOOP("Impaired"),
    I18N_NOOP("Comments1"), I18N_NOOP("Comments2")
};

const double VideoTrack::FramesPerS[4] = { -1.0, 25.00, -1.0, 29.97};
const char *VideoTrack::VideoFormat[2] = { "NTSC", "PAL"};
const char *VideoTrack::AspectRatioString[4] =
{ "4:3", "16:9", "\"?:?\"", "16:9"};
const int VideoTrack::VideoHeight[4] = { 480, 576, -1, 576};
const int VideoTrack::VideoWidth[4]  = { 720, 704, 352, 352};
const char *VideoTrack::PermittedDf[4] = {
    I18N_NOOP("P&S + Letter"), I18N_NOOP("Pan&Scan"), I18N_NOOP("Letterbox"),
    I18N_NOOP("?")
};

// bitrate tables stolen from mplex
const unsigned int Title::ac3BitrateIndex[32] = {
    32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 448, 512, 576, 640,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
const unsigned int Title::dtsBitrateIndex[32] = {
    32, 56, 64, 96, 112, 128, 192, 224, 256, 320, 384, 448, 512, 576, 640, 768, 960, 1024,
    1152, 1280, 1344, 1408, 1411, 1472, 1536, 1920, 2048, 3072, 3840, 0, 0, 0
};
const unsigned int Title::mpaBitrateIndex[2][4][16] = {
    { // MPEG audio V2
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 0},
        { 0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 0},
        { 0, 8, 16, 24, 32, 64, 80, 56, 64, 128, 160, 112, 128, 256, 320, 0}
    },
    { // MPEG audio V1
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 0},
        { 0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 0},
        { 0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0}
    }
};

#define MAXSTREAMS 50
#define PROBEBLOCKS 30720

QStringList Languages::languageIds()
{
    checkLanguages();
    return m_languageIds;
}

QString Languages::language(const QString &code)
{
    checkLanguages();
    return m_languages[code];
}

void Languages::checkLanguages()
{
    if (m_languages.isEmpty()) {
        int i = 0;

        while (LanguageStrings[i][0][0] != '\0') {
            m_languages.insert(LanguageStrings[i][1], i18n(LanguageStrings[i][0]));

            if (i >= 3) {
                m_languageIds.append(LanguageStrings[i][1]);
            }

            ++i;
        }
    }
}

#ifdef HAVE_LIBDVDREAD
Cell::Cell(int cell, pgc_t *pgc)
    : m_cell(cell)
{
    int i;
    int strt = 0;

    for (i = 0; i < m_cell; i++) {
        strt += Title::dvdTime2ms(&pgc->cell_playback[i].playback_time);
    }

    m_start = QTime().addMSecs(strt);
    m_length = Title::dvdTime2QTime(&pgc->cell_playback[cell].playback_time);
    m_startSector = pgc->cell_playback[cell].first_sector;
    m_endSector = pgc->cell_playback[cell].last_sector;

    m_chapter = false;
    m_hidden = false;

    for (i = 0; i < pgc->nr_of_programs; i++) {
        if (m_cell == pgc->program_map[i] - 1) {
            m_chapter = true;
            break;
        }
    }

    kDebug() << "  Cell " << m_cell <<
    ": " << start() << " " << length() <<
    ", " << startSector() << " - " << endSector() << ", Chapter: " <<
    m_chapter;
}
#endif


uint64_t Cell::size() const
{
    return ((uint64_t)(m_endSector - m_startSector)) * 2048;
}

QString Cell::toString() const
{
    return i18n("Cell: %1", start().toString());
}

Subtitle::Subtitle(QString langCode, QString file)
    : m_langCode(langCode)
    , m_file(file)
    , m_align(Qt::AlignCenter | Qt::AlignBottom)
    , m_encoding("UTF-8")
{
    m_font.setPointSize(28);
}

#ifdef HAVE_LIBDVDREAD
Subtitle::Subtitle(int pos, subp_attr_t *subp_attr, uint16_t subp_control,
        video_attr_t *video_attr)
{
    m_langCode.sprintf("%c%c", subp_attr->lang_code >> 8,
            subp_attr->lang_code & 0xff);

    if (m_langCode[0] == '\0') {
        m_langCode[0] = 'x';
        m_langCode[1] = 'x';
    }

    m_type = (Type)subp_attr->code_extension;

    if (subp_control & 0x80000000) {
        // there are several streams for one spu
        if (video_attr->display_aspect_ratio) {
            // 16:9
            switch (video_attr->permitted_df) {
            case 1: // letterbox
                setPosition(subp_control & 0xff);
                break;
            case 2: // pan&scan
                setPosition((subp_control >> 8) & 0xff);
                break;
            default: // widescreen
                setPosition((subp_control >> 16) & 0xff);
                break;
            }
        } else   {
            // 4:3
            setPosition((subp_control >> 24) & 0x7F);
        }
    } else   {
        setPosition(pos);
    }

    setTrackId(0x20 + position());
    m_align = Qt::AlignLeft | Qt::AlignBottom;

    kDebug() << "Subtitle: " << languageString() << ", " << typeString() <<
    ", " << trackId() << ", " << position();
}
#endif


QString Subtitle::toString() const
{
    QString s;

    if (type() >= Large) {
        s = " (" + typeString() + ')';
    }

    return i18n("Subtitle: %1%2", languageString(), s);
}

QString Subtitle::verticalAlign() const
{
    int n = 0;

    switch (alignment() & Qt::AlignVertical_Mask) {
        case Qt::AlignTop:
            n = 1;
            break;
        case Qt::AlignVCenter:
            n = 3;
            break;
        case Qt::AlignBottom:
        default:
            n = 2;
            break;
    }

    return vAlign[n];
}

QString Subtitle::horizontalAlign() const
{
    int n = 0;

    switch (alignment() & Qt::AlignHorizontal_Mask) {
        case Qt::AlignLeft:
            n = 1;
            break;
        case Qt::AlignRight:
            n = 2;
            break;
        case Qt::AlignHCenter:
            n = 3;
            break;
        default:
            n = 0;
    }

    return hAlign[n];
}

AudioTrack::AudioTrack(QString langCode) : m_langCode(langCode)
{
}

#ifdef HAVE_LIBDVDREAD
AudioTrack::AudioTrack(audio_attr_t *audio_attr, uint16_t audio_control)
    : m_bitrate(0)
{
    m_langCode.sprintf("%c%c", audio_attr->lang_code >> 8,
            audio_attr->lang_code & 0xff);

    if (m_langCode[0] == '\0') {
        m_langCode[0] = 'x';
        m_langCode[1] = 'x';
    }

    m_format = audio_attr->audio_format;
    m_sampleFreq = SampleFreq[audio_attr->sample_frequency];
    m_quantization = audio_attr->quantization;
    m_channels = audio_attr->channels + 1;
    m_apMode = audio_attr->application_mode;
    m_type = (Type)audio_attr->lang_extension;

    if (audio_control & 0x8000) {
        setPosition((audio_control & 0x7F00) >> 8);

        switch (m_format) {
            case 0x00: // ac3
                setTrackId(0x80 + position());
                break;
            case 0x02:
            case 0x03: // mpeg
                setTrackId(position());
                break;
            case 0x04: // lpcm
                setTrackId(0xa0 + position());
                break;
            case 0x06: // dts
                setTrackId(0x88 + position());
                break;
            default:
                setTrackId(-1);
                kDebug() << "Unknown audio: " << m_format;
                break;
        }
    }

    kDebug() << "Audio: " << languageString() << ", " << formatString() <<
    ", " << sampleFreq() << ", " << quantizationString() << ", " <<
    channels() << ", " << apMode() << ", " << typeString() << ", " <<
    QString("%1").arg(trackId(), 0, 16) << ", " << position();
}
#endif


QString AudioTrack::toString() const
{
    QString s;

    if (type() >= Impaired) {
        s = " (" + typeString() + ')';
    }

    return i18n("Audio: %1, %2 channels%3", languageString(), channels(), s);
}

#ifdef HAVE_LIBDVDREAD
VideoTrack::VideoTrack(pgc_t *pgc, video_attr_t *video_attr)
{
    m_fps = FramesPerS[(pgc->playback_time.frame_u & 0xc0) >> 6];
    m_format = video_attr->video_format;
    m_aspect = (AspectRatio)video_attr->display_aspect_ratio;
    m_width = VideoWidth[video_attr->picture_size];
    m_height = VideoHeight[video_attr->video_format];
    m_permittedDf = video_attr->permitted_df;
    setPosition(0);
    setTrackId(0xe0);

    kDebug() << "Video: " << fps() << ", " << formatString() << ", " <<
    aspectRatioString() << ", " << width() << ", " << height() << ", " <<
    permittedDfString();
}
#endif


QString VideoTrack::toString() const
{
    return i18n("Video: %1, %2x%3, %4", formatString(), width(), height(),
            aspectRatioString());
}

#ifdef HAVE_LIBDVDREAD
Title::Title(int titleNbr, pgc_t *pgc, int pgcNbr, title_info_t *title, vtsi_mat_t *vtsi_mat)
    : m_titleNbr(titleNbr)
    , m_pgc(pgcNbr)
{
    m_length = dvdTime2QTime(&pgc->playback_time);
    m_vtsId.sprintf("%.12s", vtsi_mat->vts_identifier);
    m_vts = title->title_set_nr;
    m_ttn = title->vts_ttn;
    m_angles = title->nr_of_angles;

    kDebug() << "Title: " << length() << ", " << vtsId() << ", " <<
    vts() << ", " << ttn() << ", " << angles();
}

bool Title::parseAudioBitrates(dvd_reader_t *dvd)
{
    unsigned char buffer[DVD_VIDEO_LB_LEN];
    StreamType type;
    uint32_t maxblocks = endSector();
    uint32_t i;
    dvd_file_t *vob;

    QList<int> handledStreams;
    int id;
    int bitrate;
    int audioTracks = m_audios.count();
    int foundAudioTracks = 0;

    vob = DVDOpenFile(dvd, m_vts, DVD_READ_TITLE_VOBS);

    if (!vob) {
        kDebug() << "ERROR: opening vobs for title " << m_vts << "failed";
        DVDClose(dvd);
        return false;
    }

    if ((endSector() - startSector()) > PROBEBLOCKS) {
        maxblocks = startSector() + PROBEBLOCKS;
    }

    for (i = startSector(); i < maxblocks; i++) {
        // read 10mb from dvd
        if (!DVDReadBlocks(vob, i, 1, buffer)) {
            kDebug() << "ERROR probing for streams";
            return false;
        }

        type = packetType(buffer);

        if (type == UNKNOWN) {
            continue;
        }

        if (handledStreams.indexOf(type) >= 0) {
            continue; // Already handled
        }

        id = buffer[23 + buffer[22]];

        switch (type) {
            case MPEG_AUDIO:
                bitrate = MPEGBitrate(buffer);
                break;
            case AC3_AUDIO:
                bitrate = AC3Bitrate(buffer);
                break;
            case DTS_AUDIO:
                bitrate = DTSBitrate(buffer);
                break;
            case LPCM_AUDIO:
                // FIXME - hard coded, no detection yet
                bitrate = 2000000;
                break;
            case MPEG_VIDEO:
            case SUBPICTURE:
            default:
                handledStreams.append(id);
                bitrate = -1;
                break;
        }

        if (bitrate > 0) {
            kDebug() << "Bitrate: " << QString("%1").arg(id, 0, 16) << " = " <<
            bitrate;
            AudioTrack *track = audioById(id);

            if (track) {
                track->setBitrate(bitrate);
                ++foundAudioTracks;
            }

            handledStreams.append(id);

            if (foundAudioTracks >= audioTracks) {
                break;
            }
        }
    }

    DVDCloseFile(vob);
    return true;
}

bool Title::parseTrackLengths(dvd_reader_t *dvd)
{
    double runtime = ((double)length().hour() *  (60.0 * 60.0) +
                      (double)length().minute() *  60.0 +
                      (double)length().second() +
                      (double)length().msec() / 1000.0);
    uint64_t total = 0;
    uint64_t overhead_size;
    uint64_t size;

    if (!parseAudioBitrates(dvd)) {
        return false;
    }

    int firstBlock = startSector();
    int lastBlock  = endSector();

    for (SubtitleList::Iterator it = m_subtitles.begin();
         it != m_subtitles.end(); ++it)
    {
        size = (int64_t)((double)(lastBlock - firstBlock) * 2048.0 / 1450.0);
        (*it).setSize(size);
        total += size;
        kDebug() << "Subtitle size: " << size;
    }

    for (AudioList::Iterator it = m_audios.begin();
         it != m_audios.end(); ++it)
    {
        size = (uint64_t)((double)(*it).bitrate() / 8.0 * runtime);
        (*it).setSize(size);
        total += size;
        kDebug() << "Audio size: " << size;
    }

    overhead_size = overheadSize();
    // sum of all non video stuff
    total += overhead_size;
    // video size = vob size - non video stuff
    size = (uint64_t)(((double)(lastBlock - firstBlock)) * 2048.0);

    if (total > size) {
        m_video.setSize(0);
    } else {
        m_video.setSize(size - total);
    }

    kDebug() << "Video size: " << m_video.size();
    return true;
}

QTime Title::dvdTime2QTime(dvd_time_t *dt)
{
    QTime result;
    double fps = VideoTrack::FramesPerS[(dt->frame_u & 0xc0) >> 6];
    int ms = 0;

    if (fps > 0) {
        ms = (long)(((dt->frame_u & 0x30) >> 3) * 5 +
                (dt->frame_u & 0x0f) * 1000.0 / fps);
    }

    result.setHMS(
            (((dt->hour &   0xf0) >> 3) * 5 + (dt->hour   & 0x0f)),
            (((dt->minute & 0xf0) >> 3) * 5 + (dt->minute & 0x0f)),
            (((dt->second & 0xf0) >> 3) * 5 + (dt->second & 0x0f)),
            ms);
    return result;
}

int Title::dvdTime2ms(dvd_time_t *dt)
{
    double fps = VideoTrack::FramesPerS[(dt->frame_u & 0xc0) >> 6];
    long   ms;

    ms  = (((dt->hour &   0xf0) >> 3) * 5 + (dt->hour   & 0x0f)) * 3600000;
    ms += (((dt->minute & 0xf0) >> 3) * 5 + (dt->minute & 0x0f)) * 60000;
    ms += (((dt->second & 0xf0) >> 3) * 5 + (dt->second & 0x0f)) * 1000;

    if (fps > 0) {
        ms += (long)(((dt->frame_u & 0x30) >> 3) * 5 +
                (dt->frame_u & 0x0f) * 1000.0 / fps);
    }

    return ms;
}
#endif


QString Title::toString() const
{
    return i18n("Title: %1, %2", m_titleNbr, length().toString());
}

Title::StreamType Title::packetType(unsigned char *buffer)
{
    unsigned char packet_type = buffer[17];

    if (packet_type == 0xBD) {
        // if private stream get sub-id
        packet_type = buffer[23 + buffer[22]];

        if ((packet_type >= 0xE0) && (packet_type <= 0xEF)) {
            return MPEG_VIDEO;
        } else if ((packet_type >= 0xC0) && (packet_type <= 0xCF)) {
            return MPEG_AUDIO;
        } else if ((packet_type >= 0x80) && (packet_type <= 0x87)) {
            return AC3_AUDIO;
        } else if ((packet_type >= 0x88) && (packet_type <= 0x8F)) {
            return DTS_AUDIO;
        } else if ((packet_type >= 0xA0) && (packet_type <= 0xAF)) {
            return LPCM_AUDIO;
        } else if ((packet_type >= 0x20) && (packet_type <= 0x3F)) {
            return SUBPICTURE;
        }
    }

    return UNKNOWN;
}

int Title::MPEGBitrate(unsigned char *buffer)
{
    unsigned char *searchptr;
    int x, y, z;

    searchptr = buffer + 17;
    // search for 1st byte of MPA Syncword
    searchptr = (unsigned char *)memchr(searchptr, 0xFF, searchptr - buffer);

    while (searchptr != NULL) {
        // check for search hit at end of buffer
        if (searchptr <= (buffer + 2045)) {
            // if next 4 bits set, we found the sync word
            if ((searchptr[1] & 0xF0) == 0xF0) {
                x = (searchptr[1] & 0x08) >> 3; // version id
                y = (searchptr[1] & 0x06) >> 1; // layer code
                z = (searchptr[2] & 0xF0) >> 4; // bitrate code
                return mpaBitrateIndex[x][y][z] * 1024;
            }
        }

        searchptr++;
        searchptr = (unsigned char *)memchr(searchptr,  0xFF, searchptr - buffer);
    }
    return -1;
}

int Title::AC3Bitrate(unsigned char *buffer)
{
    int x, y;

    // calc start offsets
    y = buffer[22] + buffer[buffer[22] + 26] + (unsigned char)26;

    // check for AC3 Syncword
    if ((buffer[y] != 0x0B) || (buffer[y + 1] != 0x77)) {
        return -1;
    }

    // 6 lowest bits
    x = buffer[y + 4] & 0x3F;
    // shift index >> 1 (same as /2 )
    return ac3BitrateIndex[x >> 1] * 1024;
}

int Title::DTSBitrate(unsigned char *buffer)
{
    int x;

    // 3 lowest bits of byte
    x = (buffer[buffer[22] + 35] & 0x3) << 3;
    // 2 highest bits of next byte
    x = x | ((buffer[buffer[22] + 36] & 0xE0) >> 5);
    // kbit * 1000 fits better to filesize, so what ...
    return dtsBitrateIndex[x] * 1000;
}

uint64_t Title::overheadSize() const
{
    // rough guess about space wasted by padding, pack headers, ...
    return (startSector() - endSector()) * 2048 / 50;
}

AudioTrack *Title::audioById(int id)
{
    for (AudioList::Iterator it = m_audios.begin();
         it != m_audios.end(); ++it)
    {
        if ((*it).trackId() == id) {
            return &(*it);
        }
    }

    return 0;
}

uint64_t Title::size() const
{
    return ((uint64_t)(endSector() - startSector())) * 2048;
}

#ifdef HAVE_LIBDVDREAD
Info::Info(const QString &device, QObject *parent)
    : QObject(parent)
{
    if (!device.isEmpty()) {
        parseDVD(device);
    }
}

bool Info::getTitleName(const char *dvd_device, QString &title)
{
    FILE *filehandle = 0;
    int  i;
    char t[33];
    QString path = dvd_device;

    if (path.startsWith('/')) {
        path = path.left(path.length() - 1);
    }

    QFileInfo fi(path);

    if (fi.isDir()) {
        title = fi.fileName();
        return true;
    }

    if (!(filehandle = fopen(dvd_device, "r"))) {
        kDebug() << "Couldn't open %s for title";
        title = i18nc("Unknown DVD title", "unknown");
        return false;
    }

    if (fseek(filehandle, 32808, SEEK_SET)) {
        fclose(filehandle);
        kDebug() << "Couldn't seek in %s for title";
        title = i18nc("Unknown DVD title", "unknown");
        return false;
    }

    if (32 != (i = fread(t, 1, 32, filehandle))) {
        fclose(filehandle);
        kDebug() << "Couldn't read enough bytes for title.";
        title = i18nc("Unknown DVD title", "unknown");
        return false;
    }

    fclose(filehandle);

    t[32] = '\0';

    while (i-- > 2) {
        if (t[i] == ' ') {
            t[i] = '\0';
        }
    }
    title = t;
    return true;
}

bool Info::parseDVD(const QString &device)
{
    pgc_t *pgc;
    pgcit_t *vts_pgcit;
    vtsi_mat_t *vtsi_mat;
    dvd_reader_t *dvd;
    ifo_handle_t *ifo_zero;
    ifo_handle_t **ifo;
    QFileInfo fi(device);
    int i;
    int j;
    int dvdTitles;
    int title_set_nr;
    int vts_ttn;
    int pgcNbr;
    vmgi_mat_t *vmgi_mat;

    clear();

    if (!fi.exists()) {
        kDebug() << "Can't find device " <<  device;
        return false;
    }

    dvd = DVDOpen((const char *)device.toLocal8Bit());

    if (!dvd) {
        kDebug() << "Can't open disc " << device;
        return false;
    }

    ifo_zero = ifoOpen(dvd, 0);

    if (!ifo_zero) {
        kDebug() << "Can't open main ifo!";
        return false;
    }

    ifo = (ifo_handle_t **)malloc((ifo_zero->vts_atrt->nr_of_vtss + 1) *
            sizeof(ifo_handle_t *));

    for (i = 1; i <= ifo_zero->vts_atrt->nr_of_vtss; i++) {
        ifo[i] = ifoOpen(dvd, i);

        if (!ifo[i]) {
            kDebug() << "Can't open ifo " << i;
            return false;
        }
    }

    dvdTitles = ifo_zero->tt_srpt->nr_of_srpts;
    vmgi_mat = ifo_zero->vmgi_mat;
    emit titles(dvdTitles);
    kapp->processEvents();

    m_device = device;
    getTitleName(device.toLocal8Bit(), m_title);
    m_vmgIdentifier.sprintf("%.12s", vmgi_mat->vmg_identifier);
    m_providerIdentifier.sprintf("%.32s", vmgi_mat->provider_identifier);
    kDebug() << m_title;

    for (j = 0; j < dvdTitles; j++) {
        // GENERAL
        if (ifo[ifo_zero->tt_srpt->title[j].title_set_nr]->vtsi_mat) {
            title_set_nr = ifo_zero->tt_srpt->title[j].title_set_nr;
            vtsi_mat   = ifo[title_set_nr]->vtsi_mat;
            vts_pgcit  = ifo[title_set_nr]->vts_pgcit;
            vts_ttn = ifo_zero->tt_srpt->title[j].vts_ttn;
            pgcNbr = ifo[title_set_nr]->vts_ptt_srpt->title[vts_ttn - 1].ptt[0].pgcn;
            pgc = vts_pgcit->pgci_srp[pgcNbr - 1].pgc;

            // TITLE
            Title title(j + 1, pgc, pgcNbr, &ifo_zero->tt_srpt->title[j], vtsi_mat);

            // VIDEO
            title.setVideo(VideoTrack(pgc, &vtsi_mat->vts_video_attr));

            // AUDIO
            for (i = 0; i < vtsi_mat->nr_of_vts_audio_streams; i++) {
                title.addAudio(AudioTrack(&vtsi_mat->vts_audio_attr[i],
                                pgc->audio_control[i]));
            }

            // CELLS & CHAPTERS
            for (i = 0; i < pgc->nr_of_cells; i++) {
                title.addCell(Cell(i, pgc));
            }

            // SUBTITLES
            for (i = 0; i < vtsi_mat->nr_of_vts_subp_streams; i++) {
                title.addSubtitle(Subtitle(i, &vtsi_mat->vts_subp_attr[i],
                                pgc->subp_control[i],
                                &vtsi_mat->vts_video_attr));
            }

            // Save longest title
            if (title.length() > m_longestTitleLength) {
                m_longestTitleLength = title.length();
                m_longestTitle = j;
            }

            title.parseTrackLengths(dvd);

            addTitle(title);
        }

        emit title(j);
        kapp->processEvents();
    }

    for (i = 1; i <= ifo_zero->vts_atrt->nr_of_vtss; i++) {
        ifoClose(ifo[i]);
    }

    ifoClose(ifo_zero);
    DVDClose(dvd);
    return true;
}
#endif


Info::~Info()
{
}

uint64_t Info::size() const
{
    uint64_t result = 0;

    for (QDVD::TitleList::ConstIterator it = m_titles.begin();
         it != m_titles.end(); ++it)
    {
        result += (*it).size();
    }

    return result;
}

QString Info::toString() const
{
    return m_title;
}

void Info::clear()
{
    m_title = "";
    m_device = "";
    m_vmgIdentifier = "";
    m_providerIdentifier = "";
    m_longestTitleLength = QTime();
    m_longestTitle = -1;
    m_titles.clear();
}
}

#include "qdvdinfo.moc"
