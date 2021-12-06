#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

#include <random>

// Seeding rng with random device
std::default_random_engine rng = std::default_random_engine(std::random_device()());
std::uniform_real_distribution<float> uniform_dist;

Entity createBackground(RenderSystem* renderer, vec2 position) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = vec2({ BACKGROUND_BB_WIDTH * defaultResolution.scaling, BACKGROUND_BB_HEIGHT * defaultResolution.scaling });

	registry.backgrounds.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BACKGROUND,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity createFinalBackground(RenderSystem* renderer, vec2 position) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = vec2({ BACKGROUND_BB_WIDTH * defaultResolution.scaling, BACKGROUND_BB_HEIGHT * defaultResolution.scaling });

	registry.backgrounds.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::FINALBACKGROUND,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity createWizard(RenderSystem* renderer, vec2 position) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::WIZARD);
	registry.meshPtrs.emplace(entity, &mesh);
	Mesh& hitbox = renderer->getMesh(GEOMETRY_BUFFER_ID::WIZARD);
	registry.hitboxes.emplace(entity, &hitbox);
	WizardAnimation& animation = registry.wizardAnimations.emplace(entity);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = vec2({ WIZARD_BB_WIDTH * defaultResolution.scaling, WIZARD_BB_HEIGHT * defaultResolution.scaling });

	registry.players.emplace(entity);
	animation.animationMode = animation.idleMode;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::WIZARDIDLE,
			EFFECT_ASSET_ID::WIZARD,
			GEOMETRY_BUFFER_ID::SPRITE });
	animation.frameIdle = 0;
	animation.idleTimer = 0;
	return entity;
}

Entity createKnight(RenderSystem* renderer, vec2 position) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);
	registry.knightAnimations.emplace(entity);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = vec2({ KNIGHT_BB_WIDTH * defaultResolution.scaling, KNIGHT_BB_HEIGHT * defaultResolution.scaling });

	registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::KNIGHT,
			EFFECT_ASSET_ID::KNIGHT,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createSword(RenderSystem* renderer, float angle, Entity playerEntity) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);
	Mesh& hitbox = renderer->getMesh(GEOMETRY_BUFFER_ID::SWORD);
	registry.hitboxes.emplace(entity, &hitbox);

	// Initialize the position, scale, and physics components		
	auto& motion = registry.motions.emplace(entity);
	motion.angle = angle;
	motion.velocity = { 0, 0 };
	motion.position = { 0, 0 };
	motion.scale = vec2({ SWORD_BB_WIDTH * defaultResolution.scaling, SWORD_BB_HEIGHT * defaultResolution.scaling });

	registry.swords.emplace(entity);
	registry.swords.get(entity).belongToPlayer = playerEntity;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::SWORD,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}


Entity createWall(vec2 position, vec2 scale) {
	Entity entity = Entity();
	Wall& wall = registry.walls.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::LINE,
			GEOMETRY_BUFFER_ID::WALLS });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = scale;

	return entity;
}

Entity createDoor(vec2 position, vec2 scale) {
	Entity entity = Entity();
	Wall& wall = registry.walls.emplace(entity);
	Door& door = registry.doors.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::LINE,
			GEOMETRY_BUFFER_ID::DOOR });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = scale;

	return entity;
}


