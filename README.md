# Battle Blaster

Production-ready tank combat game built in Unreal Engine 5 C++. Features component-based architecture, custom AI behaviors, and a scalable difficulty system.

## Features

- **Combat System:** Projectile-based ballistics with impact VFX, screen shake, and component-based health handling.
- **AI Systems:** 
  - **Tanks:** Mobile enemies with obstacle avoidance, distance management, and prediction-based aiming.
  - **Towers:** Stationary threats with line-of-sight checks and turret tracking.
- **Progression:** 3-level campaign with persistent state and automatic difficulty scaling.
- **UI:** Complete flow including main menu, HUD with countdowns, and pause menu with game state control.

## Technical Implementation

- **Engine:** Unreal Engine 5.5
- **Languages:** C++ (Core logic), Blueprints (Asset binding)
- **Subsystems:** Enhanced Input, Niagara VFX, UMG, AI Module

## Difficulty System

Custom `DifficultyManager` handling per-level scaling for:
- Player/Enemy Health & Damage
- AI Fire Rates & Accuracy
- Movement Speeds & Sight Ranges

## Key Classes

* `ABasePawn`: Shared functionality for turret rotation and projectile spawning
* `ATank`: Player controller with cursor-based aiming and input handling
* `AAITank`: Mobile AI using custom obstacle avoidance vector math
* `ATower`: Stationary AI with range and LOS checks
* `UHealthComponent`: Reusable damage handler using delegate broadcasting
* `ABattleBlasterGameMode`: Manages game loop, difficulty injection, and win/lose states
* `UBattleBlasterGameInstance`: Persistent level tracking and transitions

## Setup

1. **Build:** Generate project files and compile in Development Editor configuration.
2. **Editor:**
   - Set GameInstance to `BP_BattleBlasterGameInstance`.
   - Configure Maps: `MainMenu`, `Level_1`, `Level_2`, `Level_3`.
   - Ensure `NavMeshBoundsVolume` covers playable areas.
3. **Play:** Launch from `MainMenu` to initialize progression properly.

## Project Structure

Source files located in simple flat structure at `Source/BattleBlaster/`:
- **Core Loop:** GameMode, GameInstance, DifficultySettings
- **Entities:** Tank, AITank, Tower, BasePawn, Projectile
- **Components:** HealthComponent
- **UI:** MainMenuWidget, PauseMenuWidget, ScreenMessage
