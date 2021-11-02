#pragma once

#include <iostream>
#include <random>
#include <queue>
#include <map>
#include <stack>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"

class AISystem
{
public:
	AISystem() {
		rng = std::default_random_engine(std::random_device()());
	}
	void step(float elapsed_ms, float width, float height);

private:
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
	bool isHunterInRangeOfThePlayers(Entity hunterEntity);
	float enemyDistanceFromPlayer(const Motion& player, const Motion& hunter);
	void setHunterWonderingRandomly(Entity hunterEntity);
	void setHunterChasingThePlayer(Entity hunterEntity);
	Entity determineWhichPlayerToChase(Entity hunterEntity);
	void stepEnemyHunter(float elapsed_ms);
	void stepEnemyChase(float elapsed_ms);
	void stepEnemyBacteria(float elapsed_ms, const float width, const float height);
	bool handlePath(int positionX, int positionY, float width, float height, Entity& bacteriaEntity);
	void bfsSearchPath(float initX, float initY, float finX, float finY, Entity& bacteriaEntity, float width, float height);
	void moveToSpot(float initX, float initY, float finalX, float finalY, Entity& bacteriaEntity);
	void createAdj();
};