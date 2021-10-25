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
	void step(float elapsed_ms, float window_width_px, float window_height_px);
	void handle_collision();
private:
	std::default_random_engine rng1;
	std::uniform_real_distribution<float> uniform_dist1;
};