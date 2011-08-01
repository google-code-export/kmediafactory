// **************************************************************************
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
// **************************************************************************

#ifndef TEMPLATEOBJECT_H
#define TEMPLATEOBJECT_H

#include <KDirWatch>
#include <KUrl>

#include <kmediafactory/plugin.h>
#include "kconfigxml.h"
#include "kmfmenu.h"
#include "kmfwidget.h"

class TemplateOptions;
class KAction;

class KMFConfigXML : public KConfigXML
{
    public:
        KMFConfigXML() : KConfigXML(){};
        ~KMFConfigXML(){};

    protected:
        virtual QString parseCode(QString code);
};

class TemplateObject : public KMF::TemplateObject
{
    friend class TemplateOptions;

    Q_OBJECT

    public:
        TemplateObject(const QString &templateFile, QObject *parent);
        virtual ~TemplateObject();

        virtual void toXML(QDomElement *element) const;
        virtual bool fromXML(const QDomElement &element);
        virtual bool prepare(const QString &type);
        virtual QStringList menus();
        virtual int chaptersPerPage(const QString &page);
        virtual void actions(QList<QAction *> *) const;
        KConfigXML&customProperties()
        {
            return m_customProperties;
        };
        QVariant property(const QString &widget, const QString &name) const;
        void setProperty(const QString &widget, const QString &name, const QVariant &value);
        bool fileExists();

    public slots:
        virtual void slotProperties();
        // Help Kross plugin declaring these as slots
        virtual QPixmap pixmap() const;
        virtual QImage preview(const QString &page=QString(), int title=0, int chapter=0);
        // KMF::Object::call slots
        QVariant directPlay(QVariantList args);
        QVariant continueToNextTitle(QVariantList args);
        QVariant writeDvdAuthorXml(QVariantList args);
        QVariant language(QVariantList args);

    protected:
        const KMFMenu&menu() const
        {
            return m_menu;
        }

    private:
        KAction *m_templateProperties;
        KMFConfigXML m_customProperties;
        KMFMenu m_menu;
        QString m_file;
        QString m_type;
        KDirWatch m_templateWatch;

        QString propertyString(KConfigSkeletonItem *item) const;
        bool isUpToDate(QString type);
};

#endif // TEMPLATEOBJECT_H
