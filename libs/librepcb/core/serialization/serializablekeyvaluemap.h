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

#ifndef LIBREPCB_CORE_SERIALIZABLEKEYVALUEMAP_H
#define LIBREPCB_CORE_SERIALIZABLEKEYVALUEMAP_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "../exceptions.h"
#include "../types/elementname.h"
#include "../utils/signalslot.h"
#include "sexpression.h"

#include <QtCore>

#include <optional>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

/*******************************************************************************
 *  Class SerializableKeyValueMap
 ******************************************************************************/

/**
 * @brief The SerializableKeyValueMap class provides an easy way to serialize
 * and deserialize ordered key value pairs
 *
 * @note This map guarantees to always contain a valid default value. A default
 * value has an empty string as key and will be used as fallback for #value().
 *
 * @warning When modifying this class, make sure that it still guarantees to
 * always contain a valid default value! So, don't add a default constructor, a
 *          `clear()` method or similar!
 */
template <typename T>
class SerializableKeyValueMap final {
  Q_DECLARE_TR_FUNCTIONS(SerializableKeyValueMap)

public:
  // Signals
  enum class Event {
    ElementAdded,
    ElementRemoved,
    ElementValueChanged,
  };
  Signal<SerializableKeyValueMap<T>, const QString&, Event> onEdited;
  typedef Slot<SerializableKeyValueMap<T>, const QString&, Event> OnEditedSlot;

  // Constructors / Destructor
  SerializableKeyValueMap() = delete;
  SerializableKeyValueMap(const SerializableKeyValueMap<T>& other) noexcept
    : onEdited(*this), mValues(other.mValues) {}
  explicit SerializableKeyValueMap(
      const typename T::ValueType& defaultValue) noexcept
    : onEdited(*this) {
    mValues.insert("", defaultValue);
  }
  explicit SerializableKeyValueMap(const SExpression& node) : onEdited(*this) {
    foreach (const SExpression* child, node.getChildren(T::tagname)) {
      QString key;
      SExpression value;
      if (child->getChild("@0").isList()) {
        key = child->getChild(QString(T::keyname) % "/@0").getValue();
        value = child->getChild("@1");
      } else {
        key = QString("");
        value = child->getChild("@0");
      }
      if (mValues.contains(key)) {
        throw RuntimeError(__FILE__, __LINE__,
                           tr("Key \"%1\" defined multiple times.").arg(key));
      }
      mValues.insert(key,
                     deserialize<typename T::ValueType>(value));  // can throw
    }
    if (!mValues.contains(QString(""))) {
      throw RuntimeError(__FILE__, __LINE__,
                         tr("No default %1 defined.").arg(T::tagname));
    }
  }
  ~SerializableKeyValueMap() noexcept {}

  // Getters
  QStringList keys() const noexcept { return mValues.keys(); }
  const typename T::ValueType& getDefaultValue() const noexcept {
    auto i = mValues.find(QString(""));
    // there must always be a default value!!!
    Q_ASSERT((i != mValues.end()) && (i.key() == QString("")));
    return i.value();
  }
  bool contains(const QString& key) const noexcept {
    return mValues.contains(key);
  }
  std::optional<typename T::ValueType> tryGet(
      const QString& key) const noexcept {
    auto i = mValues.find(key);
    if ((i != mValues.end()) && (i.key() == key)) {
      return i.value();
    } else {
      return std::nullopt;
    }
  }
  const typename T::ValueType& value(
      const QStringList& keyOrder, QString* usedKey = nullptr) const noexcept {
    // search in the specified key order
    foreach (const QString& key, keyOrder) {
      auto i = mValues.find(key);
      if ((i != mValues.end()) && (i.key() == key)) {
        if (usedKey) *usedKey = key;
        return i.value();
      }
    }
    // use default value (empty key) as fallback
    if (usedKey) *usedKey = QString("");
    return getDefaultValue();
  }

  // General Methods

  void setDefaultValue(const typename T::ValueType& value) noexcept {
    insert(QString(""), value);
  }

  void insert(const QString& key, const typename T::ValueType& value) noexcept {
    auto it = mValues.find(key);
    if (it == mValues.end()) {
      mValues.insert(key, value);
      onEdited.notify(key, Event::ElementAdded);
    } else if (it.value() != value) {
      mValues.insert(key, value);
      onEdited.notify(key, Event::ElementValueChanged);
    }
  }

  /**
   * @brief Serialize into ::librepcb::SExpression node
   *
   * @param root    Root node to serialize into.
   */
  void serialize(SExpression& root) const {
    for (auto i = mValues.constBegin(); i != mValues.constEnd(); ++i) {
      root.ensureLineBreak();
      SExpression& child = root.appendList(T::tagname);
      if (!i.key().isEmpty()) {
        child.appendChild(T::keyname, i.key());
      }
      child.appendChild(i.value());
    }
    root.ensureLineBreak();
  }

  // Operator Overloadings
  SerializableKeyValueMap<T>& operator=(
      const SerializableKeyValueMap<T>& rhs) noexcept {
    foreach (const QString& key, mValues.keys()) {
      mValues.remove(key);
      onEdited.notify(key, Event::ElementRemoved);
    }
    mValues = rhs.mValues;
    QMapIterator<QString, typename T::ValueType> i(rhs.mValues);
    while (i.hasNext()) {
      i.next();
      insert(i.key(), i.value());
    }
    return *this;
  }
  bool operator==(const SerializableKeyValueMap<T>& rhs) const noexcept {
    return mValues == rhs.mValues;
  }
  bool operator!=(const SerializableKeyValueMap<T>& rhs) const noexcept {
    return mValues != rhs.mValues;
  }

private:  // Data
  QMap<QString, typename T::ValueType> mValues;
};

/*******************************************************************************
 *  Class LocalizedNameMap
 ******************************************************************************/

struct LocalizedNameMapPolicy {
  typedef ElementName ValueType;
  static constexpr const char* tagname = "name";
  static constexpr const char* keyname = "locale";
};
using LocalizedNameMap = SerializableKeyValueMap<LocalizedNameMapPolicy>;

/*******************************************************************************
 *  Class LocalizedDescriptionMap
 ******************************************************************************/

struct LocalizedDescriptionMapPolicy {
  typedef QString ValueType;
  static constexpr const char* tagname = "description";
  static constexpr const char* keyname = "locale";
};
using LocalizedDescriptionMap =
    SerializableKeyValueMap<LocalizedDescriptionMapPolicy>;

/*******************************************************************************
 *  Class LocalizedKeywordsMap
 ******************************************************************************/

struct LocalizedKeywordsMapPolicy {
  typedef QString ValueType;
  static constexpr const char* tagname = "keywords";
  static constexpr const char* keyname = "locale";
};
using LocalizedKeywordsMap =
    SerializableKeyValueMap<LocalizedKeywordsMapPolicy>;

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb

#endif
