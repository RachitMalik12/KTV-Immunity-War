#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"
#include "world_system.hpp"

// stlib
#include <vector>
#include <random>

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem
{
public:
	PhysicsSystem::PhysicsSystem()
	{
		rng1 = std::default_random_engine(std::random_device()());
	};
	void initializeSounds();
	void step(float elapsed_ms, float window_width_px, float window_height_px);
	void handle_collision();
private:
	std::default_random_engine rng1;
	std::uniform_real_distribution<float> uniform_dist1;
	vec2 get_bounding_box(const Motion& motion);
	vec3 transformVertex(Motion& motion, ColoredVertex vertex);
	bool doesRadiusCollide(const Motion& motion, const Motion& other_motion);
	bool isMeshInBoundingBox(const Entity entity, const Entity other_entity);
	vec2 alignNextPositionToBoundingBox(vec2 nextPosition, const Motion& motion);
	bool collides(const Entity entity, const Entity other_entity);
	bool blockCollides(vec2 nextPosition, const Motion& block, const Motion& motion);
	bool wallCollides(vec2 nextPosition, Entity wall, const Motion& motion);
	void drawMeshDebug(const Entity entity);
	void drawBoundingBoxDebug(const Motion& motion);
	void bounceEnemyRun(Entity curEntity);
	void bounceEnemies(Entity curEntity, bool hitABlock);
	bool hitBlockOrWall(vec2 nextPosition, Motion& motion);
	void moveEntities(float elapsed_ms);
	void drawDebugMode();
	void checkForCollision();
	void resolvePlayerDamage(Entity playerEntity, int enemyDamage);
	void rotateSwords(float elapsed_ms);
	void enemyHitHandling(Entity enemyEntity);
	void handlePowerUpCollisions(Player& playerCom, PlayerStat& playerStatCom, Entity entity, bool isPlayerOne, bool isPlayerTwo, int powerUpCost);
	void enemyHitStatUpdate(Entity enemyEntity, Entity playerEntity, vec2 waterBallVelocity);
	void calculateSwordKnockBack(Enemy& enemyCom, Entity playerEntity);
	void calculateWaterBallKnockBack(Enemy& enemyCom, Entity playerEntity, vec2 waterBallVelocity);
	Mix_Chunk* buy_sound;
	Mix_Chunk* wizard_hit_sound;
	Mix_Chunk* knight_hit_sound;
	Mix_Chunk* water_sound;
	Mix_Chunk* slash_sound;
};