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
#ifndef KMFMENUPAGE_H
#define KMFMENUPAGE_H

#include "kmfwidget.h"
#include <kmediafactory/plugin.h>
#include <QObject>
#include <QSize>

#define DPM 2700

class KMFButton;
class TemplateObject;
class KMFMenuPage;
namespace KMF 
{
  class Job;
}

class KMFMenuPage : public KMFWidget
{
    Q_OBJECT
  public:
    KMFMenuPage(QObject *parent = 0);
    ~KMFMenuPage();

    QImage* getLayer(Layer layer);
    void addButton(KMFButton* button) { m_buttons->append(button); };
    int buttonCount() { return m_buttons->count(); };
    KMFButton* button(int n) { return m_buttons->at(n); };
    KMFButton* button(const QString& name);
    const QList<KMFButton*>& buttons() const { return *m_buttons; };
    void writeDvdAuthorXml(QDomElement& element, QString type) const;
    void writeDvdAuthorXmlNoMenu(QDomElement& element) const;

    void setResolution(QSize resolution);
    const QSize& resolution() const { return m_resolution; };
    void fromXML(const QDomElement& element);
    bool parseButtons(bool addPages = true);

    int titleStart() const { return m_titleStart; }
    void setTitleStart(int titleStart) { m_titleStart = titleStart; };
    int chapterStart() const { return m_chapterStart; }
    void setChapterStart(int chapterStart) { m_chapterStart = chapterStart; };
    const int& titles() const { return m_titles; };
    void setTitles(const int& titles) { m_titles = titles; };
    int chapters() const { return m_chapters; };
    void setChapters(int chapters) { m_chapters = chapters; };
    virtual void setProperty(const QString& name, QVariant value);
    bool isVmgm() const { return m_vmgm; };
    void setVmgm(bool vmgm) { m_vmgm = vmgm; };
    bool isFirst() const { return m_index == 1; };
    int index() const { return m_index; };
    void setIndex(int titleset, int titlesetCount, int index, int count)
        { m_titleset = titleset; m_titlesetCount = titlesetCount;
          m_index = index; m_count = count; };
    bool directChapterPlay() const { return m_directChapterPlay; };
    bool directPlay() const { return m_directPlay; };
    QString sound() const { return m_sound; };

    KMF::Job* job(const QString& type) const;
    QImage preview();

  private:
    QList<KMFButton*>* m_buttons;
    QSize m_resolution;
    QString m_language;
    QString m_sound;
    int m_titles;
    int m_chapters;
    int m_titleStart;
    int m_chapterStart;
    int m_index;
    int m_titleset;
    int m_titlesetCount;
    int m_count;
    bool m_vmgm;
    bool m_directPlay;
    bool m_directChapterPlay;
    bool m_stopped;
    int m_continueToNextTitle;
    static int m_mjpegtoolsVersion;

    void checkDummyVideo() const;
    bool isUpToDate(const QString& type) const;
};

#endif
