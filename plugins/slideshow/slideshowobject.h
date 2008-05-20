//**************************************************************************
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
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************

#ifndef SLIDESHOWOBJECT_H
#define SLIDESHOWOBJECT_H

#include <kmf_stddef.h>
#include <kmediafactory/plugin.h>
#include <kurl.h>

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
    virtual bool prepare(QString type);
    virtual int timeEstimate() const;
    virtual void actions(QList<QAction*>*) const;
    virtual QImage preview(int chapter = MainPreview) const;
    virtual QString text(int chapter = MainTitle) const;
    virtual int chapters() const;
    virtual uint64_t size() const;
    virtual QTime duration() const;
    virtual QTime chapterTime(int chapter) const;

    virtual void toXML(QDomElement* element) const;
    virtual bool fromXML(const QDomElement& element);

    void addPics(QStringList list);

    const SlideList& slides() const { return m_slides; };
    void setSlides(const SlideList& slides) { m_slides = slides; };
    double slideDuration() const { return m_duration; };
    void setSlideDuration(double duration) { m_duration = duration; };
    double calculatedSlideDuration() const;
    bool loop() const { return m_loop; };
    void setLoop(bool loop) { m_loop = loop; };
    bool includeOriginals() const { return m_includeOriginals; };
    void setIncludeOriginals(bool includeOriginals) { m_includeOriginals = includeOriginals; };
    void setAudioFiles(const QStringList& audioFiles) { m_audioFiles = audioFiles; };
    QStringList audioFiles() const { return m_audioFiles; };
    QTime audioDuration() const;
    SlideList slideList(QStringList list) const;
    QString id() const { return m_id; };

  public slots:
    virtual void slotProperties();
    virtual void clean();
    // KMF::Object::call slots
    QVariant writeDvdAuthorXml(QVariantList args) const;

  protected:
    void generateId();
    const Slide& chapter(int chap) const;
    bool oooConvert(QString* file) const;

  private:
    QAction* m_slideshowProperties;
    SlideList m_slides;
    QString m_id;
    double  m_duration;
    bool m_loop;
    bool m_includeOriginals;
    QStringList m_audioFiles;
    QString m_buffer;
    QString m_type;
};

#endif // SLIDESHOWOBJECT_H
