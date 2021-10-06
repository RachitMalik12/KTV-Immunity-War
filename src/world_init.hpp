#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are hard coded to the dimensions of the entity texture
const float FISH_BB_WIDTH = 0.4f * 296.f;
const float FISH_BB_HEIGHT = 0.4f * 165.f;
const float TURTLE_BB_WIDTH = 0.4f * 300.f;
const float TURTLE_BB_HEIGHT = 0.4f * 202.f;
const float FIREBALL_BB_WIDTH = 0.3f * 512.f;
const float FIREBALL_BB_HEIGHT = 0.3f * 328.f;

// the player
Entity createSalmon(RenderSystem* renderer, vec2 pos);
// the prey
Entity createFish(RenderSystem* renderer, vec2 position);
// the enemy
Entity createTurtle(RenderSystem* renderer, vec2 position);

// the fireball 
Entity createFireball(RenderSystem* render, vec2 position, vec2 velocity); 

// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);
// a pebble
Entity createPebble(vec2 pos, vec2 size);


