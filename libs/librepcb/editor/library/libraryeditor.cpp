/*
 * LibrePCB - Professional EDA for everyone!
 * Copyright (C) 2013 LibrePCB Developers, see AUTHORS.md for contributors.
 * https://librepcb.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*******************************************************************************
 *  Includes
 ******************************************************************************/

#include "libraryeditor.h"

#include "../dialogs/aboutdialog.h"
#include "../editorcommandset.h"
#include "../utils/exclusiveactiongroup.h"
#include "../utils/menubuilder.h"
#include "../utils/undostackactiongroup.h"
#include "../widgets/searchtoolbar.h"
#include "cat/componentcategoryeditorwidget.h"
#include "cat/packagecategoryeditorwidget.h"
#include "cmp/componenteditorwidget.h"
#include "dev/deviceeditorwidget.h"
#include "eaglelibraryimportwizard/eaglelibraryimportwizard.h"
#include "lib/libraryoverviewwidget.h"
#include "pkg/packageeditorwidget.h"
#include "sym/symboleditorwidget.h"
#include "ui_libraryeditor.h"

#include <librepcb/core/application.h>
#include <librepcb/core/exceptions.h>
#include <librepcb/core/fileio/transactionalfilesystem.h>
#include <librepcb/core/library/library.h>
#include <librepcb/core/workspace/workspace.h>
#include <librepcb/core/workspace/workspacelibrarydb.h>
#include <librepcb/core/workspace/workspacesettings.h>

