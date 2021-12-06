**Milestone 4 Team 6 KTV**

**Milestone Feature Descriptions:**  
Required Features:  
-Gameplay III (ALL): Added 3 more levels including the final boss level. Added game over screen to complete the story. Added tutorial level. Added 5 new types of enemies. Rebalanced gameplay to be chanllenging but fair. 

-Delay agnostic design (RM & LT): Documented data related to zoom delay and from cross play session to show the game is delay agnostic and the lag is minimum. 
Documented changes to account for the potential lag faced by some users: https://docs.google.com/document/d/1GNYXo2HXgTYso2Dm1yZy9DnFG0UcxIwP-YOs_e9Npxk/edit?usp=sharing

-Advanced Decision-Making (FC): The algorithm implemented calculates H Cost (distance between the enemy and end goal) and a G Cost (distance between the enemy and next position). What happens is at the current position of the enemy, they will calculate the G+H cost of the positions up/left/right/down of it. whichever one has the lowest G+H cost, we will add to the queue. After we finish calculating and adding until we get to the player's position, we will slowly push all positions as positions to move to, creating a path to the player from the enemy that the enemy follows.

-Cooperative Planning (LT): Planning the action of 2 parts of 1 type of enemy (the head part and the tail part) towardws a common goal of killing the player(s). The communication is non-trivial because both the head and tail will have to stay a certain distance from each other while the head runs away from the player and the tail chases the player. This set of action is triggered by the tail checking if the head is within a distance from the player (player 1 if player 1 is alive, otherwise player 2). If the tail sees that the head is far from the player(s), then the head and tail will just move randomly. Also, the player needs to kill the head in order to kill the head and the tail. This part is tricky because the player may think that they need to kill the tail which is chasing the player but what they do not realize is that the tail's hp is very high whereas the head's hp is more realistic to kill.

-Advanced Vertex/Geometry Shader (BZ): All effects are added using vertex shaders. Added shop items bouncing effect for flavour. Added player and enemy hit shaking effect. Added enemy knock back effect that work for both sword and waterball. Added death animation for enemies getting cut in half by sword. This was done by adding two more vertices which separated the two triangles in the quad so we can animate them getting separated. Added enemy death animation shrink. This was done by separating the scale matrix from the transform matrix and creating a new scale matrix in the vertex shader. Added player animation flopping to the side. This was done by separating the rotation matrix from the transform matrix and forming a new rotation matrix in the vertex shader.

-Tutorial (RM):
Tutorial level to help players learn about movement and attack controls. We spawn 2 enemies that the player can practice with. Then we guide them into the shop 
where they can proceed to purchase powerups with guides on the screen for what each powerup means. The powerups will persist for proceeding levels, so they need 
to make a good choice. From a technical perspective, this required rendering text on screen for which we used a sprite sheet of capital letters and small letters. 
I also had to add some logic to render a sentence on screen correctly to ensure the position and scaling was correct for all resolutions. In addition, a timer to ensure the 
tutorial enemies appear after exactly 7 secs was implemented. 

-Game Balance (BS): Initially, we made some analysis based off of the example given in class, where we balanced money rewards with respect to health. However, it was overengineered, as well as fundamentally flawed; Because the cost of powerups remains constant, it didn't really make much sense to award more money, and this problem was reflected in practical testing. Thus, the idea was scrapped. Rather, we balanced enemy health with respect to level, where the goal was to make subsequent levels linearly more difficult. Details on the new, final analysis is included in the PDF. However, we have also included the initial analysis as well in another PDF.

-Audio Feedback (BS): Audio feedback for menu click, player damage, enemy damage, attacks, movement, BGM, purchase, level start/end.

Custom Features:  
-Final Boss Level (10 points custom) (JK): Added 3 enemy types on final level, the boss hand, the final boss as well as its minions. There are also 3 stages to the boss fight and restricted movement on the map to make dodging and maneuvering a little harder. The first stage, the enemies are invisible and shoot at the players (choosing one randomly) this is using the swarm logic, found in M3. In the second level, the hand moves back and forth, firing projectiles towards the player as well at a higher rate. On the final level, the boss appears at the top of the screen and shoots projectile at player. Added new sprites for the invisible enemies for a little bit of pizzazz, as well as used the blocks for level design, so players can strategically use it to block proejctiles.

-HUD (5 points custom) (BZ): Added HP and money count on the top of the screen for P1 and P2. Added shop price numbers for power ups.

Memory Management:  
-Memory and Performance Profiling (BS): A similar procedure to M3 for resource diagnostics was undertaken. No unusual nor problematic behavior of resource usage were found.

Graphic Assets (JK):  Created background and new sprites, custom for the boss/final level. Sprites include, original minion, hand, boss, final background, coins, leave piles.

Bug and Feature Fixes:  
-Level Transition Fix (LT): Removed the screen darken factor because there were quite a few bugs introduced by this feature that we realized. Also, it was more realistic to have this screen darken factor when we didn't have the shop.

-Sword Swing Fix (BZ): Fixed sword swing's angular velocity calculation so it doesn't over calculate and waste cpu usage.

Revised Project Proposal:  
Feature Changes:  
-We removed light from milestone 4