Entity createBlock(RenderSystem* renderer, vec2 pos, std::string color) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);
	Mesh& hitbox = renderer->getMesh(GEOMETRY_BUFFER_ID::TREE);
	registry.hitboxes.emplace(entity, &hitbox);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = pos;
	motion.scale = vec2({ BLOCK_BB_WIDTH * defaultResolution.scaling, BLOCK_BB_HEIGHT * defaultResolution.scaling });
	TEXTURE_ASSET_ID blockColor = TEXTURE_ASSET_ID::TREE_RED;
	if (color == "red") blockColor = TEXTURE_ASSET_ID::TREE_RED;
	else if (color == "orange") blockColor = TEXTURE_ASSET_ID::TREE_ORANGE;
	else if (color == "yellow") blockColor = TEXTURE_ASSET_ID::TREE_YELLOW;

	registry.blocks.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{   blockColor,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createEnemy(RenderSystem* renderer, vec2 position, int enemyType) {
	Entity curEnemy;
	switch (enemyType) {
		case 0:
			curEnemy = createEnemyBlob(renderer, position);
			break;
		case 1:
			curEnemy = createEnemyRun(renderer, position);
			break;
		case 2:
			curEnemy = createEnemyHunter(renderer, position);
			break;
		case 3:
			curEnemy = createEnemyBacteria(renderer, position);
			break;
		case 4:
			curEnemy = createEnemyChase(renderer, position);
			break;
		case 5:
			curEnemy = createEnemySwarmTriplet(renderer, position);
			break;
		case 6:
			curEnemy = createEnemyGerm(renderer, position);
			break;
		case 7: 
			curEnemy = createTutorialEnemy(renderer, position); 
			break;
		case 8:
			curEnemy = createEnemyAStar(renderer, position);
			break;
		case 9:  
			curEnemy = createEnemyMinions(renderer, position); 
			break;
		case 10:
			curEnemy = createEnemyBossHand(renderer, position);
			break;
		case 11:
			curEnemy = createEnemyBoss(renderer, position);
			break;
		case 12:
			curEnemy = createEnemyCoordHead(renderer, position);
			break;
		case 13:
			curEnemy = createEnemyCoordTail(renderer, position);
			break;
	}
	return curEnemy;
}

Entity createEnemyBlob(RenderSystem* renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);
	Mesh& hitbox = renderer->getMesh(GEOMETRY_BUFFER_ID::BLOBBER);
	registry.hitboxes.emplace(entity, &hitbox);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;

	motion.scale = vec2({ ENEMYBLOB_BB_WIDTH * defaultResolution.scaling, ENEMYBLOB_BB_HEIGHT * defaultResolution.scaling });

	registry.enemies.emplace(entity);
	registry.enemyBlobs.emplace(entity);
	// Set enemy attributes
	auto& enemyCom = registry.enemies.get(entity);
	enemyCom.damage = 1;
	enemyCom.hp = 3;
	enemyCom.max_hp = enemyCom.hp;
	enemyCom.loot = 2;
	enemyCom.speed = 200.f * defaultResolution.scaling;
	motion.velocity = vec2(0.f, enemyCom.speed);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMY,
			EFFECT_ASSET_ID::ENEMY,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createTutorialEnemy(RenderSystem* renderer, vec2 position)
{
	// Reserve an entity
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);
	Mesh& hitbox = renderer->getMesh(GEOMETRY_BUFFER_ID::BLOBBER);
	registry.hitboxes.emplace(entity, &hitbox);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;
	motion.velocity = vec2(0.f, 0.f); 

	motion.scale = vec2({ ENEMYBLOB_BB_WIDTH * defaultResolution.scaling, ENEMYBLOB_BB_HEIGHT * defaultResolution.scaling });

	registry.enemies.emplace(entity);
	registry.enemiesTutorial.emplace(entity);
	// Set enemy attributes
	auto& enemyCom = registry.enemies.get(entity);
	enemyCom.damage = 1;
	enemyCom.hp = 5;
	enemyCom.max_hp = enemyCom.hp;
	enemyCom.loot = 5;
	enemyCom.speed = 0.f;
	enemyCom.invinFrame = 7000.f; 
	enemyCom.isInvin = true; 

	return entity;
}

