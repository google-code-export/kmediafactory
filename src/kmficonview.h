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
#ifndef KMFTEMPLATEICONVIEW_H
#define KMFTEMPLATEICONVIEW_H

#include <kmediafactory/projectinterface.h>
#include <Q3IconView>
#include <QPixmap>

/**
*/
class KMFIconViewItem : public Q3IconViewItem
{
  public:
    KMFIconViewItem(Q3IconView* parent, const QString& text,
                    const QPixmap& icon):
        Q3IconViewItem(parent, text, icon) { calcRect(); };
    KMFIconViewItem(Q3IconView* parent, Q3IconViewItem* after,
                    const QString& text, const QPixmap& icon):
        Q3IconViewItem(parent, after, text, icon) { calcRect(); };
    virtual ~KMFIconViewItem() {};

    KMF::Object* ob() { return m_ob; };
    void setOb(KMF::Object* ob) { m_ob = ob; };

  protected:
    virtual void paintItem(QPainter* p, const QColorGroup& cg);
    virtual void paintFocus(QPainter* p, const QColorGroup& cg);
    virtual void calcRect(const QString& text_ = QString::null);

  private:
    KMF::Object* m_ob;
};

/**
*/
class KMFIconView : public Q3IconView
{
    Q_OBJECT
  public:
    KMFIconView(QWidget *parent = 0, const char *name = 0);
    ~KMFIconView();

    virtual void setCurrentItem(Q3IconViewItem* item)
        { Q3IconView::setCurrentItem(item); };
    void setAfter(Q3IconViewItem* after) { m_after = after; };

  public slots:
    void init(const QString &project_type);
    KMFIconViewItem* newItem(KMF::Object* ob);
    void itemRemoved(KMF::Object* ob);
    void setCurrentItem(KMF::Object* ob);
    void setSelected(KMF::Object* ob, bool s, bool cb = false);
    virtual void clear();

  private:
    QMap<KMF::Object*, KMFIconViewItem*> m_obs;
    Q3IconViewItem* m_after;
};

#endif // KMFTEMPLATEICONVIEW_H
