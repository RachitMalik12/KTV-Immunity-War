Milestone 3 Team 6 KTV

Milestone Feature Descriptions:

Required Features:

-Gameplay II (ALL): Shop fully implemented with power up's (RM). Level transition and shop visit phase implemented (RM). Two more levels added with two new enemies. Main characters fully animated. Players and enemies implemented with hit handling. Background texture implemented. 

-Behaviour Tree (FC): Implemented behaviour tree enemies. Enemy actions are conditional with processes as nodes. If any player is alive, the enemy will chase a player of their choosing. If any player is dead, the enemy will explode in randomized direction. Added an explanation diagram for the implementation of behaviour tree enemy and the BTNodes for future expansion of these functions. (see reports folder)

-Swarm Behaviour (BZ): Implemented swarm behaviour enemies. Swarm will spawn in triplets surrounding the same location. Each update each swarm enemy will detect the closest swarm enemy, calculate the vector to the other enemy, reverse the x and y of that vector then normalize it. Finally we multiply swarm enemy's speed to this normalized directional vector to set its new velocity. The result is that each swarm will try to spread out as much as possible. Swarm enemmies attack the player via a projectile fireball attack, so the swarms benefit from spreading out because the enemy projectiles will then attack the players from many directions.

-Keyframe Animation (frame animation) (BZ): For assets with getting hit sprites, we animate the getting hit sprite during the asset's invincibility frame. These are the player wizard, enemyHunter and enemySwarm. Also animated different sprite for each of enemyHunter's state. Also implemented sprite sheet for wizard idle, attack and movement animations.

-Keyframe Animation (fragment shader) (LT):

-Articulated Motion (BS): Implemented a swinging sword attack via articulated motion. Instead of rotating the sword around its own frame, it will rotate with respect to the knight's (its wielder's) object frame. Swords behave similarly to projectiles with a few exceptions: They do more damage, do not despawn on contact, and can only be swung in four directions. The sword is meant to be a far more practical weapon in close-quarters, giving the knight certain advantanges over the wizard.

-Advanced Fragment Shader (LT):

-Save and Reload (RM): Added a feature to save and load game state. It will save the player stats and current level in json. When the player leaves the game and loads it will resume with the level they saved on and their player stats.

-Story (JK): Created a storyline for the game to setup for the "battle" that starts within. The writing and planning for it (in full) can be found here: https://jsminea.notion.site/Story-bcbd3cb2b5ba442f80d6af9073b6de5b

Custom Features:

-Graphic Menu (10 points)(JK): Created 2 different menus (as well as the custom assets for buttons and background). One for when the game first starts, the main menu, with 4 options: Load, 1P, 2P, and Help (which displays ontop of the menu). The second menu is in-game, which can be toggled by the ESC key, and displays both in the shop and in the enemy rooms. This menu has 5 options: 2P on/off (which toggles 2 player mode), Save, Load, Restart, Help. Also refactored loading and saving code, as well as added to mouse control of the wizard to limit rendering issues. Changed help positioning to render in shop and in the enemy's room as well, as well as made functions for the future, for any in shop positioning. Button positions and menu were coded with positions relative to the center of the screen. Did not use any pre-built libraries, created our own menu system.

-Pre-game Cutscene (5 points)(JK): Created 6 different cutscene graphics and routed the story progression to click and space key. The cutscene will only play at the beginning of the game, so it will not repeat itself after restarting a level or the game (unless exited from game).

Memory Management:

Memory and Performance profiling (BS): Using built-in profiling tools in Visual Studio, there were a few things found that were sources of inefficiency (See reports/PPR.pdf for more details). The first is unnecessarily updating the title every step, which is elaborated below. Second was a memory leak found in ai_system.cpp.

Updating Window Title Improvement (BS): One of the improvements made was to only update the window title when necessary (Level switch, changes to player HP and money). Improvement described in the report mentioned above. 

EnemyChase Memory and Performance Improvement (BZ): Added a timer to update AI and eliminated the memory leak.

Bug and Feature Fixes: 

Pathfinding Enemy fix (FC): Changed the way the enemies are updated as to save memory. The enemies are now changed, instead of every update following the entire BFS path, each new velocity will be changed every update time, still using BFS.

Adaptive Resolution fix (BZ): Changed from prompting user to select resolution to auto detecting user's primary monitor resolution and setting the game resolution accordingly.

Knight Animation Sliding fix (BZ): Stopping knight's movement animation was tied to releasing the directional movement button. Fixed so that it is tied to player's velocity being 0 instead.

Player Rendering Over Other Assets fix + Help positioning (JK): Player was rendering over other items like Help and Menu, so fixed rendering order and added flags to make sure that players were not rendering over assets they were not supposed to. Also adjusted help positioning to display in the correct positions when in shop.

Graphics Assets (JK): Created background asset, coin asset, numbered text (monospace) asset for later use with the shop, 6 cutscenes (full screen), main menu and in game menu designs/assets for buttons. These were all custom made and not taken from the web.

Revised Project Proposal:

Feature Changes:

-We are pushing audio feedback to milestone 4 and doing keyframe animation instead.

-We are bringing pre-game cutscene (5 custom feature points) to this milestone.

-We are implementing custom feature graphic menu which we believe is worth 10 custom feature points. 

-We plan to implement heads-up display (HUD) for 5 custom feature points in milestone 4.

-We plan to implement a final boss for 10 custom feature points in milestone 4.

