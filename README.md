Milestone 2 Team 6 KTV
Grace Days Used: 1

Milestone Feature Descriptions:
Gameplay 1 (ALL): We load the map and spawn all the enemies on screen and the players have to kill them all to advance to the next level. The stats of players and enemies are implemented. Stats are damage, max hp, movement speed and projectile speed/firing speed.

Adaptive Resolution (BZ): We offer player a choice between 3 resolution when the game is launched. 2400 x 1600, 1200 x 800 and 600 x 400. Everything in game that involve position and velocity is scaled accordingly. 

State Machine (BZ): Implemented a state machine enemy "hunter". Hunter has three states: wondering mode, hunting mode and fleeing mode. In wondering mode, hunter wonders randomly until it gets close enough to a player. Then in hunting mode it chases after the player. Then finally, after its HP gets low, it gets into fleeing mode, which runs horizontally across the map at a high speed.

Precise Collision (BS): Implemented precise collision detection by means of a coarse mesh "hitbox" overlayed on a sprite. Current detection works by checking if one entity's hitbox collides with another entity's bounding box. Currently, the player, all enemies, the projectile and trees have mesh hitboxes.

Simple Path Finding (FC): Implemented a BFS enemy "bacteria". The enemy updates it's path to what the current player (or one of the players, if there are two players) position is. It calculates it's path using BFS with a queue, where the map is separated into an 8x8 grid as "nodes", using it's predecessor nodes to find a path to the player it is hunting. 

External library integration (RM): Integrated the library https://github.com/open-source-parsers/jsoncpp to parse json files. Modified the cmake and made sure 
it runs on all devices of our team. 

Level loading (RM): Implemented level loading mechanism that loads an arbitrary number of levels from data/levels json file (level_design.json). 
The level has information for the number, kind of enemies, positions of enemies, the player positions and the barrier/block positions that is all customizable. 
JSON parsing is done through the library JSONcpp integrated in the external lib. feature. There are 3 levels in the game with varying levels of difficulty 
and the player progresses to the next level when they kill all enemies on screen and if they stay alive.
There is also a dev mode where you can reload a level or change to a specific level using the keys 1, 2 or 3.

Revised Project Proposal:
Feature changes:
-Swarm behaviour was pushed to milestone 3 in favour of library integration for this milestone
