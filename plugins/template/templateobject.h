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
#ifndef TEMPLATEOBJECT_H
#define TEMPLATEOBJECT_H

#include "kmfwidget.h"
#include "kmfmenu.h"
#include "kconfigxml.h"
#include <kmediafactory/plugin.h>
#include <kurl.h>
#include <kaction.h>
//Added by qt3to4:
#include <Q3PtrList>
#include <QPixmap>

/**
*/

class TemplateOptions;

class KMFConfigXML : public KConfigXML
{
  public:
    KMFConfigXML() : KConfigXML() {};
    ~KMFConfigXML() {};

  protected:
    virtual QString parseCode(QString code);
};

class TemplateObject : public KMF::TemplateObject
{
  friend class TemplateOptions;

    Q_OBJECT
  public:
    TemplateObject(const QString& templateFile, QObject* parent);
    virtual ~TemplateObject();

    virtual void toXML(QDomElement& element) const;
    virtual void fromXML(const QDomElement& element);
    virtual int timeEstimate() const;
    virtual bool make(QString type);
    virtual QPixmap pixmap() const;
    virtual QStringList menus();
    virtual QImage preview(const QString& menu = "");
    virtual void actions(Q3PtrList<KAction>& actionList) const;
    KConfigXML& customProperties() { return m_customProperties; };
    QVariant property(const QString& widget, const QString& name) const;
    void setProperty(const QString& widget, const QString& name,
                     const QVariant& value);

  public slots:
    virtual void slotProperties();
    virtual void clean() { };

  protected:
    const KMFMenu& menu() const { return m_menu; }

  private:
    KAction* m_templateProperties;
    KMFConfigXML m_customProperties;
    KMFMenu m_menu;

    bool isUpToDate(QString type);
};

#endif // TEMPLATEOBJECT_H
