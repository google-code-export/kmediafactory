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
#ifndef DVDDIRECTORYOBJECT_H
#define DVDDIRECTORYOBJECT_H

#include "dvdauthorobject.h"
#include <k3process.h>
#include <QFileInfo>
#include <QPixmap>

class DvdDirectoryObject : public DvdAuthorObject
{
  Q_OBJECT
  public:
    enum { TotalPoints = 1000 };
    enum LastLine { Warning = KMF::Warning, Error = KMF::Error,
      Processing, Vobu, FixingVobu, None };

    DvdDirectoryObject(QObject *parent = 0);
    virtual ~DvdDirectoryObject();
    virtual void toXML(QDomElement& element) const;
    virtual void fromXML(const QDomElement& element);
    virtual QPixmap pixmap() const;
    virtual void actions(QList<KAction*>& actionList) const;
    virtual int timeEstimate() const;
    virtual bool make(QString type);

  private slots:
    void output(K3Process* proc, char* buffer, int buflen);
    virtual void clean();

  private:
    QString m_buffer;
    bool m_error;
    LastLine m_lastLine;
    QString m_warning;
    KAction* dvdCleanDirectory;
    K3Process dvdauthor;
    QFileInfo m_currentFile;
    int m_points;
    int m_filePoints;
    bool m_first;
    int m_lastVobu, m_vobu;
    int m_lastSize;

    void progress(int points);
    bool isUpToDate(QString type);
};

#endif // DVDDIRECTORYOBJECT_H
