//**************************************************************************
//   Copyright (C) 2004-2008 by Petri Damsten
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

#include "mediapage.h"
#include "kmediafactory.h"
#include "kmfapplication.h"
#include "sizewidget.h"
#include <kmftools.h>
#include <KXMLGUIFactory>
#include <KActionCollection>
#include <QPoint>
#include <QMenu>
#include <QTimer>
#include <QPainter>
#include <QStyledItemDelegate>

class MediaItemDelegate : public QStyledItemDelegate
{
    public:
        
    MediaItemDelegate(QObject *p, QWidget *widget) : QStyledItemDelegate(p), m_widget(widget) { }
    virtual ~MediaItemDelegate() { }
    
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QSize sz(QStyledItemDelegate::sizeHint(option, index));
        int   textHeight=m_widget->fontMetrics().height();
        return QSize(qMax(sz.width(), constIconSize)+(constBorder*2), qMax((textHeight+constBorder)*3, sz.height()+(constBorder*2)));
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        if (!index.isValid())
            return;

        QVariant dec=index.data(Qt::DecorationRole);
        QVariant disp=index.data(Qt::DisplayRole);
        QVariant user=index.data(Qt::UserRole);

        if(QVariant::Pixmap==dec.type() && QVariant::String==disp.type() && QVariant::String==user.type())
        {
            m_widget->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, 0L);

            QRect                r(option.rect);
            QFont                fnt(m_widget->font());
            QPalette::ColorGroup cg = option.state & QStyle::State_Enabled
                                            ? QPalette::Normal : QPalette::Disabled;
            QPixmap              pix(dec.value<QPixmap>());
            int                  textHeight=m_widget->fontMetrics().height(),
                                 iconPosModX=constBorder+((constIconSize-pix.width())/2),
                                 iconPosModY=(option.rect.height()-pix.height())/2;
            
            painter->drawPixmap(r.adjusted(iconPosModX, iconPosModY, iconPosModX, iconPosModY).topLeft(), pix);
            
            fnt.setBold(true);
            
            painter->setFont(fnt);
            
            if (QPalette::Normal==cg && !(option.state & QStyle::State_Active))
                cg = QPalette::Inactive;

            r.moveLeft(constIconSize+(constBorder*2));
            r.moveTop(r.y()+(constBorder+textHeight));
            painter->setPen(option.palette.color(cg, option.state&QStyle::State_Selected ? QPalette::HighlightedText : QPalette::Text));
            painter->drawText(r.topLeft(), disp.toString());
            r.moveTop(r.y()+(constBorder+textHeight));
            painter->setFont(m_widget->font());
            painter->drawText(r.topLeft(), user.toString());
            
            if (option.state & QStyle::State_HasFocus)
            {
                QStyleOptionFocusRect o;
                o.QStyleOption::operator=(option);
                o.rect = option.rect; // m_widget->style()->subElementRect(QStyle::SE_ItemViewItemFocusRect, &option, m_widget);
                o.state |= QStyle::State_KeyboardFocusChange;
                o.state |= QStyle::State_Item;
                cg = option.state&QStyle::State_Enabled  ? QPalette::Normal : QPalette::Disabled;
                o.backgroundColor = option.palette.color(cg, option.state&QStyle::State_Selected ? QPalette::Highlight : QPalette::Window);
                m_widget->style()->drawPrimitive(QStyle::PE_FrameFocusRect, &o, painter, m_widget);
            }
        }
        else
            QStyledItemDelegate::paint(painter, option, index);
    }
    
    static const int constIconSize=96;
    static const int constBorder=6;
    
    QWidget *m_widget;
};

MediaPage::MediaPage(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);

//   mediaFiles->setSpacing(5);
  mediaFiles->setItemDelegate(new MediaItemDelegate(this, mediaFiles));
//   mediaFiles->setIconSize(QSize(128, 128));
  connect(mediaFiles, SIGNAL(customContextMenuRequested(const QPoint&)),
          this, SLOT(contextMenuRequested(const QPoint&)));
}

MediaPage::~MediaPage()
{
}

void MediaPage::projectInit()
{
  mediaModified();
  mediaFiles->setModel(kmfApp->project()->mediaObjects());
}

void MediaPage::mediaModified()
{
  calculateSizes();

  QList<KMF::MediaObject*> mobs = kmfApp->project()->mediaObjects()->list();
  KMF::MediaObject         *mob;
  QTime                    duration;

  foreach(mob, mobs)
    duration = duration.addMSecs((mob->duration().hour()*60*60*1000)+
                                 (mob->duration().minute()*60*1000)+
                                 (mob->duration().second()*1000)+
                                  mob->duration().msec());

  emit details(i18n("Titles: %1 Duration: %2",
                    mobs.count(),
                    KGlobal::locale()->formatTime(duration, true, true)));
}

void MediaPage::contextMenuRequested(const QPoint &pos)
{
  QModelIndex i = mediaFiles->indexAt(pos);

  if(!kmfApp->project()->mediaObjects()->isValid(i))
    return;

  KMF::MediaObject* ob = kmfApp->project()->mediaObjects()->at(i);
  KMediaFactory* mainWindow = kmfApp->mainWindow();
  KXMLGUIFactory* factory = mainWindow->factory();

  QList<QAction*> actions;
  ob->actions(&actions);
  factory->plugActionList(mainWindow,
      QString::fromLatin1("media_file_actionlist"), actions);
  QWidget *w = factory->container("media_file_popup", mainWindow);
  if(w)
  {
    QMenu* popup = static_cast<QMenu*>(w);
    popup->exec(mediaFiles->viewport()->mapToGlobal(pos));
  }
  factory->unplugActionList(mainWindow, "media_file_actionlist");
}

void MediaPage::calculateSizes()
{
  quint64 max = 4700372992LL;
  quint64 size = 0;

  if(kmfApp->project())
  {
    QList<KMF::MediaObject*> mobs = kmfApp->project()->mediaObjects()->list();
    KMF::MediaObject *mob;

    foreach(mob, mobs)
      size += mob->size();
    size += mobs.size() * 200 * 1024; // Not very good estimate...
  }

  emit sizes(max, size);
}

#include "mediapage.moc"
