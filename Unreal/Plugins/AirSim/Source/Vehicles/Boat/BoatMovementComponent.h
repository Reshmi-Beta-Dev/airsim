#pragma once
#pragma once
/*
Confidential and Proprietary — Evaluation Only
Copyright (c) 2025. All rights reserved.
Licensed solely for internal evaluation and testing. Redistribution, commercial use,
or transfer is prohibited without written permission. See LICENSE-EVALUATION.txt.
*/

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BoatMovementComponent.generated.h"

/**
 * Physics-based movement component for a simple boat.
 * Applies thrust, rudder torque, hydrodynamic drag and a simple buoyancy model.
 *
 * Attach to a pawn that has a simulated physics root mesh (UStaticMeshComponent).
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UBoatMovementComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBoatMovementComponent();

    // UActorComponent
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    /** Set the physics mesh to drive. If null, the component will try to use the owner's root as a UPrimitiveComponent. */
    UFUNCTION(BlueprintCallable, Category="Boat|Movement")
    void SetUpdatedComponent(UPrimitiveComponent* InUpdatedComponent);

    /** Set throttle input in range [-1, 1]. Positive forward, negative reverse. */
    UFUNCTION(BlueprintCallable, Category="Boat|Input")
    void SetThrottleInput(float InThrottle);

    /** Set steering input in range [-1, 1]. Negative left, positive right. */
    UFUNCTION(BlueprintCallable, Category="Boat|Input")
    void SetSteeringInput(float InSteering);

    /** Apply quick stop drag when handbrake is enabled. */
    UFUNCTION(BlueprintCallable, Category="Boat|Input")
    void SetHandbrake(bool bInHandbrake);

    /** Returns the current forward speed in cm/s in the boat's forward direction. */
    UFUNCTION(BlueprintPure, Category="Boat|State")
    float GetForwardSpeed() const;

    /** Returns the physics body we are driving. */
    UFUNCTION(BlueprintPure, Category="Boat|State")
    UPrimitiveComponent* GetUpdatedComponent() const { return UpdatedComponent; }

public: // Tunables
    /** Maximum forward thrust (Newtons). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boat|Tuning")
    float MaxForwardThrust;

    /** Maximum reverse thrust (Newtons). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boat|Tuning")
    float MaxReverseThrust;

    /** Rudder torque coefficient. Higher values turn faster. Units: N*m per steer unit per m/s forward. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boat|Tuning")
    float RudderTorqueCoefficient;

    /** Linear water drag coefficient (N per (m/s)). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boat|Tuning")
    float LinearWaterDragCoefficient;

    /** Quadratic water drag coefficient (N per (m/s)^2). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boat|Tuning")
    float QuadraticWaterDragCoefficient;

    /** Lateral (sideways) drag multiplier to reduce side slip. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boat|Tuning")
    float LateralDragMultiplier;

    /** Angular damping torque coefficient (N*m per rad/s). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boat|Tuning")
    float AngularDampingCoefficient;

    /** Simple buoyancy: upward force proportional to submergence depth below water level. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boat|Buoyancy")
    float BuoyancyCoefficient;

    /** Height (Z) of the water plane in world space (cm). If using a dynamic water system, update this at runtime. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boat|Buoyancy")
    float WaterLevelZ;

    /** Maximum buoyancy force to clamp to (N). 0 means no clamp. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boat|Buoyancy")
    float MaxBuoyancyForce;

    /** When true, applies buoyancy and water drag only when below WaterLevelZ. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boat|Buoyancy")
    bool bOnlyInWater;

    /** Additional linear damping when handbrake is engaged. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boat|Tuning")
    float HandbrakeLinearDamping;

    /** Additional angular damping when handbrake is engaged. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boat|Tuning")
    float HandbrakeAngularDamping;

private:
    void ApplyThrust(float DeltaTime);
    void ApplyRudderTorque(float DeltaTime);
    void ApplyHydrodynamics(float DeltaTime);
    void ApplyBuoyancy(float DeltaTime);

    FVector GetForwardVector() const;
    FVector GetRightVector() const;

private:
    UPROPERTY(Transient)
    UPrimitiveComponent* UpdatedComponent;

    float ThrottleInput;
    float SteeringInput;
    bool bHandbrake;
};


