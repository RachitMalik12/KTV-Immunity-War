#pragma once
#include <vector>

#include "tiny_ecs.hpp"
#include "components.hpp"

class ECSRegistry
{
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	// Manually created list of all components this game has
	ComponentContainer<EndLevelTimer> endLevelTimers;
	ComponentContainer<StartLevelTimer> startLevelTimers;
	ComponentContainer<DeathTimer> deathTimers;
	ComponentContainer<StuckTimer> stuckTimers;
	ComponentContainer<Motion> motions;
	ComponentContainer<Collision> collisions;
	ComponentContainer<Player> players;
	ComponentContainer<PlayerStat> playerStats;
	ComponentContainer<Mesh*> meshPtrs;
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer<ScreenState> screenStates;
	ComponentContainer<DebugComponent> debugComponents;
	ComponentContainer<MouseDestination> mouseDestinations;
	ComponentContainer<Projectile> projectiles;
	ComponentContainer<EnemyProjectile> enemyProjectiles;
	ComponentContainer<Block> blocks;
	ComponentContainer<Wall> walls;
	ComponentContainer<Door> doors;
	ComponentContainer<vec3> colors;
	ComponentContainer<Enemy> enemies;
	ComponentContainer<EnemyBlob> enemyBlobs;
	ComponentContainer<EnemyRun> enemiesrun;
	ComponentContainer<EnemyHunter> enemyHunters;
	ComponentContainer<EnemyBacteria> enemyBacterias;
	ComponentContainer<EnemyGerm> enemyGerms;
	ComponentContainer<EnemyChase> enemyChase;
	ComponentContainer<EnemySwarm> enemySwarms;
	ComponentContainer<Powerup> powerups; 
	ComponentContainer<Flip> flips;
	ComponentContainer<InShop> inShops;
	ComponentContainer<Mesh*> hitboxes;
	ComponentContainer<HelpMode> helpModes;
	ComponentContainer<Step> steps;
	ComponentContainer<KnightAnimation> knightAnimations;
	ComponentContainer<WizardAnimation> wizardAnimations;
	ComponentContainer<StoryMode> storyModes;
	ComponentContainer<Sword> swords;
	ComponentContainer<Title> titles;
	ComponentContainer<Background> backgrounds;


	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry()
	{
		registry_list.push_back(&startLevelTimers);
		registry_list.push_back(&endLevelTimers);
		registry_list.push_back(&deathTimers);
		registry_list.push_back(&stuckTimers);
		registry_list.push_back(&motions);
		registry_list.push_back(&collisions);
		registry_list.push_back(&players);
		registry_list.push_back(&playerStats);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&screenStates);
		registry_list.push_back(&debugComponents);
		registry_list.push_back(&mouseDestinations);
		registry_list.push_back(&projectiles);
		registry_list.push_back(&enemyProjectiles);
		registry_list.push_back(&blocks);
		registry_list.push_back(&walls);
		registry_list.push_back(&doors);
		registry_list.push_back(&colors);
		registry_list.push_back(&enemies);
		registry_list.push_back(&enemyBlobs);
		registry_list.push_back(&enemiesrun);
		registry_list.push_back(&enemyHunters);
		registry_list.push_back(&enemyBacterias);
		registry_list.push_back(&enemyGerms);
		registry_list.push_back(&enemyChase);
		registry_list.push_back(&enemySwarms);
		registry_list.push_back(&powerups);
		registry_list.push_back(&flips);
		registry_list.push_back(&inShops);
		registry_list.push_back(&hitboxes);
		registry_list.push_back(&helpModes);
		registry_list.push_back(&steps);
		registry_list.push_back(&knightAnimations);
		registry_list.push_back(&wizardAnimations);
		registry_list.push_back(&storyModes);
		registry_list.push_back(&swords);
		registry_list.push_back(&titles);
		registry_list.push_back(&backgrounds);
	}

	void clear_all_components() {
		for (ContainerInterface* reg : registry_list)
			reg->clear();
	}

	void list_all_components() {
		printf("Debug info on all registry entries:\n");
		for (ContainerInterface* reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e) {
		printf("Debug info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface* reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e) {
		for (ContainerInterface* reg : registry_list)
			reg->remove(e);
	}
};

extern ECSRegistry registry;