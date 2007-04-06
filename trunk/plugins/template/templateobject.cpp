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

#include "templateobject.h"
#include "templateplugin.h"
#include "kmfmenu.h"
#include "kmftools.h"
#include "kmftemplate.h"
#include "kmflanguagewidgets.h"
#include "templatepluginsettings.h"
#include "dvdauthorparser.h"
#include <kmimetype.h>
#include <kapplication.h>
#include <kurl.h>
#include <klocale.h>
#include <kconfigdialog.h>
#include <kaction.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <kicon.h>
#include <kactioncollection.h>
#include <kconfigdialogmanager.h>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QBuffer>
#include <QUiLoader>
#include <QPixmap>
#include <QTranslator>

class KMFTranslator : public QTranslator
{
  public:
    KMFTranslator(QObject *parent, const KMFTemplate& tmpl) :
      QTranslator(parent), m_tmpl(tmpl) {};
    virtual QString translate(const char*,
                              const char* sourceText,
                              const char*) const
    {
      //kdDebug() << k_funcinfo << context << endl;
      return m_tmpl.translate(sourceText);
    }

  private:
    const KMFTemplate& m_tmpl;
};

QString KMFConfigXML::parseCode(QString code)
{
  if(code.indexOf("TemplatePluginSettings::defaultMenuLanguage()"))
  {
    return TemplatePluginSettings::defaultMenuLanguage();
  }
  return "";
}

TemplateObject::TemplateObject(const QString& templateFile, QObject* parent):
  KMF::TemplateObject(parent), m_templateProperties(0),
  m_menu(templateFile, this)
{
  setObjectName(m_menu.id());
  if(m_menu.templateStore()->hasFile("settings.kcfg") &&
     m_menu.templateStore()->hasFile("settings.ui"))
  {
    m_templateProperties = new KAction(KIcon("pencil"),
                                       i18n("&Properties"),this);
    kDebug() << k_funcinfo << m_templateProperties << endl;
    plugin()->actionCollection()->addAction("tob_properties",
           m_templateProperties);
    connect(m_templateProperties, SIGNAL(triggered()), SLOT(slotProperties()));

    QByteArray ba = m_menu.templateStore()->readFile("settings.kcfg");
    m_customProperties.parse(&ba);
  }
  m_menu.setLanguage("ui", KGlobal::locale()->language());
  setTitle(m_menu.templateStore()->translate(m_menu.title().toLocal8Bit()));
  uiInterface()->addTemplateObject(this);
}

TemplateObject::~TemplateObject()
{
  KMF::UiInterface *ui = uiInterface();
  if(ui)
    uiInterface()->removeTemplateObject(this);
}

void TemplateObject::actions(QList<QAction*>& actionList) const
{
  kDebug() << k_funcinfo << m_templateProperties << endl;
  if(m_templateProperties)
  {
    actionList.append(m_templateProperties);
  }
}

void TemplateObject::fromXML(const QDomElement& element)
{
  QDomNode n = element.firstChild();
  while(!n.isNull())
  {
    QDomElement e = n.toElement();
    if(!e.isNull())
    {
      if(e.tagName() == "custom_properties")
      {
        QDomNode n2 = e.firstChild();
        while(!n2.isNull())
        {
          QDomElement e2 = n2.toElement();
          if(!e2.isNull())
          {
            if(e2.tagName() == "properties")
            {
              QString widget = e2.attribute("widget");
              QDomNode n3 = e2.firstChild();
              while(!n3.isNull())
              {
                QDomElement e3 = n3.toElement();
                if(!e3.isNull())
                {
                  if(e3.tagName() == "property")
                  {
                    setProperty(widget, e3.attribute("name"),
                                QVariant(e3.attribute("value")));
                  }
                }
                n3 = n3.nextSibling();
              }
            }
          }
          n2 = n2.nextSibling();
        }
      }
    }
    n = n.nextSibling();
  }
}

void TemplateObject::toXML(QDomElement& element) const
{
  QDomDocument doc = element.ownerDocument();
  QDomElement tmplate = doc.createElement("custom_properties");

  KConfigSkeletonItem::List items = m_customProperties.items();
  KConfigSkeletonItem::List::ConstIterator it;
  QString group;
  QDomElement e;

  for(it = items.begin(); it != items.end(); ++it)
  {
    if(group != (*it)->group())
    {
      if(!group.isEmpty())
        tmplate.appendChild(e);
      e = doc.createElement("properties");
      group = (*it)->group();
      e.setAttribute("widget", group);
    }
    QDomElement e2 = doc.createElement("property");
    e2.setAttribute("name", (*it)->name());
    e2.setAttribute("value", (*it)->property().toString());
    e.appendChild(e2);
  }
  if(!group.isEmpty())
    tmplate.appendChild(e);
  element.appendChild(tmplate);
}

int TemplateObject::timeEstimate() const
{
  return KMFMenu::TotalPoints;
}

bool TemplateObject::make(QString type)
{
  // Make menu
  if(isUpToDate(type))
  {
    uiInterface()->message(KMF::Info, i18n("Menus are up to date"));
    uiInterface()->progress(KMFMenu::TotalPoints);
    return true;
  }
  else
    return m_menu.makeMenu(type);
}

