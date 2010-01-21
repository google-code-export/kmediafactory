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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//**************************************************************************
#ifndef SLIDESHOWPLUGIN_H
#define SLIDESHOWPLUGIN_H

#include <kmf_stddef.h>
#include <kmediafactory/plugin.h>

class QAction;

class SlideshowPlugin : public KMF::Plugin
{
    Q_OBJECT
  public:

    enum EBackend
    {
        BACKEND_NOT_FOUND,
        BACKEND_MELT,
        BACKEND_DVD_SLIDESHOW
    };
    
    SlideshowPlugin(QObject *parent, const QVariantList&);

    QString app() { return m_app; };
    EBackend backend() { return m_backend; }

  public slots:
    virtual void init(const QString &type);

  public:
    virtual const KMF::ConfigPage* configPage() const;
    virtual KMF::MediaObject* createMediaObject(const QDomElement& element);
    virtual QStringList supportedProjectTypes() const;

  public slots:
    void slotAddSlideshow();

  protected:
    void setupActions();

  private:
    EBackend m_backend;
    QString  m_app;
};

#endif /* SLIDESHOWPLUGIN_H */

