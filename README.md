## AirSim Boat Pawn (UE 4.27)

Physics-based Boat pawn and movement component for Unreal Engine 4.27, designed to integrate with Microsoft AirSim and be controllable via the Car API (Python `CarClient`).

### Features
- Physics-driven boat movement (throttle, reverse, steering)
- Hydrodynamics: longitudinal and lateral drag, angular damping
- Simple buoyancy with configurable `WaterLevelZ`
- Handbrake behavior for rapid stop
- Sensor-ready mounts (Lidar, GPS, IMU, Camera)
- AirSim adapter exposing Car API controls

### Repository layout
- `Unreal/Plugins/AirSim/Source/Vehicles/Boat/`
  - `BoatPawn.*` – C++ pawn with physics and sensor mount points
  - `BoatMovementComponent.*` – thrust, steering, drag, buoyancy
  - `BoatApiAdapter.*` – bridges Car API to `ABoatPawn`
  - `README_Boat.md` – in-plugin notes
  - `settings_boat.json` – example AirSim settings

### Prerequisites
- Unreal Engine 4.27
- AirSim plugin in your UE project (same engine version)
- Visual Studio 2019 with C++ components

### Setup
1) Copy `Unreal/Plugins/AirSim/` from this repo into your Unreal project (or integrate the files into your existing AirSim plugin). Final path should be:
   - `YourProject/Plugins/AirSim/Source/Vehicles/Boat/`
2) Regenerate Visual Studio project files for your `.uproject`, open in UE4.27 or VS, and build.
3) Create a Blueprint derived from `ABoatPawn` (e.g., `BP_BoatPawn`); assign a boat static mesh with collision. Enable `Simulate Physics` on the mesh and use a blocking collision profile (e.g., `PhysicsActor`).
4) Water surface:
   - Add a large plane (or water mesh) at `Z=0`, set `NoCollision`
   - Set the movement component's `WaterLevelZ` to match the water height
5) AirSim settings: copy `settings_boat.json` into your `%USERPROFILE%/Documents/AirSim/settings.json` (Windows) and adjust the `PawnBP` path. Minimal example:
```json
{
  "SettingsVersion": 1.2,
  "SimMode": "Car",
  "Vehicles": {
    "Boat1": {
      "VehicleType": "Boat",
      "AutoCreate": true,
      "PawnBP": "/AirSim/Boat/BP_BoatPawn.BP_BoatPawn_C"
    }
  }
}
```
6) Ensure your sim mode registers `VehicleType: "Boat"` to spawn `ABoatPawn` and bind `BoatApiAdapter` so `CarClient` works. See `README_Boat.md` for notes.

### Control via Python (Car API)
```python
import airsim
from airsim import CarControls

client = airsim.CarClient()
client.confirmConnection()
client.enableApiControl(True)

controls = CarControls()
controls.throttle = 0.5
controls.steering = 0.2
client.setCarControls(controls)
```

### Notes
- Sensors are configured via AirSim `settings.json`.
- For dynamic water, update `WaterLevelZ` each tick from your water system.