QStringList TemplateObject::menus()
{
  return m_menu.menus();
}

QImage TemplateObject::preview(const QString& menu)
{
  return m_menu.makeMenuPreview(menu);
}

QPixmap TemplateObject::pixmap() const
{
  return QPixmap::fromImage(m_menu.icon());
}

void TemplateObject::slotProperties()
{
  KMFTranslator kmftr(kapp, *m_menu.templateStore());
  LanguageListModel model;

  kapp->installTranslator(&kmftr);
  kDebug() << k_funcinfo << KGlobal::locale()->language() << endl;
  m_menu.setLanguage("ui", KGlobal::locale()->language());

  KConfigDialog dialog(kapp->activeWindow(), "TemplateSettings",
                       &m_customProperties, KPageDialog::Plain,
                       KDialog::Ok | KDialog::Cancel);

  QIODevice* di = m_menu.templateStore()->device("settings.ui");
  QUiLoader loader;
  QWidget* page = loader.load(di, &dialog);
  m_menu.templateStore()->close();
  //KMF::Tools::printChilds(page);
  //kDebug() << k_funcinfo << loader.availableWidgets() << endl;
  /*
  kDebug() << k_funcinfo << &m_customProperties << endl;
  KConfigSkeletonItem::List list = m_customProperties.items();
  KConfigSkeletonItem::List::iterator it;
  for(it = list.begin(); it != list.end(); ++it)
    kDebug() << k_funcinfo << (*it)->group() << " / " <<
        (*it)->key() << " = " << (*it)->property() << endl;
  */
  if(page)
  {
    // Give special treatment to widget named kcfg_language so we can show only
    // languages actually found from template
    QObject* w = page->findChild<QObject*>("kcfg_language");
    if(w && QString(w->metaObject()->className()) == "KMFLanguageListBox")
    {
      KMFLanguageListBox* lbox = static_cast<KMFLanguageListBox*>(w);
      lbox->model()->setLanguages(m_menu.templateStore()->languages());
    }

    //QWidget* w = (QWidget*)page->child("kcfg_language");
    //QSqlPropertyMap::defaultMap()->setProperty(w, "en");

    dialog.addPage(page, title(), "kmediafactory");
  }

  dialog.exec();

  kDebug() << k_funcinfo << &m_customProperties << endl;
  KConfigSkeletonItem::List list = m_customProperties.items();
  KConfigSkeletonItem::List::iterator it;
  for(it = list.begin(); it != list.end(); ++it)
    kDebug() << k_funcinfo << (*it)->group() << " / " <<
        (*it)->key() << " = " << (*it)->property() << endl;


  if(dialog.result() == QDialog::Accepted)
    projectInterface()->setDirty(KMF::ProjectInterface::DirtyTemplate);

  kapp->removeTranslator(&kmftr);
}

QVariant TemplateObject::property(const QString& widget,
                                  const QString& name) const
{
  KConfigSkeletonItem::List items = m_customProperties.items();
  KConfigSkeletonItem::List::ConstIterator it;
  QString group;
  QDomElement e;

  for(it = items.begin(); it != items.end(); ++it)
  {
    if((*it)->group() == widget && (*it)->name() == name)
    {
      /*
      kdDebug() << k_funcinfo
          << "Widget: " << widget
          << "\nName  : " << name
          << "\nValue : " << (*it)->property()
          << endl;
      */
      return (*it)->property();
    }
  }
  return QVariant();
}

void TemplateObject::setProperty(const QString& widget,
                                 const QString& name, const QVariant& value)
{
  KConfigSkeletonItem::List items = m_customProperties.items();
  KConfigSkeletonItem::List::Iterator it;
  QString group;
  QDomElement e;
  /*
  kdDebug() << k_funcinfo
      << "Widget: " << widget
      << "\nName  : " << name
      << "\nValue : " << value
      << endl;
  */
  for(it = items.begin(); it != items.end(); ++it)
  {
    if((*it)->group() == widget && (*it)->name() == name)
    {
      (*it)->setProperty(value);
      return;
    }
  }
}

bool TemplateObject::isUpToDate(QString type)
{
  if(type != projectInterface()->lastSubType())
    return false;

  QDateTime lastModified = projectInterface()->lastModified(
      KMF::ProjectInterface::DirtyMediaOrTemplate);
  QString file = projectInterface()->projectDir() + "/dvdauthor.xml";
  QFileInfo fileInfo(file);

  if(fileInfo.exists() == false || lastModified > fileInfo.lastModified())
    return false;

  KMF::DVDAuthorParser da;
  da.setFile(file);
  QStringList files = da.files();

  if(files.count() < 1)
    return false;

  for(QStringList::Iterator it = files.begin(); it != files.end(); ++it)
  {
    if((*it).startsWith("./menus/"))
    {
      fileInfo.setFile(projectInterface()->projectDir() + "/" + *it);

      if(fileInfo.exists() == false || lastModified > fileInfo.lastModified())
        return false;
    }
  }
  return true;
}

#include "templateobject.moc"
