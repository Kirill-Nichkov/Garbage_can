#pragma once

#include <optional>
#include <string>

struct MapCoordinate {
    double x;
    double y;
};

class GarbageBin {
public:
    GarbageBin(int id,
               const std::wstring& streetName,
               MapCoordinate mapCoordinate,
               double fillLevel,
               double internalFillRatePerHour);

    int id() const;
    const std::wstring& streetName() const;
    MapCoordinate mapCoordinate() const;

    double fillLevel() const;
    int quantizedFillLevel() const;
    double internalFillRatePerHour() const;
    double approximatedFillRatePerHour() const;

    std::optional<long long> scheduledPickupHour() const;

    void advanceOneHour();
    void schedulePickup(long long pickupHour);
    bool pickupDue(long long currentHour) const;
    void applyScheduledPickup();

    static int quantizeFillLevel(double fillLevel);

private:
    void setFillLevel(double fillLevel);
    void updateVisibleFillAfterGrowth(int previousQuantizedFill);
    void resetVisibleObservation();

    int id_;
    std::wstring streetName_;
    MapCoordinate mapCoordinate_;
    double fillLevel_;
    int quantizedFillLevel_;
    double internalFillRatePerHour_;
    double approximatedFillRatePerHour_;
    int observedQuantizedFillLevel_;
    int hoursSinceQuantizedChange_;
    std::optional<long long> scheduledPickupHour_;
};
