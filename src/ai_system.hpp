#pragma once

#include <vector>
#include <iostream>
#include <random>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"

class AISystem
{
public:
	AISystem() {
		rng = std::default_random_engine(std::random_device()());
	}
	void step(float elapsed_ms);

private:
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist;
	bool isHunterInRangeOfThePlayers(Entity hunterEntity);
	float hunterDistanceFromPlayer(const Motion& player, const Motion& hunter);
	void setHunterWonderingRandomly(Entity hunterEntity);
	void setHunterChasingThePlayer(Entity hunterEntity);
	Entity determineWhichPlayerToChase(Entity hunterEntity);
};