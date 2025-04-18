# Grand Wizard Tournament

An educational roguelike game that teaches programming concepts through an engaging magical combat system

## 📖 Description

Grand Wizard Tournament is an innovative educational game that teaches programming concepts through the fantasy lens of spellcrafting. Players navigate a procedurally generated magical labyrinth, battling waves of enemies while learning fundamental programming concepts through a visual spell-creation system.

In the world of Grand Wizard Tournament, the discovery of "Whispering Oracles" (a metaphor for AI tools) has made spellcasting too easy, with young wizards asking for spells without understanding the underlying principles. The Grand Wizard Council has created this tournament where Oracles don't function, forcing wizards to rely on their foundational knowledge.

The game seamlessly integrates computational thinking, algorithmic reasoning, and programming principles into the gameplay mechanics. As players create increasingly complex spells, they naturally absorb these concepts in an engaging and entertaining environment.

## 🌟 Key Features

- **Visual Spell-Crafting Editor**: Design spells using an intuitive node-based programming interface inspired by Scratch and Unreal Engine Blueprints
- **Procedurally Generated Labyrinth**: Explore a dynamic 3×3×3 magical maze with rooms that can be rotated and reconfigured
- **Progressive Learning System**: Master programming concepts through a natural difficulty curve from basic sequencing to complex logic and loops
- **Diverse Room Types**: Discover combat chambers, treasure vaults, magical shops, and puzzle rooms
- **Comprehensive Equipment System**: Collect wands, hats, and robes that enhance your wizard's abilities
- **Educational Analytics**: Track your progress in mastering programming concepts with detailed performance metrics

## 🧙‍♂️ Educational Concepts

Grand Wizard Tournament teaches fundamental programming concepts through gameplay mechanics:

- **Sequential Logic**: Arrange spell components in order to create magical effects
- **Conditional Statements**: Implement IF-THEN-ELSE logic using condition nodes for situationally adaptive spells
- **Variables & Data Management**: Store and manipulate spell properties through variable nodes
- **Loops & Iteration**: Create sustained or repeated magical effects using loop constructs
- **Functional Abstraction**: Build reusable spell modules that combine into more complex spells

## 🛠️ Technical Implementation

The game is built with a modular architecture using Unreal Engine and C++:

- **Spell System**: Node-based visual programming interface with spell execution context
- **Level Generation**: Procedural room generation with different room types and connectivity
- **Enemy AI**: Intelligent enemy behavior with pathfinding and adaptive combat strategies
- **Progression System**: Player advancement through experience and skill improvement
- **Educational Tracking**: Analytics system to monitor learning progress

## 🔄 Core Game Loop

1. Navigate procedurally generated rooms
2. Collect spell components and items
3. Craft and modify spells using visual programming
4. Complete wave-specific objectives
5. Progress through increasingly challenging waves
6. Compete for high scores and rankings

## 🚧 Development Status

This project is currently in active development.

## 🧪 Technologies Used

- **Engine**: Unreal Engine 5
- **Programming Language**: C++
- **Target Platforms**: Windows, Linux

## 🎮 Gameplay Systems

### 🪄 Spell-Crafting System: "The Grimoire"
### Foundational Principles

1. **Node Categories**: All spell components fall into six fundamental types that map to programming concepts:
   - **Magic Nodes**: Core spell functionality (Primary functions)
   - **Trigger Nodes**: Event detection (Event handlers)
   - **Effect Nodes**: Result production (Output/actions)
   - **Condition Nodes**: Branch control (Conditional logic)
   - **Variable Nodes**: Data management (Variables/state)
   - **Flow Nodes**: Execution control (Loops/control flow)

2. **Connection System**: Nodes connect through a directed graph system with:
   - **Input Ports**: Accept incoming connections with defined data types
   - **Output Ports**: Send signals or data to connected nodes
   - **Port Compatibility**: Enforces logical connection rules between node types

3. **Execution Model**: Spells execute through a signal propagation system:
   - Signal flow begins at Trigger nodes
   - Travels through the connected node graph
   - Follows conditional branches based on runtime evaluation
   - Allows for parallel execution paths

## 📜 License

This project is licensed under Creative Commons Attribution-NonCommercial 4.0 International License - see the [LICENSE.md](LICENSE.md) file for details.