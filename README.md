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