Entity createEnemyRun(RenderSystem* renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);
	Mesh& hitbox = renderer->getMesh(GEOMETRY_BUFFER_ID::RUNNER);
	registry.hitboxes.emplace(entity, &hitbox);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;

	motion.scale = vec2({ ENEMYRUN_BB_WIDTH * defaultResolution.scaling, ENEMYRUN_BB_HEIGHT * defaultResolution.scaling });

	registry.enemies.emplace(entity);
	registry.enemiesrun.emplace(entity);
	// Set enemy attributes
	auto& enemyCom = registry.enemies.get(entity);
	enemyCom.damage = 1;
	enemyCom.hp = 3;
	enemyCom.max_hp = enemyCom.hp;
	enemyCom.loot = 2;
	enemyCom.speed = 150.f * defaultResolution.scaling;
	motion.velocity = vec2(uniform_dist(rng) * enemyCom.speed, uniform_dist(rng) * enemyCom.speed);;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMYRUN,
			EFFECT_ASSET_ID::ENEMY,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createEnemyHunter(RenderSystem* renderer, vec2 position) {
	auto entity = Entity();
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);
	Mesh& hitbox = renderer->getMesh(GEOMETRY_BUFFER_ID::HUNTER);
	registry.hitboxes.emplace(entity, &hitbox);

	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = vec2(0, 0);
	motion.position = position;

	motion.scale = vec2({ ENEMYHUNTER_BB_WIDTH * defaultResolution.scaling, ENEMYHUNTER_BB_HEIGHT * defaultResolution.scaling });

	registry.enemies.emplace(entity);
	registry.enemyHunters.emplace(entity);
	// Set enemy attributes
	auto& enemyCom = registry.enemies.get(entity);
	enemyCom.damage = 1;
	enemyCom.hp = 6;
	enemyCom.max_hp = enemyCom.hp;
	enemyCom.loot = 2;
	enemyCom.speed = 150.f * defaultResolution.scaling;
	auto& hunterCom = registry.enemyHunters.get(entity);
	hunterCom.currentState = hunterCom.searchingMode;
	hunterCom.huntingRange = hunterCom.huntingRange * defaultResolution.scaling;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMYHUNTER,
			EFFECT_ASSET_ID::ENEMY,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createEnemyBacteria(RenderSystem* renderer, vec2 position) {
	auto entity = Entity();
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);
	Mesh& hitbox = renderer->getMesh(GEOMETRY_BUFFER_ID::BACTERIA);
	registry.hitboxes.emplace(entity, &hitbox);

	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = vec2(0, 0);
	motion.position = position;

	motion.scale = vec2({ ENEMYBACTERIA_BB_WIDTH * defaultResolution.scaling, ENEMYBACTERIA_BB_HEIGHT * defaultResolution.scaling });

	registry.enemies.emplace(entity);
	registry.enemyBacterias.emplace(entity);
	// Set enemy attributes
	auto& enemyCom = registry.enemies.get(entity);
	enemyCom.damage = 1;
	enemyCom.hp = 6;
	enemyCom.max_hp = enemyCom.hp;
	enemyCom.loot = 2;
	enemyCom.speed = 150.f * defaultResolution.scaling;
	auto& bacteria = registry.enemyBacterias.get(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMYBACTERIA,
			EFFECT_ASSET_ID::ENEMY,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createEnemyGerm(RenderSystem* renderer, vec2 position) {
	auto entity = Entity();
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);
	Mesh& hitbox = renderer->getMesh(GEOMETRY_BUFFER_ID::GERM);
	registry.hitboxes.emplace(entity, &hitbox);

	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = vec2(0, 0);
	motion.position = position;

	motion.scale = vec2({ ENEMYGERM_BB_WIDTH * defaultResolution.scaling, ENEMYGERM_BB_HEIGHT * defaultResolution.scaling });
	
	registry.enemies.emplace(entity);
	registry.enemyGerms.emplace(entity);
	// Set enemy attributes
	auto& enemyCom = registry.enemies.get(entity);
	enemyCom.damage = 1;
	enemyCom.hp = 7;
	enemyCom.loot = 2;
	enemyCom.speed = 125.f * defaultResolution.scaling;
	auto& germ = registry.enemyGerms.get(entity);
	germ.mode = (rand() % 10) + 1;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::GERM,
			EFFECT_ASSET_ID::ENEMY,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createEnemyAStar(RenderSystem* renderer, vec2 position) {
	auto entity = Entity();
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = vec2(0, 0);
	motion.position = position;

	motion.scale = vec2({ ENEMYASTAR_BB_WIDTH * defaultResolution.scaling, ENEMYASTAR_BB_HEIGHT * defaultResolution.scaling });

	registry.enemies.emplace(entity);
	registry.enemyAStars.emplace(entity);
	// Set enemy attributes
	auto& enemyCom = registry.enemies.get(entity);
	enemyCom.damage = 1;
	enemyCom.hp = 8;
	enemyCom.loot = 2;
	enemyCom.speed = 200.f * defaultResolution.scaling;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMYASTAR,
			EFFECT_ASSET_ID::ENEMY,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

// Enemy that chases the player
Entity createEnemyChase(RenderSystem* renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;

	motion.scale = vec2({ ENEMYCHASE_BB_WIDTH * defaultResolution.scaling, ENEMYCHASE_BB_HEIGHT * defaultResolution.scaling });

	registry.enemies.emplace(entity);
	registry.enemyChase.emplace(entity);
	// Set enemy attributes
	auto& enemyCom = registry.enemies.get(entity);
	enemyCom.damage = 1;
	enemyCom.hp = 3;
	enemyCom.max_hp = enemyCom.hp;
	enemyCom.loot = 1;
	enemyCom.speed = 50.f * defaultResolution.scaling;
	motion.velocity = vec2(uniform_dist(rng) * enemyCom.speed, uniform_dist(rng) * enemyCom.speed);

	Motion& player_motion = motion;
	for (Entity player : registry.players.entities) {
		player_motion = registry.motions.get(entity);
		break;
	}

	vec2 enemy_to_player = vec2(player_motion.position.x - motion.position.x, player_motion.position.y - motion.position.y);
	float radians = atan2f(enemy_to_player.y, -enemy_to_player.x);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMYCHASE,
			EFFECT_ASSET_ID::ENEMY,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createEnemySwarmTriplet(RenderSystem* renderer, vec2 position)
{
	// Reserve en entity
	auto entity = createEnemySwarm(renderer, position);
	float swarmSpawnGap = 60.f;
	vec2 enemyTwoPosition = vec2(position.x + (swarmSpawnGap * defaultResolution.scaling), position.y - (swarmSpawnGap * defaultResolution.scaling));
	createEnemySwarm(renderer, enemyTwoPosition);
	vec2 enemyThreePosition = vec2(position.x + (swarmSpawnGap * defaultResolution.scaling), position.y + (swarmSpawnGap * defaultResolution.scaling));
	createEnemySwarm(renderer, enemyThreePosition);

	return entity;
}

Entity createEnemySwarm(RenderSystem* renderer, vec2 position) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;
	motion.scale = vec2({ ENEMYSWARM_BB_WIDTH  * defaultResolution.scaling, ENEMYSWARM_BB_HEIGHT  * defaultResolution.scaling });
	registry.enemies.emplace(entity);
	// Set enemy attributes
	EnemySwarm& swarm = registry.enemySwarms.emplace(entity);
	swarm.projectileSpeed = swarm.projectileSpeed * defaultResolution.scaling;
	auto& enemyCom = registry.enemies.get(entity);
	enemyCom.damage = 1;
	enemyCom.hp = 6;
	enemyCom.max_hp = enemyCom.hp;
	enemyCom.loot = 2;
	enemyCom.speed = 100.f * defaultResolution.scaling;
	motion.velocity = vec2(0, 0);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMYSWARM,
			EFFECT_ASSET_ID::ENEMY,
			GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity createEnemyCoordHead(RenderSystem* renderer, vec2 position) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;
	motion.scale = vec2({ ENEMYHEAD_BB_WIDTH * defaultResolution.scaling, ENEMYHEAD_BB_HEIGHT * defaultResolution.scaling });
	auto& enemyCom = registry.enemies.emplace(entity);
	// Set enemy attributes
	auto& head = registry.enemyCoordHeads.emplace(entity);
	head.minDistFromTail *= defaultResolution.scaling;
	enemyCom.damage = 1;
	enemyCom.hp = 15;
	enemyCom.max_hp = enemyCom.hp;
	enemyCom.loot = 4;
	enemyCom.speed = 100.f * defaultResolution.scaling;
	motion.velocity = vec2(0, 0);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMYHEAD,
			EFFECT_ASSET_ID::ENEMY,
			GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity createEnemyCoordTail(RenderSystem* renderer, vec2 position) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;
	motion.scale = vec2({ ENEMYTAIL_BB_WIDTH * defaultResolution.scaling, ENEMYTAIL_BB_HEIGHT * defaultResolution.scaling });
	registry.enemies.emplace(entity);
	// Set enemy attributes
	auto& tail = registry.enemyCoordTails.emplace(entity);
	auto& enemyCom = registry.enemies.get(entity);
	tail.minDistFromHead *= defaultResolution.scaling;
	enemyCom.damage = 1;
	enemyCom.hp = 100;
	enemyCom.max_hp = enemyCom.hp;
	enemyCom.loot = 0;
	enemyCom.speed = 100.f * defaultResolution.scaling;
	motion.velocity = vec2(0, 0);

	// refer correct head to this tail
	Entity headEntity = registry.enemyCoordHeads.entities[registry.enemyCoordTails.size() - 1];
	registry.enemyCoordHeads.get(headEntity).belongToTail = entity;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMYTAIL,
			EFFECT_ASSET_ID::ENEMY,
			GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity createEnemyBoss(RenderSystem* renderer, vec2 position) {
	// Reserve en entity
	auto entity = Entity();

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = vec2({ BOSS_BB_WIDTH * defaultResolution.scaling, BOSS_BB_HEIGHT * defaultResolution.scaling });
	auto& enemyCom = registry.enemies.emplace(entity);
	registry.enemyBoss.emplace(entity);
	// Set enemy attributes
	enemyCom.damage = 1;
	enemyCom.hp = 55;
	enemyCom.max_hp = enemyCom.hp;
	enemyCom.loot = 99;
	enemyCom.speed = 0.f;
	motion.velocity = vec2(0.f, 0.f);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BOSS,
			EFFECT_ASSET_ID::BOSS,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createEnemyMinions(RenderSystem* renderer, vec2 position) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;
	motion.scale = vec2({ ENEMYMINION_BB_WH * defaultResolution.scaling, ENEMYMINION_BB_WH * defaultResolution.scaling });
	registry.enemies.emplace(entity);
	// Set enemy attributes
	EnemySwarm& swarm = registry.enemySwarms.emplace(entity);
	swarm.projectileSpeed = swarm.projectileSpeed * defaultResolution.scaling;
	auto& enemyCom = registry.enemies.get(entity);
	enemyCom.damage = 1;
	enemyCom.hp = 11;
	enemyCom.max_hp = enemyCom.hp;
	enemyCom.loot = 2;
	enemyCom.speed = 100.f * defaultResolution.scaling;
	motion.velocity = vec2(0, 0);

	return entity;
}

Entity createEnemyBossHand(RenderSystem* renderer, vec2 position) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;
	motion.scale = vec2({ HAND_BB_WIDTH * defaultResolution.scaling, HAND_BB_HEIGHT * defaultResolution.scaling });
	registry.enemies.emplace(entity);
	// Set enemy attributes
	EnemySwarm& hand = registry.enemySwarms.emplace(entity);
	EnemyBossHand& boss = registry.enemyBossHand.emplace(entity);
	hand.projectileSpeed = 300.f* defaultResolution.scaling;
	auto& enemyCom = registry.enemies.get(entity);
	enemyCom.damage = 1;
	enemyCom.hp = 30;
	enemyCom.max_hp = enemyCom.hp;
	enemyCom.loot = 2;
	enemyCom.speed = 400.f * defaultResolution.scaling;
	motion.velocity = vec2(enemyCom.speed, 0);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HAND,
			EFFECT_ASSET_ID::ENEMY,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createProjectile(RenderSystem* renderer, vec2 pos, vec2 velocity, float angle, Entity playerEntity) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);
	Mesh& hitbox = renderer->getMesh(GEOMETRY_BUFFER_ID::WATERBALL);
	registry.hitboxes.emplace(entity, &hitbox);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = angle;
	motion.velocity = velocity;
	motion.position = pos;

	// Setting initial values
	motion.scale = vec2({ WATERBALL_BB_WIDTH * defaultResolution.scaling, WATERBALL_BB_HEIGHT * defaultResolution.scaling });

	registry.projectiles.emplace(entity);
	registry.projectiles.get(entity).belongToPlayer = playerEntity;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::WATERBALL,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createEnemyProjectile(RenderSystem* renderer, vec2 pos, vec2 velocity, float angle, Entity enemyEntity) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = angle;
	motion.velocity = velocity;
	motion.position = pos;

	// Setting initial values
	motion.scale = vec2({ FIREBALL_BB_WIDTH * defaultResolution.scaling, FIREBALL_BB_HEIGHT * defaultResolution.scaling });

	EnemyProjectile& projectile = registry.enemyProjectiles.emplace(entity);
	projectile.belongToEnemy = enemyEntity;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::FIREBALL,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createHandProjectile(RenderSystem* renderer, vec2 pos, vec2 velocity, float angle, Entity enemyEntity) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = angle;
	motion.velocity = velocity;
	motion.position = pos;

	// Setting initial values
	motion.scale = vec2({ FIREBALL_BB_WIDTH * defaultResolution.scaling, FIREBALL_BB_HEIGHT * defaultResolution.scaling });

	EnemyProjectile& projectile = registry.enemyProjectiles.emplace(entity);
	projectile.belongToEnemy = enemyEntity;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BOSSFIREBALL,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createLine(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::LINE,
		 GEOMETRY_BUFFER_ID::DEBUG_LINE });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = scale;

	registry.debugComponents.emplace(entity);
	return entity;
}

