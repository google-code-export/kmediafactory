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

#ifndef KMFTEMPLATE_H
#define KMFTEMPLATE_H

#include <QtCore/QString>
#include <QtCore/QStringList>

#include "kmfintl.h"

class KoStore;

class KMFTemplate
{
    public:
        KMFTemplate(const QString file = QString::null);
        ~KMFTemplate();

        bool setStore(const QString &store);
        QByteArray readFile(const QString &file) const;
        QIODevice*device(const QString &file);
        void close();
        bool hasFile(const QString &file) const;
        void setLanguage(const QString &file, const QString &lang);
        const QString&language() const
        {
            return m_language;
        };
        QStringList languages() const;
        QString translate(const char *text) const;

    private:
        KoStore *m_store;
        QString m_storeName;
        QString m_language;
        QString m_file;
        struct kmf_loaded_l10nfile m_domain;
};

#endif
