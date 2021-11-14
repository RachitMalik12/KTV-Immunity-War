Milestone 3 Team 6 KTV

Milestone Feature Descriptions:
Required Features:
-Gameplay II (ALL):

-Behaviour Tree (FC):

-Swarm Behaviour (BZ): Implemented swarm behaviour enemies. Swarm will spawn in triplets surrounding the same location. Each update each swarm enemy will detect the closest swarm enemy, calculate the vector to the other enemy, reverse the x and y of that vector then normalize it. Finally we multiply swarm enemy's speed to this normalized directional vector to set its new velocity. The result is that each swarm will try to spread out as much as possible. Swarm enemmies attack the player via a projectile fireball attack, so the swarms benefit from spreading out because the enemy projectiles will then attack the players from many directions.

-Keyframe animation (LT):

-Articulated motion (BS):

-Advanced fragment shader (LT):

-Save and Reload (RM):

-Story (JK):

Custom Features:
-Graphic Menu (10 points)(JK): 

-Pre-game cutscene (5 points)(JK):

Memory Management:
Memory and Performance profiling (BS):

Updating Window Title Improvement (BS):

Bug and feature fixes:
Pathfinding enemy fix (FC):

adaptive resolution fix (BZ): Changed from prompting user to select resolution to auto detecting user's primary monitor resolution and setting the game resolution accordingly.

Animation Sliding fix:

Graphics assets (JK):

Revised Project Proposal:
feature changes:
-We are bringing pre-game cutscene (5 custom feature points) to this milestone.
-We are implementing custom feature graphic menu which we believe is worth 10 custom feature points.
-We plan to implement heads-up display (HUD) for 5 custom feature points in milestone 4.
-We plan to implement a final boss for 10 custom feature points in milestone 4.
