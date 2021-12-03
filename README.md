**Milestone 4 Team 6 KTV**

**Milestone Feature Descriptions:**  
Required Features:  
-Gameplay III (ALL):

-Advanced Decision-Making (FC):

-Cooperative Planning (LT):

-Advanced Vertex/Geometry Shader (BZ): All effects are added using vertex shaders. Added shop items bouncing effect for flavour. Added player and enemy hit shaking effect. Added enemy knock back effect that work for both sword and waterball. Added death animation for enemies getting cut in half by sword. This was done by adding two more vertices which separated the two triangles in the quad so we can animate them getting separated. Added enemy death animation shrink. This was done by separating the scale matrix from the transform matrix and creating a new scale matrix in the vertex shader. Added player animation flopping to the side. This was done by separating the rotation matrix from the transform matrix and forming a new rotation matrix in the vertex shader.

-Tutorial (RM):
Tutorial level to help players learn about movement and attack controls. We spawn 2 enemies that the player can practice with. Then we guide them into the shop 
where they can proceed to purchase powerups with guides on the screen for what each powerup means. The powerups will persist for proceeding levels, so they need 
to make a good choice. From a technical perspective, this required rendering text on screen for which we used a sprite sheet of capital letters and small letters. 
I also had to add some logic to render a sentence on screen correctly to ensure the position and scaling was correct for all resolutions. In addition, a timer to ensure the 
tutorial enemies appear after exactly 7 secs was implemented. 

-Game Balance (BS):

-Audio Feedback (BS):

Custom Features:  
-Final Boss Level (10 points custom) (JK): Added 2 enemy types on final level, the final boss as well as its minions.

-HUD (5 points custom) (BZ): Added HP and money count on the top of the screen for P1 and P2. Added shop price numbers for power ups.
Memory Management:  
-Memory and Performance Profiling (BS):

Graphic Assets (JK):  Created background and new sprites, custom for the boss/final level.

Bug and Feature Fixes:  
-Level Transition Fix (LT):

-Menu Lighting Fix (LT):

-Save and Load Scaling Fix (RM):

Revised Project Proposal:  
Feature Changes:  
-We removed light from milestone 4

