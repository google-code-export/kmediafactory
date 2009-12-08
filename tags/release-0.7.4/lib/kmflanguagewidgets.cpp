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
#include "kmflanguagewidgets.h"
#include <KDebug>
#include <QPixmap>
#include <QBitmap>
#include "qdvdinfo.h"

LanguageListModel::LanguageListModel(QObject *parent) :
    QAbstractListModel(parent), m_audio(0), m_subtitle(0)
{
}

void LanguageListModel::useAllLanguages()
{
  setLanguages(QDVD::Languages::languageIds());
}

void LanguageListModel::setLanguages(QStringList languages)
{
  m_languageList = languages;
}

int LanguageListModel::rowCount(const QModelIndex&) const
{
  if(m_audio)
    return m_audio->count();
  else if(m_subtitle)
    return m_subtitle->count();
  else
    return m_languageList.count();
}

QVariant LanguageListModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (index.row() >= rowCount(index))
    return QVariant();

  if (role == Qt::DisplayRole)
    return QDVD::Languages::language(at(index.row()));
  else if (role == Qt::DecorationRole)
    return flag(at(index.row()));
  else
    return QVariant();
}

QVariant LanguageListModel::headerData(int, Qt::Orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  return QString("Language");
}

QPixmap LanguageListModel::flag(QString languageId) const
{
  if(m_dvd2l10n.isEmpty())
  {
    m_dvd2l10n["en"] = "gb";
    m_dvd2l10n["fi"] = "fi";
    m_dvd2l10n["da"] = "dk";
    m_dvd2l10n["no"] = "no";
    m_dvd2l10n["sv"] = "se";
    m_dvd2l10n["is"] = "is";
    m_dvd2l10n["de"] = "de";
    m_dvd2l10n["bg"] = "bg";
    m_dvd2l10n["zh"] = "cn";
    m_dvd2l10n["hr"] = "hr";
    m_dvd2l10n["cs"] = "cz";
    m_dvd2l10n["nl"] = "nl";
    m_dvd2l10n["et"] = "ee";
    m_dvd2l10n["fr"] = "fr";
    m_dvd2l10n["el"] = "gr";
    m_dvd2l10n["hu"] = "hu";
    m_dvd2l10n["ga"] = "ie";
    m_dvd2l10n["it"] = "it";
    m_dvd2l10n["ja"] = "jp";
    m_dvd2l10n["jv"] = "jp";
    m_dvd2l10n["ko"] = "kr";
    m_dvd2l10n["lv"] = "lv";
    m_dvd2l10n["lt"] = "lt";
    m_dvd2l10n["mk"] = "mk";
    m_dvd2l10n["na"] = "nr";
    m_dvd2l10n["ne"] = "np";
    m_dvd2l10n["pl"] = "pl";
    m_dvd2l10n["pt"] = "pt";
    m_dvd2l10n["ro"] = "ro";
    m_dvd2l10n["ru"] = "ru";
    m_dvd2l10n["sk"] = "sk";
    m_dvd2l10n["sl"] = "si";
    m_dvd2l10n["es"] = "es";
    m_dvd2l10n["th"] = "th";
    m_dvd2l10n["tr"] = "tr";
    m_dvd2l10n["uk"] = "ua";
    m_dvd2l10n["vi"] = "vn";
  }

  QString temp = m_dvd2l10n[languageId];
  QString file;

  if(!temp.isEmpty())
    file = KStandardDirs::locate("locale", "l10n/" + temp + "/flag.png");
  if(file.isEmpty())
  {
    QPixmap noFlag(21, 14);
    noFlag.fill();
    noFlag.setMask(noFlag.createHeuristicMask());
    return noFlag;
  }
  else
    return QPixmap(file);
}

QString LanguageListModel::at(int i) const
{
  QString language = "en";

  if(i >= 0)
  {
    if(m_audio)
    {
      if(i < m_audio->count())
        language = m_audio->at(i).language();
    }
    else if(m_subtitle)
    {
      if(i < m_subtitle->count())
        language = m_subtitle->at(i).language();
    }
    else
    {
      if(i < m_languageList.count())
        language = m_languageList.at(i);
    }
  }
  return language;
}

QModelIndex LanguageListModel::index(const QString& lang) const
{
  int i = -1;

  if(m_audio)
  {
    for(i = 0; i < m_audio->count(); ++i)
      if(m_audio->at(i).language() == lang)
        break;
  }
  else if(m_subtitle)
  {
    for(i = 0; i < m_subtitle->count(); ++i)
      if(m_subtitle->at(i).language() == lang)
        break;
  }
  else
    i = m_languageList.indexOf(lang);
  if(i >= 0)
    return createIndex(i, 0);
  return QModelIndex();
}

QMap<QString, QString> LanguageListModel::m_dvd2l10n;

#include "kmflanguagewidgets.moc"
