#if 0
// Confidential and Proprietary — Evaluation Only
// Licensed solely for internal evaluation and testing. See LICENSE-EVALUATION.txt.
#endif
#include "BoatMovementComponent.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"

UBoatMovementComponent::UBoatMovementComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    MaxForwardThrust = 25000.0f;         // N
    MaxReverseThrust = 15000.0f;         // N
    RudderTorqueCoefficient = 3500.0f;   // N*m per steer per m/s
    LinearWaterDragCoefficient = 800.0f; // N per (m/s)
    QuadraticWaterDragCoefficient = 120.0f; // N per (m/s)^2
    LateralDragMultiplier = 3.0f;
    AngularDampingCoefficient = 1200.0f; // N*m per rad/s
    BuoyancyCoefficient = 60000.0f;      // N per meter submerged
    WaterLevelZ = 0.0f;
    MaxBuoyancyForce = 0.0f;
    bOnlyInWater = true;
    HandbrakeLinearDamping = 4.0f;
    HandbrakeAngularDamping = 2000.0f;

    ThrottleInput = 0.0f;
    SteeringInput = 0.0f;
    bHandbrake = false;
}

void UBoatMovementComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!UpdatedComponent)
    {
        UpdatedComponent = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
    }

    if (UpdatedComponent)
    {
        UpdatedComponent->SetSimulatePhysics(true);
        UpdatedComponent->SetEnableGravity(true);
    }
}

void UBoatMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!UpdatedComponent)
    {
        return;
    }

    ApplyThrust(DeltaTime);
    ApplyRudderTorque(DeltaTime);
    ApplyHydrodynamics(DeltaTime);
    ApplyBuoyancy(DeltaTime);
}

void UBoatMovementComponent::SetUpdatedComponent(UPrimitiveComponent* InUpdatedComponent)
{
    UpdatedComponent = InUpdatedComponent;
}

void UBoatMovementComponent::SetThrottleInput(float InThrottle)
{
    ThrottleInput = FMath::Clamp(InThrottle, -1.0f, 1.0f);
}

void UBoatMovementComponent::SetSteeringInput(float InSteering)
{
    SteeringInput = FMath::Clamp(InSteering, -1.0f, 1.0f);
}

void UBoatMovementComponent::SetHandbrake(bool bInHandbrake)
{
    bHandbrake = bInHandbrake;
}

float UBoatMovementComponent::GetForwardSpeed() const
{
    if (!UpdatedComponent)
    {
        return 0.0f;
    }

    const FVector Forward = GetForwardVector();
    const FVector Velocity = UpdatedComponent->GetComponentVelocity();
    return FVector::DotProduct(Velocity, Forward);
}

FVector UBoatMovementComponent::GetForwardVector() const
{
    return UpdatedComponent ? UpdatedComponent->GetComponentQuat().GetForwardVector() : FVector::ForwardVector;
}

FVector UBoatMovementComponent::GetRightVector() const
{
    return UpdatedComponent ? UpdatedComponent->GetComponentQuat().GetRightVector() : FVector::RightVector;
}

void UBoatMovementComponent::ApplyThrust(float DeltaTime)
{
    if (!UpdatedComponent)
    {
        return;
    }

    const float SpeedForward = GetForwardSpeed() / 100.0f; // convert cm/s to m/s
    const float MaxThrust = (ThrottleInput >= 0.0f) ? MaxForwardThrust : MaxReverseThrust;
    const float Thrust = MaxThrust * ThrottleInput;

    const FVector Force = GetForwardVector() * Thrust;
    UpdatedComponent->AddForce(Force, NAME_None, true);
}

void UBoatMovementComponent::ApplyRudderTorque(float DeltaTime)
{
    if (!UpdatedComponent)
    {
        return;
    }

    const float ForwardSpeedMS = FMath::Abs(GetForwardSpeed()) / 100.0f;
    const float TorqueMagnitude = RudderTorqueCoefficient * SteeringInput * ForwardSpeedMS;
    const FVector Up = FVector::UpVector;
    const FVector Torque = Up * TorqueMagnitude;
    UpdatedComponent->AddTorqueInRadians(Torque, NAME_None, true);
}

void UBoatMovementComponent::ApplyHydrodynamics(float DeltaTime)
{
    if (!UpdatedComponent)
    {
        return;
    }

    const FVector Velocity = UpdatedComponent->GetComponentVelocity() / 100.0f; // m/s
    if (Velocity.IsNearlyZero())
    {
        return;
    }

    const FVector Forward = GetForwardVector();
    const FVector Right = GetRightVector();
    const float ForwardSpeed = FVector::DotProduct(Velocity, Forward);
    const float LateralSpeed = FVector::DotProduct(Velocity, Right);

    // Longitudinal drag
    const float SignForward = FMath::Sign(ForwardSpeed);
    const float LongitudinalDragN = LinearWaterDragCoefficient * FMath::Abs(ForwardSpeed) +
                                    QuadraticWaterDragCoefficient * FMath::Square(ForwardSpeed);
    const FVector LongitudinalForce = -Forward * LongitudinalDragN * SignForward;

    // Lateral drag for stability
    const float SignLateral = FMath::Sign(LateralSpeed);
    const float LateralDragN = (LinearWaterDragCoefficient * FMath::Abs(LateralSpeed) +
                                QuadraticWaterDragCoefficient * FMath::Square(LateralSpeed)) * LateralDragMultiplier;
    const FVector LateralForce = -Right * LateralDragN * SignLateral;

    FVector TotalDrag = (LongitudinalForce + LateralForce) * 100.0f; // convert back to cm-based units

    if (bHandbrake)
    {
        TotalDrag *= 1.5f;
    }

    UpdatedComponent->AddForce(TotalDrag, NAME_None, true);

    // Angular damping
    const FVector AngularVelRad = UpdatedComponent->GetPhysicsAngularVelocityInRadians();
    FVector AngularDampingTorque = -AngularVelRad * AngularDampingCoefficient;
    if (bHandbrake)
    {
        AngularDampingTorque.Z -= AngularVelRad.Z * HandbrakeAngularDamping;
    }
    UpdatedComponent->AddTorqueInRadians(AngularDampingTorque, NAME_None, true);
}

void UBoatMovementComponent::ApplyBuoyancy(float DeltaTime)
{
    if (!UpdatedComponent)
    {
        return;
    }

    const FVector Location = UpdatedComponent->GetComponentLocation();
    const float Depth = (WaterLevelZ - Location.Z) / 100.0f; // meters submerged (positive when below water)
    const bool bIsInWater = Depth > 0.0f;
    if (bOnlyInWater && !bIsInWater)
    {
        return;
    }

    const float BuoyancyForceN = FMath::Max(0.0f, Depth) * BuoyancyCoefficient;
    const float ClampedBuoyancyN = (MaxBuoyancyForce > 0.0f) ? FMath::Min(BuoyancyForceN, MaxBuoyancyForce) : BuoyancyForceN;
    const FVector UpwardForce = FVector::UpVector * (ClampedBuoyancyN * 100.0f); // convert to cm*kg/s^2
    UpdatedComponent->AddForce(UpwardForce, NAME_None, true);

    // Extra linear damping when handbrake
    if (bHandbrake)
    {
        const FVector Vel = UpdatedComponent->GetComponentVelocity();
        UpdatedComponent->AddForce(-Vel * HandbrakeLinearDamping, NAME_None, true);
    }
}


