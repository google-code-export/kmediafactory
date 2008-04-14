//**************************************************************************
//   Copyright (C) 2008 Petri Damsten <damu@iki.fi>
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

#ifndef KROSSUIINTERFACE_H
#define KROSSUIINTERFACE_H

#include <QObject>
#include <QMap>
#include <kross/core/object.h>
#include "kmediafactory/uiinterface.h"

class KrossUiInterface : public QObject
{
  Q_OBJECT
  public:
    KrossUiInterface(QObject* parent, KMF::UiInterface* uiIf);
    ~KrossUiInterface();

  public slots:
      bool addMediaAction(const QString& icon, const QString& text,
                          const QString& shortcut, const QString& name,
                          Kross::Object::Ptr obj, const QString& method);
      void setActionEnabled(const QString& name, bool enabled);
      bool addMediaObject(Kross::Object::Ptr media) const;
      bool addTemplateObject(Kross::Object::Ptr tob);
      bool addOutputObject(Kross::Object::Ptr oob);
      bool removeMediaObject(Kross::Object::Ptr media) const;
      bool removeTemplateObject(Kross::Object::Ptr tob);
      bool removeOutputObject(Kross::Object::Ptr oob);
      void addMediaObjectFromXML(const QString& xml);
      void setTemplateFromXML(const QString& xml);
      void setOutputFromXML(const QString& xml);

      bool message(KMF::MsgType type, const QString& msg);
      bool progress(int advance);
      bool setItemTotalSteps(int totalSteps);
      bool setItemProgress(int progress);
      QObject* logger();

      // Plugin helpers
      QStringList getOpenFileNames(const QString &startDir, const QString &filter, 
                                   const QString &caption);
      void debug(const QString &txt);
      int  messageBox(const QString &caption, const QString &txt, int type);
      QObject* progressDialog(const QString &caption, const QString &label, int maximum);
      QObject* progressDialog();

  private:
    KMF::UiInterface* m_uiIf;
};

#endif // KROSSUIINTERFACE_H
