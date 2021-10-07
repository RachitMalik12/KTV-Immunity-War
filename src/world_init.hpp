#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are hard coded to the dimensions of the entity texture
const float WIZARD_BB_WIDTH = 0.08f * 2048.f;
const float WIZARD_BB_HEIGHT = 0.08f * 2048.f;
const float BLOCK_BB_WIDTH = 0.08f * 2048.f;
const float BLOCK_BB_HEIGHT = 0.08f * 2048.f;
const float FIREBALL_BB_WIDTH = 0.3f * 512.f;
const float FIREBALL_BB_HEIGHT = 0.3f * 328.f;

// the player
Entity createSalmon(RenderSystem* renderer, vec2 pos);
Entity createWizard(RenderSystem* renderer, vec2 pos);

// the fireball 
Entity createFireball(RenderSystem* render, vec2 pos, vec2 velocity); 

// create wall
Entity createWall(vec2 position, vec2 size, bool isVertical);

// map blocks
Entity createBlock(RenderSystem* renderer, vec2 pos, std::string color);

// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);
// a pebble
Entity createPebble(vec2 pos, vec2 size);


