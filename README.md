Milestone 3 Team 6 KTV

Milestone Feature Descriptions:

Required Features:

-Gameplay II (ALL):

-Behaviour Tree (FC): Implemented behaviour tree enemies. Enemy actions are conditional with processes as nodes. If any player is alive, the enemy will chase a player of their choosing. If any player is dead, the enemy will explode in randomized direction. 

-Swarm Behaviour (BZ): Implemented swarm behaviour enemies. Swarm will spawn in triplets surrounding the same location. Each update each swarm enemy will detect the closest swarm enemy, calculate the vector to the other enemy, reverse the x and y of that vector then normalize it. Finally we multiply swarm enemy's speed to this normalized directional vector to set its new velocity. The result is that each swarm will try to spread out as much as possible. Swarm enemmies attack the player via a projectile fireball attack, so the swarms benefit from spreading out because the enemy projectiles will then attack the players from many directions.

-Keyframe Animation (frame animation) (BZ): For assets with getting hit sprites, we animate the getting hit sprite during the asset's invincibility frame. These are the player wizard, enemyHunter and enemySwarm. Also animated different sprite for each of enemyHunter's state. Also implemented sprite sheet for wizard idle, attack and movement animations.

-Keyframe Animation (fragment shader) (LT):

-Articulated Motion (BS): Implemented a swinging sword attack via articulated motion. Instead of rotating the sword around its own frame, it will rotate with respect to the knight's (its wielder's) object frame. Swords behave similarly to projectiles with a few exceptions: They do more damage, do not despawn on contact, and can only be swung in four directions. The sword is meant to be a far more practical weapon in close-quarters, giving the knight certain advantanges over the wizard.

-Advanced Fragment Shader (LT):

-Save and Reload (RM):

-Story (JK):

Custom Features:

-Graphic Menu (10 points)(JK):

-Pre-game Cutscene (5 points)(JK):

Memory Management:

Memory and Performance profiling (BS): Using built-in profiling tools in Visual Studio, there were a few things found that were sources of inefficiency (See reports/PPR.pdf for more details). The first is unnecessarily updating the title every step, which is elaborated below. Second was a memory leak found in ai_system.cpp.

Updating Window Title Improvement (BS): One of the improvements made was to only update the window title when necessary (Level switch, changes to player HP and money). Improvement described in the report mentioned above. 

EnemyChase Memory and Performance Improvement (LT): 

Bug and Feature Fixes: 

Pathfinding Enemy fix (FC): Changed the way the enemies are updated as to save memory. The enemies are now changed, instead of every update following the entire BFS path, each new velocity will be changed every update time, still using BFS.

Adaptive Resolution fix (BZ): Changed from prompting user to select resolution to auto detecting user's primary monitor resolution and setting the game resolution accordingly.

Knight Animation Sliding fix (BZ): Stopping knight's movement animation was tied to releasing the directional movement button. Fixed so that it is tied to player's velocity being 0 instead.

Graphics Assets (JK):

Revised Project Proposal:

Feature Changes:

-We are pushing audio feedback to milestone 4 and doing keyframe animation instead.

-We are bringing pre-game cutscene (5 custom feature points) to this milestone.

-We are implementing custom feature graphic menu which we believe is worth 10 custom feature points. 

-We plan to implement heads-up display (HUD) for 5 custom feature points in milestone 4.

-We plan to implement a final boss for 10 custom feature points in milestone 4.

