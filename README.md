# Battle Blaster

Tank combat game built in Unreal Engine 5 C++.

## Features

- **Combat System:** Projectile ballistics with impact VFX, screen shake, and component-based health.
- **AI Systems:** 
  - **Tanks:** Mobile enemies with obstacle avoidance and distance management.
  - **Towers:** Stationary turrets with line-of-sight checks.
- **Progression:** 3-level campaign with difficulty scaling.
- **UI:** Main menu, HUD, and pause menu.

## Campaign

### Level 1: Close Quarters
- Small enclosed map with gravity-affected projectiles
- Enemies: AI Tanks + Towers
- Focus: Close-range combat

### Level 2: Long Range
- Open terrain with extended sight lines
- Fast, flat projectiles for distant engagements
- Focus: Precision shooting

### Level 3: Hard Mode
- Aggressive AI with faster fire rates
- Multiple simultaneous threats
- Focus: Survival under pressure

## Technical

- **Engine:** Unreal Engine 5.5
- **Language:** C++
- **Subsystems:** Enhanced Input, Niagara, UMG, AI Module

## Key Classes

- `ABasePawn`: Turret rotation and projectile spawning
- `ATank`: Player controller with cursor aiming
- `AAITank`: Mobile AI with obstacle avoidance
- `ATower`: Stationary AI turret
- `UHealthComponent`: Damage handling via delegates
- `ABattleBlasterGameMode`: Game loop and difficulty injection
- `UBattleBlasterGameInstance`: Level persistence
- `FDifficultyManager`: Per-level stat configuration

## Setup

1. Generate project files and compile
2. Set GameInstance to `BP_BattleBlasterGameInstance`
3. Configure Maps: `MainMenu`, `Level_1`, `Level_2`, `Level_3`
4. Launch from `MainMenu`
