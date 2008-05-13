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
#include "dvdauthorobject.h"
#include "outputplugin.h"
#include <KMimeType>
#include <KIconLoader>
#include <KLocale>
#include <KDebug>
#include <KAboutData>
#include <QPixmap>
#include <QFile>

DvdAuthorObject::DvdAuthorObject(QObject* parent)
  : OutputObject(parent)
{
  setObjectName("dvdauthor");
  setTitle(i18n("DVDAuthor project"));
}

DvdAuthorObject::~DvdAuthorObject()
{
  KMF::UiInterface *ui = uiInterface();
  if(ui)
    ui->removeOutputObject(this);
}

void DvdAuthorObject::actions(QList<QAction*>&) const
{
  //actionList.append(dvdAuthorProperties);
}

bool DvdAuthorObject::fromXML(const QDomElement&)
{
  return true;
}

int DvdAuthorObject::timeEstimate() const
{
  return TotalPoints;
}

QDomElement DvdAuthorObject::toElement(const QVariant& element)
{
  if (element.canConvert<QDomElement>()) {
    return element.value<QDomElement>();
  } else {
    QDomDocument doc;
    doc.setContent(element.toString());
    return doc.documentElement();
  }
}

/*
bool KMFMenu::writeDvdAuthorXml(const QString& fileName, QString type)
{
  QDomDocument doc("");
  doc.appendChild(doc.createProcessingInstruction("xml",
                  "version=\"1.0\" encoding=\"UTF-8\""));
  if(writeDvdAuthorXml(doc, type))
  {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
      return false;
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream << doc.toString();
    file.close();
    return true;
  }
  return false;
}
*/

bool DvdAuthorObject::make(QString)
{
  if(uiInterface()->message(KMF::Info, i18n("Generating DVDAuthor xml")))
    return false;

  QDomDocument doc("");
  doc.appendChild(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));
  QDomElement root = doc.createElement("dvdauthor");
  KMF::TemplateObject* tempObj = projectInterface()->templateObject();
  int i;
  QString tmp;
  QString header = "\n"
    " *********************************************************************\n"
    " %1\n"
    " *********************************************************************\n";
  QString comment = i18n("\n"
    "**********************************************************************\n"
    "\n"
    "This file was made with %1 - %2\n"
    "http://www.iki.fi/damu/software/kmediafactory/\n"
    "\n"
    "**********************************************************************\n"
    "\n"
    "Register usage:\n"
    "================\n"
    "g0: Playing chapter saved here.\n"
    "g1: Play this chapter when entering titleset menu. 0 == show menu.\n"
    "    Also used by Back button.\n"
    "g2: Used vmgm button is saved here\n"
    "g3: Jump to this titleset. Also used for direct play.\n"
    "g4: Used titleset button is saved here\n"
    "g5: vmgm page is saved here\n"
    "g6: titleset page is saved here\n"
    "g7: temporary variable\n"
    "\n"
    "**********************************************************************\n"
    , KGlobal::mainComponent().aboutData()->programName()
    , KGlobal::mainComponent().aboutData()->version());

  doc.appendChild(doc.createComment(comment));
  doc.appendChild(doc.createTextNode("\n"));

  root.setAttribute("dest", "./DVD");
  doc.appendChild(root);

  root.appendChild(doc.createTextNode("\n "));
  tmp = i18n("Main menu for %1", projectInterface()->title());
  root.appendChild(doc.createComment(header.arg(tmp)));
  root.appendChild(doc.createTextNode("\n "));

  QDomElement vmgm = doc.createElement("vmgm");

  QDomElement fpc = doc.createElement("fpc");

  int titleset = 0;
  if (tempObj->call("directPlay").toBool()) {
      titleset = 1;
  }
  fpc.appendChild(doc.createTextNode(QString(
      " { g0 = 0; g1 = 0; g2 = 0; g3 = %1; g4 = 0; g5 = 0; g6 = 0;"
      " jump menu 1; }").arg(titleset)));
  vmgm.appendChild(fpc);

  KMF::MediaObject *mob;
  QList<KMF::MediaObject*> mobs =  projectInterface()->mediaObjects();

  QDomElement menus = toElement(tempObj->call("writeDvdAuthorXml", QVariantList() << 0));
  menus.setTagName("menus");
  if(menus.hasChildNodes())
    vmgm.appendChild(menus);
  root.appendChild(vmgm);

  i=1;
  foreach(mob, mobs)
  {
    root.appendChild(doc.createTextNode("\n "));
    root.appendChild(doc.createComment(header.arg(mob->text())));
    root.appendChild(doc.createTextNode("\n "));

    QDomElement menus = toElement(tempObj->call("writeDvdAuthorXml", QVariantList() << i));
    menus.setTagName("menus");

    QDomElement titleset = toElement(mob->call("writeDvdAuthorXml", 
        QVariantList() << tempObj->call("language")));
    titleset.setTagName("titleset");
    titleset.appendChild(menus);

    QString postString;
    if(i < mobs.count() && tempObj->call("continueToNextTitle").toInt() == 1)
    {
      postString = QString(" g3 = %1 ; ").arg(i+1);
    }
    postString += " call vmgm menu 1 ; ";
    QDomElement post = doc.createElement("post");
    QDomText text = doc.createTextNode(postString);
    post.appendChild(text);
    QDomNodeList pgcList = titleset.elementsByTagName("pgc");
    if (pgcList.count() > 0) 
    {
      QDomElement pgc = pgcList.at(0).toElement();
      pgc.appendChild(post);
    }
    root.appendChild(titleset);
    ++i;
  }

  QFile file(projectInterface()->projectDir() + "/dvdauthor.xml");
  if (!file.open(QIODevice::WriteOnly)) {
    uiInterface()->message(KMF::Error, i18n("Error in saving dvdauthor.xml"));
    return false;
  }
  QTextStream stream(&file);
  stream.setCodec("UTF-8");
  doc.save(stream, 2);
  file.close();

  uiInterface()->message(KMF::OK, i18n("DVDAuthor project ready"));
  uiInterface()->progress(TotalPoints);
  return true;
}

void DvdAuthorObject::toXML(QDomElement&) const
{
}

QPixmap DvdAuthorObject::pixmap() const
{
  return KIconLoader::global()->loadIcon("media-optical", KIconLoader::NoGroup,
                                         KIconLoader::SizeLarge);
}

#include "dvdauthorobject.moc"
