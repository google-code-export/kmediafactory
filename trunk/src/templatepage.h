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
#ifndef TEMPLATEPAGE_H
#define TEMPLATEPAGE_H

#include <ui_templatepage.h>
#include <qdatetime.h>
#include "kmfuiinterface.h"
#include <kmficonview.h>

class KPageWidgetItem;

/**
@author Petri Damsten
*/
class TemplatePage : public QWidget, public Ui::TemplatePage
{
    Q_OBJECT
  public:
    TemplatePage(QWidget *parent = 0);
    virtual ~TemplatePage();

    void loadingFinished();

  public slots:
    void currentChanged(const QModelIndex& index,
                        const QModelIndex& previous);
    void currentPageChanged(KPageWidgetItem*, KPageWidgetItem*);
    void updatePreview();
    void contextMenuRequested(const QPoint &pos);
    void imageContextMenuRequested(const QPoint &pos);
    void templatesModified();
    void projectInit();

  protected slots:
    virtual void previewClicked();

  private:
    int m_menu;
    QStringList m_menus;
    QDateTime m_lastUpdate;
    QModelIndex m_previous;
    bool m_settingPrevious;
    KMFObjectListModel<KMF::TemplateObject> m_model;
};

#endif
