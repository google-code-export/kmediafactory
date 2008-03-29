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
#include <qpixmap.h>
#include <qbitmap.h>

// Test if the widget is in designer window. We don't want to fill listbox or
// combobox when in designer window because items are then saved to ui file.
// There is propably better way to do this which I would be glad to here...
bool inDesigner(QObject* p)
{
  while(p)
  {
    if(strcmp(p->name(), "designer_mainwindow") == 0)
      return true;
    p = p->parent();
  }
  return false;
}

QMap<QString, QString> KMFLanguageItem::m_dvd2l10n;

KMFLanguageItem::KMFLanguageItem(QListBox* listbox,
                                 const QString& languageId)
  : QListBoxPixmap(listbox, flag(languageId),
                   QDVD::Languages::language(languageId)),
    m_languageId(languageId)
{
}

KMFLanguageItem::KMFLanguageItem(QListBox* listbox,
                                 const QString& languageId,
                                 QListBoxItem* after)
  : QListBoxPixmap(listbox,
                   flag(languageId),
                   QDVD::Languages::language(languageId),
                   after),
    m_languageId(languageId)
{
}

QPixmap KMFLanguageItem::flag(QString languageId)
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
    file = ::locate("locale", "l10n/" + temp + "/flag.png");
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
/*
void KMFLanguageItem::setLanguageId(const QString& languageId)
{
  m_languageId = languageId;

  if(listBox())
  {
    kdDebug() << k_funcinfo << languageId << listBox()->index(this) << endl;
    listBox()->changeItem(flag(languageId),
        QDVD::Languages::language(languageId), listBox()->index(this));
  }
}
*/
KMFLanguageComboBox::KMFLanguageComboBox(QWidget *parent, const char *name)
 : QComboBox(parent, name)
{
  if(!inDesigner(parent))
  {
    QStringList languages = QDVD::Languages::languageIds();

    for(QStringList::Iterator it = languages.begin();
        it != languages.end(); ++it)
    {
      new KMFLanguageItem(listBox(), *it);
    }
    listBox()->sort();
    new KMFLanguageItem(listBox(), "", 0);
  }
}

KMFLanguageComboBox::~KMFLanguageComboBox()
{
}

QString KMFLanguageComboBox::language() const
{
  KMFLanguageItem* item =
      static_cast<KMFLanguageItem*>(listBox()->item(currentItem()));
  if(item)
    return item->languageId();
  return "";
}

void KMFLanguageComboBox::setLanguage(QString language)
{
  for(uint i=0; i < listBox()->count(); ++i)
  {
    KMFLanguageItem* langItem =
        static_cast<KMFLanguageItem*>(listBox()->item(i));
    if(langItem && langItem->languageId() == language)
    {
      setCurrentItem(i);
      return;
    }
  }
}

KMFLanguageListBox::KMFLanguageListBox(QWidget *parent, const char *name)
  : QListBox(parent, name), m_autoFill(false)
{
}

KMFLanguageListBox::~KMFLanguageListBox()
{
}

QString KMFLanguageListBox::language() const
{
  KMFLanguageItem* langItem =
      static_cast<KMFLanguageItem*>(selectedItem());
  if(langItem)
  {
    return langItem->languageId();
  }
  return "";
}

void KMFLanguageListBox::setLanguage(QString language)
{
  for(uint i=0; i < count(); ++i)
  {
    KMFLanguageItem* langItem =
        static_cast<KMFLanguageItem*>(item(i));
    if(langItem && langItem->languageId() == language)
    {
      setSelected(langItem, true);
      return;
    }
  }
  setSelected(0, true);
}

void KMFLanguageListBox::setAutoFill(bool autoFill)
{
  m_autoFill = autoFill;
  if(m_autoFill)
    fill();
}

void KMFLanguageListBox::fill()
{
  clear();
  if(!inDesigner(parent()))
  {
    QStringList languages = QDVD::Languages::languageIds();

    for(QStringList::Iterator it = languages.begin();
        it != languages.end(); ++it)
    {
      new KMFLanguageItem(this, *it);
    }
    sort();
    new KMFLanguageItem(this, "", 0);
  }
}

void KMFLanguageListBox::setItemLanguage(QString language, int i)
{
  QListBoxItem* lbitem;

  if(i == -1)
    lbitem = selectedItem();
  else
    lbitem = item(i);
  if(lbitem)
  {
    int n = index(lbitem);
    lbitem = lbitem->prev();
    removeItem(n);
  }
  setSelected(new KMFLanguageItem(this, language, lbitem), true);
}

void KMFLanguageListBox::filter(const QStringList& list)
{
  QString lang = language();
  KMFLanguageItem* langItem = static_cast<KMFLanguageItem*>(item(0));

  while(langItem)
  {
    QString id = langItem->languageId();
    KMFLanguageItem* temp = langItem;

    langItem = static_cast<KMFLanguageItem*>(langItem->next());
    if(list.findIndex(id) == -1)
      delete temp;
  }
  setLanguage(lang);
}

#include "kmflanguagewidgets.moc"
