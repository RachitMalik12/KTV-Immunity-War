#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are hard coded to the dimensions of the entity texture
const float WIZARD_BB_WIDTH = 0.1f * 1231.f;
const float WIZARD_BB_HEIGHT = 0.1f * 1767.f;
const float KNIGHT_BB_WIDTH = 3.1f * 64.f;
const float KNIGHT_BB_HEIGHT = 3.1f * 64.f;
const float BLOCK_BB_WIDTH = 0.1f * 871.f;
const float BLOCK_BB_HEIGHT = 0.1f * 1549.f;
const float WATERBALL_BB_WIDTH = 0.04f * 1954.f;
const float WATERBALL_BB_HEIGHT = 0.04f * 1602.f;
const float ENEMYBLOB_BB_HEIGHT = 0.3f * 250.f;
const float ENEMYBLOB_BB_WIDTH = 0.3f * 250.f;
const float ENEMYRUN_BB_HEIGHT = 0.3f * 240.f;
const float ENEMYRUN_BB_WIDTH = 0.3f * 240.f;
const float ENEMYHUNTER_BB_HEIGHT = 0.11f * 691.f;
const float ENEMYHUNTER_BB_WIDTH = 0.11f * 582.f;
const float ENEMYBACTERIA_BB_HEIGHT = 0.11f * 691.f;
const float ENEMYBACTERIA_BB_WIDTH = 0.11f * 800.f;
const float ENEMYCHASE_BB_HEIGHT = 0.05f * 1024.f;
const float ENEMYCHASE_BB_WIDTH = 0.05f * 1258.f;
const float HELP_BB_WIDTH = 1000.f;
const float HELP_BB_HEIGHT = 641.f;
const float STORY_BB_WIDTH = 0.5*2388.f;
const float STORY_BB_HEIGHT = 0.5*1668.f;

// menu dimensions
const float BUTTON_BB_WIDTH = 0.5 * 459.f;
const float BUTTON_BB_HEIGHT = 0.5 * 140.f;
// top buttons (top left, top right)
const vec2 TL_BUTTONPOS = { -0.5 * 585.f, 0.5 * 398.f};
const vec2 TR_BUTTONPOS = { -0.5 * 18.f, 0.5 * 398.f};
// bottom buttons 
const vec2 BL_BUTTONPOS = { -0.5 * 585.f, 0.5 * 575.f};
const vec2 BR_BUTTONPOS = { -0.5 * 18.f, 0.5 * 575.f};



// the player
Entity createWizard(RenderSystem* renderer, vec2 pos);

Entity createKnight(RenderSystem* renderer, vec2 pos);

// the projectile  
Entity createProjectile(RenderSystem* render, vec2 pos, vec2 velocity, Entity playerEntity); 

// create wall
Entity createWall(vec2 position, vec2 scale);

// map blocks
Entity createBlock(RenderSystem* renderer, vec2 pos, std::string color);

Entity createDoor(vec2 position, vec2 scale);

Entity createEnemy(RenderSystem* renderer, vec2 position, int enemyType);

// the enemy 
Entity createEnemyBlob(RenderSystem* renderer, vec2 position);

// the enemy that tries to avoid wizards 
Entity createEnemyRun(RenderSystem* renderer, vec2 position);

// state machine enemy
Entity createEnemyHunter(RenderSystem* renderer, vec2 position);

// BFS enemy
Entity createEnemyBacteria(RenderSystem* renderer, vec2 position);

// BFS enemy
Entity createEnemyChase(RenderSystem* renderer, vec2 position);

// The powerup 
Entity createPowerup(RenderSystem* render, vec2 pos); 

// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);

// help popup
Entity createHelp();

// story cutscenes
Entity createStory();

// menu
Entity createMenu();