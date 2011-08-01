// **************************************************************************
//   Copyright (C) 2004-2008 by Petri Damsten
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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
// **************************************************************************

#include "templatepage.h"

#include <QtCore/QTimer>
#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QMenu>

#include <KCursor>
#include <KDebug>
#include <KFileDialog>
#include <KFileItemDelegate>
#include <KLocale>
#include <KMessageBox>
#include <KPageDialog>
#include <KXMLGUIFactory>

#include <kmediafactorysettings.h>
#include <kmfimageview.h>
#include <kmftools.h>
#include "kmediafactory.h"
#include "kmfapplication.h"

TemplatePage::TemplatePage(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    templates->setSpacing(5);
    templates->setItemDelegate(new KFileItemDelegate(this));
    templates->setIconSize(QSize(KIconLoader::SizeHuge, KIconLoader::SizeHuge));
    templatePreview->setScaled(KMediaFactorySettings::previewScaled());
    connect(templates, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(contextMenuRequested(const QPoint &)));
    connect(templatePreview,
            SIGNAL(contextMenuRequested(const QPoint &)),
            this, SLOT(imageContextMenuRequested(const QPoint &)));
}

TemplatePage::~TemplatePage()
{
}

void TemplatePage::projectInit()
{
    templates->setModel(kmfApp->project()->templateObjects());
    connect(templates->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(selectionChanged(const QItemSelection &, const QItemSelection &)));
    templates->blockSignals(true);
    KMF::TemplateObject *obj = kmfApp->project()->templateObj();
    QModelIndex i = kmfApp->project()->templateObjects()->indexOf(obj);

    if (i == QModelIndex()) {
        i = kmfApp->project()->templateObjects()->index(0);
    }

    templates->selectionModel()->select(i, QItemSelectionModel::ClearAndSelect);
    templates->blockSignals(false);
}

void TemplatePage::selectionChanged(const QItemSelection &selected,
                                    const QItemSelection &deselected)
{
    if (selected.indexes().count() == 0) {
        templates->selectionModel()->select(deselected, QItemSelectionModel::ClearAndSelect);
        return;
    }

    if (kmfApp->project()) {
        QModelIndex index = selected.indexes()[0];
        KMF::TemplateObject *ob = kmfApp->project()->templateObjects()->at(index);

        if (ob) {
            if (ob->clicked() == false) {
                m_menu.reset();
                kmfApp->project()->setTemplateObj(ob);
                updatePreview(index.row());
                connect(ob, SIGNAL(updated()), SLOT(updatePreview()));
            } else   {
                templates->selectionModel()->select(deselected, QItemSelectionModel::ClearAndSelect);
            }
        }
    }
}

void TemplatePage::currentPageChanged(KPageWidgetItem *current, KPageWidgetItem *)
{
    Q_UNUSED(current)

    if (/*current->parent() == this &&*/ isVisible() &&
                                         ((templatePreview->image().size() == QSize(0, 0)) ||
                                          (m_lastUpdate <
                                           kmfApp->project()->lastModified(KMF::Media))))
    {
        QTimer::singleShot(0, this, SLOT(updatePreview()));
    }

    // Arranges icon in a nice row. Otherwise icons are arranged in one column
    templates->setViewMode(QListView::IconMode);
}

void TemplatePage::updatePreview(int n)
{
    if (!isVisible()) {
        return;
    }

    if ((n < 0) && templates->selectionModel()) {
        QModelIndexList selected = templates->selectionModel()->selectedIndexes();

        if (selected.count() > 0) {
            n = selected[0].row();
        }
    }

    if ((n >= 0) && (n < kmfApp->project()->templateObjects()->rowCount())) {
        QString menu;

        kmfApp->setOverrideCursor(QCursor(Qt::WaitCursor));
        kmfApp->interface()->setUseMessageBox(true);
        kmfApp->interface()->setStopped(false);

        if (kmfApp->project()->mediaObjects()->count() > 0 /*&&
                                                       * previewCheckBox->isChecked()*/
            ) {
            QModelIndexList selected = templates->selectionModel()->selectedIndexes();
            KMF::TemplateObject *ob =
                kmfApp->project()->templateObjects()->at(selected[0].row());
            QStringList menus = ob->menus();

            if (m_menu.page < menus.count()) {
                menu = menus[m_menu.page];
            } else {
                menu = "Main";
            }
        }

        KMF::TemplateObject *ob = kmfApp->project()->templateObjects()->at(n);
        // Scale to real 4:3. Should get aspect ratio from template plugin?
        QImage image = ob->preview(menu, m_menu.title, m_menu.chapter).scaled(768, 576, Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation);
        templatePreview->setImage(image);
        kmfApp->interface()->setUseMessageBox(false);
        kmfApp->restoreOverrideCursor();
        m_lastUpdate = QDateTime::currentDateTime();
    }
}

