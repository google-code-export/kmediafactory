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
#ifndef SLIDESHOWPLUGIN_H
#define SLIDESHOWPLUGIN_H

#include <kmf_stddef.h>
#include <kmediafactory/plugin.h>

class QAction;

class SlideshowPlugin : public KMF::Plugin
{
    Q_OBJECT
  public:
    SlideshowPlugin(QObject *parent, const QVariantList&);

    QString dvdslideshowBin() { return m_dvdslideshow; };

  public slots:
    virtual void init(const QString &type);

  public:
    virtual const KMF::ConfigPage* configPage() const;
    virtual KMF::MediaObject* createMediaObject(const QDomElement& element);
    virtual QStringList supportedProjectTypes() const;

  public slots:
    void slotAddSlideshow();

  protected:
    QAction* setupActions();

  private:
    QString  m_dvdslideshow;
};

#endif /* SLIDESHOWPLUGIN_H */

