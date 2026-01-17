# Pause Menu Setup Guide

## Overview
The pause menu allows players to pause the game mid-gameplay using the ESC key, providing options to resume, restart the current level, restart the entire game, or quit.

## C++ Implementation
The following files were added/modified:
- **PauseMenuWidget.h/cpp** - Widget with pause menu logic
- **Tank.h/cpp** - Added pause handling

## Unreal Engine Setup Steps

### 1. Create Pause Input Action

1. Navigate to `Content/Input/` folder
2. Right-click → Input → Input Action
3. Name it `IA_Pause`
4. Open `IA_Pause`
5. Set Value Type: **Digital (bool)**
6. Save

### 2. Add Pause Action to Input Mapping Context

1. Open `Content/Input/IMC_Default`
2. Click **+ Mappings** to add a new mapping
3. Select `IA_Pause` from dropdown
4. Click **+ (plus icon)** next to the action to add a key binding
5. Press **Escape** key to bind it
6. Save

### 3. Create Pause Menu Widget Blueprint

1. Navigate to `Content/UI/` folder
2. Right-click → User Interface → Widget Blueprint
3. Name it `WBP_PauseMenu`
4. Open `WBP_PauseMenu`
5. In the **Graph** tab, go to **Class Settings**
6. Set **Parent Class** to: `PauseMenuWidget` (should appear after compilation)

### 4. Design the Pause Menu UI

In the **Designer** tab of `WBP_PauseMenu`:

1. Add a **Canvas Panel** (root)
2. Add an **Overlay** as child
3. Add an **Image** (for background dimming):
   - Set anchors to **fill screen**
   - Set color to black with 50% opacity
4. Add a **Vertical Box** centered on screen:
   - Set alignment: Center, Middle
5. Add **4 Buttons** to the Vertical Box:
   - Button 1: Name = `ResumeButton`
   - Button 2: Name = `RestartLevelButton`
   - Button 3: Name = `RestartGameButton`
   - Button 4: Name = `QuitButton`
6. Add **Text Block** child to each button with labels:
   - "Resume"
   - "Restart Level"
   - "Restart Game"
   - "Quit"
7. Style buttons as desired (size, padding, colors)

**Important**: Button variable names MUST match exactly:
- `ResumeButton`
- `RestartLevelButton`
- `RestartGameButton`
- `QuitButton`

Check the **Is Variable** checkbox for each button in the Details panel.

### 5. Configure Tank Blueprint

1. Open `Content/Blueprint/BP_Tank`
2. In the **Details** panel, find:
   - **Input** → **Pause Action**: Select `IA_Pause`
   - **UI** → **Pause Menu Class**: Select `WBP_PauseMenu`
3. Compile and Save

### 6. Test the Pause Menu

1. Play in PIE (Play in Editor)
2. Wait for countdown to finish
3. Press **ESC** key
4. Game should pause and menu should appear
5. Test all buttons:
   - **Resume**: Unpause and return to game
   - **Restart Level**: Reload current level
   - **Restart Game**: Return to Level 1
   - **Quit**: Exit game (works in packaged build)

## Common Issues

### "Pause menu doesn't appear"
- Check that `IA_Pause` is assigned in BP_Tank
- Verify `WBP_PauseMenu` is assigned in BP_Tank
- Check button names match exactly (case-sensitive)

### "Buttons don't work"
- Verify buttons are marked as **Is Variable** in Blueprint
- Check button names: `ResumeButton`, `RestartLevelButton`, `RestartGameButton`, `QuitButton`
- Ensure `WBP_PauseMenu` parent class is `PauseMenuWidget`

### "ESC key doesn't work"
- Verify `IA_Pause` is added to `IMC_Default`
- Check ESC key is bound to `IA_Pause`
- Ensure input mapping context is loaded (should be automatic via BP_Tank)

### "Game doesn't unpause"
- Check that Resume button properly removes widget from parent
- Verify input mode is restored to GameAndUI

## Technical Details

**Pause Flow:**
1. Player presses ESC → `TogglePause()` called in Tank.cpp
2. Game paused via `UGameplayStatics::SetGamePaused()`
3. Widget created and added to viewport
4. Input mode switched to UI-only
5. Clicking Resume → unpause and restore game input

**Input Modes:**
- **Gameplay**: GameAndUI mode (allows mouse and keyboard)
- **Paused**: UIOnly mode (only UI interactions)
- **Resume**: Returns to GameAndUI mode

**Game State:**
- Pausing stops all Tick functions and timers
- Physics simulation pauses
- AI behavior pauses
- Audio continues (by default)

## Files Modified
- `Source/BattleBlaster/Tank.h` - Added pause action and widget class
- `Source/BattleBlaster/Tank.cpp` - Added pause toggle logic
- `Source/BattleBlaster/PauseMenuWidget.h` - New widget class
- `Source/BattleBlaster/PauseMenuWidget.cpp` - Widget implementation