Entity createHelp() {
	Entity entity = Entity();

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HELPPANEL,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = { defaultResolution.width / 2, defaultResolution.height / 2 };
	
	motion.scale = vec2({ HELP_BB_WIDTH * defaultResolution.scaling, HELP_BB_HEIGHT * defaultResolution.scaling });
	

	registry.helpModes.emplace(entity);

	return entity;
}

Entity createStory() {
	Entity entity = Entity();

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::FRAME1,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = { defaultResolution.width / 2, defaultResolution.height / 2 };
	motion.scale = vec2({ STORY_BB_WIDTH * defaultResolution.scaling, STORY_BB_HEIGHT * defaultResolution.scaling });

	registry.storyModes.emplace(entity);

	return entity;
}

Entity createEndScene() {
	Entity entity = Entity();

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::END1,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = { defaultResolution.width / 2, defaultResolution.height / 2 };
	motion.scale = vec2({ STORY_BB_WIDTH * defaultResolution.scaling, STORY_BB_HEIGHT * defaultResolution.scaling });

	registry.storyModes.emplace(entity);

	return entity;
}

Entity createMenu() {
	Entity entity = Entity();

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::MENU,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = { defaultResolution.width / 2, defaultResolution.height / 2 };
	motion.scale = vec2({ STORY_BB_WIDTH * defaultResolution.scaling, STORY_BB_HEIGHT * defaultResolution.scaling });

	registry.menuModes.emplace(entity);

	return entity;
}

