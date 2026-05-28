#include "garbage_bin.h"

#include <algorithm>
#include <cmath>

GarbageBin::GarbageBin(int id,
                       const std::wstring& streetName,
                       MapCoordinate mapCoordinate,
                       double fillLevel,
                       double internalFillRatePerHour)
    : id_(id),
      streetName_(streetName),
      mapCoordinate_(mapCoordinate),
      fillLevel_(0.0),
      quantizedFillLevel_(0),
      internalFillRatePerHour_(std::max(0.0, internalFillRatePerHour)),
      approximatedFillRatePerHour_(std::max(0.0, internalFillRatePerHour)),
      observedQuantizedFillLevel_(0),
      hoursSinceQuantizedChange_(0) {
    setFillLevel(fillLevel);
    resetVisibleObservation();
}

int GarbageBin::id() const {
    return id_;
}

const std::wstring& GarbageBin::streetName() const {
    return streetName_;
}

MapCoordinate GarbageBin::mapCoordinate() const {
    return mapCoordinate_;
}

double GarbageBin::fillLevel() const {
    return fillLevel_;
}

int GarbageBin::quantizedFillLevel() const {
    return quantizedFillLevel_;
}

double GarbageBin::internalFillRatePerHour() const {
    return internalFillRatePerHour_;
}

double GarbageBin::approximatedFillRatePerHour() const {
    return approximatedFillRatePerHour_;
}

std::optional<long long> GarbageBin::scheduledPickupHour() const {
    return scheduledPickupHour_;
}

void GarbageBin::advanceOneHour() {
    const int previousQuantizedFill = quantizedFillLevel_;
    setFillLevel(fillLevel_ + internalFillRatePerHour_);
    updateVisibleFillAfterGrowth(previousQuantizedFill);
}

void GarbageBin::schedulePickup(long long pickupHour) {
    scheduledPickupHour_ = pickupHour;
}

bool GarbageBin::pickupDue(long long currentHour) const {
    return scheduledPickupHour_.has_value() && scheduledPickupHour_.value() <= currentHour;
}

void GarbageBin::applyScheduledPickup() {
    setFillLevel(0.0);
    scheduledPickupHour_.reset();
    resetVisibleObservation();
}

int GarbageBin::quantizeFillLevel(double fillLevel) {
    const double clamped = std::clamp(fillLevel, 0.0, 100.0);
    const int quantized = static_cast<int>(std::round(clamped / 20.0) * 20.0);
    return std::clamp(quantized, 0, 100);
}

void GarbageBin::setFillLevel(double fillLevel) {
    fillLevel_ = std::clamp(fillLevel, 0.0, 100.0);
    quantizedFillLevel_ = quantizeFillLevel(fillLevel_);
}

void GarbageBin::updateVisibleFillAfterGrowth(int previousQuantizedFill) {
    ++hoursSinceQuantizedChange_;

    if (quantizedFillLevel_ > previousQuantizedFill && hoursSinceQuantizedChange_ > 0) {
        approximatedFillRatePerHour_ =
            static_cast<double>(quantizedFillLevel_ - observedQuantizedFillLevel_) /
            static_cast<double>(hoursSinceQuantizedChange_);
        observedQuantizedFillLevel_ = quantizedFillLevel_;
        hoursSinceQuantizedChange_ = 0;
    }
}

void GarbageBin::resetVisibleObservation() {
    quantizedFillLevel_ = quantizeFillLevel(fillLevel_);
    observedQuantizedFillLevel_ = quantizedFillLevel_;
    hoursSinceQuantizedChange_ = 0;
}
