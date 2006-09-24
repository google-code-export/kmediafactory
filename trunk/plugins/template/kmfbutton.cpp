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
#include "kmfbutton.h"
#include "kmfmenu.h"
#include "kmfmenupage.h"
#include <qdom.h>

KMFButton::KMFButton(QObject *parent)
 : KMFWidget(parent)
{
  page()->addButton(this);
}

KMFButton::~KMFButton()
{
}

void KMFButton::fromXML(const QDomElement& element)
{
  KMFWidget::fromXML(element);
  m_directions[Up] = element.attribute("up");
  m_directions[Down] = element.attribute("down");
  m_directions[Left] = element.attribute("left");
  m_directions[Right] = element.attribute("right");
  m_jumpS = element.attribute("jump").toUpper();
}

void KMFButton::writeDvdAuthorXml(QDomText& element, QString type)
{
  QString s;
  QString preCommand;
  int menu;
  KMFMenuPage* menuPage = page();

  if(menuPage->isVmgm())
  {
    // VMGM jumps
    if(menuPage->directChapterPlay() && m_jump.titleSet() > 0 &&
       m_jump.chapter() == 0)
      m_jump.setTitle(1, m_jump.titleSet());

    if(m_jump.chapter() > 0 || m_jump.titleSet() > 0)
      preCommand = QString("g1=%1; g2=button; g5=%2;")
          .arg(m_jump.chapter()).arg(menuPage->index());
    else
      preCommand = QString("g1=0; g2=0; g5=0;");

    if(m_jump.chapter() > 0)
      menu = 1;
    else
      menu = m_jump.menu();

    s = preCommand + m_jump.command() + " jump ";
    if(m_jump.titleSet() > 0)
      s += QString("titleset %1 menu").arg(m_jump.titleSet());
    else
      s += QString("menu %1 ").arg(menu);
  }
  else
  {
    // titleset jumps
    if(m_jump.chapter() > 0 || m_jump.titleSet() > 0)
      preCommand = QString("g0=%1; g4=button; g6=%2;")
          .arg(m_jump.chapter()).arg(menuPage->index());
    else
      preCommand = QString("g4=0; g6=0;");

    s = preCommand + m_jump.command() + " jump ";

    if (m_jump.chapter() > 0)
    {
      int chapter;

      if(type != "dummy")
        chapter = m_jump.chapter();
      else
        chapter = 1;
      s += QString("title 1 chapter %1 ").arg(chapter);
    }
    else
    {
      if (m_jump.titleSet() == KMFJump::TS_VMGM)
        s += "vmgm ";
      s += QString("menu %1 ").arg(m_jump.menu());
    }
  }

  s = " { " + s.trimmed() + "; } ";
  element.setData(s);
}

bool KMFButton::parseJump(bool addPages)
{
  KMFMenuPage* menuPage = page();
  KMFMenu* dvdMenu = menu();

  if(m_jumpS == "PREV")
  {
    if(menuPage->titles() > 0 && menuPage->titleStart() > 0)
    {
      m_jump.setMenu(menuPage->titleStart() / menuPage->titles());
    }
    else if(menuPage->chapters() > 0 && menuPage->chapterStart() > 0)
    {
      m_jump.setMenu(menuPage->chapterStart() / menuPage->chapters());
    }
    else
    {
      hide();
    }
  }
  else if(m_jumpS == "NEXT")
  {
    if(menuPage->titles() > 0 &&
       (menuPage->titles() + menuPage->titleStart()) < dvdMenu->mediaObjCount())
    {
      m_jump.setMenu(menuPage->titleStart() / menuPage->titles() + 2);
    }
    else if(menuPage->chapters() > 0 &&
            (menuPage->chapters() + menuPage->chapterStart()) <
             dvdMenu->mediaObjChapterCount(menuPage->titleStart()))
    {
      m_jump.setMenu(menuPage->chapterStart() / menuPage->chapters() + 2);
    }
    else
    {
      hide();
    }
  }
  else if(m_jumpS == "BACK")
  {
    m_jump.setMenu(1, KMFJump::TS_VMGM);
    m_jump.setCommand(" if (g0 gt 0) resume; else");
  }
  else
  {
    int title = menuPage->titleStart(), chapter = menuPage->chapterStart();
    QStringList j = m_jumpS.split(":");

    // TODO clean these ugly ifs
    if(j[0].indexOf(".") > -1)
    {
      parseTitleChapter(j[0], title, chapter);
      if(menuPage->titles() != 0 && title > dvdMenu->mediaObjCount())
      {
        hide();
        return true;
      }
      if(menuPage->chapters() != 0 &&
         chapter > dvdMenu->mediaObjChapterCount(menuPage->titleStart()))
      {
        hide();
        return true;
      }
      if(chapter < 1)
        chapter = 1;
      m_jump.setTitle(chapter, title);
      //if(menuPage->isVmgm())
        //m_jump.setCommand("g1=1;");
    }
    else
    {
      if(j.count() > 1)
      {
        parseTitleChapter(j[1], title, chapter);
        if(menuPage->titles() > 0 && title > dvdMenu->mediaObjCount())
        {
          hide();
          return true;
        }
        if(menuPage->chapters() > 0 &&
           chapter > dvdMenu->mediaObjChapterCount(menuPage->titleStart()))
        {
          hide();
          return true;
        }
      }
      if(addPages)
        if(menu()->addPage(j[0], title, chapter) == false)
          return false;
      m_jump.setMenu(1, title);
    }
  }
  return true;
}

KMFButton* KMFButton::parseDirection(const QString& dir)
{
  QStringList d = dir.split(",");
  KMFMenuPage* p = page();
  KMFButton* btn = 0;

  for(QStringList::Iterator it=d.begin(); it!=d.end(); ++it)
  {
    btn = p->button(*it);
    if(btn)
      return btn;
  }
  return 0;
}

void KMFButton::parseDirections()
{
#warning TODO
#if 0
  setUp(parseDirection(direction(Up)));
  setLeft(parseDirection(direction(Left)));
  setDown(parseDirection(direction(Down)));
  setRight(parseDirection(direction(Right)));
#endif
}

#include "kmfbutton.moc"
