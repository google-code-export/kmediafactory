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
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************
#ifndef KMFLANGUAGEWIDGETS_H
#define KMFLANGUAGEWIDGETS_H

#include "kmf_stddef.h"
#include "qdvdinfo.h"
#include <kstandarddirs.h>
#include <QComboBox>
#include <QListView>
#include <QPixmap>
#include <QAbstractListModel>

/**
@author Petri Damsten
*/

class KDE_EXPORT LanguageListModel : public QAbstractListModel
{
  Q_OBJECT
  public:
    LanguageListModel(QObject *parent = 0);

    int rowCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    void useAllLanguages();
    void setLanguages(QStringList languages);
    void setLanguages(const QDVD::AudioList* audio) { m_audio = audio; };
    void setLanguages(const QDVD::SubtitleList* subtitle)
      { m_subtitle = subtitle; };
    QStringList* list() { return &m_languageList; };
    QString at(int i) const;
    QModelIndex index(const QString& lang) const;

  private:
    QPixmap flag(QString languageId) const;

    QStringList m_languageList;
    const QDVD::AudioList* m_audio;
    const QDVD::SubtitleList* m_subtitle;
    static QMap<QString, QString> m_dvd2l10n;
};

#endif
