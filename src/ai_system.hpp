#pragma once

#include <iostream>
#include <random>
#include <map>
#include <cmath>
#include <limits>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "world_init.hpp"

class AISystem
{
public:
	AISystem(RenderSystem* renderer_arg) {
		rng = std::default_random_engine(std::random_device()());
		this->renderer = renderer_arg;
	}
	void step(float elapsed_ms, float width, float height);

private:
	RenderSystem* renderer;
	bool blocksInitialized = false;
	std::vector<int> path;
	std::pair<int, int> pred[8][8]{};
	std::pair<int, int> adj[8][8]{};
	std::pair<int, int> calculations[8][8]{};

	bool visited[8][8]{};
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist;
	bool isEnemyInRangeOfThePlayers(Entity enemyEntity);
	float enemyDistanceFromPlayer(const Motion& player, const Motion& hunter);
	void setEnemyWonderingRandomly(Entity enemyEntity);
	void setEnemyChasingThePlayer(Entity enemyEntity);
	Entity determineWhichPlayerToChase(Entity hunterEntity);
	void stepEnemyHunter(float elapsed_ms);
	void resolveHunterAnimation(Entity hunterEntity, Enemy& hunterStatus, EnemyHunter& hunter);
	void stepEnemyChase(float elapsed_ms);
	void stepEnemyAStar(float elapsed_ms, float width, float height);
	void stepEnemyGerm(float elapsed_ms);
	void stepEnemyBacteria(float elapsed_ms, const float width, const float height);
	void stepEnemySwarm(float elapsed_ms);
	void stepEnemyCoord(float elapsed_ms, float width, float height);
	void moveAwayfromOtherCoord(Entity enemyEntity, Entity otherEnemyEntity, float elapsed_ms);
	void handleCoordEnemyUpdate(Motion& playerMotion, Motion& enemyMotion, Motion& otherEnemyMotion, Entity enemyEntity, Entity otherEnemyEntity);
	void stepEnemyBoss(float elapsed_ms);
	bool handlePath(float width, float height, Entity& bacteriaEntity);
	void bfsSearchPath(float initX, float initY, float finX, float finY, Entity& bacteriaEntity, float width, float height);
	void moveToSpot(float initX, float initY, float finalX, float finalY, Entity& bacteriaEntity);
	void createAdj();
	void findPath(Entity& bacteriaEntity);
	void handleAStarPathCalculation(Entity& player, Entity& enemy, float width, float height);
	vec2 calculateHCost(const Motion& player, vec2 currNode);
	vec2 calculateGCost(Entity& enemy, int dir);
	vec2 nextNode(vec2 currNode, Entity& player, Entity& enemy, float width, float height);
	void stepMovement(Entity& enemyAStar);
	vec2 findFinalPosition(vec2 currNode, Entity& enemy, float width, float height, int currMinPosition);
	void pathCalculationInit(Entity& enemyAStar, float width, float height);
	int findMin(vec2 upSumCost, vec2 rightSumCost, vec2 downSumCost, vec2 leftSumCost);
	Entity pickAPlayer();
	void swarmFireProjectileAtPlayer(Entity swarmEntity);
	void bossFireProjectileAtPlayer(Entity entity);
	void swarmSpreadOut(Entity swarmEntity);
	Entity findClosestSwarm(Entity swarmEntity);
	void moveAwayfromOtherSwarm(Entity enemyEntity, Entity otherEnemyEntity);
};