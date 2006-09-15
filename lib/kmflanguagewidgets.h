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
#include <QComboBox>
#include <QListView>
#include <QPixmap>
#include <QAbstractListModel>

/**
@author Petri Damsten
*/

class LanguageListModel : public QAbstractListModel
{
  Q_OBJECT
  public:
    LanguageListModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual bool setData(const QModelIndex& index,
                         const QVariant& value, int role = Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    void useAllLanguages();
    void setLanguages(QStringList languages);
    virtual QModelIndex languageIndex(
        const QString& languageId,
        const QModelIndex& parent = QModelIndex()) const;
    virtual QString language(QModelIndex index) const;
    QString language(int index) const;

  private:
    QPixmap flag(QString languageId) const;
    QStringList m_languageList;
    static QMap<QString, QString> m_dvd2l10n;
};

class KMFLanguageComboBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QString language READ language WRITE setLanguage)
  public:
    KMFLanguageComboBox(QWidget *parent = 0);
    ~KMFLanguageComboBox();

    QString language() const;
    void setLanguage(QString language);

  private:
    LanguageListModel* m_model;
};

class KMFLanguageListBox : public QListView
{
    Q_OBJECT
    Q_PROPERTY(QString language READ language WRITE setLanguage)
    Q_PROPERTY(bool autoFill READ autoFill WRITE setAutoFill)
  public:
    KMFLanguageListBox(QWidget *parent = 0);
    ~KMFLanguageListBox();

    QString language() const;
    void setLanguage(QString language);
    void setItemLanguage(QString language, int index = -1);
    bool autoFill() const { return m_autoFill; };
    void setAutoFill(bool autoFill);

    void filter(const QStringList& list);

  private:
    bool m_autoFill;
    LanguageListModel* m_model;
};

#endif
