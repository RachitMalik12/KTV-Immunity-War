#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"
#include <vector>

// These are hard coded to the dimensions of the entity texture
const float WIZARD_BB_WIDTH = 0.08f * 1231.f;
const float WIZARD_BB_HEIGHT = 0.08f * 1767.f;
const float KNIGHT_BB_WIDTH = 2.2f * 64.f;
const float KNIGHT_BB_HEIGHT = 2.2f * 64.f;
const float SWORD_BB_WIDTH = 3.1f * 64.f;
const float SWORD_BB_HEIGHT = 3.1f * 23.f;
const float BLOCK_BB_WIDTH = 0.09f * 871.f;
const float BLOCK_BB_HEIGHT = 0.09f * 1549.f;
const float WATERBALL_BB_WIDTH = 0.04f * 1954.f;
const float WATERBALL_BB_HEIGHT = 0.04f * 1602.f;
const float FIREBALL_BB_WIDTH = 0.15f * 328.f;
const float FIREBALL_BB_HEIGHT = 0.15f * 328.f;
const float ENEMYBLOB_BB_HEIGHT = 0.3f * 250.f;
const float ENEMYBLOB_BB_WIDTH = 0.3f * 250.f;
const float ENEMYRUN_BB_HEIGHT = 0.3f * 240.f;
const float ENEMYRUN_BB_WIDTH = 0.3f * 240.f;
const float ENEMYHUNTER_BB_HEIGHT = 0.11f * 691.f;
const float ENEMYHUNTER_BB_WIDTH = 0.11f * 582.f;
const float ENEMYBACTERIA_BB_HEIGHT = 0.11f * 691.f;
const float ENEMYBACTERIA_BB_WIDTH = 0.11f * 800.f;
const float ENEMYGERM_BB_HEIGHT = 0.11f * 691.f;
const float ENEMYGERM_BB_WIDTH = 0.11f * 800.f;
const float ENEMYASTAR_BB_HEIGHT = 0.11f * 550.f;
const float ENEMYASTAR_BB_WIDTH = 0.11f * 700.f;
const float ENEMYCHASE_BB_HEIGHT = 0.05f * 1024.f;
const float ENEMYCHASE_BB_WIDTH = 0.05f * 1258.f;
const float ENEMYMINION_BB_WH = 0.1f*820.f;
const float HELP_BB_WIDTH = 800.f;
const float HELP_BB_HEIGHT = 400.f;
const float STORY_BB_WIDTH = 0.5*2388.f;
const float STORY_BB_HEIGHT = 0.5*1668.f;
const float ENEMYSWARM_BB_WIDTH = 0.14 * 553.f;
const float ENEMYSWARM_BB_HEIGHT = 0.14 * 411.f;
const float ENEMYHEAD_BB_WIDTH = 0.2 * 501.f;
const float ENEMYHEAD_BB_HEIGHT = 0.2 * 450.f;
const float ENEMYTAIL_BB_WIDTH = 0.25 * 420.f;
const float ENEMYTAIL_BB_HEIGHT = 0.25 * 420.f;
const float DAMAGE_POWERUP_WIDTH = 0.0384f * 1954.f;
const float DAMAGE_POWERUP_HEIGHT = 0.0468f * 1602.f;
const float MOVEMENT_POWERUP_WIDTH = 0.0366f * 2048.f; 
const float MOVEMENT_POWERUP_HEIGHT = 0.0366f * 2048.f;
const float HP_POWERUP_WIDTH = 0.0366f * 2048.f;
const float HP_POWERUP_HEIGHT = 0.0366f * 2048.f;
const float ATTACK_POWERUP_WIDTH = 0.0615f * 1218.f; 
const float ATTACK_POWERUP_HEIGHT = 0.05112f * 1467.f; 
const float BACKGROUND_BB_WIDTH = 1200.f;
const float BACKGROUND_BB_HEIGHT = 1600.f;
const float NUMBER_BB_WIDTH = 0.6 * 69.7;
const float NUMBER_BB_HEIGHT = 0.6 * 85.f;
const float SMALLLETTER_BB_WIDTH = 0.329 * 127.f;
const float SMALLLETTER_BB_HEIGHT = 0.266 * 192.f;
const float CAPSLETTER_BB_WIDTH = 0.3216 * 128.f;
const float CAPSLETTER_BB_HEIGHT = 0.23103 * 221.f; 
const float HUD_HP_BB_WIDTH = 0.0249 * 2048.f;
const float HUD_HP_BB_HEIGHT = 0.0249 * 2048.f;
const float HUD_COIN_BB_WIDTH = 0.0249 * 2048.f;
const float HUD_COIN_BB_HEIGHT = 0.0249 * 2048.f;
const float HUD_KNIGHT_HEAD_BB_WIDTH = 1.7 * 30.f;
const float HUD_KNIGHT_HEAD_BB_HEIGHT = 1.7 * 28.f;
const float HUD_WIZARD_HEAD_BB_WIDTH = 0.02886 * 1231.f;
const float HUD_WIZARD_HEAD_BB_HEIGHT = 0.02886 * 1767.f;
const float TUTORIAL_INSTRUCTIONS_WIDTH = (5.f / 8.f) * 1600.f; 
const float TUTORIAL_INSTRUCTIONS_HEIGHT = (0.5f) * 800.f; 
const float ARROW_WIDTH = (0.05088195386) * 1474.f;
const float ARROW_HEIGHT = (0.05572065378f) * 1346.f;
const float BOSS_BB_WIDTH = 492.f;
const float BOSS_BB_HEIGHT = 161.f;
const float HAND_BB_WIDTH = 269.f;
const float HAND_BB_HEIGHT = 189.f;


