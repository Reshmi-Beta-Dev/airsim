#include "BoatPawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/CollisionProfile.h"
#include "PhysicsEngine/BodyInstance.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "BoatMovementComponent.h"

ABoatPawn::ABoatPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    BoatMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoatMesh"));
    RootComponent = BoatMesh;
    BoatMesh->SetSimulatePhysics(true);
    BoatMesh->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
    BoatMesh->BodyInstance.bEnableGravity = true;

    MovementComponent = CreateDefaultSubobject<UBoatMovementComponent>(TEXT("MovementComponent"));
    MovementComponent->SetUpdatedComponent(BoatMesh);

    SensorsRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SensorsRoot"));
    SensorsRoot->SetupAttachment(RootComponent);

    LidarMount = CreateDefaultSubobject<USceneComponent>(TEXT("LidarMount"));
    LidarMount->SetupAttachment(SensorsRoot);
    LidarMount->SetRelativeLocation(FVector(100.f, 0.f, 120.f));

    GpsMount = CreateDefaultSubobject<USceneComponent>(TEXT("GpsMount"));
    GpsMount->SetupAttachment(SensorsRoot);
    GpsMount->SetRelativeLocation(FVector(0.f, 0.f, 120.f));

    ImuMount = CreateDefaultSubobject<USceneComponent>(TEXT("ImuMount"));
    ImuMount->SetupAttachment(SensorsRoot);
    ImuMount->SetRelativeLocation(FVector(0.f, 0.f, 100.f));

    CameraMount = CreateDefaultSubobject<USceneComponent>(TEXT("CameraMount"));
    CameraMount->SetupAttachment(SensorsRoot);
    CameraMount->SetRelativeLocation(FVector(100.f, 0.f, 150.f));
}

void ABoatPawn::BeginPlay()
{
    Super::BeginPlay();
}

void ABoatPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABoatPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("Throttle", this, &ABoatPawn::InputThrottle);
    PlayerInputComponent->BindAxis("Steer", this, &ABoatPawn::InputSteering);
    PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &ABoatPawn::InputHandbrakePressed);
    PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &ABoatPawn::InputHandbrakeReleased);
}

void ABoatPawn::InputThrottle(float Value)
{
    if (MovementComponent)
    {
        MovementComponent->SetThrottleInput(Value);
    }
}

void ABoatPawn::InputSteering(float Value)
{
    if (MovementComponent)
    {
        MovementComponent->SetSteeringInput(Value);
    }
}

void ABoatPawn::InputHandbrakePressed()
{
    if (MovementComponent)
    {
        MovementComponent->SetHandbrake(true);
    }
}

void ABoatPawn::InputHandbrakeReleased()
{
    if (MovementComponent)
    {
        MovementComponent->SetHandbrake(false);
    }
}

float ABoatPawn::GetForwardSpeed() const
{
    return MovementComponent ? MovementComponent->GetForwardSpeed() : 0.0f;
}


