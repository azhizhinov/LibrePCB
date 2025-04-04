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

#ifndef LIBREPCB_CORE_UUID_H
#define LIBREPCB_CORE_UUID_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include <QtCore>

#include <optional>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

/*******************************************************************************
 *  Class Uuid
 ******************************************************************************/

/**
 * @brief The Uuid class is a replacement for QUuid to get UUID strings without
 * {} braces
 *
 * This class implements an RFC4122 compliant UUID of type "DCE" in Version 4
 * (random UUID). Other types and/or versions of UUIDs are considered as
 * invalid. The characters in a UUID are always lowercase.
 *
 * A valid UUID looks like this: "d79d354b-62bd-4866-996a-78941c575e78"
 *
 * @note This class guarantees that only Uuid objects representing a valid UUID
 * can be created (in opposite to QUuid which allows "Null UUIDs")! If you need
 * a nullable UUID, use std::optional<librepcb::Uuid> instead.
 *
 * @see https://de.wikipedia.org/wiki/Universally_Unique_Identifier
 * @see https://tools.ietf.org/html/rfc4122
 */
class Uuid final {
  Q_DECLARE_TR_FUNCTIONS(Uuid)

public:
  // Constructors / Destructor

  /**
   * @brief Default constructor (disabled to avoid creating invalid UUIDs)
   */
  Uuid() = delete;

  /**
   * @brief Copy constructor
   *
   * @param other     Another ::librepcb::Uuid object
   */
  Uuid(const Uuid& other) noexcept : mUuid(other.mUuid) {}

  /**
   * @brief Destructor
   */
  ~Uuid() noexcept
#if defined(__GNUC__) && (__GNUC__ == 13) && (__GNUC_MINOR__ == 2)
      ;  // Workaround for compiler warning "maybe-uninitialized".
#else
      = default;
#endif

  // Getters

  /**
   * @brief Get the UUID as a string (without braces)
   *
   * @return The UUID as a string
   */
  QString toStr() const noexcept { return mUuid; }

  //@{
  /**
   * @brief Operator overloadings
   *
   * @param rhs   The other object to compare
   *
   * @return Result of comparing the UUIDs as strings
   */
  Uuid& operator=(const Uuid& rhs) noexcept {
    mUuid = rhs.mUuid;
    return *this;
  }
  bool operator==(const Uuid& rhs) const noexcept { return mUuid == rhs.mUuid; }
  bool operator!=(const Uuid& rhs) const noexcept { return mUuid != rhs.mUuid; }
  bool operator<(const Uuid& rhs) const noexcept { return mUuid < rhs.mUuid; }
  bool operator>(const Uuid& rhs) const noexcept { return mUuid > rhs.mUuid; }
  bool operator<=(const Uuid& rhs) const noexcept { return mUuid <= rhs.mUuid; }
  bool operator>=(const Uuid& rhs) const noexcept { return mUuid >= rhs.mUuid; }
  //@}

  // Static Methods

  /**
   * @brief Check if a string is a valid UUID
   *
   * @param str       The string to check
   *
   * @retval true     If str is a valid UUID
   * @retval false    If str is not a valid UUID
   */
  static bool isValid(const QString& str) noexcept;

  /**
   * @brief Create a new random UUID
   *
   * @return The new UUID
   */
  static Uuid createRandom() noexcept;

  /**
   * @brief Create Uuid from a string
   *
   * @param str           Input string
   *
   * @return The created Uuid object
   *
   * @throws Exception if the string does not contain a valid UUID
   */
  static Uuid fromString(const QString& str);

  /**
   * @brief Try creating a Uuid from a string, returning empty optional if
   * invalid
   *
   * @param str           Input string
   *
   * @retval Uuid         The created Uuid object if str was valid
   * @retval std::nullopt  If str was not a valid UUID
   */
  static std::optional<Uuid> tryFromString(const QString& str) noexcept;

private:  // Methods
  /**
   * @brief Constructor which creates a Uuid object from a string
   *
   * @param str       The uuid as a string (lowercase and without braces)
   */
  explicit Uuid(const QString& str) noexcept : mUuid(str) {}

private:  // Data
  QString mUuid;  ///< Guaranteed to always contain a valid UUID
};

/*******************************************************************************
 *  Non-Member Functions
 ******************************************************************************/

inline QDataStream& operator<<(QDataStream& stream, const Uuid& uuid) noexcept {
  stream << uuid.toStr();
  return stream;
}

inline QDebug operator<<(QDebug stream, const Uuid& uuid) noexcept {
  stream << QString("Uuid(%1)").arg(uuid.toStr());
  return stream;
}

inline std::size_t qHash(const Uuid& key, std::size_t seed = 0) noexcept {
  return ::qHash(key.toStr(), seed);
}

}  // namespace librepcb

namespace std {
inline size_t qHash(const optional<librepcb::Uuid>& key,
                    size_t seed = 0) noexcept {
  return ::qHash(key ? key->toStr() : QString(), seed);
}
}  // namespace std

/*******************************************************************************
 *  End of File
 ******************************************************************************/

#endif