#include <QtCore>
#include <QtWidgets>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {
namespace editor {

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

LibraryEditor::LibraryEditor(Workspace& ws, const FilePath& libFp,
                             bool readOnly)
  : QMainWindow(nullptr),
    mWorkspace(ws),
    mIsOpenedReadOnly(readOnly),
    mUi(new Ui::LibraryEditor),
    mCurrentEditorWidget(nullptr),
    mLibrary(nullptr) {
  mUi->setupUi(this);

  // add overview tab
  EditorWidgetBase::Context context{mWorkspace, *this, false, readOnly};
  LibraryOverviewWidget* overviewWidget =
      new LibraryOverviewWidget(context, libFp);
  mLibrary = &overviewWidget->getLibrary();
  mUi->tabWidget->addTab(overviewWidget, overviewWidget->windowIcon(),
                         overviewWidget->windowTitle());
  connect(overviewWidget, &LibraryOverviewWidget::windowTitleChanged, this,
          &LibraryEditor::updateTabTitles);
  connect(overviewWidget, &LibraryOverviewWidget::dirtyChanged, this,
          &LibraryEditor::updateTabTitles);
  connect(overviewWidget, &EditorWidgetBase::elementEdited,
          &mWorkspace.getLibraryDb(), &WorkspaceLibraryDb::startLibraryRescan);
  connect(overviewWidget, &LibraryOverviewWidget::newComponentCategoryTriggered,
          this, &LibraryEditor::newComponentCategoryTriggered);
  connect(overviewWidget, &LibraryOverviewWidget::newPackageCategoryTriggered,
          this, &LibraryEditor::newPackageCategoryTriggered);
  connect(overviewWidget, &LibraryOverviewWidget::newSymbolTriggered, this,
          &LibraryEditor::newSymbolTriggered);
  connect(overviewWidget, &LibraryOverviewWidget::newPackageTriggered, this,
          &LibraryEditor::newPackageTriggered);
  connect(overviewWidget, &LibraryOverviewWidget::newComponentTriggered, this,
          &LibraryEditor::newComponentTriggered);
  connect(overviewWidget, &LibraryOverviewWidget::newDeviceTriggered, this,
          &LibraryEditor::newDeviceTriggered);
  connect(overviewWidget,
          &LibraryOverviewWidget::editComponentCategoryTriggered, this,
          &LibraryEditor::editComponentCategoryTriggered);
  connect(overviewWidget, &LibraryOverviewWidget::editPackageCategoryTriggered,
          this, &LibraryEditor::editPackageCategoryTriggered);
  connect(overviewWidget, &LibraryOverviewWidget::editSymbolTriggered, this,
          &LibraryEditor::editSymbolTriggered);
  connect(overviewWidget, &LibraryOverviewWidget::editPackageTriggered, this,
          &LibraryEditor::editPackageTriggered);
  connect(overviewWidget, &LibraryOverviewWidget::editComponentTriggered, this,
          &LibraryEditor::editComponentTriggered);
  connect(overviewWidget, &LibraryOverviewWidget::editDeviceTriggered, this,
          &LibraryEditor::editDeviceTriggered);
  connect(overviewWidget,
          &LibraryOverviewWidget::duplicateComponentCategoryTriggered, this,
          &LibraryEditor::duplicateComponentCategoryTriggered);
  connect(overviewWidget,
          &LibraryOverviewWidget::duplicatePackageCategoryTriggered, this,
          &LibraryEditor::duplicatePackageCategoryTriggered);
  connect(overviewWidget, &LibraryOverviewWidget::duplicateSymbolTriggered,
          this, &LibraryEditor::duplicateSymbolTriggered);
  connect(overviewWidget, &LibraryOverviewWidget::duplicatePackageTriggered,
          this, &LibraryEditor::duplicatePackageTriggered);
  connect(overviewWidget, &LibraryOverviewWidget::duplicateComponentTriggered,
          this, &LibraryEditor::duplicateComponentTriggered);
  connect(overviewWidget, &LibraryOverviewWidget::duplicateDeviceTriggered,
          this, &LibraryEditor::duplicateDeviceTriggered);
  connect(overviewWidget, &LibraryOverviewWidget::removeElementTriggered, this,
          &LibraryEditor::closeTabIfOpen);

  // remove close button on first tab (which is the library overview)
  QTabBar* tabBar = mUi->tabWidget->tabBar();
  Q_ASSERT(tabBar);
  tabBar->setTabButton(0, QTabBar::RightSide, nullptr);

  // set window title and icon
  const QStringList localeOrder =
      mWorkspace.getSettings().libraryLocaleOrder.get();
  QString libName = *mLibrary->getNames().value(localeOrder);
  if (readOnly) libName.append(tr(" [Read-Only]"));
  setWindowTitle(tr("%1 - LibrePCB Library Editor").arg(libName));
  setWindowIcon(mLibrary->getIconAsPixmap());

  // if the library was opened in read-only mode, we guess that it's a remote
  // library and thus show a warning that all modifications are lost after the
  // next update
  mUi->lblRemoteLibraryWarning->setVisible(readOnly);

  // setup status bar
  mUi->statusBar->setFields(StatusBar::ProgressBar);
  mUi->statusBar->setProgressBarTextFormat(tr("Scanning libraries (%p%)"));
  connect(&mWorkspace.getLibraryDb(), &WorkspaceLibraryDb::scanProgressUpdate,
          mUi->statusBar, &StatusBar::setProgressBarPercent,
          Qt::QueuedConnection);

  // add all required schematic layers
  addLayer(GraphicsLayer::sSchematicReferences);
  addLayer(GraphicsLayer::sSchematicSheetFrames);
  addLayer(GraphicsLayer::sSymbolOutlines);
  addLayer(GraphicsLayer::sSymbolGrabAreas);
  addLayer(GraphicsLayer::sSymbolHiddenGrabAreas, true);
  addLayer(GraphicsLayer::sSymbolPinCirclesOpt);
  addLayer(GraphicsLayer::sSymbolPinCirclesReq);
  addLayer(GraphicsLayer::sSymbolPinNames);
  addLayer(GraphicsLayer::sSymbolPinNumbers);
  addLayer(GraphicsLayer::sSymbolNames);
  addLayer(GraphicsLayer::sSymbolValues);
  addLayer(GraphicsLayer::sSchematicNetLines);
  addLayer(GraphicsLayer::sSchematicNetLabels);
  addLayer(GraphicsLayer::sSchematicNetLabelAnchors);
  addLayer(GraphicsLayer::sSchematicDocumentation);
  addLayer(GraphicsLayer::sSchematicComments);
  addLayer(GraphicsLayer::sSchematicGuide);

  // add all required board layers
  addLayer(GraphicsLayer::sBoardSheetFrames);
  addLayer(GraphicsLayer::sBoardOutlines);
  addLayer(GraphicsLayer::sBoardMillingPth);
  addLayer(GraphicsLayer::sBoardDrillsNpth);
  addLayer(GraphicsLayer::sBoardViasTht);
  addLayer(GraphicsLayer::sBoardPadsTht);
  addLayer(GraphicsLayer::sBoardAirWires);
  addLayer(GraphicsLayer::sBoardMeasures);
  addLayer(GraphicsLayer::sBoardAlignment);
  addLayer(GraphicsLayer::sBoardDocumentation);
  addLayer(GraphicsLayer::sBoardComments);
  addLayer(GraphicsLayer::sBoardGuide);
  addLayer(GraphicsLayer::sTopCopper);
  for (int i = 1; i <= GraphicsLayer::getInnerLayerCount(); ++i) {
    addLayer(GraphicsLayer::getInnerLayerName(i));
  }
  addLayer(GraphicsLayer::sBotCopper);
  addLayer(GraphicsLayer::sTopReferences);
  addLayer(GraphicsLayer::sBotReferences);
  addLayer(GraphicsLayer::sTopGrabAreas);
  addLayer(GraphicsLayer::sBotGrabAreas);
  addLayer(GraphicsLayer::sTopHiddenGrabAreas, true);
  addLayer(GraphicsLayer::sBotHiddenGrabAreas, true);
  addLayer(GraphicsLayer::sTopPlacement);
  addLayer(GraphicsLayer::sBotPlacement);
  addLayer(GraphicsLayer::sTopDocumentation);
  addLayer(GraphicsLayer::sBotDocumentation);
  addLayer(GraphicsLayer::sTopNames);
  addLayer(GraphicsLayer::sBotNames);
  addLayer(GraphicsLayer::sTopValues);
  addLayer(GraphicsLayer::sBotValues);
  addLayer(GraphicsLayer::sTopCourtyard, true);
  addLayer(GraphicsLayer::sBotCourtyard, true);
  addLayer(GraphicsLayer::sTopStopMask, true);
  addLayer(GraphicsLayer::sBotStopMask, true);
  addLayer(GraphicsLayer::sTopSolderPaste, true);
  addLayer(GraphicsLayer::sBotSolderPaste, true);
  addLayer(GraphicsLayer::sTopGlue, true);
  addLayer(GraphicsLayer::sBotGlue, true);

  // add debug layers
#ifdef QT_DEBUG
  addLayer(GraphicsLayer::sDebugGraphicsItemsBoundingRects);
  addLayer(GraphicsLayer::sDebugGraphicsItemsTextsBoundingRects);
  addLayer(GraphicsLayer::sDebugSymbolPinNetSignalNames);
  addLayer(GraphicsLayer::sDebugNetLinesNetSignalNames);
  addLayer(GraphicsLayer::sDebugInvisibleNetPoints);
  addLayer(GraphicsLayer::sDebugComponentSymbolsCounts);
#endif

  // Create all actions, window menus, toolbars and dock widgets.
  createActions();
  createToolBars();
  createMenus();

  // Open the overview tab.
  setActiveEditorWidget(overviewWidget);
  connect(mUi->tabWidget, &QTabWidget::currentChanged, this,
          &LibraryEditor::currentTabChanged);
  connect(mUi->tabWidget, &QTabWidget::tabCloseRequested, this,
          &LibraryEditor::tabCloseRequested);

  // Restore window geometry.
  QSettings clientSettings;
  restoreGeometry(
      clientSettings.value("library_editor/window_geometry").toByteArray());
  restoreState(
      clientSettings.value("library_editor/window_state").toByteArray());
}

LibraryEditor::~LibraryEditor() noexcept {
  setActiveEditorWidget(nullptr);
  mLibrary = nullptr;
  for (int i = mUi->tabWidget->count() - 1; i >= 0; --i) {
    QWidget* widget = mUi->tabWidget->widget(i);
    mUi->tabWidget->removeTab(i);
    delete widget;
  }
  qDeleteAll(mLayers);
  mLayers.clear();
}

/*******************************************************************************
 *  Public Methods
 ******************************************************************************/

bool LibraryEditor::closeAndDestroy(bool askForSave) noexcept {
  // close tabs
  for (int i = mUi->tabWidget->count() - 1; i >= 0; --i) {
    if (askForSave) {
      if (!closeTab(i)) {
        return false;
      }
    } else {
      QWidget* widget = mUi->tabWidget->widget(i);
      mUi->tabWidget->removeTab(i);
      delete widget;
    }
  }

  // Save Window Geometry
  QSettings clientSettings;
  clientSettings.setValue("library_editor/window_geometry", saveGeometry());
  clientSettings.setValue("library_editor/window_state", saveState());

  deleteLater();
  return true;
}

/*******************************************************************************
 *  Public Slots
 ******************************************************************************/

void LibraryEditor::closeTabIfOpen(const FilePath& fp) noexcept {
  for (int i = 0; i < mUi->tabWidget->count(); i++) {
    EditorWidgetBase* widget =
        dynamic_cast<EditorWidgetBase*>(mUi->tabWidget->widget(i));
    if (widget && (widget->getFilePath() == fp)) {
      QWidget* widget = mUi->tabWidget->widget(i);
      mUi->tabWidget->removeTab(i);
      delete widget;
      return;
    }
  }
}

/*******************************************************************************
 *  GUI Event Handlers
 ******************************************************************************/

void LibraryEditor::newComponentCategoryTriggered() noexcept {
  newLibraryElement(NewElementWizardContext::ElementType::ComponentCategory);
}

void LibraryEditor::newPackageCategoryTriggered() noexcept {
  newLibraryElement(NewElementWizardContext::ElementType::PackageCategory);
}

void LibraryEditor::newSymbolTriggered() noexcept {
  newLibraryElement(NewElementWizardContext::ElementType::Symbol);
}

void LibraryEditor::newPackageTriggered() noexcept {
  newLibraryElement(NewElementWizardContext::ElementType::Package);
}

void LibraryEditor::newComponentTriggered() noexcept {
  newLibraryElement(NewElementWizardContext::ElementType::Component);
}

void LibraryEditor::newDeviceTriggered() noexcept {
  newLibraryElement(NewElementWizardContext::ElementType::Device);
}

void LibraryEditor::editComponentCategoryTriggered(
    const FilePath& fp) noexcept {
  editLibraryElementTriggered<ComponentCategoryEditorWidget>(fp, false);
}

void LibraryEditor::editPackageCategoryTriggered(const FilePath& fp) noexcept {
  editLibraryElementTriggered<PackageCategoryEditorWidget>(fp, false);
}

void LibraryEditor::editSymbolTriggered(const FilePath& fp) noexcept {
  editLibraryElementTriggered<SymbolEditorWidget>(fp, false);
}

void LibraryEditor::editPackageTriggered(const FilePath& fp) noexcept {
  editLibraryElementTriggered<PackageEditorWidget>(fp, false);
}

void LibraryEditor::editComponentTriggered(const FilePath& fp) noexcept {
  editLibraryElementTriggered<ComponentEditorWidget>(fp, false);
}

void LibraryEditor::editDeviceTriggered(const FilePath& fp) noexcept {
  editLibraryElementTriggered<DeviceEditorWidget>(fp, false);
}

void LibraryEditor::duplicateComponentCategoryTriggered(
    const FilePath& fp) noexcept {
  duplicateLibraryElement(
      NewElementWizardContext::ElementType::ComponentCategory, fp);
}

void LibraryEditor::duplicatePackageCategoryTriggered(
    const FilePath& fp) noexcept {
  duplicateLibraryElement(NewElementWizardContext::ElementType::PackageCategory,
                          fp);
}

void LibraryEditor::duplicateSymbolTriggered(const FilePath& fp) noexcept {
  duplicateLibraryElement(NewElementWizardContext::ElementType::Symbol, fp);
}

void LibraryEditor::duplicatePackageTriggered(const FilePath& fp) noexcept {
  duplicateLibraryElement(NewElementWizardContext::ElementType::Package, fp);
}

void LibraryEditor::duplicateComponentTriggered(const FilePath& fp) noexcept {
  duplicateLibraryElement(NewElementWizardContext::ElementType::Component, fp);
}

void LibraryEditor::duplicateDeviceTriggered(const FilePath& fp) noexcept {
  duplicateLibraryElement(NewElementWizardContext::ElementType::Device, fp);
}

template <typename EditWidgetType>
void LibraryEditor::editLibraryElementTriggered(const FilePath& fp,
                                                bool isNewElement) noexcept {
  try {
    for (int i = 0; i < mUi->tabWidget->count(); i++) {
      EditorWidgetBase* widget =
          dynamic_cast<EditorWidgetBase*>(mUi->tabWidget->widget(i));
      if (widget && (widget->getFilePath() == fp)) {
        mUi->tabWidget->setCurrentIndex(i);
        return;
      }
    }

    EditorWidgetBase::Context context{mWorkspace, *this, isNewElement,
                                      mIsOpenedReadOnly};
    EditWidgetType* widget = new EditWidgetType(context, fp);
    connect(widget, &QWidget::windowTitleChanged, this,
            &LibraryEditor::updateTabTitles);
    connect(widget, &EditorWidgetBase::dirtyChanged, this,
            &LibraryEditor::updateTabTitles);
    connect(widget, &EditorWidgetBase::elementEdited,
            &mWorkspace.getLibraryDb(),
            &WorkspaceLibraryDb::startLibraryRescan);
    int index = mUi->tabWidget->addTab(widget, widget->windowIcon(),
                                       widget->windowTitle());
    mUi->tabWidget->setCurrentIndex(index);
  } catch (const UserCanceled& e) {
    // User requested to abort -> do nothing.
  } catch (const Exception& e) {
    QMessageBox::critical(this, tr("Failed to open category"), e.getMsg());
  }
}

void LibraryEditor::currentTabChanged(int index) noexcept {
  setActiveEditorWidget(
      dynamic_cast<EditorWidgetBase*>(mUi->tabWidget->widget(index)));
}

void LibraryEditor::tabCloseRequested(int index) noexcept {
  // Don't allow closing the overview widget
  LibraryOverviewWidget* widget =
      dynamic_cast<LibraryOverviewWidget*>(mUi->tabWidget->widget(index));
  if (widget != nullptr) {
    return;
  }

  closeTab(index);
}

bool LibraryEditor::closeTab(int index) noexcept {
  // Get editor widget reference
  EditorWidgetBase* widget =
      dynamic_cast<EditorWidgetBase*>(mUi->tabWidget->widget(index));
  if (widget == nullptr) {
    qCritical()
        << "Cannot close tab, widget is not an EditorWidgetBase subclass";
    return false;
  }

  // Move focus out of the editor widget to enforce updating the "dirty" state
  // of the editor before closing it. This is needed to make sure the
  // "save changes?" message box appears if the user just edited some property
  // of the library element and the focus is still in the property editor
  // widget. See https://github.com/LibrePCB/LibrePCB/issues/492.
  if (QWidget* focus = focusWidget()) {
    focus->clearFocus();
  }

  // Handle closing
  if (widget == mCurrentEditorWidget) {
    setActiveEditorWidget(nullptr);
  }
  if (widget->isDirty()) {
    QString msg =
        tr("You have unsaved changes in the library element.\n"
           "Do you want to save them before closing it?");
    QMessageBox::StandardButton choice = QMessageBox::question(
        this, tr("Unsaved changes"), msg,
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
        QMessageBox::Yes);
    switch (choice) {
      case QMessageBox::Yes:
        if (widget->save()) {
          delete widget;
        } else {
          return false;
        }
        break;
      case QMessageBox::No:
        delete widget;
        break;
      default:
        return false;
    }
  } else {
    delete widget;
  }
  return true;
}

/*******************************************************************************
 *  Private Methods
 ******************************************************************************/

void LibraryEditor::createActions() noexcept {
  const EditorCommandSet& cmd = EditorCommandSet::instance();

  mActionAbort.reset(cmd.abort.createAction(this, this, [this]() {
    if (mCurrentEditorWidget) mCurrentEditorWidget->abortCommand();
  }));
  mActionCopy.reset(cmd.copy.createAction(this, this, [this]() {
    if (mCurrentEditorWidget) mCurrentEditorWidget->copy();
  }));
  mActionCut.reset(cmd.cut.createAction(this, this, [this]() {
    if (mCurrentEditorWidget) mCurrentEditorWidget->cut();
  }));
  mActionExportImage.reset(cmd.exportImage.createAction(this, this, [this]() {
    if (mCurrentEditorWidget) mCurrentEditorWidget->exportImage();
  }));
  mActionExportPdf.reset(cmd.exportPdf.createAction(this, this, [this]() {
    if (mCurrentEditorWidget) mCurrentEditorWidget->exportPdf();
  }));
  mActionGridProperties.reset(
      cmd.gridProperties.createAction(this, this, [this]() {
        if (mCurrentEditorWidget) mCurrentEditorWidget->editGridProperties();
      }));
  mActionPaste.reset(cmd.paste.createAction(this, this, [this]() {
    if (mCurrentEditorWidget) mCurrentEditorWidget->paste();
  }));
  mActionPrint.reset(cmd.print.createAction(this, this, [this]() {
    if (mCurrentEditorWidget) mCurrentEditorWidget->print();
  }));
  mActionQuit.reset(cmd.quit.createAction(this, this, &LibraryEditor::close));
  mActionRedo.reset(cmd.redo.createAction(this));
  mActionRemove.reset(cmd.remove.createAction(this, this, [this]() {
    if (mCurrentEditorWidget) mCurrentEditorWidget->remove();
  }));
  mActionRemove->setEnabled(!mIsOpenedReadOnly);
  mActionRotateCcw.reset(cmd.rotateCcw.createAction(this, this, [this]() {
    if (mCurrentEditorWidget) mCurrentEditorWidget->rotateCcw();
  }));
  mActionRotateCw.reset(cmd.rotateCw.createAction(this, this, [this]() {
    if (mCurrentEditorWidget) mCurrentEditorWidget->rotateCw();
  }));
  mActionSave.reset(cmd.save.createAction(this, this, [this]() {
    if (mCurrentEditorWidget) mCurrentEditorWidget->save();
  }));
  mActionSave->setEnabled(!mIsOpenedReadOnly);
  mActionSelectAll.reset(cmd.selectAll.createAction(this, this, [this]() {
    if (mCurrentEditorWidget) mCurrentEditorWidget->selectAll();
  }));
  mActionToolSelect.reset(cmd.toolSelect.createAction(this));
  mActionToolPin.reset(cmd.toolPin.createAction(this));
  mActionToolThtPad.reset(cmd.toolThtPad.createAction(this));
  mActionToolSmtPad.reset(cmd.toolSmtPad.createAction(this));
  mActionToolLine.reset(cmd.toolLine.createAction(this));
  mActionToolRect.reset(cmd.toolRect.createAction(this));
  mActionToolPolygon.reset(cmd.toolPolygon.createAction(this));
  mActionToolCircle.reset(cmd.toolCircle.createAction(this));
  mActionToolText.reset(cmd.toolText.createAction(this));
  mActionToolHole.reset(cmd.toolHole.createAction(this));
  mActionToolName.reset(cmd.toolName.createAction(this));
  mActionToolValue.reset(cmd.toolValue.createAction(this));
  mActionUndo.reset(cmd.undo.createAction(this));
  mActionRescanLibraries.reset(cmd.rescanLibraries.createAction(
      this, &mWorkspace.getLibraryDb(),
      &WorkspaceLibraryDb::startLibraryRescan));
  mActionZoomFit.reset(cmd.zoomFit.createAction(this, this, [this]() {
    if (mCurrentEditorWidget) mCurrentEditorWidget->zoomAll();
  }));
  mActionZoomIn.reset(cmd.zoomIn.createAction(this, this, [this]() {
    if (mCurrentEditorWidget) mCurrentEditorWidget->zoomIn();
  }));
  mActionZoomOut.reset(cmd.zoomOut.createAction(this, this, [this]() {
    if (mCurrentEditorWidget) mCurrentEditorWidget->zoomOut();
  }));
  mActionOnlineDocumentation.reset(cmd.onlineDocumentation.createAction(
      this, qApp,
      []() { QDesktopServices::openUrl(QUrl("https://docs.librepcb.org")); }));
  mActionWebsite.reset(cmd.website.createAction(this, qApp, []() {
    QDesktopServices::openUrl(QUrl("https://librepcb.org"));
  }));
  mActionAboutLibrePcb.reset(
      cmd.aboutLibrePcb.createAction(this, this, [this]() {
        AboutDialog aboutDialog(this);
        aboutDialog.exec();
      }));
  mActionAboutQt.reset(
      cmd.aboutQt.createAction(this, qApp, &QApplication::aboutQt));
  mActionNewElement.reset(
      cmd.newLibraryElement.createAction(this, this, [this]() {
        NewElementWizard wizard(mWorkspace, *mLibrary, *this, this);
        if (wizard.exec() == QDialog::Accepted) {
          FilePath fp = wizard.getContext().getOutputDirectory();
          editNewLibraryElement(wizard.getContext().mElementType, fp);
          mWorkspace.getLibraryDb().startLibraryRescan();
        }
      }));
  mActionNewElement->setEnabled(!mIsOpenedReadOnly);
  mActionShowInFileManager.reset(
      cmd.showInFileManager.createAction(this, this, [this]() {
        if (mCurrentEditorWidget) {
          FilePath fp = mCurrentEditorWidget->getFilePath();
          QDesktopServices::openUrl(fp.toQUrl());
        }
      }));
  mActionMirrorHorizontal.reset(
      cmd.mirrorHorizontal.createAction(this, this, [this]() {
        if (mCurrentEditorWidget) mCurrentEditorWidget->mirror();
      }));
  mActionFlipHorizontal.reset(
      cmd.flipHorizontal.createAction(this, this, [this]() {
        if (mCurrentEditorWidget) mCurrentEditorWidget->flip();
      }));
  mActionImportDxf.reset(cmd.importDxf.createAction(this, this, [this]() {
    if (mCurrentEditorWidget) mCurrentEditorWidget->importDxf();
  }));
  mActionImportEagleLibrary.reset(
      cmd.importEagleLibrary.createAction(this, this, [this]() {
        EagleLibraryImportWizard wizard(
            mWorkspace, mLibrary->getDirectory().getAbsPath(), this);
        wizard.exec();
      }));

  // Undo stack action group.
  mUndoStackActionGroup.reset(new UndoStackActionGroup(
      *mActionUndo, *mActionRedo, nullptr, nullptr, this));

  // Tools action group.
  mToolsActionGroup.reset(new ExclusiveActionGroup());
  mToolsActionGroup->addAction(EditorWidgetBase::Tool::SELECT,
                               mActionToolSelect.data());
  mToolsActionGroup->addAction(EditorWidgetBase::Tool::DRAW_LINE,
                               mActionToolLine.data());
  mToolsActionGroup->addAction(EditorWidgetBase::Tool::DRAW_RECT,
                               mActionToolRect.data());
  mToolsActionGroup->addAction(EditorWidgetBase::Tool::DRAW_POLYGON,
                               mActionToolPolygon.data());
  mToolsActionGroup->addAction(EditorWidgetBase::Tool::DRAW_CIRCLE,
                               mActionToolCircle.data());
  mToolsActionGroup->addAction(EditorWidgetBase::Tool::ADD_NAMES,
                               mActionToolName.data());
  mToolsActionGroup->addAction(EditorWidgetBase::Tool::ADD_VALUES,
                               mActionToolValue.data());
  mToolsActionGroup->addAction(EditorWidgetBase::Tool::DRAW_TEXT,
                               mActionToolText.data());
  mToolsActionGroup->addAction(EditorWidgetBase::Tool::ADD_PINS,
                               mActionToolPin.data());
  mToolsActionGroup->addAction(EditorWidgetBase::Tool::ADD_THT_PADS,
                               mActionToolThtPad.data());
  mToolsActionGroup->addAction(EditorWidgetBase::Tool::ADD_SMT_PADS,
                               mActionToolSmtPad.data());
  mToolsActionGroup->addAction(EditorWidgetBase::Tool::ADD_HOLES,
                               mActionToolHole.data());
  mToolsActionGroup->setEnabled(false);
}

void LibraryEditor::createToolBars() noexcept {
  // File.
  mToolBarFile.reset(new QToolBar(tr("File"), this));
  mToolBarFile->setObjectName("toolBarFile");
  mToolBarFile->addAction(mActionNewElement.data());
  mToolBarFile->addAction(mActionSave.data());
  mToolBarFile->addAction(mActionPrint.data());
  mToolBarFile->addAction(mActionExportPdf.data());
  mToolBarFile->addSeparator();
  mToolBarFile->addAction(mActionUndo.data());
  mToolBarFile->addAction(mActionRedo.data());
  addToolBar(Qt::TopToolBarArea, mToolBarFile.data());

  // Edit.
  mToolBarEdit.reset(new QToolBar(tr("Edit"), this));
  mToolBarEdit->setObjectName("toolBarEdit");
  mToolBarEdit->addAction(mActionCut.data());
  mToolBarEdit->addAction(mActionCopy.data());
  mToolBarEdit->addAction(mActionPaste.data());
  mToolBarEdit->addAction(mActionRemove.data());
  mToolBarEdit->addAction(mActionRotateCcw.data());
  mToolBarEdit->addAction(mActionRotateCw.data());
  mToolBarEdit->addAction(mActionMirrorHorizontal.data());
  mToolBarEdit->addAction(mActionFlipHorizontal.data());
  addToolBar(Qt::TopToolBarArea, mToolBarEdit.data());

  // View.
  mToolBarView.reset(new QToolBar(tr("View"), this));
  mToolBarView->setObjectName("toolBarView");
  mToolBarView->addAction(mActionGridProperties.data());
  mToolBarView->addAction(mActionZoomIn.data());
  mToolBarView->addAction(mActionZoomOut.data());
  mToolBarView->addAction(mActionZoomFit.data());
  addToolBar(Qt::TopToolBarArea, mToolBarView.data());

  // Search.
  mToolBarSearch.reset(new SearchToolBar(this));
  mToolBarSearch->setObjectName("toolBarSearch");
  mToolBarSearch->setPlaceholderText(tr("Filter elements..."));
  addToolBar(Qt::TopToolBarArea, mToolBarSearch.data());
  LibraryOverviewWidget* overviewWidget =
      dynamic_cast<LibraryOverviewWidget*>(mUi->tabWidget->widget(0));
  Q_ASSERT(overviewWidget);
  connect(mToolBarSearch.data(), &SearchToolBar::textEdited, overviewWidget,
          &LibraryOverviewWidget::setFilter);

  // Command.
  mToolBarCommand.reset(new QToolBar(tr("Command"), this));
  mToolBarCommand->setObjectName("toolBarCommand");
  mToolBarCommand->addAction(mActionAbort.data());
  mToolBarCommand->addSeparator();
  addToolBarBreak(Qt::TopToolBarArea);
  addToolBar(Qt::TopToolBarArea, mToolBarCommand.data());

  // Tools.
  mToolBarTools.reset(new QToolBar(tr("Tools"), this));
  mToolBarTools->setObjectName("toolBarTools");
  mToolBarTools->addAction(mActionToolSelect.data());
  mToolBarTools->addAction(mActionToolLine.data());
  mToolBarTools->addAction(mActionToolRect.data());
  mToolBarTools->addAction(mActionToolPolygon.data());
  mToolBarTools->addAction(mActionToolCircle.data());
  mToolBarTools->addAction(mActionToolName.data());
  mToolBarTools->addAction(mActionToolValue.data());
  mToolBarTools->addAction(mActionToolText.data());
  mToolBarTools->addSeparator();
  mToolBarTools->addAction(mActionToolPin.data());
  mToolBarTools->addSeparator();
  mToolBarTools->addAction(mActionToolThtPad.data());
  mToolBarTools->addAction(mActionToolSmtPad.data());
  mToolBarTools->addAction(mActionToolHole.data());
  addToolBar(Qt::LeftToolBarArea, mToolBarTools.data());
}

void LibraryEditor::createMenus() noexcept {
  MenuBuilder mb(mUi->menuBar);

  // File.
  mb.newMenu(&MenuBuilder::createFileMenu);
  mb.addAction(mActionNewElement);
  mb.addAction(mActionSave);
  mb.addAction(mActionShowInFileManager);
  mb.addAction(mActionRescanLibraries);
  mb.addSeparator();
  {
    MenuBuilder smb(mb.addSubMenu(&MenuBuilder::createImportMenu));
    smb.addAction(mActionImportDxf);
    smb.addAction(mActionImportEagleLibrary);
  }
  {
    MenuBuilder smb(mb.addSubMenu(&MenuBuilder::createExportMenu));
    smb.addAction(mActionExportPdf);
    smb.addAction(mActionExportImage);
  }
  mb.addSeparator();
  mb.addAction(mActionPrint);
  mb.addSeparator();
  mb.addAction(mActionQuit);

  // Edit.
  mb.newMenu(&MenuBuilder::createEditMenu);
  mb.addAction(mActionUndo);
  mb.addAction(mActionRedo);
  mb.addSeparator();
  mb.addAction(mActionSelectAll);
  mb.addSeparator();
  mb.addAction(mActionRotateCcw);
  mb.addAction(mActionRotateCw);
  mb.addAction(mActionMirrorHorizontal);
  mb.addAction(mActionFlipHorizontal);
  mb.addAction(mActionCopy);
  mb.addAction(mActionCut);
  mb.addAction(mActionPaste);
  mb.addAction(mActionRemove);

  // View.
  mb.newMenu(&MenuBuilder::createViewMenu);
  mb.addAction(mActionGridProperties);
  mb.addSeparator();
  mb.addAction(mActionZoomIn);
  mb.addAction(mActionZoomOut);
  mb.addAction(mActionZoomFit);

  // Tools.
  mb.newMenu(&MenuBuilder::createToolsMenu);
  mb.addAction(mActionToolSelect);
  mb.addAction(mActionToolLine);
  mb.addAction(mActionToolRect);
  mb.addAction(mActionToolPolygon);
  mb.addAction(mActionToolCircle);
  mb.addAction(mActionToolName);
  mb.addAction(mActionToolValue);
  mb.addAction(mActionToolText);
  mb.addSeparator();
  mb.addAction(mActionToolPin);
  mb.addSeparator();
  mb.addAction(mActionToolThtPad);
  mb.addAction(mActionToolSmtPad);
  mb.addAction(mActionToolHole);

  // Help.
  mb.newMenu(&MenuBuilder::createHelpMenu);
  mb.addAction(mActionOnlineDocumentation);
  mb.addAction(mActionWebsite);
  mb.addAction(mActionAboutLibrePcb);
  mb.addAction(mActionAboutQt);
}

void LibraryEditor::setActiveEditorWidget(EditorWidgetBase* widget) {
  bool hasGraphicalEditor = false;
  bool supportsFlip = false;
  bool isOverviewTab = dynamic_cast<LibraryOverviewWidget*>(widget) != nullptr;
  if (mCurrentEditorWidget) {
    mCurrentEditorWidget->setUndoStackActionGroup(nullptr);
    mCurrentEditorWidget->setToolsActionGroup(nullptr);
    mCurrentEditorWidget->setCommandToolBar(nullptr);
    mCurrentEditorWidget->setStatusBar(nullptr);
  }
  mCurrentEditorWidget = widget;
  if (mCurrentEditorWidget) {
    mCurrentEditorWidget->setUndoStackActionGroup(mUndoStackActionGroup.data());
    mCurrentEditorWidget->setToolsActionGroup(mToolsActionGroup.data());
    mCurrentEditorWidget->setCommandToolBar(mToolBarCommand.data());
    mCurrentEditorWidget->setStatusBar(mUi->statusBar);
    hasGraphicalEditor = mCurrentEditorWidget->hasGraphicalEditor();
    supportsFlip = mCurrentEditorWidget->supportsFlip();
  }
  foreach (QAction* action, mToolBarEdit->actions()) {
    bool enabled = hasGraphicalEditor;
    if (action != mActionCopy.data()) {
      enabled = enabled && (!mIsOpenedReadOnly);
    }
    action->setEnabled(enabled);
  }
  mActionSelectAll->setEnabled(hasGraphicalEditor);
  mActionFlipHorizontal->setEnabled(supportsFlip && (!mIsOpenedReadOnly));
  if (isOverviewTab) {
    mActionRemove->setEnabled(!mIsOpenedReadOnly);
  }
  mActionImportDxf->setEnabled((!mIsOpenedReadOnly) && hasGraphicalEditor);
  mActionImportEagleLibrary->setEnabled(!mIsOpenedReadOnly);
  mActionImportEagleLibrary->setEnabled(!mIsOpenedReadOnly);
  mActionExportImage->setEnabled(hasGraphicalEditor);
  mActionExportPdf->setEnabled(hasGraphicalEditor);
  mActionPrint->setEnabled(hasGraphicalEditor);
  foreach (QAction* action, mToolBarView->actions()) {
    action->setEnabled(hasGraphicalEditor);
  }
  mToolBarCommand->setEnabled(hasGraphicalEditor && (!mIsOpenedReadOnly));
  mToolBarSearch->setEnabled(isOverviewTab);
  updateTabTitles();  // force updating the "Save" action title
}

void LibraryEditor::newLibraryElement(
    NewElementWizardContext::ElementType type) {
  NewElementWizard wizard(mWorkspace, *mLibrary, *this, this);
  wizard.setNewElementType(type);
  if (wizard.exec() == QDialog::Accepted) {
    FilePath fp = wizard.getContext().getOutputDirectory();
    editNewLibraryElement(wizard.getContext().mElementType, fp);
    mWorkspace.getLibraryDb().startLibraryRescan();
  }
}

void LibraryEditor::duplicateLibraryElement(
    NewElementWizardContext::ElementType type, const FilePath& fp) {
  NewElementWizard wizard(mWorkspace, *mLibrary, *this, this);
  wizard.setElementToCopy(type, fp);
  if (wizard.exec() == QDialog::Accepted) {
    FilePath fp = wizard.getContext().getOutputDirectory();
    editNewLibraryElement(wizard.getContext().mElementType, fp);
    mWorkspace.getLibraryDb().startLibraryRescan();
  }
}

void LibraryEditor::editNewLibraryElement(
    NewElementWizardContext::ElementType type, const FilePath& fp) {
  switch (type) {
    case NewElementWizardContext::ElementType::ComponentCategory:
      editLibraryElementTriggered<ComponentCategoryEditorWidget>(fp, true);
      break;
    case NewElementWizardContext::ElementType::PackageCategory:
      editLibraryElementTriggered<PackageCategoryEditorWidget>(fp, true);
      break;
    case NewElementWizardContext::ElementType::Symbol:
      editLibraryElementTriggered<SymbolEditorWidget>(fp, true);
      break;
    case NewElementWizardContext::ElementType::Package:
      editLibraryElementTriggered<PackageEditorWidget>(fp, true);
      break;
    case NewElementWizardContext::ElementType::Component:
      editLibraryElementTriggered<ComponentEditorWidget>(fp, true);
      break;
    case NewElementWizardContext::ElementType::Device:
      editLibraryElementTriggered<DeviceEditorWidget>(fp, true);
      break;
    default:
      break;
  }
}

void LibraryEditor::updateTabTitles() noexcept {
  for (int i = 0; i < mUi->tabWidget->count(); ++i) {
    QWidget* widget = mUi->tabWidget->widget(i);
    const EditorWidgetBase* editorWidget =
        dynamic_cast<EditorWidgetBase*>(widget);
    if (editorWidget) {
      if (editorWidget->isDirty()) {
        mUi->tabWidget->setTabText(i, '*' % editorWidget->windowTitle());
      } else {
        mUi->tabWidget->setTabText(i, editorWidget->windowTitle());
      }
    } else {
      qWarning() << "Tab widget is not a subclass of EditorWidgetBase!";
    }
  }

  if (mCurrentEditorWidget && (!mIsOpenedReadOnly)) {
    mActionSave->setEnabled(true);
    mActionSave->setText(mActionSave->text() % "'" %
                         mCurrentEditorWidget->windowTitle() % "'");
  } else {
    mActionSave->setEnabled(false);
  }
}

void LibraryEditor::closeEvent(QCloseEvent* event) noexcept {
  if (closeAndDestroy(true)) {
    QMainWindow::closeEvent(event);
  } else {
    event->ignore();
  }
}

void LibraryEditor::addLayer(const QString& name, bool forceVisible) noexcept {
  QScopedPointer<GraphicsLayer> layer(new GraphicsLayer(name));
  if (forceVisible) layer->setVisible(true);
  mLayers.append(layer.take());
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace librepcb
