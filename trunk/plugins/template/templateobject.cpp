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
#include <KMimeType>
#include <KApplication>
#include <KUrl>
#include <KLocale>
#include <KConfigDialog>
#include <KDebug>
#include <KStandardDirs>
#include <KIconLoader>
#include <KIcon>
#include <KActionCollection>
#include <KAction>
#include <KConfigDialogManager>
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
      //kDebug() << context;
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
  m_menu(templateFile, this), m_file(templateFile)
{
  setObjectName(m_menu.id());
  if(m_menu.templateStore()->hasFile("settings.kcfg") &&
     m_menu.templateStore()->hasFile("settings.ui"))
  {
    m_templateProperties = new KAction(KIcon("pencil"),
                                       i18n("&Properties"),this);
    plugin()->actionCollection()->addAction("tob_properties",
           m_templateProperties);
    connect(m_templateProperties, SIGNAL(triggered()), SLOT(slotProperties()));

    QByteArray ba = m_menu.templateStore()->readFile("settings.kcfg");
    m_customProperties.parse(&ba);
  }
  m_menu.setLanguage("ui", KGlobal::locale()->language());
  setTitle(m_menu.templateStore()->translate(m_menu.title().toLocal8Bit()));
  interface()->addTemplateObject(this);
}

TemplateObject::~TemplateObject()
{
  KMF::PluginInterface *ui = interface();
  if(ui)
    interface()->removeTemplateObject(this);
}

void TemplateObject::actions(QList<QAction*>* actionList) const
{
  if(m_templateProperties)
  {
    actionList->append(m_templateProperties);
  }
}

bool TemplateObject::fromXML(const QDomElement& element)
{
  QDomNode n = element.firstChild();
  if(n.isNull())
    return false;
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
  return true;
}

void TemplateObject::toXML(QDomElement* element) const
{
  QDomDocument doc = element->ownerDocument();
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
    e2.setAttribute("value", propertyString(*it));
    e.appendChild(e2);
  }
  if(!group.isEmpty())
    tmplate.appendChild(e);
  element->appendChild(tmplate);
}

int TemplateObject::timeEstimate() const
{
  return KMFMenu::TotalPoints;
}

bool TemplateObject::prepare(const QString& type)
{
  m_type = type;
  return m_menu.makeMenu();
}

QStringList TemplateObject::menus()
{
  //kDebug() << m_menu.menus();
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
  //kDebug() << KGlobal::locale()->language();
  m_menu.setLanguage("ui", KGlobal::locale()->language());

  KConfigDialog dialog(kapp->activeWindow(), "TemplateSettings",
                       &m_customProperties);
  dialog.setFaceType(KPageDialog::Plain);
  dialog.setButtons(KDialog::Ok | KDialog::Cancel);
  QIODevice* di = m_menu.templateStore()->device("settings.ui");
  QUiLoader loader;
  QWidget* page = loader.load(di, &dialog);
  m_menu.templateStore()->close();
  //KMF::Tools::printChilds(page);
  //kDebug() << loader.availableWidgets();
  /*
  kDebug() << &m_customProperties;
  KConfigSkeletonItem::List list = m_customProperties.items();
  KConfigSkeletonItem::List::iterator it;
  for(it = list.begin(); it != list.end(); ++it)
    kDebug() << (*it)->group() << " / " <<
        (*it)->key() << " = " << (*it)->property();
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

  /*
  kDebug() << &m_customProperties;
  KConfigSkeletonItem::List list = m_customProperties.items();
  KConfigSkeletonItem::List::iterator it;
  for(it = list.begin(); it != list.end(); ++it)
    kDebug() << (*it)->group() << " / " <<
        (*it)->key() << " = " << (*it)->property();
  */

  if(dialog.result() == QDialog::Accepted)
    interface()->setDirty(KMF::DirtyTemplate);

  kapp->removeTranslator(&kmftr);
}

QString TemplateObject::propertyString(KConfigSkeletonItem* item) const
{
  QVariant value = item->property();

  if(QString(value.typeName()) != "KUrl")
    return value.toString();
  else
    return value.value<KUrl>().prettyUrl();
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
      kDebug()
          << "Widget: " << widget
          << "\nName  : " << name
          << "\nValue : " << (*it)->property()
         ;
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
  kDebug()
      << "Widget: " << widget
      << "\nName  : " << name
      << "\nValue : " << value
     ;
  */
  for(it = items.begin(); it != items.end(); ++it)
  {
    if((*it)->group() == widget && (*it)->name() == name)
    {
      if(QString((*it)->property().typeName()) == "KUrl")
        (*it)->setProperty(KUrl(value.toString()));
      else
        (*it)->setProperty(value);
      return;
    }
  }
}

bool TemplateObject::fileExists()
{
    return QFileInfo(m_file).exists();
}

QVariant TemplateObject::directPlay(QVariantList args)
{
  Q_UNUSED(args);
  return property("%KMFMenuPage%", "direct_play");
}

QVariant TemplateObject::continueToNextTitle(QVariantList args)
{
  Q_UNUSED(args);
  return (property("%KMFMenuPage%", "continue_to_next_title") == 1);
}

QVariant TemplateObject::language(QVariantList args)
{
  Q_UNUSED(args);
  return m_menu.templateStore()->language();
}

QVariant TemplateObject::writeDvdAuthorXml(QVariantList args)
{
  QVariant result;
  QDomElement elem = m_menu.writeDvdAuthorXml(m_type, args[0].toInt());
  result.setValue(elem);
  return result;
}

#include "templateobject.moc"
