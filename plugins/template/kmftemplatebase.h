// **************************************************************************
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
// **************************************************************************

#ifndef KMFTEMPLATEBASE_H
#define KMFTEMPLATEBASE_H

#include <QtCore/QObject>

namespace KMF
{
class PluginInterface;
class Plugin;
}

class TemplateObject;
class KMFMenu;
class KMFMenuPage;

class KMFTemplateBase : public QObject
{
    Q_OBJECT

    public:
        KMFTemplateBase(QObject *parent = 0);
        ~KMFTemplateBase();

        static QString uiText(const QString &s);

    protected:
        KMF::PluginInterface *m_interface;

        KMFMenuPage*page();
        KMFMenu*menu();
        TemplateObject*templateObject();
        KMF::Plugin*plugin();
};

#endif // KMFTEMPLATEBASE_H
