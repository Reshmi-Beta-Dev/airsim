#include "BoatApiAdapter.h"
#include "BoatPawn.h"

BoatApiAdapter::BoatApiAdapter(ABoatPawn* InBoatPawn)
    : boatPawn_(InBoatPawn)
{
}

void BoatApiAdapter::setCarControls(const CarControls& controls)
{
    controls_ = controls;
    if (!boatPawn_) return;

    boatPawn_->InputThrottle(controls.throttle - controls.brake);
    boatPawn_->InputSteering(controls.steering);
    if (controls.handbrake)
        boatPawn_->InputHandbrakePressed();
    else
        boatPawn_->InputHandbrakeReleased();
}

msr::airlib::CarApiBase::CarState BoatApiAdapter::getCarState() const
{
    CarState state;
    state.speed = boatPawn_ ? boatPawn_->GetForwardSpeed() / 100.0f : 0.0f; // m/s
    state.gear = (controls_.throttle - controls_.brake) >= 0.0f ? 1 : -1;
    state.rc_data = msr::airlib::RCData();
    state.timestamp = msr::airlib::ClockFactory::get()->nowNanos();
    return state;
}


