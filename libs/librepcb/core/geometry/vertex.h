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

#ifndef LIBREPCB_CORE_VERTEX_H
#define LIBREPCB_CORE_VERTEX_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "../types/angle.h"
#include "../types/point.h"

#include <QtCore>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

/*******************************************************************************
 *  Class Vertex
 ******************************************************************************/

/**
 * @brief The Vertex class
 */
class Vertex final {
public:
  // Constructors / Destructor
  Vertex() noexcept : mPos(), mAngle() {}
  Vertex(const Vertex& other) noexcept
    : mPos(other.mPos), mAngle(other.mAngle) {}
  explicit Vertex(const Point& pos, const Angle& angle = Angle::deg0()) noexcept
    : mPos(pos), mAngle(angle) {}
  explicit Vertex(const SExpression& node);
  ~Vertex() noexcept {}

  // Getters
  const Point& getPos() const noexcept { return mPos; }
  const Angle& getAngle() const noexcept { return mAngle; }

  // Setters
  void setPos(const Point& pos) noexcept { mPos = pos; }
  void setAngle(const Angle& angle) noexcept { mAngle = angle; }

  // General Methods

  /**
   * @brief Serialize into ::librepcb::SExpression node
   *
   * @param root    Root node to serialize into.
   */
  void serialize(SExpression& root) const;

  // Operator Overloadings
  Vertex& operator=(const Vertex& rhs) noexcept;
  bool operator==(const Vertex& rhs) const noexcept;
  bool operator!=(const Vertex& rhs) const noexcept { return !(*this == rhs); }

  /**
   * @brief The "<" operator to compare two ::librepcb::Vertex objects
   *
   * Useful for sorting vertex lists/sets (e.g. to for canonical order in
   * files), or to store them in a QMap.
   *
   * @param rhs The right hand side object.
   *
   * @return true if this vertex is smaller, else false
   */
  bool operator<(const Vertex& rhs) const noexcept;

private:  // Data
  Point mPos;
  Angle mAngle;  ///< angle of the line between this vertex and the following
                 ///< vertex
};

/*******************************************************************************
 *  Non-Member Functions
 ******************************************************************************/

QDebug operator<<(QDebug stream, const Vertex& vertex);

inline std::size_t qHash(const Vertex& key, std::size_t seed = 0) noexcept {
  return ::qHash(qMakePair(key.getPos(), key.getAngle()), seed);
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb

#endif
