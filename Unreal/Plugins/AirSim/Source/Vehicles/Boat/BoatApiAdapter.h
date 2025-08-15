#pragma once

#include <memory>

// AirSim
#include "vehicles/car/api/CarApiBase.hpp"

class ABoatPawn;

// Minimal adapter that lets AirSim's CarClient control the boat pawn.
class BoatApiAdapter : public msr::airlib::CarApiBase
{
public:
    explicit BoatApiAdapter(ABoatPawn* InBoatPawn);

    // CarApiBase overrides
    virtual void setCarControls(const CarControls& controls) override;
    virtual CarControls getCarControls() const override { return controls_; }
    virtual CarState getCarState() const override;

    // no-op implementations for features not used by the boat
    virtual void enableApiControl(bool) override {}
    virtual bool isApiControlEnabled() const override { return true; }
    virtual void reset() override {}
    virtual void setRCData(const msr::airlib::RCData&) override {}

private:
    ABoatPawn* boatPawn_;
    CarControls controls_;
};