// background
Entity createBackground(RenderSystem* renderer, vec2 pos);
Entity createFinalBackground(RenderSystem* renderer, vec2 position);

// menu dimensions
const float BUTTON_BB_WIDTH = 0.5 * 459.f;
const float BUTTON_BB_HEIGHT = 0.5 * 140.f;
// top buttons (top left, top right)
const vec2 TTR_BUTTONPOS = { -0.5 * 18.f, 0.5 * 221.f };
const vec2 TL_BUTTONPOS = { -0.5 * 585.f, 0.5 * 398.f};
const vec2 TR_BUTTONPOS = { -0.5 * 18.f, 0.5 * 398.f};
// bottom buttons 
const vec2 BL_BUTTONPOS = { -0.5 * 585.f, 0.5 * 575.f};
const vec2 BR_BUTTONPOS = { -0.5 * 18.f, 0.5 * 575.f};



// the player
Entity createWizard(RenderSystem* renderer, vec2 pos);

Entity createKnight(RenderSystem* renderer, vec2 pos);

// the sword
Entity createSword(RenderSystem* renderer, float angle, Entity playerEntity);
// the projectile  
Entity createProjectile(RenderSystem* render, vec2 pos, vec2 velocity, float angle, Entity playerEntity);
Entity createEnemyProjectile(RenderSystem* renderer, vec2 pos, vec2 velocity, float angle, Entity enemyEntity);
Entity createHandProjectile(RenderSystem* renderer, vec2 pos, vec2 velocity, float angle, Entity enemyEntity);

// create wall
Entity createWall(vec2 position, vec2 scale);

// map blocks
Entity createBlock(RenderSystem* renderer, vec2 pos, std::string color);

Entity createDoor(vec2 position, vec2 scale);

Entity createEnemy(RenderSystem* renderer, vec2 position, int enemyType);

// the enemy 
Entity createEnemyBlob(RenderSystem* renderer, vec2 position);

// Tutorial enemy 
Entity createTutorialEnemy(RenderSystem* renderer, vec2 position); 

// the enemy that tries to avoid wizards 
Entity createEnemyRun(RenderSystem* renderer, vec2 position);

// state machine enemy
Entity createEnemyHunter(RenderSystem* renderer, vec2 position);

// BFS enemy
Entity createEnemyBacteria(RenderSystem* renderer, vec2 position);

// Chase enemy
Entity createEnemyChase(RenderSystem* renderer, vec2 position);

// AStar enemy
Entity createEnemyAStar(RenderSystem* renderer, vec2 position);

// Behaviour Tree enemy
Entity createEnemyGerm(RenderSystem* renderer, vec2 position);

// Final boss
Entity createEnemyBoss(RenderSystem* renderer, vec2 position);

// Final boss minions
Entity createEnemyMinions(RenderSystem* renderer, vec2 position);

// Final boss minions
Entity createEnemyBossHand(RenderSystem* renderer, vec2 position);

// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);

// help popup
Entity createHelp();

// story cutscenes
Entity createStory();
Entity createEndScene();

// Swarm
Entity createEnemySwarm(RenderSystem* renderer, vec2 position);
Entity createEnemySwarmTriplet(RenderSystem* renderer, vec2 position);
Entity createStory();

// Coord Enemy
Entity createEnemyCoordHead(RenderSystem* renderer, vec2 position);
Entity createEnemyCoordTail(RenderSystem* renderer, vec2 position);

// menu
Entity createMenu();
Entity createHpPowerup(vec2 position); 
Entity createDamagePowerup(vec2 position);
Entity createAttackSpeedPowerup(vec2 position);
Entity createMovementSpeedPowerup(vec2 position);

// on screen text
std::vector<Entity> createNumber(vec2 position, int number);
Entity createSingleDigitNumber(vec2 position, int singleDigitNumber);
std::vector<Entity> createDoubleDigitNumber(vec2 position, int doubleDigitNumber);
Entity createSmallLetter(vec2 position, int offset); 
Entity createCapsLetter(vec2 position, int offset); 
std::vector<Entity> createSentence(vec2 position, std::string sentence);

// HUD
Entity createHUD(vec2 position, Entity playerEntity);
Entity createHeadshot(vec2 position, Entity playerEntity);
Entity createCoin(vec2 position);
Entity createHp(vec2 position);
std::vector<Entity> createHps(vec2 position, int hpCount);

//TUTORIAL 
Entity createMovementAndAttackInstructions(vec2 position); 
Entity createArrow(vec2 position); 

// hud update
void updateHudHp(PlayerCharacter player);
void updateHudCoin(PlayerCharacter player);
void updateHudHp(vec2 position, Entity hudEntity, Entity playerEntity);
void updateHudCoin(vec2 position, Entity hudEntity, Entity playerEntity);
void HUDLocationSwitch(Entity hudEntity);
void moveHudFromShopToBattleRoom(Entity hudEntity);
void moveHudFromBattleToShopRoom(Entity hudEntity);