Entity createHpPowerup(vec2 position) {
	Entity entity = Entity();

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HPPOWERUP,
		  EFFECT_ASSET_ID::POWERUP,
			GEOMETRY_BUFFER_ID::SPRITE });

	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position; 
	motion.scale = vec2({ HP_POWERUP_WIDTH * defaultResolution.scaling, HP_POWERUP_HEIGHT * defaultResolution.scaling });

	registry.hpPowerup.emplace(entity);
	Powerup& powerup = registry.powerups.emplace(entity); 
	powerup.cost = 5; 

	return entity;
}

Entity createDamagePowerup(vec2 position) {
	Entity entity = Entity();

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::DAMAGEPOWERUP,
		  EFFECT_ASSET_ID::POWERUP,
			GEOMETRY_BUFFER_ID::SPRITE });

	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = vec2({ DAMAGE_POWERUP_WIDTH * defaultResolution.scaling, DAMAGE_POWERUP_HEIGHT * defaultResolution.scaling });

	registry.damagePowerUp.emplace(entity);
	Powerup& powerup = registry.powerups.emplace(entity);
	powerup.cost = 5;

	return entity;
}
Entity createAttackSpeedPowerup(vec2 position) {
	Entity entity = Entity();

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ATTACKPOWERUP,
		  EFFECT_ASSET_ID::POWERUP,
			GEOMETRY_BUFFER_ID::SPRITE });

	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = vec2({ ATTACK_POWERUP_WIDTH * defaultResolution.scaling, ATTACK_POWERUP_HEIGHT * defaultResolution.scaling });

	registry.attackSpeedPowerUp.emplace(entity); 
	Powerup& powerup = registry.powerups.emplace(entity);
	powerup.cost = 5;

	return entity;
}

