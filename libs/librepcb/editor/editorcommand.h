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

#ifndef LIBREPCB_EDITOR_EDITORCOMMAND_H
#define LIBREPCB_EDITOR_EDITORCOMMAND_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include <QtCore>
#include <QtWidgets>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {
namespace editor {

/*******************************************************************************
 *  Class EditorCommand
 ******************************************************************************/

/**
 * @brief Command for editors, e.g. to be added to a QMenu
 */
class EditorCommand final : public QObject {
  Q_OBJECT

public:
  // Constructors / Destructor
  EditorCommand() = delete;
  EditorCommand(const EditorCommand& other) = delete;
  EditorCommand(const QString& identifier, const QString& text,
                const QString& description, const QIcon& icon,
                const QList<QKeySequence>& defaultKeySequences,
                QObject* parent = nullptr) noexcept;
  ~EditorCommand() noexcept;

  // Getters
  const QString& getIdentifier() const noexcept { return mIdentifier; }
  const QString& getText() const noexcept { return mText; }
  const QString& getDescription() const noexcept { return mDescription; }
  const QIcon& getIcon() const noexcept { return mIcon; }
  const QList<QKeySequence> getDefaultKeySequences() const noexcept {
    return mDefaultKeySequences;
  }
  const QList<QKeySequence>& getKeySequences() const noexcept {
    return mKeySequences;
  }

  // Setters
  void setKeySequences(const QList<QKeySequence>& sequences) noexcept;

  // General Methods
  QAction* createAction(QObject* parent) const noexcept;
  template <typename TContext, typename TSlot>
  QAction* createAction(QObject* parent, const TContext* context,
                        TSlot slot) const noexcept {
    QAction* action = createAction(parent);
    QObject::connect(action, &QAction::triggered, context, slot);
    return action;
  }

  // Operator Overloadings
  EditorCommand& operator=(const EditorCommand& rhs) = delete;

signals:
  void shortcutsChanged(const QList<QKeySequence>& sequences);

private:  // Methods
  QAction* setupAction(QAction* action) const noexcept;
  bool eventFilter(QObject* obj, QEvent* event) noexcept override;

private:  // Data
  QString mIdentifier;
  QString mText;
  QString mDescription;
  QIcon mIcon;
  QList<QKeySequence> mDefaultKeySequences;
  QList<QKeySequence> mKeySequences;
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace librepcb

#endif
