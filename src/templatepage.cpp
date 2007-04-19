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
#include "templatepage.h"
#include "kmficonview.h"
#include "kmediafactory.h"
#include "kmfapplication.h"
#include "kmfimageview.h"
#include <kmftools.h>
#include <kpagedialog.h>
#include <kcursor.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kxmlguifactory.h>
#include <QTimer>
#include <QCheckBox>
#include <QLabel>
#include <QMenu>

TemplatePage::TemplatePage(QWidget *parent) :
  QWidget(parent), m_menu(0), m_settingPrevious(false)
{
  setupUi(this);
  templates->setSpacing(5);
  templates->setItemDelegate(new KMFItemDelegate());
  connect(templates, SIGNAL(customContextMenuRequested(const QPoint&)),
          this, SLOT(contextMenuRequested(const QPoint&)));
  connect(templatePreview,
          SIGNAL(contextMenuRequested(const QPoint&)),
          this, SLOT(imageContextMenuRequested(const QPoint&)));
}

TemplatePage::~TemplatePage()
{
}

void TemplatePage::projectInit()
{
  QList<KMF::TemplateObject*>* tobs = kmfApp->project()->templateObjects();
  m_model.setData(tobs);
  templates->setModel(&m_model);
  connect(templates->selectionModel(),
          SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
          this, SLOT(currentChanged(const QModelIndex&, const QModelIndex&)));
  templates->blockSignals(true);
  KMF::TemplateObject* obj = kmfApp->project()->templateObj();
  QModelIndex i = m_model.index(obj);
  templates->selectionModel()->select(i, QItemSelectionModel::ClearAndSelect);
  templates->blockSignals(false);
}

void TemplatePage::templatesModified()
{
  m_model.changed();
}

void TemplatePage::currentChanged(const QModelIndex& index, const QModelIndex&)
{
  if(kmfApp->project())
  {
    KMF::TemplateObject* ob =
        kmfApp->project()->templateObjects()->at(index.row());

    if(ob->clicked() == false and !m_settingPrevious)
    {
      m_menus = ob->menus();
      m_menu = 0;

      kmfApp->project()->setTemplateObj(ob);
      updatePreview();
      m_previous = index;
    }
    else if(!m_settingPrevious)
    {
      m_settingPrevious = true;
      templates->setCurrentIndex(m_previous);
      m_settingPrevious = false;
    }
  }
}

void TemplatePage::currentPageChanged(KPageWidgetItem* current,
                                      KPageWidgetItem*)
{
  m_previous = templates->currentIndex();

  if (current->parent() == this &&
      (templatePreview->image().size() == QSize(0,0) ||
      m_lastUpdate <
      kmfApp->project()->lastModified(KMF::ProjectInterface::DirtyMedia)))
  {
    QTimer::singleShot(0, this, SLOT(updatePreview()));
  }
  // Arranges icon in a nice row. Otherwise icons are arranged in one column
  templates->setViewMode(QListView::IconMode);
}

void TemplatePage::updatePreview()
{
  QModelIndexList selected = templates->selectionModel()->selectedIndexes();

  if(selected.count() > 0)
  {
    int n = selected[0].row();

    if(n < kmfApp->project()->templateObjects()->count())
    {
      QString menu;

      kmfApp->setOverrideCursor(QCursor(Qt::WaitCursor));
      kmfApp->uiInterface()->setUseMessageBox(true);
      kmfApp->uiInterface()->setStopped(false);
      if(kmfApp->project()->mediaObjects()->count() > 0 &&
        previewCheckBox->isChecked())
      {
        if(m_menu < m_menus.count())
          menu = m_menus[m_menu];
        else
          menu = "Main";
      }
      KMF::TemplateObject* ob = kmfApp->project()->templateObjects()->at(n);
      // Scale to real 4:3. Should get aspect ratio from template plugin?
      QImage image = ob->preview(menu).scaled(768, 576, Qt::IgnoreAspectRatio,
                                              Qt::SmoothTransformation);
      templatePreview->setImage(image);
      kmfApp->uiInterface()->setUseMessageBox(false);
      kmfApp->restoreOverrideCursor();
      m_lastUpdate = QDateTime::currentDateTime();
    }
  }
}

void TemplatePage::contextMenuRequested(const QPoint &pos)
{
  QModelIndex i = templates->indexAt(pos);

  if(i.row() < 0 || i.row() > kmfApp->project()->templateObjects()->count())
    return;

  KMF::TemplateObject* ob = kmfApp->project()->templateObjects()->at(i.row());
  KMediaFactory* mainWindow = kmfApp->mainWindow();
  KXMLGUIFactory* factory = mainWindow->factory();

  QList<QAction*> actions;
  ob->actions(actions);
  factory->plugActionList(mainWindow,
      QString::fromLatin1("template_actionlist"), actions);
  QWidget *w = factory->container("template_popup", mainWindow);
  if(w)
  {
    QMenu* popup = static_cast<QMenu*>(w);
    if(popup->actions().count() > 0)
      if(popup->exec(templates->viewport()->mapToGlobal(pos)) != 0)
        updatePreview();
  }
  factory->unplugActionList(mainWindow, "template_actionlist");
}

void TemplatePage::imageContextMenuRequested(const QPoint& pos)
{
  QMenu popup;
  int i = 0;

  QAction* action;
  QAction* saveAction = new QAction(i18n("Save image"), this);

  popup.addAction(saveAction);
  popup.insertSeparator(saveAction);
  for(QStringList::Iterator it = m_menus.begin();
      it != m_menus.end(); ++it, ++i)
  {
    action = new QAction(*it, this);
    action->setCheckable(true);
    action->setData(i);
    if(i == m_menu)
      action->setChecked(true);
    popup.addAction(action);
  }
  if((action = popup.exec(pos)) != 0)
  {
    if(action == saveAction)
    {
      KUrl url = KFileDialog::getSaveUrl(KUrl("kfiledialog:///<KMFPreview>"),
                                         i18n("*.png|PNG Graphics file"));
      if (!url.isEmpty() && url.isValid())
      {
        if(!templatePreview->image().save(url.path(), "PNG", 0))
        {
          KMessageBox::error(kmfApp->activeWindow(),
              i18n("Could not write to file: %1").arg(url.prettyUrl()));
        }
      }
    }
    else
    {
      m_menu = action->data().toInt();
      previewCheckBox->setChecked(true);
      updatePreview();
    }
  }
}

void TemplatePage::previewClicked()
{
  updatePreview();
}

void TemplatePage::loadingFinished()
{
#warning TODO ???
  /*
  Q3IconViewItem* item = templates->lastItem();

  if(item)
  {
    item = item->prevItem();
    if(item)
      templates->setAfter(item);
  }
  */
}

#include "templatepage.moc"
