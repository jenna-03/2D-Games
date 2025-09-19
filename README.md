# Simple 2D Scene

## 📝 Project Overview

This project creates an interactive 2D scene using OpenGL featuring animated characters with various transformations including movement, rotation, and relative positioning. The scene showcases fundamental game programming concepts including delta time-based animations and texture mapping.

## ✨ Features

- **Multiple Textured Objects**: Two distinct characters with unique image textures
- **Dynamic Movement**: Characters animate with smooth translations
- **Rotation Effects**: Spinning animations for visual appeal
- **Relative Positioning**: Objects transform in relation to each other
- **Delta Time Animation**: Smooth, frame-rate independent movement

## 🎮 Scene Description

The scene features:
- **Gudetama** (lazy egg character) - floating and rotating with a dreamy animation
- **Stitch** - moving character with bounce animations
- **Blue gradient background** - creating a cheerful, sky-like atmosphere
- **Interactive elements** with smooth transitions and movements

## 🛠️ Technical Implementation

### Requirements Implemented
- ✅ **Two Textured Objects** - Gudetama and Stitch characters with unique sprites
- ✅ **Movement** - Both characters translate across the screen with smooth animations  
- ✅ **Rotation** - Gudetama rotates while floating through the scene
- ✅ **Relative Positioning** - Characters move in relation to each other creating dynamic interactions

### Technologies Used
- **OpenGL** for rendering
- **SDL2** for window management
- **C++** programming language
- **Texture mapping** for character sprites
- **Delta time** for smooth animations

## 🚀 How to Run

1. Clone this repository
2. Open `Simple2D.xcodeproj` in Xcode
3. Ensure you have the necessary OpenGL and SDL2 frameworks linked
4. Build and run the project in Xcode
5. The application will launch displaying the animated 2D scene

## 📂 Project Structure

```
Simple2DScene/
├── assets/              # Asset folder containing textures
│   ├── gudetama.png     # Gudetama character sprite (148 KB)
│   └── stitch.png       # Stitch character sprite (316 KB)
├── Simple2D/            # Source code folder
├── Simple2D.xcodeproj   # Xcode project file
└── README.md           # Project documentation
```

## 📸 Screenshots

The main scene shows both characters in a vibrant blue environment with smooth animations and engaging visual effects.

![Simple 2D Scene Screenshot](Simple2DScene/Simple2DScene.png)

---------------------------------------------------------------

# Pong

A themed Pong clone set in an underwater world, featuring fish paddles and bubble balls for an immersive aquatic gaming experience.

## 🎮 Game Overview

This is a classic Pong game reimagined with an underwater theme. Players control fish paddles to hit a bubble ball back and forth in an aquatic environment. The game features both two-player and single-player modes with smooth animations and engaging underwater visuals.

![Underwater Pong Screenshot](pong/pong.png)

## 🌊 Theme & Visuals

- **Fish Paddles**: Players control orange and pink fish instead of traditional rectangular paddles
- **Bubble Ball**: A floating underwater bubble serves as the game ball
- **Aquatic Environment**: Underwater-themed background and visual effects
- **Win Screens**: Custom victory displays when a player wins
- **Textured Objects**: All game elements feature detailed underwater-themed textures

## ✨ Features

### Core Gameplay
- **Two-Player Mode**: Both players can control their fish using keyboard inputs
  - Player 1: W/S keys for up/down movement
  - Player 2: Arrow keys (Up/Down) for movement
- **Single-Player Mode**: Press 'T' to toggle AI-controlled opponent with automatic up-and-down movement
- **Multi-Ball Mode**: Press number keys to control the number of bubbles in play
  - Press '2': Two bubbles active simultaneously
  - Press '3': Three bubbles active simultaneously
- **Ball Physics**: Bubbles bounce realistically off paddles and walls
- **Boundary Collision**: Fish paddles are constrained to screen boundaries
- **Game Over Detection**: Game ends when bubble reaches left or right screen edges with victory screen display

### Technical Implementation
- **Delta Time Animation**: Smooth, frame-rate independent movement
- **Box-to-Box Collision**: Reliable collision detection between all game objects
- **Textured Rendering**: All objects use custom underwater-themed textures
- **Keyboard Input Handling**: Responsive controls for both players

## 🎯 Gameplay Mechanics

1. **Paddle Movement**: Control your fish paddle to intercept the bubble
2. **Ball Bouncing**: Bubble bounces off top/bottom walls and fish paddles
3. **Scoring**: Game ends when bubble passes a player's fish
4. **Mode Switching**: Toggle between 2-player and 1-player modes anytime
5. **Collision Physics**: Realistic bounce mechanics for engaging gameplay

