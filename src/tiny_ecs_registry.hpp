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
	ComponentContainer<StartLevelTimer> startLevelTimers;
	ComponentContainer<TutorialTimer> tutorialTimers; 
	ComponentContainer<Motion> motions;
	ComponentContainer<Collision> collisions;
	ComponentContainer<Player> players;
	ComponentContainer<PlayerStat> playerStats;
	ComponentContainer<DeadPlayer> deadPlayers;
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
	ComponentContainer<DeadEnemy> deadEnemies;
	ComponentContainer<EnemyBlob> enemyBlobs;
	ComponentContainer<EnemyTutorial> enemiesTutorial; 
	ComponentContainer<EnemyRun> enemiesrun;
	ComponentContainer<EnemyHunter> enemyHunters;
	ComponentContainer<EnemyBacteria> enemyBacterias;
	ComponentContainer<EnemyGerm> enemyGerms;
	ComponentContainer<EnemyChase> enemyChase;
	ComponentContainer<EnemySwarm> enemySwarms;
	ComponentContainer<EnemyCoordHead> enemyCoordHeads;
	ComponentContainer<EnemyCoordTail> enemyCoordTails;
	ComponentContainer<EnemyAStar> enemyAStars;
	ComponentContainer<EnemyBoss> enemyBoss;
	ComponentContainer<EnemyBossHand> enemyBossHand;
	ComponentContainer<Powerup> powerups; 
	ComponentContainer<Flip> flips;
	ComponentContainer<InShop> inShops;
	ComponentContainer<Mesh*> hitboxes;
	ComponentContainer<HelpMode> helpModes;
	ComponentContainer<Step> steps;
	ComponentContainer<KnightAnimation> knightAnimations;
	ComponentContainer<WizardAnimation> wizardAnimations;
	ComponentContainer<Number> numbers;
	ComponentContainer<Letter> letters; 
	ComponentContainer<HUD> huds;
	ComponentContainer<HUDElement> hudElements;
	ComponentContainer<StoryMode> storyModes;
	ComponentContainer<Sword> swords;
	ComponentContainer<MenuMode> menuModes;
	ComponentContainer<MovementSpeedPowerUp> movementSpeedPowerup; 
	ComponentContainer<HpPowerUp> hpPowerup; 
	ComponentContainer<DamagePowerUp> damagePowerUp; 
	ComponentContainer<AtackSpeedPowerUp> attackSpeedPowerUp;
	ComponentContainer<Background> backgrounds;
	ComponentContainer<MovementAndAttackTutInst> instructions; 
	ComponentContainer<Arrow> arrows; 
	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry()
	{
		registry_list.push_back(&startLevelTimers);
		registry_list.push_back(&tutorialTimers); 
		registry_list.push_back(&motions);
		registry_list.push_back(&collisions);
		registry_list.push_back(&players);
		registry_list.push_back(&playerStats);
		registry_list.push_back(&deadPlayers);
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
		registry_list.push_back(&deadEnemies);
		registry_list.push_back(&enemyBlobs);
		registry_list.push_back(&enemiesTutorial); 
		registry_list.push_back(&enemiesrun);
		registry_list.push_back(&enemyHunters);
		registry_list.push_back(&enemyBacterias);
		registry_list.push_back(&enemyGerms);
		registry_list.push_back(&enemyChase);
		registry_list.push_back(&enemySwarms);
		registry_list.push_back(&enemyCoordHeads);
		registry_list.push_back(&enemyCoordTails);
		registry_list.push_back(&enemyAStars);
		registry_list.push_back(&enemyBoss);
		registry_list.push_back(&enemyBossHand);
		registry_list.push_back(&powerups);
		registry_list.push_back(&flips);
		registry_list.push_back(&inShops);
		registry_list.push_back(&hitboxes);
		registry_list.push_back(&helpModes);
		registry_list.push_back(&steps);
		registry_list.push_back(&knightAnimations);
		registry_list.push_back(&wizardAnimations);
		registry_list.push_back(&numbers);
		registry_list.push_back(&letters); 
		registry_list.push_back(&huds);
		registry_list.push_back(&hudElements);
		registry_list.push_back(&storyModes);
		registry_list.push_back(&swords);
		registry_list.push_back(&menuModes);
		registry_list.push_back(&movementSpeedPowerup); 
		registry_list.push_back(&hpPowerup); 
		registry_list.push_back(&damagePowerUp);
		registry_list.push_back(&attackSpeedPowerUp);
		registry_list.push_back(&backgrounds);
		registry_list.push_back(&instructions); 
		registry_list.push_back(&arrows); 
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