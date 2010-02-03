// **************************************************************************
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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
// **************************************************************************

#ifndef SLIDESHOWPROPERTIES_H
#define SLIDESHOWPROPERTIES_H

#include <QtGui/QPixmap>

#include <kmflistmodel.h>
#include <ui_slideshowproperties.h>
#include "slideshowobject.h"

class KFileItem;
class SlideshowObject;
class SubtitleOptionsWidget;

class SlideListModel : public KMFListModel<Slide>
{
    virtual int columnCount(const QModelIndex &) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual QVariant headerData(int column, Qt::Orientation, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    public:
        void setPreview(const QString &file, const QPixmap &pixmap);

    private:
        QMap<QString, QPixmap> m_previews;
};

class SlideshowProperties : public KDialog, public Ui::SlideshowProperties
{
    Q_OBJECT

    public:
        SlideshowProperties(QWidget *parent, bool showSubPage);
        ~SlideshowProperties();

        void getData(SlideshowObject &obj) const;
        void setData(const SlideshowObject &obj);

    protected:
        void addSlides(const SlideList &slides);
        void addAudio(const QStringList &files);

    public slots:
        void gotPreview(const KFileItem &item, const QPixmap &pixmap);

    protected slots:
        virtual void moveUp();
        virtual void moveDown();
        virtual void add();
        virtual void remove();
        virtual void moveUpAudio();
        virtual void moveDownAudio();
        virtual void addAudio();
        virtual void removeAudio();
        virtual void okClicked();
        void updateInfo();

    private:
        const SlideshowObject *m_sob;
        SlideListModel m_model;
        KMFListModel<QString> m_audioModel;
        SubtitleOptionsWidget *m_subtitleWidget;
};

#endif
