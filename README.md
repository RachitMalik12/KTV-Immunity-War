Milestone 2 Team 6 KTV
Grace Days Used: 1

Milestone Feature Descriptions:
Gameplay 1 (ALL): We load the map and spawn all the enemies on screen and the players have to kill them all to advance to the next level. The stats of players and enemies are implemented. Stats are damage, max hp, movement speed and projectile speed/firing speed.

Adaptive Resolution (BZ): We offer player a choice between 3 resolution when the game is launched. 2400 x 1600, 1200 x 800 and 600 x 400. Everything in game that involve position and velocity is scaled accordingly. 

State Machine (BZ): Implemented a state machine enemy "hunter". Hunter has three states: wondering mode, hunting mode and fleeing mode. In wondering mode, hunter wonders randomly until it gets close enough to a player. Then in hunting mode it chases after the player. Then finally, after its HP gets low, it gets into fleeing mode, which runs horizontally across the map at a high speed.

Revised Project Proposal:
Feature changes:
-Swarm behaviour was pushed to milestone 3 in favour of library integration for this milestone
