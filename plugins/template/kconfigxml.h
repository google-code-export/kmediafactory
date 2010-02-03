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

#ifndef KCONFIGXML_H
#define KCONFIGXML_H

#include <KConfigSkeleton>

class QIODevice;
class QDomElement;
class QDomDocument;

class KConfigXML : public KConfigSkeleton
{
    public:
        KConfigXML();
        KConfigXML(QString kcfgFile);
        KConfigXML(QIODevice *kcfgFile);
        KConfigXML(QByteArray *kcfgFile);
        ~KConfigXML();

        bool parse(QString kcfgFile);
        bool parse(QIODevice *kcfgFile);
        bool parse(QByteArray *kcfgFile);
        bool parse(QDomDocument &doc);

    protected:
        virtual QString parseCode(QString)
        {
            return "";
        };

    private:
        void parseKCFGXMLEntry(const QDomElement &element);

        QList<QString *> m_strings;
        QList<bool *>    m_bools;
        QList<QColor *>  m_colors;
        QList<QFont *>   m_fonts;
        QList<qint32 *>  m_ints;
        QList<KUrl *>    m_urls;
};

#endif
