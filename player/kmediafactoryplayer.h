/***************************************************************************
 *   Copyright (C) 2005 by Petri Damsten   *
 *   petri.damsten@iki.fi   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef KMEDIAFACTORYPLAYER_H
#define KMEDIAFACTORYPLAYER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// Include these Qt headers here, so they get included before any X11 headers.
// This enables build with --enable-final
#include <qdir.h>
#include <qvariant.h>
#include <qslider.h>
// End of pre include

#include <kmainwindow.h>

namespace KMF
{
  class Player;
};

/**
 * @short Application Main Window
 * @author Petri Damsten <petri.damsten@iki.fi>
 * @version 0.1
 */
class KMediaFactoryPlayer : public KMainWindow
{
    Q_OBJECT
  public:
    /**
     * Default Constructor
     */
    KMediaFactoryPlayer();

    /**
     * Default Destructor
     */
    virtual ~KMediaFactoryPlayer();

  private:
    KMF::Player* m_player;
};

#endif // KMEDIAFACTORYPLAYER_H