Entity createMovementSpeedPowerup(vec2 position) {
	Entity entity = Entity();

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::MOVEMENTSPEEDPOWERUP,
		  EFFECT_ASSET_ID::POWERUP,
			GEOMETRY_BUFFER_ID::SPRITE });

	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = vec2({ MOVEMENT_POWERUP_WIDTH * defaultResolution.scaling, MOVEMENT_POWERUP_HEIGHT * defaultResolution.scaling });

	registry.movementSpeedPowerup.emplace(entity);
	Powerup& powerup = registry.powerups.emplace(entity);
	powerup.cost = 5;

	return entity;
}

/**
	* Render a number between 0 and 99 inclusive on screen at the desired position. Will render 0 if range is incorrect.
	*
	* @param  singleDigitNumber   the number to be displayed on screen, non-negative single digit or double digits only, meaning 0 - 99
	* @return					  the newly created entity
	*/
std::vector<Entity> createNumber(vec2 position, int number) {
	if (number < 0 || number > 99) {
		std::vector<Entity> numberEntity;
		numberEntity.push_back(createSingleDigitNumber(position, 0));
		return numberEntity;
	}
	if (number < 10) {
		std::vector<Entity> numberEntity;
		numberEntity.push_back(createSingleDigitNumber(position, number));
		return numberEntity;
	}
	else {
		return createDoubleDigitNumber(position, number);
	}
}

