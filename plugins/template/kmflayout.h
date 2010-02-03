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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//**************************************************************************
#ifndef KMFLAYOUT_H
#define KMFLAYOUT_H



#include "kmfwidget.h"

class KMFGrid : public KMFWidget
{
  Q_OBJECT
  public:
    KMFGrid(QObject *parent = 0)
        : KMFWidget(parent) {};
    ~KMFGrid() {};
    virtual bool isHidden() const;
    virtual int childY(const KMFWidget* child) const;
    virtual int childHeight() const;
    virtual int childHeightPercentage() const;
    virtual int childX(const KMFWidget* child) const;
    virtual int childWidth() const;
    virtual int childWidthPercentage() const;
    virtual int minimumPaintWidth() const;
    virtual int minimumPaintHeight() const;
};

class KMFVBox : public KMFGrid
{
    Q_OBJECT
  public:
    KMFVBox(QObject *parent = 0);
    ~KMFVBox();
    virtual int minimumPaintWidth() const;
    virtual int childX(const KMFWidget*) const { return paintX(); };
    virtual int childWidth() const  { return paintWidth(); };
    virtual int childWidthPercentage() const { return 10000; };
};

class KMFHBox : public KMFGrid
{
    Q_OBJECT
  public:
    KMFHBox(QObject *parent = 0);
    ~KMFHBox();
    virtual int minimumPaintHeight() const;
    virtual int childY(const KMFWidget*) const { return paintY(); };
    virtual int childHeight() const  { return paintHeight(); };
    virtual int childHeightPercentage() const { return 10000; };
};

#endif