## 🚀 How to Run

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/your-username/2D-Games.git
   cd 2D-Games/pong
   ```

2. **Open in Xcode**:
   - Open `PongProject.xcodeproj` in Xcode
   - Ensure you have the necessary OpenGL and SDL2 frameworks linked

3. **Build and Run**:
   - Press the "Play" button in Xcode or use Cmd+R
   - The game window will launch

4. **System Requirements**:
   - macOS with Xcode installed
   - OpenGL and SDL2 frameworks

## 🎮 How to Play

1. **Start the Game**: Launch the application to begin
2. **Choose Mode**: 
   - Two-player: Both players control their fish
   - Single-player: Press 'T' to enable AI opponent
3. **Controls**:
   - Player 1 (Left Fish): W (up) / S (down)
   - Player 2 (Right Fish): Up Arrow / Down Arrow
   - Multi-Ball: 2 (two bubbles) / 3 (three bubbles)
4. **Objective**: Keep the bubbles from passing your fish paddle
5. **Win Condition**: Game ends when bubble reaches screen edge

## 🎬 Gameplay Video

[Watch Gameplay Demo](https://github.com/user-attachments/assets/ab133e9e-93b6-41e1-b1ab-8e8a9b3d9037)

## 🛠️ Technical Details

- **Graphics**: OpenGL rendering with texture mapping
- **Input**: SDL2 keyboard event handling
- **Physics**: Custom collision detection and movement systems
- **Animation**: Delta time-based smooth animations
- **Architecture**: Object-oriented design with separate paddle and ball entities

## 📂 Project Structure

```
2D-Games/
├── pong/                    # Pong game folder
│   ├── assets/              # Game textures and sprites
│   │   ├── orange.png       # Orange fish paddle
│   │   ├── pink.png         # Pink fish paddle
│   │   ├── bubble.png       # Bubble ball
│   │   ├── orange-wins.png  # Orange player victory screen
│   │   ├── pink-wins.png    # Pink player victory screen
│   │   ├── game-over.png    # Game over screen
│   │   └── underwater.jpg   # Background texture
│   ├── PongProject.xcodeproj # Xcode project file
│   ├── PongProject/         # Source code folder
│   └── README.md           # Game documentation
└── README.md               # Main project documentation
```

----------------------------------------------------------

# Lunar Lander

A physics-based space landing game featuring realistic gravity simulation, fuel management, and precision piloting challenges.

## 🚀 Game Overview

This is a classic Lunar Lander game where players pilot a spacecraft through a challenging landing sequence. Navigate using thruster controls while managing limited fuel and fighting against gravity to achieve a successful landing on designated platforms.

![Lunar Lander Screenshot](lunar-lander/lunar-lander.png)

## 🌌 Theme & Visuals

- **Space Environment**: Atmospheric space-themed background and terrain
- **Spacecraft**: Detailed player-controlled lander with thruster effects
- **Landing Platforms**: Strategic landing zones for mission completion
- **Realistic Physics**: Gravity and momentum simulation for authentic flight dynamics
- **Mission UI**: Clear feedback for mission status and fuel levels

## ✨ Features

### Core Gameplay
- **Physics-Based Movement**: Realistic gravity affects the spacecraft continuously
- **Acceleration Controls**: 
  - Left/Right arrow keys for horizontal thrust
  - Thruster input changes acceleration, not direct velocity
  - Momentum and drift mechanics for realistic space flight
- **Fuel Management**: Limited fuel supply that depletes with thruster use
- **Mission Objectives**: Land safely on designated platforms to win
- **Collision Detection**: Crash detection for failed landing attempts

### Technical Implementation
- **Entity Class System**: Object-oriented design with reusable entity components
- **Fixed Time Steps**: Consistent physics simulation regardless of frame rate
- **Delta Time Animation**: Smooth, frame-rate independent movement
- **Acceleration-Based Physics**: Realistic momentum and drift mechanics
- **State Management**: Game state tracking for win/loss conditions

## 🎯 Gameplay Mechanics

1. **Gravity Physics**: Spacecraft continuously falls due to simulated lunar gravity
2. **Thruster Control**: Use arrow keys to apply acceleration forces
3. **Momentum System**: Ship drifts when thrusters are released
4. **Fuel Consumption**: Each thruster activation consumes precious fuel
5. **Landing Challenge**: Touch down gently on landing platforms to succeed
6. **Mission Failure**: Crashing into terrain or obstacles ends the mission

## 🚀 How to Run

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/your-username/2D-Games.git
   cd 2D-Games/lunar-lander
   ```

2. **Open in Xcode**:
   - Open `lunar-lander.xcodeproj` in Xcode
   - Ensure you have the necessary OpenGL and SDL2 frameworks linked

3. **Build and Run**:
   - Press the "Play" button in Xcode or use Cmd+R
   - The game window will launch

4. **System Requirements**:
   - macOS with Xcode installed
   - OpenGL and SDL2 frameworks

## 🎮 How to Play

1. **Start the Game**: Launch the application to begin your landing mission
2. **Controls**:
   - Left Arrow: Fire left thruster (move right)
   - Right Arrow: Fire right thruster (move left)
   - Monitor your fuel gauge carefully
3. **Objective**: 
   - Navigate to a landing platform
   - Land gently without crashing
   - Manage fuel consumption efficiently
4. **Win Condition**: Successfully touch down on a designated landing platform
5. **Lose Condition**: Crash into terrain or run out of fuel before landing

## 🎬 Gameplay Video

[Watch Gameplay Demo](your-video-link-here)

## 🛠️ Technical Details

- **Graphics**: OpenGL rendering with texture mapping
- **Physics**: Custom gravity and acceleration systems
- **Input**: SDL2 keyboard event handling for thruster controls
- **Architecture**: Entity-based design with modular components
- **Collision**: Precise collision detection for landing and crash scenarios
- **Game Logic**: State machine for mission success/failure handling

## 📂 Project Structure

```
2D-Games/
├── lunar-lander/            # Lunar Lander game folder
│   ├── lunar-lander-screenshot.png # Game screenshot
│   ├── assets/              # Game textures and sprites
│   │   ├── spacecraft.png   # Player ship sprite
│   │   ├── platform.png     # Landing platform texture
│   │   ├── background.png   # Space background
│   │   └── font.png         # UI text font
│   ├── lunar-lander.xcodeproj # Xcode project file
│   ├── lunar-lander/        # Source code folder
│   └── README.md           # Game documentation
└── README.md               # Main project documentation
```

## 🌟 Advanced Features

- **Fuel UI Display**: Real-time fuel gauge showing remaining thruster power
- **Moving Platforms**: Dynamic landing targets that add extra challenge
- **Physics Accuracy**: Realistic space flight simulation with momentum conservation

---

*Navigate the challenges of space and achieve a perfect lunar landing*