Entity createSingleDigitNumber(vec2 position, int singleDigitNumber) {
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = vec2(NUMBER_BB_WIDTH * defaultResolution.scaling, NUMBER_BB_HEIGHT * defaultResolution.scaling);

	Number& number = registry.numbers.emplace(entity);
	number.frame = singleDigitNumber;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::NUMBER,
		  EFFECT_ASSET_ID::NUMBER,
		  GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

std::vector<Entity> createDoubleDigitNumber(vec2 position, int doubleDigitNumber) {
	std::vector<Entity> numberEntities;
	numberEntities.push_back(createSingleDigitNumber(vec2(position.x + (NUMBER_BB_WIDTH / 2) * defaultResolution.scaling, position.y), doubleDigitNumber % 10));
	numberEntities.push_back(createSingleDigitNumber(vec2(position.x - (NUMBER_BB_WIDTH / 2) * defaultResolution.scaling, position.y), doubleDigitNumber / 10));
	return numberEntities;
}
std::vector<Entity> createSentence(vec2 position, std::string sentence) {
	std::vector<Entity> letters;
	int index = 0; 
	for (char s : sentence) {
		int next = index + 1; 
		if (s != ' ' && isalpha(s)) {
			if (islower(s)) {
				letters.push_back(createSmallLetter(position, s - 'a'));
				// Apply appropriate position increment based on next type of letter. 
				if (islower(sentence[next])) {
					position.x += ((SMALLLETTER_BB_WIDTH) * defaultResolution.scaling);
				}
				else {
					position.x += ((CAPSLETTER_BB_WIDTH) * defaultResolution.scaling);
				}
			}
			else {
				letters.push_back(createCapsLetter(position, s - 'A'));
				if (islower(sentence[next])) {
					position.x += ((SMALLLETTER_BB_WIDTH) * defaultResolution.scaling);
				}
				else {
					position.x += ((CAPSLETTER_BB_WIDTH) * defaultResolution.scaling);
				}
			}
		}
		else {
			// any invalid character, insert a space. 
			position.x += (CAPSLETTER_BB_WIDTH) * defaultResolution.scaling; 
		}
		index++; 
	}
	return letters; 
}
Entity createSmallLetter(vec2 position, int offset) {
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = vec2(SMALLLETTER_BB_WIDTH * defaultResolution.scaling, SMALLLETTER_BB_HEIGHT * defaultResolution.scaling);

	Letter& smolBoi = registry.letters.emplace(entity);
	smolBoi.frame = offset; 

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::SMALLETTER,
		  EFFECT_ASSET_ID::LETTER,
		  GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity createCapsLetter(vec2 position, int offset) {
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = vec2(CAPSLETTER_BB_WIDTH *defaultResolution.scaling, CAPSLETTER_BB_HEIGHT * defaultResolution.scaling);

	Letter& bigBoi = registry.letters.emplace(entity);
	bigBoi.frame = offset;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::CAPSLETTER,
		  EFFECT_ASSET_ID::LETTER,
		  GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity createHUD(vec2 position, Entity playerEntity) {
	auto entity = Entity();
	HUD& hud = registry.huds.emplace(entity);
	Player& player = registry.players.get(playerEntity);
	PlayerStat& playerStat = registry.playerStats.get(player.playerStat);
	hud.hps = createHps(position, player.hp);
	vec2 headShotPosition = vec2(position.x, position.y + (HUD_HP_BB_HEIGHT * defaultResolution.scaling));
	hud.headShot = createHeadshot(headShotPosition, playerEntity);
	hud.coin = createCoin(vec2(position.x + HUD_COIN_BB_WIDTH * defaultResolution.scaling, position.y + (HUD_HP_BB_HEIGHT * defaultResolution.scaling)));
	vec2 coinCountPosition = vec2(position.x + (2 * HUD_COIN_BB_WIDTH + (NUMBER_BB_WIDTH / 2)) * defaultResolution.scaling, position.y + HUD_HP_BB_HEIGHT * defaultResolution.scaling);
	hud.coinCount = createNumber(coinCountPosition, playerStat.money);
	return entity;
}

Entity createHeadshot(vec2 position, Entity playerEntity) {
	auto entity = Entity();
	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	registry.hudElements.emplace(entity);
	if (playerEntity.getId() == registry.players.entities.front()) {
		motion.scale = vec2(HUD_KNIGHT_HEAD_BB_WIDTH * defaultResolution.scaling, HUD_KNIGHT_HEAD_BB_HEIGHT * defaultResolution.scaling);
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::KNIGHTICON,
			  EFFECT_ASSET_ID::TEXTURED,
			  GEOMETRY_BUFFER_ID::SPRITE });
	}
	else {
		motion.scale = vec2(HUD_WIZARD_HEAD_BB_WIDTH * defaultResolution.scaling, HUD_WIZARD_HEAD_BB_HEIGHT * defaultResolution.scaling);
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::WIZARDICON,
			  EFFECT_ASSET_ID::TEXTURED,
			  GEOMETRY_BUFFER_ID::SPRITE });
	}
	return entity;
}

Entity createCoin(vec2 position) {
	auto entity = Entity();
	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = vec2(HUD_COIN_BB_WIDTH * defaultResolution.scaling, HUD_COIN_BB_HEIGHT * defaultResolution.scaling);
	registry.hudElements.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::COIN,
		  EFFECT_ASSET_ID::TEXTURED,
		  GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity createHp(vec2 position) {
	auto entity = Entity();
	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = vec2(HUD_HP_BB_WIDTH * defaultResolution.scaling, HUD_HP_BB_HEIGHT * defaultResolution.scaling);
	registry.hudElements.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HP,
		  EFFECT_ASSET_ID::TEXTURED,
		  GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

std::vector<Entity> createHps(vec2 position, int hpCount) {
	const int maxHpShownOnHUD = 5;
	if (hpCount > maxHpShownOnHUD) {
		hpCount = maxHpShownOnHUD;
	}
	std::vector<Entity> hps;
	for (int i = 0; i < hpCount; i++) {
		vec2 curHpPosition = vec2(position.x + (i * HUD_HP_BB_WIDTH * defaultResolution.scaling), position.y);
		hps.push_back(createHp(curHpPosition));
	}
	return hps;
}

Entity createMovementAndAttackInstructions(vec2 position) {
	auto entity = Entity(); 
	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = vec2(TUTORIAL_INSTRUCTIONS_WIDTH * defaultResolution.scaling, TUTORIAL_INSTRUCTIONS_HEIGHT * defaultResolution.scaling);
	registry.instructions.emplace(entity); 

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TUTINSTRUCTIONS,
		  EFFECT_ASSET_ID::TEXTURED,
		  GEOMETRY_BUFFER_ID::SPRITE });
	return entity; 
}

