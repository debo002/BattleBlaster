# Battle Blaster

Tank combat game built in Unreal Engine 5 with C++. Extended from tutorial foundation with custom AI systems, difficulty scaling, and level progression.

## Features

- Player tank with turret aiming and projectile combat
- Two AI enemy types: mobile tanks and stationary towers
- Dynamic difficulty scaling across 3-level campaign
- Component-based health and damage system
- Main menu with level progression tracking
- Game state management with countdown and win/lose conditions

## Custom Systems

**AI Tank Behavior**  
Mobile enemy with distance-based combat tactics, independent turret tracking, and pathfinding.

**Difficulty Scaling**  
Per-level stat adjustments for health, damage, fire rate, and movement speed across all actors.

**Level Progression**  
Campaign flow with GameInstance persistence, automatic advancement, and restart handling.

**Main Menu System**  
Dedicated menu level with UI-only input mode and level transition handling.

## Technical Stack

- **Engine:** Unreal Engine 5.7
- **Language:** C++
- **Systems:** Enhanced Input, Niagara VFX, AI Module, Navigation System

## Architecture

Component-based health system with delegate-driven death notifications. Pawn hierarchy shares turret rotation and firing logic. GameMode orchestrates countdown, win/lose conditions, and difficulty application.

## Project Structure

```
Source/BattleBlaster/
├── Core/          # GameMode, GameInstance, difficulty settings
├── Pawns/         # Tank, Tower, AITank, shared base
├── Components/    # Health component
├── Actors/        # Projectile
└── UI/            # Menu widgets and controllers
```

## Build Requirements

- Unreal Engine 5.7+
- Visual Studio 2022
- Windows 10/11

## Setup

1. Clone repository
2. Generate Visual Studio project files
3. Build in Development Editor
4. Configure Blueprint classes in editor:
   - Set GameInstance to `BP_BattleBlasterGameInstance`
   - Set GameMode to `BP_BattleBlasterGameMode` in gameplay levels
   - Name levels: `Level_1`, `Level_2`, `Level_3`
   - Place NavMeshBoundsVolume in each level

## Key Classes

- `ABasePawn` - Shared pawn functionality
- `ATank` - Player controller with cursor aiming and pause handling
- `ATower` / `AAITank` - AI enemy variants
- `UHealthComponent` - Reusable health/damage
- `ABattleBlasterGameMode` - Game orchestration
- `UBattleBlasterGameInstance` - Level progression persistence
- `UPauseMenuWidget` - In-game pause menu with game control options

---

Extended tutorial project with significant custom implementations.
