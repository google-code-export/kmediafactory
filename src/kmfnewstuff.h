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

#ifndef KMFNEWSTUFF_H
#define KMFNEWSTUFF_H

#include <qobject.h>
#include <kurl.h>
#include "knewstuff/knewstuff.h"
#include "knewstuff/entry.h"

class KMFNewStuff : public QObject, public KNewStuff
{
  Q_OBJECT
  public:
    KMFNewStuff(const QString& provider, QWidget* parentWidget);

    bool install(const QString &fileName);
    bool createUploadFile(const QString &fileName);

  signals:
    void scriptInstalled();

  private:
    QString m_tmpFile;
};

#endif // KMFNEWSTUFF_H
