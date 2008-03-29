//**************************************************************************
//   Copyright (C) 2004 by Petri Damstén
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
#ifndef KMFPROJECT_H
#define KMFPROJECT_H

#include <kmediafactory/plugin.h>
#include <kmediafactory/projectinterface.h>
#include <kurl.h>
#include <qdatetime.h>

/**
*/
class KMFProject : public QObject
{
    Q_OBJECT
  public:
    KMFProject(QObject *parent = 0, const char *name = 0);
    ~KMFProject();

  public:
    void addItem(KMF::MediaObject *mob);
    void removeItem(KMF::MediaObject *mob);
    bool save(KURL url = QString::null);
    bool open(const KURL &url);
    QString toXML();
    void fromXML(QString xml);
    QPtrList<KMF::MediaObject>* mediaObjects() { return &m_list; };
    int mediaCount() const { return m_list.count(); };

    const QString& type() const { return m_type; };
    QMap<QString, QString> subTypes() const;
    void setType(const QString& type);
    QString directory(const QString& subDir = "") const;
    void cleanFiles(const QString& subDir, const QStringList& files) const;
    void setDirectory(const QString& directory);
    const QString& title() const { return m_title; };
    void setTitle(const QString& title);
    void setTemplateObj(KMF::TemplateObject* tmplate);
    KMF::TemplateObject* templateObj() const { return m_template; };
    void setOutput(KMF::OutputObject* output);
    KMF::OutputObject* output() const { return m_output; };
    bool make(QString type);
    int timeEstimate() const;
    void setDirty(KMF::ProjectInterface::DirtyType type, bool dirty = true);
    bool isModified() const { return m_modified; };
    QDateTime lastModified(KMF::ProjectInterface::DirtyType type) const;
    const KURL& url() { return m_url; };
    bool validProject() const;
    const QString& error() const { return m_error; };
    void init();
    int serial() { return ++m_serial; };
    static bool mkdir(const QString &path);
    void mediaObjFromXML(const QDomElement& e);
    void templateFromXML(const QDomElement& e);
    void outputFromXML(const QDomElement& e);
    QString lastSubType() const { return m_subType; };

  signals:
    void init(const QString&);
    void newItem(KMF::Object*);
    void itemRemoved(KMF::Object*);
    void modified(const QString& file, bool modified);

  private:
    enum Modified { ModMedia = 0, ModTemplate = 1, ModOutput = 2, ModLast = 3 };

    QString m_type;
    QString m_directory;
    QString m_title;
    QPtrList<KMF::MediaObject> m_list;
    KMF::TemplateObject* m_template;
    KMF::OutputObject* m_output;
    KURL m_url;
    bool m_modified;
    QDateTime m_lastModified[ModLast];
    bool m_loading;
    bool m_initializing;
    int m_serial;
    mutable QString m_error;
    QString m_subType;

    void setError(const QString& error = QString::null) const
        { m_error = error; };
    void saveObj(QDomDocument& doc, QDomElement& root,
                             const char* name, KMF::Object* ob);
};

#endif // KMFPROJECT_H
