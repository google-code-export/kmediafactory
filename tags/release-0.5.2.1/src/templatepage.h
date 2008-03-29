//**************************************************************************
//   Copyright (C) 2004 by Petri Damst�
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

#include <templatepagelayout.h>
#include <qdatetime.h>

class QIconViewItem;

/**
@author Petri Damsten
*/
class TemplatePage : public TemplatePageLayout
{
    Q_OBJECT
  public:
    TemplatePage(QWidget *parent = 0, const char *name = 0);
    ~TemplatePage();

    void loadingFinished();

  public slots:
    void currentChanged(QIconViewItem* item);
    void aboutToShowPage(QWidget* page);
    void updatePreview();
    void contextMenuRequested(QIconViewItem *item, const QPoint &pos);
    void imageContextMenuRequested(const QPoint &pos);

  protected slots:
    virtual void previewClicked();

  private:
    uint m_menu;
    QStringList m_menus;
    QDateTime m_lastUpdate;
    QIconViewItem* m_previous;
    bool m_settingPrevious;
};

#endif
