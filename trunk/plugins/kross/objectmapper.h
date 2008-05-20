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

#ifndef OBJECTMAPPER_H
#define OBJECTMAPPER_H

#include <kross/core/object.h>

class ObjectMapper
{
  public:
    ObjectMapper(const Kross::Object::Ptr krossObject, QObject* qObject);
    ~ObjectMapper();
    
    template<class T> static T qObject(Kross::Object::Ptr krossObject)
    {
      return qobject_cast<T>(m_map[krossObject.data()]);
    }

  private:
    static QMap<const Kross::Object*, QObject*> m_map;
    const Kross::Object* m_me;
};

#endif // OBJECTMAPPER_H
