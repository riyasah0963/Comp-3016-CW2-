# Visual Studio 2022 Setup Guide

## 🎮 How to Run the Game in Visual Studio 2022

### Step 1: Open the Project
1. **Double-click** on `EchoesGame.sln` file
2. Visual Studio 2022 will open automatically
3. Wait for the solution to load (you'll see "Ready" in the bottom left)

### Step 2: Select Configuration
1. At the top toolbar, you'll see two dropdown menus
2. Select **"Debug"** or **"Release"** from the first dropdown
3. Select **"x64"** from the second dropdown (should already be selected)

**Recommended for testing**: Use **Debug** mode  
**Recommended for professor review**: Use **Release** mode (faster)

### Step 3: Build and Run
**Option A - One Click (Recommended)**:
1. Press **F5** key on your keyboard
2. OR click the green **"Local Windows Debugger"** ▶️ button at the top
3. The game will compile and start automatically!

**Option B - Manual Build**:
1. Menu: **Build → Build Solution** (or press **Ctrl+Shift+B**)
2. Wait for compilation (see Output window)
3. Menu: **Debug → Start Debugging** (or press **F5**)

### Step 4: Play the Game
- Game window opens at 1280x720 resolution
- Background music starts automatically
- Use WASD keys to move, mouse to look around
- Press F key to pickup items
- Left click to attack enemies

---

## 📁 Which File to Open?

### ✅ OPEN THIS FILE:
**`EchoesGame.sln`** (the Solution file)

### ❌ DON'T OPEN THESE:
- Individual `.cpp` or `.h` files (these will open in text editor)
- `EchoesGame.vcxproj` directly (confusing)
- `Makefile` (this is for Linux)

---

## 🔧 What Happens When You Press F5?

### Automatic Process:
```
1. Visual Studio reads EchoesGame.vcxproj
   ↓
2. Compiler (MSVC) compiles all .cpp files:
   - main.cpp
   - GameEngine.cpp
   - AudioEngine.cpp
   - ParticleSystem.cpp
   - WorldManager.cpp
   - OpenGLRenderer.cpp
   - Camera.cpp, Shader.cpp, Mesh.cpp
   - Player.cpp, Enemy.cpp, Room.cpp, Item.cpp
   ↓
3. Linker links compiled code with libraries:
   - opengl32.lib (Graphics)
   - glew32s.lib (OpenGL Extensions)
   - glfw3.lib (Window Management)
   - OpenAL32.lib (Audio)
   ↓
4. Creates executable:
   bin/Debug/EchoesGame.exe (or bin/Release/EchoesGame.exe)
   ↓
5. Runs the game automatically!
```

**Time**: Usually 10-30 seconds for first build, 2-5 seconds for rebuilds

---

## 📂 Project Structure in Visual Studio

When you open the solution, you'll see:

```
Solution 'EchoesGame' (1 of 1 project)
  └── EchoesGame
      ├── Header Files (.h)
      │   ├── AudioEngine.h
      │   ├── GameEngine.h
      │   ├── ParticleSystem.h
      │   ├── WorldManager.h
      │   ├── OpenGLRenderer.h
      │   ├── Camera.h, Shader.h, Mesh.h
      │   └── Player.h, Enemy.h, Room.h, Item.h
      │
      └── Source Files (.cpp)
          ├── main.cpp ⭐ (Entry point)
          ├── AudioEngine.cpp
          ├── GameEngine.cpp
          ├── ParticleSystem.cpp
          ├── WorldManager.cpp
          ├── OpenGLRenderer.cpp
          ├── Camera.cpp, Shader.cpp, Mesh.cpp
          └── Player.cpp, Enemy.cpp, Room.cpp, Item.cpp
```

**⭐ main.cpp** is the starting point - Visual Studio knows this automatically!

---

## 🚀 Quick Start Commands

| Action | Keyboard Shortcut | Button |
|--------|------------------|--------|
| **Build & Run** | F5 | Green ▶️ |
| **Build Only** | Ctrl+Shift+B | Build → Build Solution |
| **Stop Running** | Shift+F5 | Red ⏹️ |
| **Rebuild All** | Ctrl+Alt+F7 | Build → Rebuild Solution |

---

## ✅ Pre-Flight Checklist

Before running, ensure:
- [x] Visual Studio 2022 installed
- [x] C++ Desktop Development workload installed
- [x] All files in correct folders:
  - `src/` folder has all .cpp and .h files
  - `lib/` folder has library files (.lib)
  - `include/` folder has header files
  - `sounds/` folder has audio files (.wav)

---

## 🎯 For Professor Review

### Quick Test (30 seconds):
1. Double-click `EchoesGame.sln`
2. Wait for Visual Studio to open
3. Press **F5**
4. Game starts - you'll hear music and see 3D graphics
5. Walk around with WASD keys

### Full Test (5 minutes):
1. Press F5 to start
2. Move around Village Square (WASD)
3. Pickup items (press F near items)
4. Fight enemy (left click)
5. Travel to different rooms (explore)
6. Check particle effects (blood, dust, sparkles)
7. Listen for 3D audio (footsteps, combat sounds)

---

## 🔍 Troubleshooting

### Build Errors?
**Solution**: 
1. Clean solution: **Build → Clean Solution**
2. Rebuild: **Build → Rebuild Solution** (Ctrl+Alt+F7)

### Missing Libraries?
**Error**: "Cannot open file 'opengl32.lib'" or similar

**Solution**: Check `lib/` folder contains:
- glew32s.lib
- glfw3.lib
- OpenAL32.lib

### Game Crashes on Start?
**Solutions**:
1. Update graphics drivers
2. Check OpenGL version: Right-click desktop → Graphics Properties
3. Run in Release mode instead of Debug
4. Ensure sounds/ folder has .wav files (game works without audio though)

### No Audio?
**Not a problem**: Game continues silently if audio fails. Check:
- OpenAL32.dll in same folder as .exe
- sounds/ folder has .wav files
- Audio drivers installed

---

## 📊 Build Configuration Differences

### Debug Mode (F5 with "Debug" selected)
- **Optimization**: None (slow but easy to debug)
- **Executable Size**: Large (~5-10 MB)
- **Build Time**: Faster compilation
- **Output**: `bin/Debug/EchoesGame.exe`
- **Best for**: Development, testing, debugging

### Release Mode (F5 with "Release" selected)
- **Optimization**: Full (fast performance)
- **Executable Size**: Smaller (~2-5 MB)
- **Build Time**: Slower compilation
- **Output**: `bin/Release/EchoesGame.exe`
- **Best for**: Final submission, professor testing

**Recommendation for grading**: Use **Release** mode for best performance!

---

## 🎓 Academic Note

This project demonstrates:
- ✅ Professional Visual Studio project structure
- ✅ Proper build configuration (Debug/Release)
- ✅ Correct library linking
- ✅ C++17 standard compliance
- ✅ Clean code organization (headers/source separation)
- ✅ One-click build and run (no manual steps required)

**Professor can literally just**:
1. Open `EchoesGame.sln`
2. Press F5
3. Game runs perfectly!

---

## 💡 Pro Tips

1. **First Build Takes Longer**: Initial compilation ~30 seconds. Subsequent builds ~2-5 seconds.

2. **Code Changes**: After editing code, just press F5 again - Visual Studio auto-recompiles changed files only!

3. **View Output**: Window → Output (Ctrl+Alt+O) shows build progress and any errors.

4. **Error List**: Window → Error List (Ctrl+\\, E) shows compilation errors if any.

5. **Performance Mode**: For best FPS, use Release mode.

6. **Console Window**: Game runs in console mode - you'll see debug messages there.

---

## 🏆 Success Indicators

When game runs successfully, you'll see:
1. **Console window** opens (black screen with text)
2. **Game window** opens (1280x720, 3D graphics)
3. **Background music** starts playing
4. **Player in Village Square** with blue sky
5. **Smooth 60 FPS** rendering
6. **No crashes** or errors

---

## 📞 Quick Reference

| What | Where |
|------|-------|
| Solution File | `EchoesGame.sln` ← Open this! |
| Entry Point | `src/main.cpp` |
| Main Game Logic | `src/GameEngine.cpp` |
| Output Executable | `bin/Debug/EchoesGame.exe` or `bin/Release/EchoesGame.exe` |
| Build Config | Top toolbar (Debug/Release, x64) |
| Run Button | Green ▶️ "Local Windows Debugger" |
| Keyboard Shortcut | **F5** |

---

**You're all set! Just open `EchoesGame.sln` and press F5! 🚀**
