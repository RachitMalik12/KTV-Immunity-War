#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are hard coded to the dimensions of the entity texture
const float WIZARD_BB_WIDTH = 0.1f * 1231.f;
const float WIZARD_BB_HEIGHT = 0.1f * 1767.f;
const float BLOCK_BB_WIDTH = 0.1f * 871.f;
const float BLOCK_BB_HEIGHT = 0.1f * 1549.f;
const float FIREBALL_BB_WIDTH = 0.04f * 1954.f;
const float FIREBALL_BB_HEIGHT = 0.04f * 1602.f;
const float ENEMYBLOB_BB_HEIGHT = 0.3f * 250.f;
const float ENEMYBLOB_BB_WIDTH = 0.3f * 250.f;
const float POWERUP_BB_HEIGHT = 0.3f * 250.f;
const float POWERUP_BB_WIDTH = 0.3f * 250.f;
const float ENEMYRUN_BB_HEIGHT = 0.3f * 240.f;
const float ENEMYRUN_BB_WIDTH = 0.3f * 240.f;
const float ENEMYHUNTER_BB_HEIGHT = 0.11f * 691.f;
const float ENEMYHUNTER_BB_WIDTH = 0.11f * 582.f;
const float HELP_BB_WIDTH = 1000.f;
const float HELP_BB_HEIGHT = 641.f;

// the player
Entity createWizard(RenderSystem* renderer, vec2 pos);

// the projectile  
Entity createProjectile(RenderSystem* render, vec2 pos, vec2 velocity, Entity playerEntity); 

// create wall
Entity createWall(vec2 position, vec2 scale);

// map blocks
Entity createBlock(RenderSystem* renderer, vec2 pos, std::string color);

Entity createDoor(vec2 position, vec2 scale);

// the enemy 
Entity createEnemyBlob(RenderSystem* renderer, vec2 position, vec2 velocity);

// the enemy that tries to avoid wizards 
Entity createEnemyRun(RenderSystem* renderer, vec2 position, vec2 velocity);

// state machine enemy
Entity createEnemyHunter(RenderSystem* renderer, vec2 position, vec2 velocity);

// The powerup 
Entity createPowerup(RenderSystem* render, vec2 pos); 

// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);

// help popup
Entity createHelp();