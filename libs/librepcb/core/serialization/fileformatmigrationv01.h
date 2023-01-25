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

#ifndef LIBREPCB_CORE_FILEFORMATMIGRATIONV01_H
#define LIBREPCB_CORE_FILEFORMATMIGRATIONV01_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "../graphics/graphicslayername.h"
#include "../types/alignment.h"
#include "../types/angle.h"
#include "../types/point.h"
#include "../types/uuid.h"
#include "fileformatmigration.h"

#include <QtCore>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

class SExpression;

/*******************************************************************************
 *  Class FileFormatMigrationV01
 ******************************************************************************/

/**
 * @brief Migration to upgrade file format v0.1
 */
class FileFormatMigrationV01 final : public FileFormatMigration {
  Q_OBJECT

  struct Text {
    Uuid uuid;
    GraphicsLayerName layerName;
    QString text;
    Point position;
    Angle rotation;
    PositiveLength height;
    Alignment align;
  };

  struct Symbol {
    QList<Text> texts;
  };

  struct Gate {
    Uuid uuid;
    Uuid symbolUuid;
  };

  struct ComponentSymbolVariant {
    Uuid uuid;
    QList<Gate> gates;
  };

  struct Component {
    QList<ComponentSymbolVariant> symbolVariants;
    ;
  };

  struct ComponentInstance {
    Uuid libCmpUuid;
    Uuid libSymbVarUuid;
  };

  struct LoadedData {
    QHash<Uuid, Symbol> symbols;
    QHash<Uuid, Component> components;
    QMap<Uuid, ComponentInstance> componentInstances;
  };

public:
  // Constructors / Destructor
  explicit FileFormatMigrationV01(QObject* parent = nullptr) noexcept;
  FileFormatMigrationV01(const FileFormatMigrationV01& other) = delete;
  ~FileFormatMigrationV01() noexcept;

  // General Methods
  virtual void upgradeComponentCategory(TransactionalDirectory& dir) override;
  virtual void upgradePackageCategory(TransactionalDirectory& dir) override;
  virtual void upgradeSymbol(TransactionalDirectory& dir) override;
  virtual void upgradePackage(TransactionalDirectory& dir) override;
  virtual void upgradeComponent(TransactionalDirectory& dir) override;
  virtual void upgradeDevice(TransactionalDirectory& dir) override;
  virtual void upgradeLibrary(TransactionalDirectory& dir) override;
  virtual void upgradeProject(TransactionalDirectory& dir) override;
  virtual void upgradeWorkspaceData(TransactionalDirectory& dir) override;

  // Operator Overloadings
  FileFormatMigrationV01& operator=(const FileFormatMigrationV01& rhs) = delete;

private:  // Methods
  void upgradeGrid(SExpression& node);
  void upgradeHoles(SExpression& node);
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb

#endif
