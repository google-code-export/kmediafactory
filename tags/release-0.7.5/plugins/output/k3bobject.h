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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//**************************************************************************

#ifndef K3BOBJECT_H
#define K3BOBJECT_H

#include "dvddirectoryobject.h"
#include <QPixmap>

class KUrl;

class K3bObject : public DvdDirectoryObject
{
    Q_OBJECT
  public:
    enum { TotalPoints = 100 };

    K3bObject(QObject *parent = 0);
    ~K3bObject();

    virtual void toXML(QDomElement* element) const;
    virtual bool fromXML(const QDomElement& element);
    virtual void actions(QList<QAction*>* actionList) const;
    virtual bool prepare(const QString& type);

  public slots:
    // Help Kross plugin declaring these as slots
    virtual QPixmap pixmap() const;

  private:
    bool isUpToDate(const QString& type);

    bool saveDocument(const KUrl& url);
    bool saveDocumentData(QDomElement* docElem);
    bool saveGeneralDocumentData(QDomElement* part);
    void saveDocumentDataOptions(QDomElement& optionsElem);
    void saveDocumentDataHeader(QDomElement& headerElem);
    void addFiles(const QString& directory, QDomDocument* doc,
                  QDomElement* parent);
    void saveDataItem(const QFileInfo* fi, QDomDocument* doc,
                      QDomElement* parent);

    QString m_rootDir;
};

#endif
