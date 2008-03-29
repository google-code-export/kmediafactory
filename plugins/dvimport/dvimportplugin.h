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
#ifndef DVIMPORTPLUGIN_H
#define DVIMPORTPLUGIN_H

#include <kmf_stddef.h>
#include <kmediafactory/plugin.h>

class KAction;
class VideoObject;
class QFile;

class DVImportPlugin :public KMF::Plugin
{
    Q_OBJECT
  public:
    DVImportPlugin(QObject *parent, const char* name, const QStringList&);
    virtual KMF::MediaObject* createMediaObject(const QDomElement& element);

    bool parseDV(VideoObject* vob, QString fileName);

  public slots:
    virtual void init(const QString &type);
    virtual QStringList supportedProjectTypes();
    void slotAddDV();

  protected slots:
    virtual void cancel();

  private:
    KAction* addDVAction;
    bool m_cancel;

    int readFrame(QFile& video, char *frameBuffer, bool& isPAL);
};

#endif /* DVIMPORTPLUGIN_H */

