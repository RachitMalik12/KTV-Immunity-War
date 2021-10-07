Milestone 1 Team 6 KTV

Milestone Feature Descriptions:
Random/coded action (LT): Enemies will move at a set pattern. For one enemy type, once the player character move closer to this enemy, it 
will try to move away from the player character, thus changing its movement pattern.

Textured geometry (JK): Custom sprites drawn by JK is loaded in as various tree blocks and the player character. The player character will
face the correct direction when moving, which means loading the correct sprite or doing the correct transformation, in this case reflection.

Debugging graphics (BS): A another debug mode that draws a rectangle around the sprites is implemented. We can combine this with the original
red cross for a better visualization of sprites.

Observer pattern (FC): For one enemy type, once it collides with a player, both the player and the enemy will bounce back a short distance.

Keyboard control for player 1 (RM): WSAD for movement and TFGH for 4 directional projectile firing.

Camera control (FC): Camera follows the player when the player moves from battle arena to shop.

Basic collision detection (BZ): Implemented walls and blocks on map that will detect objects in motion moving into them thus stopping the objects'
movement. Collision detection is using the same as A1 template for now for blocks and wall, although they work slightly differently. Better collision
physics will be introduced in the next milestone (more on this in the revised proposal).

Collision Resolution (LT): Projectiles hitting the wall will disappear. Projectile hitting the enemy will kill the enemy and make the projectile
disappear. Enemy hitting player will bounce both of them back (related to observer pattern).

Graphics assets (JK): JK is drawing our assets and she will continue to do so throughout the semester.

Lag mitgation (ALL): We tested playing the game with zoom screen share and the lag is around half a second. For now we see this as acceptable.

Crash free (ALL): We encountered a few build problem with one local project_path.hpp unintentionally uploaded to github. Problem was fixed with
paired debugging by BZ and RM. We made sure other members don't have the same problems during our zoom meetings. We also make sure a build works
before we merge a pull request into dev branch.

Simple currency System (RM): Killing enemies grants the player money which then can be spent to buy power ups in the shop. The shop will be expanded
in future milestones.

Co-op second player (BS): The second player is controlled with mouse to not conflict with GLFW keyboard control. One mouse click to move the character
and the other mouse click to fire projectiles at the direction the mouse is at.

Organization (ALL): Organization of the group was done very early into the course. Our group of 6 was formed almost immediately and we have been 
communicating on slack and messenger ever since. Activities such as game pitch, proposal and milestones planning were done together in zoom meetings. 
We have a scheduled weekly meeting every Sunday at 8PM and extra meetings when we have a deadline. We have good github practices such as having a dev
branch and having people doing their individual work on their own branches. We must have at least one approval before a pull request is merged and we
do group review during our zoom meetings. We also assign task by using github issues which clearly indicates who does what. We are flexible with our 
tasks assignments because our effective communications. Constant daily communications are done on slack and messenger combined with the frequent zoom
meetings means that whenever one person wants to change their task, whether to ask for help or ask for more work he or she can do with ease.

Revised Project Proposal:
Regarding TA feedback:
-GLFW conflict: We were told that having two players both using the keyboard would have problem with GLFW. Our solution is to have one player controlled
with the keyboard and the other player with the mouse.
-Particle System: We highly underestimated the particle system and we decided that we won't implement it in our game, thus dropping it from M4.
-Keyboard rebind and controller support: We were told these two features are were too simple to do to get points, thus we are dropping them from M4.
-To account for the extra 10 points from organization we are dropping parallax scrolling background from our game entirely.

Feature changes:
We discovered that precise collision is too difficult and beyond our knowledge currently. We are moving precise collision from M1 to M2. We are instead
doing co-op second player in M1, moved from M3. We are moving save and reload from M2 to M3.

For revised milestones, please see MilestoneSubmissionFormM1.pdf.
