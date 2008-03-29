//**************************************************************************
//   Copyright (C) 2004 by Petri Damstén
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
#include <kiconview.h>

/**
*/
class KMFIconViewItem : public KIconViewItem
{
  public:
    KMFIconViewItem(QIconView* parent, const QString& text,
                    const QPixmap& icon):
        KIconViewItem(parent, text, icon) { calcRect(); };
    KMFIconViewItem(QIconView* parent, QIconViewItem* after,
                    const QString& text, const QPixmap& icon):
        KIconViewItem(parent, after, text, icon) { calcRect(); };
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
class KMFIconView : public KIconView
{
    Q_OBJECT
  public:
    KMFIconView(QWidget *parent = 0, const char *name = 0);
    ~KMFIconView();

    virtual void setCurrentItem(QIconViewItem* item)
        { QIconView::setCurrentItem(item); };
    void setAfter(QIconViewItem* after) { m_after = after; };

  public slots:
    void init(const QString &project_type);
    KMFIconViewItem* newItem(KMF::Object* ob);
    void itemRemoved(KMF::Object* ob);
    void setCurrentItem(KMF::Object* ob);
    void setSelected(KMF::Object* ob, bool s, bool cb = false);
    virtual void clear();

  private:
    QMap<KMF::Object*, KMFIconViewItem*> m_obs;
    QIconViewItem* m_after;
};

#endif // KMFTEMPLATEICONVIEW_H
