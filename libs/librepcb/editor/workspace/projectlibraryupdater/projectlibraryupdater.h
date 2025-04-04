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

#ifndef LIBREPCB_EDITOR_PROJECTLIBRARYUPDATER_H
#define LIBREPCB_EDITOR_PROJECTLIBRARYUPDATER_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include <librepcb/core/fileio/filepath.h>
#include <librepcb/core/types/uuid.h>

#include <QtCore>
#include <QtWidgets>

#include <memory>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

class TransactionalFileSystem;
class Workspace;
class WorkspaceLibraryDb;

namespace editor {

namespace Ui {
class ProjectLibraryUpdater;
}

/*******************************************************************************
 *  Class ProjectLibraryUpdater
 ******************************************************************************/

/**
 * @brief The ProjectLibraryUpdater class
 *
 * @note This updater is currently an ugly hack with very limited functionality.
 * The whole project library update concept needs to be refactored some time to
 * provide an updater with much more functionality and higher reliability.
 */
class ProjectLibraryUpdater : public QDialog {
  Q_OBJECT

public:
  typedef std::function<bool(const FilePath&)> CloseCallback;

  explicit ProjectLibraryUpdater(Workspace& ws, const FilePath& project,
                                 CloseCallback cb) noexcept;
  ~ProjectLibraryUpdater();

private slots:
  void btnUpdateClicked();

signals:
  void finished(const FilePath& fp);

private:
  void log(const QString& msg) noexcept;
  QString prettyPath(const FilePath& fp) const noexcept;
  template <typename T>
  void updateElements(std::shared_ptr<TransactionalFileSystem> fs,
                      const QString& type);

private:
  Workspace& mWorkspace;
  const FilePath mProjectFilePath;
  const CloseCallback mCloseCallback;
  QScopedPointer<Ui::ProjectLibraryUpdater> mUi;
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace librepcb

#endif
