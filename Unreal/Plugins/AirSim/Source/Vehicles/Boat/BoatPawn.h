#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BoatPawn.generated.h"

class UStaticMeshComponent;
class UBoatMovementComponent;
class ULidarComponent;
class UCameraComponent;
class USpringArmComponent;
class UBoxComponent;
class UAirSimSensorFactory;

/**
 * A physics-driven boat pawn compatible with AirSim sensor attachments.
 */
UCLASS()
class ABoatPawn : public APawn
{
    GENERATED_BODY()

public:
    ABoatPawn();

    // APawn
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    // AirSim control inputs
    void InputThrottle(float Value);
    void InputSteering(float Value);
    void InputHandbrakePressed();
    void InputHandbrakeReleased();

    /** Returns forward speed in cm/s */
    UFUNCTION(BlueprintPure, Category="Boat|State")
    float GetForwardSpeed() const;

protected:
    UPROPERTY(VisibleAnywhere, Category="Boat|Components")
    UStaticMeshComponent* BoatMesh;

    UPROPERTY(VisibleAnywhere, Category="Boat|Components")
    UBoatMovementComponent* MovementComponent;

    // Sensor attach points (empty scene components). AirSim will attach sensors here via settings.json
    UPROPERTY(VisibleAnywhere, Category="Boat|Sensors")
    USceneComponent* SensorsRoot;

    UPROPERTY(VisibleAnywhere, Category="Boat|Sensors")
    USceneComponent* LidarMount;

    UPROPERTY(VisibleAnywhere, Category="Boat|Sensors")
    USceneComponent* GpsMount;

    UPROPERTY(VisibleAnywhere, Category="Boat|Sensors")
    USceneComponent* ImuMount;

    UPROPERTY(VisibleAnywhere, Category="Boat|Sensors")
    USceneComponent* CameraMount;
};


