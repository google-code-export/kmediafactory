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
#ifndef KMFLANGUAGEWIDGETS_H
#define KMFLANGUAGEWIDGETS_H

#include "kmf_stddef.h"
#include <kstandarddirs.h>
#include <qcombobox.h>
#include <qlistbox.h>

/**
@author Petri Damsten
*/
class KMFLanguageItem : public QListBoxPixmap
{
  public:
    KMFLanguageItem(QListBox* listbox,
                    const QString& languageId = QString::null);
    KMFLanguageItem(QListBox* listbox,
                    const QString& languageId,
                    QListBoxItem* after);
    ~KMFLanguageItem() {};

    const QString& languageId() const { return m_languageId; };
    //void  setLanguageId(const QString& languageId);
    QPixmap flag(QString languageId);

  private:
    QString m_languageId;
    static QMap<QString, QString> m_dvd2l10n;
};

class KMFLanguageComboBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QString language READ language WRITE setLanguage)
  public:
    KMFLanguageComboBox(QWidget *parent = 0, const char *name = 0);
    ~KMFLanguageComboBox();

    QString language() const;
    void setLanguage(QString language);
};

class KMFLanguageListBox : public QListBox
{
    Q_OBJECT
    Q_PROPERTY(QString language READ language WRITE setLanguage)
    Q_PROPERTY(bool autoFill READ autoFill WRITE setAutoFill)
  public:
    KMFLanguageListBox(QWidget *parent = 0, const char *name = 0);
    ~KMFLanguageListBox();

    QString language() const;
    void setLanguage(QString language);
    void setItemLanguage(QString language, int index = -1);
    bool autoFill() const { return m_autoFill; };
    void setAutoFill(bool autoFill);

    void fill();
    void filter(const QStringList& list);

  private:
    bool m_autoFill;

};

#endif
