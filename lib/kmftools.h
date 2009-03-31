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

#ifndef KMFKMFTOOLS_H
#define KMFKMFTOOLS_H

#include <KUrl>
#include <QString>
#include <QSize>
#include <QModelIndex>
#include <QStringList>
#include <stdint.h>

class KXMLGUIClient;
class QStringList;
class QFont;
class QDomElement;
class QAbstractItemView;
class QStringListModel;
class QPainter;

namespace KMF
{
  class KDE_EXPORT Tools
  {
    public:
      Tools();
      ~Tools();

      static QString toAscii(QString text);
      static QString simpleName(QString s);
      static QString simpleBaseName(QString file);
      static QString simple2Title(QString s);
      static void removeDuplicates(QStringList* list);
      static QString sizeString(uint64_t size);
      //static uint64_t fileSize(QString file);
      static void stripExisting(KUrl::List* src, const KUrl& dest);
      static QString addSlash(QString path);
      static QString joinPaths(QString path1, QString path2);
      static QStringList file2List(const QString& file,
                                   const QString& comment = QString::null,
                                   const QString& startsWith = QString::null);
      static QString findExe(const QString& exe, const QStringList& paths,
                             const QString& extraPrefix = QString::null);
      static QStringList findAllResources(const char* type,
                                          const QString& filter=QString::null);
      static QSize resolution(const QSize& original, const QSize& originalRatio,
                              const QSize& dest, const QSize& destRatio,
                              Qt::AspectRatioMode mode =
                                  Qt::KeepAspectRatioByExpanding);
      static QSize maxResolution(const QString &type);
      static QSize guessRatio(const QSize& image, const QSize& video);
      static void fontToXML(const QFont& font, QDomElement* element);
      static QFont fontFromXML(const QDomElement& element);
      static QString fontFile(const QFont& font);
      static QFont realFont(const QFont& font);
      static QString longFontName(const QFont& font);
      static int fcWeight2QtWeight(int fcWeight);
      static void printChilds(QObject* obj, int level = 0);
      static int hex2int(QChar hexchar);
      static QColor toColor(const QString& s);
      static QMap<QString, QString> readIniFile(const QString& ini);
      static void drawRoundRect(QPainter* painter, const QRect& rect,
                                int radius);
      static void printActions(KXMLGUIClient* client);
      static void spy(QObject* obj);
      static uint frames(const QString &type);
      static void cleanFiles(const QString& dir, const QStringList& files = QStringList());
      
      static bool loadStringFromFile(const KUrl& url, QString* string, bool showFailed = true);
      static bool saveString2File(const KUrl& url, const QString& string, 
                                  bool showFailed = true);
      static QString xmlElement2String(const QDomElement& elem);
      static QDomElement string2XmlElement(const QString& elem);
      static QImage variantList2Image(QVariant v);
      static QMap<QString, QString> variantMap2StringMap(const QMap<QString, QVariant>& map);
      static QStringList variantList2StringList(const QVariantList& list);
  };
}

#endif // KMFKMFTOOLS_H