Entity createArrow(vec2 position) {
	auto entity = Entity();
	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.scale = vec2(ARROW_WIDTH * defaultResolution.scaling, ARROW_HEIGHT * defaultResolution.scaling);
	registry.arrows.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ARROW,
		  EFFECT_ASSET_ID::TEXTURED,
		  GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

void updateHudHp(vec2 position, Entity hudEntity, Entity playerEntity) {
	HUD& hud = registry.huds.get(hudEntity);
	Player& player = registry.players.get(playerEntity);
	while (!hud.hps.empty()) {
		registry.remove_all_components_of(hud.hps.back());
		hud.hps.pop_back();
	}
	hud.hps = createHps(position, player.hp);
}
void updateHudCoin(vec2 position, Entity hudEntity, Entity playerEntity) {
	HUD& hud = registry.huds.get(hudEntity);
	Player& player = registry.players.get(playerEntity);
	PlayerStat& playerStat = registry.playerStats.get(player.playerStat);
	while (!hud.coinCount.empty()) {
		registry.remove_all_components_of(hud.coinCount.back());
		hud.coinCount.pop_back();
	}
	vec2 coinCountPosition = vec2(position.x + (2 * HUD_COIN_BB_WIDTH + (NUMBER_BB_WIDTH / 2)) * defaultResolution.scaling, position.y + HUD_HP_BB_HEIGHT * defaultResolution.scaling);
	hud.coinCount = createNumber(coinCountPosition, playerStat.money);
}

void updateHudHp(PlayerCharacter player) {
	if (player == KNIGHT) {
		Entity player_entity = registry.players.entities.front();
		if (gameHud.currentLocation == BATTLE_ROOM) {
			updateHudHp(gameHud.playerOneBattleRoomLocation, gameHud.playerOneHudEntity, player_entity);
		}
		else {
			updateHudHp(gameHud.playerOneShopRoomLocation, gameHud.playerOneHudEntity, player_entity);
		}
	}
	else {
		Entity player_entity = registry.players.entities.back();
		if (gameHud.currentLocation == BATTLE_ROOM) {
			updateHudHp(gameHud.playerTwoBattleRoomLocation, gameHud.playerTwoHudEntity, player_entity);
		}
		else {
			updateHudHp(gameHud.playerTwoShopRoomLocation, gameHud.playerTwoHudEntity, player_entity);
		}
	}
}
void updateHudCoin(PlayerCharacter player) {
	if (player == KNIGHT) {
		Entity player_entity = registry.players.entities.front();
		if (gameHud.currentLocation == BATTLE_ROOM) {
			updateHudCoin(gameHud.playerOneBattleRoomLocation, gameHud.playerOneHudEntity, player_entity);
		}
		else {
			updateHudCoin(gameHud.playerOneShopRoomLocation, gameHud.playerOneHudEntity, player_entity);
		}
	}
	else {
		Entity player_entity = registry.players.entities.back();
		if (gameHud.currentLocation == BATTLE_ROOM) {
			updateHudCoin(gameHud.playerTwoBattleRoomLocation, gameHud.playerTwoHudEntity, player_entity);
		}
		else {
			updateHudCoin(gameHud.playerTwoShopRoomLocation, gameHud.playerTwoHudEntity, player_entity);
		}
	}
}

void HUDLocationSwitch(Entity hudEntity) {
	if (gameHud.currentLocation == BATTLE_ROOM) {
		moveHudFromShopToBattleRoom(hudEntity);
	}
	else {
		moveHudFromBattleToShopRoom(hudEntity);
	}
}

void moveHudFromShopToBattleRoom(Entity hudEntity) {
	HUD& hud = registry.huds.get(hudEntity);
	registry.motions.get(hud.headShot).position.y -= defaultResolution.defaultHeight;
	registry.motions.get(hud.coin).position.y -= defaultResolution.defaultHeight;
	for (Entity numberEntity : hud.coinCount) {
		registry.motions.get(numberEntity).position.y -= defaultResolution.defaultHeight;
	}
	for (Entity hpEntity : hud.hps) {
		registry.motions.get(hpEntity).position.y -= defaultResolution.defaultHeight;
	}
}

void moveHudFromBattleToShopRoom(Entity hudEntity) {
	HUD& hud = registry.huds.get(hudEntity);
	registry.motions.get(hud.headShot).position.y += defaultResolution.defaultHeight;
	registry.motions.get(hud.coin).position.y += defaultResolution.defaultHeight;
	for (Entity numberEntity : hud.coinCount) {
		registry.motions.get(numberEntity).position.y += defaultResolution.defaultHeight;
	}
	for (Entity hpEntity : hud.hps) {
		registry.motions.get(hpEntity).position.y += defaultResolution.defaultHeight;
	}
}
