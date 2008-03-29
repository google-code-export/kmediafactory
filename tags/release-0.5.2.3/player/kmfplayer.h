//**************************************************************************
//   Copyright (C) 2004, 2005 by Petri Damstén
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
#ifndef KMFPLAYER_H
#define KMFPLAYER_H

#include "kmfplayerlayout.h"
#include <qdatetime.h>

class KURL;

namespace KMF
{
  /**
  @author Petri Damsten
  */
  class Player : public PlayerLayout
  {
      Q_OBJECT
    public:
      Player(QWidget *parent = 0, const char *name = 0);
      ~Player();

      void setURL(const KURL& dvdURL);
      void checkUrl(const KURL& url);
      void updateInfo();
      void simple();

    protected slots:
      virtual void saveFrame();
      virtual void xineMessage(const QString&);
      virtual void open();
      virtual void configureFileDialog(KURLRequester*);
      virtual void prevChapter();
      virtual void nextChapter();
      virtual void slotNewPosition(int pos, const QTime& playtime);

    private:
      QTime m_pos;
  };
}

#endif
