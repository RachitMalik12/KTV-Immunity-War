#pragma once

#include <iostream>
#include <random>
#include <queue>
#include <map>
#include <stack>
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
	std::queue<std::pair<int, int>> adjacentsQueue;
	std::stack<std::pair<int, int>> traversalStack;
	std::vector<int> path;
	std::pair<int, int> pred[8][8]{};
	std::pair<int, int> adj[8][8]{};
	bool visited[8][8]{};
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist;
	float next_bacteria_BFS_calculation;
	bool isEnemyInRangeOfThePlayers(Entity enemyEntity);
	float enemyDistanceFromPlayer(const Motion& player, const Motion& hunter);
	void setEnemyWonderingRandomly(Entity enemyEntity);
	void setEnemyChasingThePlayer(Entity enemyEntity);
	Entity determineWhichPlayerToChase(Entity hunterEntity);
	void stepEnemyHunter(float elapsed_ms);
	void stepEnemyChase(float elapsed_ms);
	void stepEnemyBacteria(float elapsed_ms, const float width, const float height);
	void stepEnemySwarm(float elapsed_ms);
	bool handlePath(int positionX, int positionY, float width, float height, Entity& bacteriaEntity);
	void bfsSearchPath(float initX, float initY, float finX, float finY, Entity& bacteriaEntity, float width, float height);
	void moveToSpot(float initX, float initY, float finalX, float finalY, Entity& bacteriaEntity);
	void createAdj();
	Entity pickAPlayer();
	void swarmFireProjectileAtPlayer(Entity swarmEntity);
	void swarmSpreadOut(Entity swarmEntity);
	Entity findClosestSwarm(Entity swarmEntity);
	void moveAwayfromOtherSwarm(Entity enemyEntity, Entity otherEnemyEntity);
};