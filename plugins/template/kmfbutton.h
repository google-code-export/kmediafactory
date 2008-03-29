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
#ifndef KMFBUTTON_H
#define KMFBUTTON_H

#include <kmfwidget.h>

class QDomText;

/**
*/
class KMFJump
{
  public:
    enum { TS_NONE = -1, TS_VMGM = 0 };
    KMFJump() :
      m_titleset(TS_NONE), m_menu(-1),
      m_title(-1), m_chapter(0) {};
    KMFJump(int menu, int titleset = TS_NONE) { setMenu(menu, titleset); };

    void setMenu(int menu, int titleset = TS_NONE)
      { m_titleset = titleset; m_title = -1; m_chapter = 0; m_menu = menu; };
    void setTitle(int chapter, int titleset = TS_NONE)
      { m_titleset = titleset; m_title = 1; m_chapter = chapter;
        m_menu = -1; };
    int titleSet() { return m_titleset; };
    int menu() { return m_menu; };
    //int title() { return m_title; };
    int chapter() { return m_chapter; };
    const QString& command() const { return m_command; };
    void setCommand(const QString& command) { m_command = command; };
  private:
    int m_titleset;
    int m_menu;
    int m_title;
    int m_chapter;
    QString m_command;
};

/**
*/
class KMFButton : public KMFWidget
{
    Q_OBJECT
  public:
    enum Direction { Up = 0, Right, Down, Left };

    KMFButton(QObject *parent = 0, const char *name = 0);
    ~KMFButton();

    KMFButton* up() { return m_buttons[Up]; };
    void setUp(KMFButton* up) { m_buttons[Up] = up; };
    KMFButton* down() { return m_buttons[Down]; };
    void setDown(KMFButton* down) { m_buttons[Down] = down; };
    KMFButton* left() { return m_buttons[Left]; };
    void setLeft(KMFButton* left) { m_buttons[Left] = left; };
    KMFButton* right() { return m_buttons[Right]; };
    void setRight(KMFButton* right) { m_buttons[Right] = right; };
    const QString& direction(Direction dir) { return m_directions[dir]; };

    KMFJump jump() { return m_jump; };
    void setJump(KMFJump jump) { m_jump = jump; };

    void fromXML(const QDomElement& element);
    void writeDvdAuthorXml(QDomText& element, QString type);
    bool parseJump(bool addPages = true);
    void parseDirections();

  private:
    KMFButton* m_buttons[4];
    QString m_directions[4];
    KMFJump m_jump;
    QString m_jumpS;

    KMFButton* parseDirection(const QString& dir);
};

#endif
