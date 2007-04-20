//**************************************************************************
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
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************
#ifndef SLIDESHOWOBJECT_H
#define SLIDESHOWOBJECT_H

#include <kmf_stddef.h>
#include <kmediafactory/plugin.h>
#include <kurl.h>

/**
*/

class Slide
{
  public:
    Slide();

    QString picture;
    QString comment;
    bool    chapter;

    bool operator <(const Slide &t) const { return (comment < t.comment); }
};

Q_DECLARE_METATYPE(Slide);

typedef QList<Slide> SlideList;

class SlideshowObject : public KMF::MediaObject
{
    Q_OBJECT
  public:
    enum { TotalPoints = 100 };

    SlideshowObject(QObject* parent);
    virtual ~SlideshowObject();
    virtual QPixmap pixmap() const;
    virtual bool make(QString type);
    virtual int timeEstimate() const;
    virtual void actions(QList<QAction*>&) const;
    virtual void writeDvdAuthorXml(QDomElement& element,
                                   QString preferredLanguage,
                                   QString post, QString type);
    virtual QImage preview(int chapter = MainPreview) const;
    virtual QString text(int chapter = MainTitle) const;
    virtual int chapters() const;
    virtual uint64_t size() const;
    virtual QTime duration() const;
    virtual QTime chapterTime(int chapter) const;

    virtual void toXML(QDomElement& element) const;
    virtual void fromXML(const QDomElement& element);

    void addPics(QStringList list);

    const SlideList& slides() const { return m_slides; };
    void setSlides(const SlideList& slides) { m_slides = slides; };
    double slideDuration() const { return m_duration; };
    void setSlideDuration(double duration) { m_duration = duration; };
    bool loop() const { return m_loop; };
    void setLoop(bool loop) { m_loop = loop; };
    bool includeOriginals() const { return m_includeOriginals; };
    void setIncludeOriginals(bool includeOriginals)
        { m_includeOriginals = includeOriginals; };
    const QStringList& audioFile() const { return m_audioFiles; };
    void setAudioFile(const QStringList& audioFiles)
        { m_audioFiles = audioFiles; };

    SlideList slideList(QStringList list) const;

  public slots:
    virtual void slotProperties();
    void output(K3Process* process, char* buffer, int buflen);
    virtual void clean();

  protected:
    void generateId();
    const Slide& chapter(int chap) const;
    bool writeSlideshowFile() const;
    bool convertToDVD() const;
    bool copyOriginals() const;
    bool oooConvert(QString* file) const;
    QTime audioDuration() const;
    double calculatedSlideDuration() const;
    bool lastChapter(SlideList::ConstIterator& iter);

  private:
    QAction* m_slideshowProperties;
    QList<Slide> m_slides;
    QString m_id;
    double  m_duration;
    bool m_loop;
    bool m_includeOriginals;
    QStringList m_audioFiles;
    QString m_buffer;
};

#endif // SLIDESHOWOBJECT_H
