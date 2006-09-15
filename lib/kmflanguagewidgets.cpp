//**************************************************************************
//   Copyright (C) 2004, 2005, 2006 by Petri Damsten
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
#include "kmflanguagewidgets.h"
#include "qdvdinfo.h"
#include <kdebug.h>
#include <QPixmap>
#include <QBitmap>

// Test if the widget is in designer window. We don't want to fill listbox or
// combobox when in designer window because items are then saved to ui file.
// There is propably better way to do this which I would be glad to here...
bool inDesigner(QObject* p)
{
  while(p)
  {
    if(p->objectName() == "designer_mainwindow")
      return true;
    p = p->parent();
  }
  return false;
}


LanguageListModel::LanguageListModel(QObject *parent) :
    QAbstractListModel(parent)
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
  return m_languageList.count();
}

QVariant LanguageListModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (index.row() >= m_languageList.size())
    return QVariant();

  if (role == Qt::DisplayRole)
    return QDVD::Languages::language(m_languageList.at(index.row()));
  else if (role == Qt::DecorationRole)
    return flag(m_languageList.at(index.row()));
  else
    return QVariant();
}

bool LanguageListModel::setData(const QModelIndex& index,
                                const QVariant& value, int)
{
  if(index.row() < m_languageList.count())
  {
    m_languageList[index.row()] = value.toString();
    emit dataChanged(index, index);
    return true;
  }
  return false;
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

QModelIndex LanguageListModel::languageIndex(
    const QString& languageId, const QModelIndex& parent) const
{
  int i = m_languageList.indexOf(languageId);
  return index(i, 0, parent);
}

QString LanguageListModel::language(QModelIndex index) const
{
  return m_languageList[index.row()];
}

QString LanguageListModel::language(int index) const
{
  return m_languageList[index];
}

QMap<QString, QString> LanguageListModel::m_dvd2l10n;


KMFLanguageComboBox::KMFLanguageComboBox(QWidget *parent)
 : QComboBox(parent)
{
  m_model = new LanguageListModel;
  setModel(m_model);
}

KMFLanguageComboBox::~KMFLanguageComboBox()
{
  delete m_model;
}

QString KMFLanguageComboBox::language() const
{
  int index = currentIndex();
  if(index >= 0)
    return m_model->language(index);
  return "";
}

void KMFLanguageComboBox::setLanguage(QString language)
{
  QModelIndex i = m_model->languageIndex(language);
  setCurrentIndex(i.row());
}


KMFLanguageListBox::KMFLanguageListBox(QWidget *parent)
  : QListView(parent), m_autoFill(false)
{
  m_model = new LanguageListModel;
  setModel(m_model);
}

KMFLanguageListBox::~KMFLanguageListBox()
{
  delete m_model;
}

QString KMFLanguageListBox::language() const
{
  QModelIndexList indexes = selectionModel()->selectedIndexes();
  if(indexes.count() > 0)
    return m_model->language(indexes[0]);
  return "";
}

void KMFLanguageListBox::setLanguage(QString language)
{
  QModelIndex i = m_model->languageIndex(language);
  selectionModel()->select(QItemSelection(i, i), QItemSelectionModel::Select);
}

void KMFLanguageListBox::setAutoFill(bool autoFill)
{
  m_autoFill = autoFill;
  if(m_autoFill)
    m_model->useAllLanguages();
}

void KMFLanguageListBox::setItemLanguage(QString language, int i)
{
  QModelIndex index = m_model->index(i, 0);
  m_model->setData(index, language);
}

void KMFLanguageListBox::filter(const QStringList& list)
{
  QString lang = language();
  m_model->setLanguages(list);
  setLanguage(lang);
}

#include "kmflanguagewidgets.moc"
