# Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

(C) 2021 - 2022 by Daniel Brendel

**Version**: 1.0\
**Contact**: dbrendel1988(at)gmail(dot)com\
**GitHub**: https://github.com/danielbrendel<br/>

Released under the MIT license

## Description
This software is a game engine which is meant to be used to create your own
2D singleplayer top-down games. Therefore it provides an interface to let you
implement your game via AngelScript. The engine can be used to create your main
game and also load mods (both are refered to as packages). It supports Steam 
integration in order to publish your game on Steam and use the Steam Workshop 
for downloading new game mods. 
The goal of a game map is to go into the portal to go to the next map or finish
the game.

## Technical aspects
Each game or mod are located as a package in the packages directory. A package consists
of entities, maps, graphics, sounds and definition files. When loading a map you can define
map specific information such as descriptions, required map entities, solid sprites (walls)
and spawn entities initially. 
For each package their is the player definition script file which defines the player behaviour.
Of course you can define additional custom entities in different scripts. Scripts which are used
to be spawned via the map definition file need to define a special callback function which is used
to spawn the entity in the map. All other entities can be spawned dynamically via other entities 
(e.g. weapon bullets, temporary sprites, etc).
For a list of API functions, class interface layouts and more about scripting please refer to the 
API documentation.