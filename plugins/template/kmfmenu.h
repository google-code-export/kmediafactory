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
#ifndef KMFMENU_H
#define KMFMENU_H

#include "kmfmenupage.h"
#include "kmftemplatebase.h"
#include "kmftemplate.h"
#include <kmediafactory/plugininterface.h>
#include <QImage>
#include <QObject>

class KMFMenu : public KMFTemplateBase
{
    Q_OBJECT
  public:
    enum { TotalPoints = 1000 };
    KMFMenu(const QString& tmplate, QObject *parent = 0);
    ~KMFMenu();

    bool makeMenu();
    QImage makeMenuPreview(QString page = "");
    QImage icon() const { return templateImage("icon.png"); };
    bool addPage(const QString& name, int title, int chapter);
    QDomElement writeDvdAuthorXml(const QString& type, int titleset) const;
    bool addMenuMpegJobs();
    KMFTemplate* templateStore() { return &m_template; };
    const QStringList& menus() const { return m_menus; };
    const QString& title() { return m_title; };
    const QString& id() { return m_id; };
    void clear() { m_pages.clear(); };
    void setLanguage(const QString& file, const QString& lang)
        { m_template.setLanguage(file, lang); };

    int mediaObjCount();
    int mediaObjChapterCount(int title);

  protected:
    QImage templateImage(const QString& image) const;
    QList<KMFMenuPage*>* titlePages(int title);
    QDomElement getPage(const QDomNode& node, const QString& name);
    bool addPage(const QDomElement& element, int pageSet,
                int title, int chapter);

  private:
    void progress(int points);
    int pages() const;

    QList< QList<KMFMenuPage*> > m_pages;
    KMFTemplate m_template;
    QDomDocument m_templateXML;
    QStringList m_menus;
    QString m_title;
    QString m_id;
    int m_points;
    int m_pagePoints;
};

#endif