void TemplatePage::contextMenuRequested(const QPoint &pos)
{
    QModelIndex i = templates->indexAt(pos);

    if ((i.row() < 0) || (i.row() > kmfApp->project()->templateObjects()->count())) {
        return;
    }

    KMF::TemplateObject *ob = kmfApp->project()->templateObjects()->at(i.row());
    KMediaFactory *mainWindow = kmfApp->mainWindow();
    KXMLGUIFactory *factory = mainWindow->factory();

    QList<QAction *> actions;
    ob->actions(&actions);
    factory->plugActionList(mainWindow,
            QString::fromLatin1("template_actionlist"), actions);
    QWidget *w = factory->container("template_popup", mainWindow);

    if (w) {
        QMenu *popup = static_cast<QMenu *>(w);

        if (popup->actions().count() > 0) {
            if (popup->exec(templates->viewport()->mapToGlobal(pos)) != 0) {
                updatePreview();
            }
        }
    }

    factory->unplugActionList(mainWindow, "template_actionlist");
}

void TemplatePage::imageContextMenuRequested(const QPoint &pos)
{
    QMenu popup;
    QAction *action;
    QAction *saveAction = new QAction(i18n("Save Image..."), this);
    QAction *scaledAction = new QAction(i18nc("Whether image is shown scaled or not",
                                              "Scaled"), this);
    int selected = templates->selectionModel()->selection().indexes()[0].row();
    KMF::TemplateObject *ob = kmfApp->project()->templateObjects()->at(selected);
    QStringList menus;
    int i = 0;

    if (ob) {
        menus = ob->menus();
    }

    scaledAction->setCheckable(true);
    scaledAction->setChecked(KMediaFactorySettings::previewScaled());

    popup.addAction(scaledAction);
    popup.addAction(saveAction);

    if (menus.count()) {
        popup.addSeparator();
    }

    for (QStringList::Iterator it = menus.begin();
         it != menus.end(); ++it, ++i)
    {
        QList<MenuIndex> pages;

        if(ob && kmfApp->project()->mediaObjects()) {
            int                                      chaptersPerPage=ob->chaptersPerPage(*it),
                                                     chapter(0);
            QList<KMF::MediaObject *>                mo=kmfApp->project()->mediaObjects()->list();
            QList<KMF::MediaObject *>::ConstIterator moIt(mo.constBegin()),
                                                     moEnd(mo.constEnd());

            for(int title=0; moIt!=moEnd; ++moIt, ++title) {
                for(int ch=0; ch<(*moIt)->chapters(); ++ch) {
                    chapter++;
                    if(1==chapter) {
                        pages.append(MenuIndex(i, title, ch));
                    }
                    else if(chapter==chaptersPerPage) {
                        chapter=0;
                    }
                }
            }
        }
        
        if(pages.isEmpty()) {
            pages.append(MenuIndex());
        }
        
        QList<MenuIndex>::ConstIterator pageIt(pages.constBegin()),
                                        pageEnd(pages.constEnd());
                                  
        for(int p=1; pageIt!=pageEnd; ++pageIt, ++p) {
            QString pageName(QLatin1String("CHAPTER_PAGE")==(*it)
                                ? i18n("Chapter Page") 
                                : QLatin1String("Main")==(*it)
                                    ? i18n("Main Page")
                                    : *it);
            action = new QAction(pages.count()>1 ? i18nc("<page name> <page number>", "%1 %2", pageName, p) : pageName, this);
            action->setCheckable(true);
            action->setData((*pageIt).toInt());
            action->setChecked(*pageIt == m_menu);
            popup.addAction(action);
        }
    }

    QList<QAction *> templateActions;

    if (templates->selectionModel()) {
        QModelIndexList selected = templates->selectionModel()->selectedIndexes();

        if ((selected.count() > 0) && (selected[0].row() >= 0) &&
            (selected[0].row() < kmfApp->project()->templateObjects()->count())) {
            KMF::TemplateObject *ob = kmfApp->project()->templateObjects()->at(selected[0].row());

            ob->actions(&templateActions);

            if (templateActions.count()) {
                QList<QAction *>::ConstIterator it(templateActions.begin()),
                end(templateActions.end());

                popup.addSeparator();

                for (; it != end; ++it) {
                    popup.addAction(*it);
                }
            }
        }
    }

    if ((action = popup.exec(pos)) != 0) {
        if (action == scaledAction) {
            KMediaFactorySettings::setPreviewScaled(
                    KMediaFactorySettings::previewScaled() ? false : true);
            templatePreview->setScaled(KMediaFactorySettings::previewScaled());
            KMediaFactorySettings::self()->writeConfig();
        } else if (action == saveAction)    {
            KUrl url = KFileDialog::getSaveUrl(KUrl("kfiledialog:///<KMFPreview>"),
                    "image/png",
                    kapp->activeWindow());

            if (!url.isEmpty() && url.isValid()) {
                if (!templatePreview->image().save(url.path(), "PNG", 0)) {
                    KMessageBox::error(kmfApp->activeWindow(),
                            i18n("Could not write to file: %1", url.prettyUrl()));
                }
            }
        } else if (!templateActions.contains(action))    {
            m_menu = MenuIndex(action->data().toInt());
            // previewCheckBox->setChecked(true);
            updatePreview();
        }
    }
}

void TemplatePage::previewClicked()
{
    updatePreview();
}

#include "templatepage.moc"
