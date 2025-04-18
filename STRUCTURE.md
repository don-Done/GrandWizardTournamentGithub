## Core Game Framework

AGWTGameMode - Controls game flow and wave progression
AGWTGameState - Tracks global game state information
UGWTObjective - Defines gameplay objectives for each wave

## Spell System

UGWTSpellNode - Base class for all spell nodes
UGWTMagicNode - Core spell functionality node
UGWTTriggerNode - Event-based trigger node
UGWTEffectNode - Action-producing effect node
UGWTConditionNode - Logic control node
UGWTVariableNode - Data storage node
UGWTFlowNode - Loop and branch flow control node
UGWTSpell - Complete spell composed of nodes
UGWTSpellExecutionContext - Manages spell execution state
UGWTGrimoire - Manages player's spell collection

## Level Generation System

AGWTLevelGenerator - Generates the procedural labyrinth
AGWTRoom - Represents a single room in the labyrinth

## Character System

AGWTCharacter - Base character class with stats
AGWTPlayerCharacter - Player-specific character implementation
AGWTPlayerController - Handles player input and UI

## Enemy System

AGWTEnemyCharacter - Base class for all enemy types
UGWTEnemySpawner - Manages enemy spawning


## UI System

UGWTHUDWidget - Gameplay heads-up display
UGWTSpellEditorWidget - Visual spell programming interface
UGWTNodeWidget - Visual representation of a spell node
UGWTMiniMapWidget - Displays labyrinth map

## Item System

UGWTItem - Base class for all items
UGWTEquipment - Base class for equipment
UGWTWand - Wand equipment type
UGWTRobe - Robe equipment type
UGWTHat - Hat equipment type
UGWTConsumable - Items with temporary effects
UGWTSpellComponent - Items that unlock spell nodes
UGWTInventory - Manages player's items

## Educational System

UGWTEducationalTracker - Tracks programming concept understanding
UGWTTutorialManager - Manages tutorial sequences

## Progression System

UGWTPlayerProgression - Manages player level and unlocks
UGWTRankingSystem - Handles competitive rankings