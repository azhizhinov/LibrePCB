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
#include "trace.h"

#include <QtCore>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {

/*******************************************************************************
 *  Class TraceAnchor
 ******************************************************************************/

TraceAnchor::TraceAnchor(const std::optional<Uuid>& junction,
                         const std::optional<Uuid>& via,
                         const std::optional<PadAnchor>& pad) noexcept
  : mJunction(junction), mVia(via), mPad(pad) {
  Q_ASSERT(((junction) && (!via) && (!pad)) ||
           ((!junction) && (via) && (!pad)) ||
           ((!junction) && (!via) && (pad)));
}

TraceAnchor::TraceAnchor(const TraceAnchor& other) noexcept
  : mJunction(other.mJunction), mVia(other.mVia), mPad(other.mPad) {
}

TraceAnchor::TraceAnchor(const SExpression& node) {
  if (const SExpression* junctionNode = node.tryGetChild("junction")) {
    mJunction = deserialize<Uuid>(junctionNode->getChild("@0"));
  } else if (const SExpression* viaNode = node.tryGetChild("via")) {
    mVia = deserialize<Uuid>(viaNode->getChild("@0"));
  } else {
    mPad = PadAnchor{deserialize<Uuid>(node.getChild("device/@0")),
                     deserialize<Uuid>(node.getChild("pad/@0"))};
  }
}

TraceAnchor::~TraceAnchor() noexcept {
}

void TraceAnchor::serialize(SExpression& root) const {
  if (mJunction) {
    root.appendChild("junction", *mJunction);
  } else if (mVia) {
    root.appendChild("via", *mVia);
  } else if (mPad) {
    root.appendChild("device", mPad->device);
    root.appendChild("pad", mPad->pad);
  } else {
    throw LogicError(__FILE__, __LINE__);
  }
}

bool TraceAnchor::operator==(const TraceAnchor& rhs) const noexcept {
  return (mJunction == rhs.mJunction) && (mVia == rhs.mVia) &&
      (mPad == rhs.mPad);
}

TraceAnchor& TraceAnchor::operator=(const TraceAnchor& rhs) noexcept {
  mJunction = rhs.mJunction;
  mVia = rhs.mVia;
  mPad = rhs.mPad;
  return *this;
}

TraceAnchor TraceAnchor::junction(const Uuid& junction) noexcept {
  return TraceAnchor(junction, std::nullopt, std::nullopt);
}

TraceAnchor TraceAnchor::via(const Uuid& via) noexcept {
  return TraceAnchor(std::nullopt, via, std::nullopt);
}

TraceAnchor TraceAnchor::pad(const Uuid& device, const Uuid& pad) noexcept {
  return TraceAnchor(std::nullopt, std::nullopt, PadAnchor{device, pad});
}

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

Trace::Trace(const Trace& other) noexcept
  : onEdited(*this),
    mUuid(other.mUuid),
    mLayer(other.mLayer),
    mWidth(other.mWidth),
    mStart(other.mStart),
    mEnd(other.mEnd) {
}

Trace::Trace(const Uuid& uuid, const Trace& other) noexcept : Trace(other) {
  mUuid = uuid;
}

Trace::Trace(const Uuid& uuid, const Layer& layer, const PositiveLength& width,
             const TraceAnchor& start, const TraceAnchor& end) noexcept
  : onEdited(*this),
    mUuid(uuid),
    mLayer(&layer),
    mWidth(width),
    mStart(start),
    mEnd(end) {
}

Trace::Trace(const SExpression& node)
  : onEdited(*this),
    mUuid(deserialize<Uuid>(node.getChild("@0"))),
    mLayer(deserialize<const Layer*>(node.getChild("layer/@0"))),
    mWidth(deserialize<PositiveLength>(node.getChild("width/@0"))),
    mStart(node.getChild("from")),
    mEnd(node.getChild("to")) {
}

Trace::~Trace() noexcept {
}

/*******************************************************************************
 *  Setters
 ******************************************************************************/

bool Trace::setUuid(const Uuid& uuid) noexcept {
  if (uuid == mUuid) {
    return false;
  }

  mUuid = uuid;
  onEdited.notify(Event::UuidChanged);
  return true;
}

bool Trace::setLayer(const Layer& layer) noexcept {
  if (&layer == mLayer) {
    return false;
  }

  mLayer = &layer;
  onEdited.notify(Event::LayerChanged);
  return true;
}

bool Trace::setWidth(const PositiveLength& width) noexcept {
  if (width == mWidth) {
    return false;
  }

  mWidth = width;
  onEdited.notify(Event::WidthChanged);
  return true;
}

bool Trace::setStartPoint(const TraceAnchor& start) noexcept {
  if (start == mStart) {
    return false;
  }

  mStart = start;
  onEdited.notify(Event::StartPointChanged);
  return true;
}

bool Trace::setEndPoint(const TraceAnchor& end) noexcept {
  if (end == mEnd) {
    return false;
  }

  mEnd = end;
  onEdited.notify(Event::EndPointChanged);
  return true;
}

/*******************************************************************************
 *  General Methods
 ******************************************************************************/

void Trace::serialize(SExpression& root) const {
  root.appendChild(mUuid);
  root.appendChild("layer", *mLayer);
  root.appendChild("width", mWidth);
  root.ensureLineBreak();
  mStart.serialize(root.appendList("from"));
  root.ensureLineBreak();
  mEnd.serialize(root.appendList("to"));
  root.ensureLineBreak();
}

/*******************************************************************************
 *  Operator Overloadings
 ******************************************************************************/

bool Trace::operator==(const Trace& rhs) const noexcept {
  if (mUuid != rhs.mUuid) return false;
  if (mLayer != rhs.mLayer) return false;
  if (mWidth != rhs.mWidth) return false;
  if (mStart != rhs.mStart) return false;
  if (mEnd != rhs.mEnd) return false;
  return true;
}

Trace& Trace::operator=(const Trace& rhs) noexcept {
  setUuid(rhs.mUuid);
  setLayer(*rhs.mLayer);
  setWidth(rhs.mWidth);
  setStartPoint(rhs.mStart);
  setEndPoint(rhs.mEnd);
  return *this;
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb
