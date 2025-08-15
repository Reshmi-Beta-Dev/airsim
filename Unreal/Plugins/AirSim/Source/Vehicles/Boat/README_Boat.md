## Boat Pawn for AirSim (UE 4.27)

This folder contains a physics-based `ABoatPawn` and `UBoatMovementComponent` intended to integrate a simple boat into AirSim.

### Files
- `BoatPawn.h/.cpp`: Pawn with physics mesh and sensor mount points
- `BoatMovementComponent.h/.cpp`: Thrust, steering, drag, and buoyancy

### How to integrate with AirSim
1) Add these files under `Unreal/Plugins/AirSim/Source/Vehicles/Boat/`
2) Regenerate project files and open in UE 4.27
3) Create a Blueprint derived from `ABoatPawn`. Name it `BP_BoatPawn` and place it in `Content/AirSim/Boat/`
   - Assign a boat/static mesh with proper collision (block all dynamic)
   - Keep `Simulate Physics` enabled on the mesh
4) Water plane
   - Place a large flat mesh (plane) as water surface at Z=0
   - Set its collision to `NoCollision` so the boat interacts only with the world, not the water plane
   - Set `WaterLevelZ` in `UBoatMovementComponent` (default 0) to match world water height

### Register the Boat vehicle type in AirSim (C++)
AirSim needs a SimApi for your vehicle. The simplest path is to reuse the Car RPC interface so Python `CarClient` works:

In `SimModeBase` (AirSim plugin), extend vehicle creation logic (pseudo-code):

```cpp
// In SimModeBase::createVehicleApi(...)
if (vehicle_settings->vehicle_type == "Boat") {
    // Reuse CarApiBase so CarClient works
    auto boat_pawn = Cast<ABoatPawn>(findOrSpawnActor<ABoatPawn>(...));
    auto api = std::make_shared<msr::airlib::CarApiBase>(/* implement a small adapter that forwards to ABoatPawn */);
    // Hook up api provider, sensors, etc., analogous to PhysXCar case
    return api;
}
```

Implement a minimal adapter that derives from `msr::airlib::CarApiBase` and translates `CarControls` to `SetThrottleInput`, `SetSteeringInput` and handbrake on the `ABoatPawn`.

Alternatively, you can add `VehicleType: "PhysXCar"` in settings and set `PawnBP` to your `BP_BoatPawn`. Then modify the `createVehicleApi` case for `PhysXCar` to accept a non-`AWheeledVehicle` pawn and route controls to `ABoatPawn` when detected.

### Example settings.json
See `settings_boat.json` alongside this file for a working example.

### Input mappings (optional for manual play)
Add in Project Settings → Input:
- Axis `Throttle`: W=+1, S=-1
- Axis `Steer`: A=-1, D=+1
- Action `Handbrake`: Spacebar

### Notes
- Sensors are configured via AirSim `settings.json` and will attach to the pawn. The provided mount points (`LidarMount`, `GpsMount`, `ImuMount`, `CameraMount`) are for convenience; AirSim applies the sensor relative pose directly.
- For dynamic water height, set `WaterLevelZ` programmatically from your water system.


