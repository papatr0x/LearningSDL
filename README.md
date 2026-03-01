# LEngine

LEngine is a basic 2D game engine built on top of **SDL3**. This project was created as a personal learning exercise to become familiar with the SDL3 library and engine development.

## Goal
The primary objective of LEngine is to provide a simple, educational implementation of a game engine. It was originally designed to get familiar with SDL3, but I believe it can be helpful for those who are starting in game development and want to understand how an engine works "under the hood" without getting lost in the complex source code of professional, production-grade engines.

## Features
LEngine implements several core systems commonly found in game engines:

- **Component-Based Architecture**: Uses an `Object-Component` system to manage game entities and their behaviors.
- **2D Rendering System**: 
  - Texture management via `TextureManager`.
  - Sprite rendering with support for pivots, offsets, and rotation.
  - Multi-layer rendering support.
  - Screen-space vs. World-space rendering (for UI vs Game objects).
- **Input System**: Comprehensive support for:
  - Keyboard (Key states and events).
  - Mouse (Position, delta, and buttons).
  - Gamepad (Buttons and analog axes).
- **Collision System**:
  - AABB (Axis-Aligned Bounding Box) collision detection.
  - Broad-phase optimization (Sort and Prune).
  - Collision events: `onEnter`, `onStay`, and `onExit`.
- **Animation System**: Frame-based sprite animations via `AnimatorComponent`.
- **Audio System**: Support for both background Music and Sound Effects (SFX) using `SDL_mixer`.
- **Text Rendering**: Support for TrueType fonts (TTF) via `TextComponent`.
- **Scene Management**: 
  - `SceneOrchestrator` for managing multiple scenes.
  - Support for scene transitions and intermediate "loading" scenes.
- **Fixed Timestep Update**: Supports fixed-interval updates (`updatePhysics`) to ensure deterministic behavior, facilitating integration with physics engines like **Jolt**.
- **Camera System**: Simple camera support for world navigation.
- **Scripting**: Logic can be attached to objects using `ScriptComponent`.

## Demo Application
The project includes a sample application (located in the `game/` directory) that exercises and demonstrates most of the engine's features, including scene transitions, player movement, collisions, and audio.

## Platform Support
LEngine has been tested on:
- **macOS** (AppleClang)
- **Windows** (Visual Studio 2022 Professional)

## Requirements
- SDL3
- SDL3_mixer
- SDL3_ttf
- CMake 3.20+
- A C++20 compatible compiler

