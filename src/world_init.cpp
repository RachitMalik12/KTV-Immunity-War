#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

#include <random>

// Seeding rng with random device
std::default_random_engine rng = std::default_random_engine(std::random_device()());
std::uniform_real_distribution<float> uniform_dist;

Entity createWizard(RenderSystem* renderer, vec2 position) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);
	Mesh& hitbox = renderer->getMesh(GEOMETRY_BUFFER_ID::WIZARD);
	registry.hitboxes.emplace(entity, &hitbox);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = vec2({ WIZARD_BB_WIDTH * defaultResolution.scaling, WIZARD_BB_HEIGHT * defaultResolution.scaling });

	registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::WIZARD,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity createKnight(RenderSystem* renderer, vec2 position) {
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
	motion.scale = vec2({ KNIGHT_BB_WIDTH * defaultResolution.scaling, KNIGHT_BB_HEIGHT * defaultResolution.scaling });

	registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::KNIGHT,
			EFFECT_ASSET_ID::KNIGHT,
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
	if (enemyType == 0) {
		curEnemy = createEnemyBlob(renderer, position);
	} else if (enemyType == 1) {
		curEnemy = createEnemyRun(renderer, position);
	} else if (enemyType == 2) {
		curEnemy = createEnemyHunter(renderer, position);
	} else if (enemyType == 3) {
		curEnemy = createEnemyBacteria(renderer, position);
	}
	else if (enemyType == 4) {
		curEnemy = createEnemyChase(renderer, position);
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
	enemyCom.loot = 1;
	enemyCom.speed = 200.f * defaultResolution.scaling;
	motion.velocity = vec2(0.f, enemyCom.speed);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMY,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

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
	enemyCom.hp = 2;
	enemyCom.loot = 1;
	enemyCom.speed = 200.f * defaultResolution.scaling;
	motion.velocity = vec2(uniform_dist(rng) * enemyCom.speed, uniform_dist(rng) * enemyCom.speed);;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMYRUN,
			EFFECT_ASSET_ID::TEXTURED,
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
	enemyCom.hp = 5;
	enemyCom.loot = 2;
	enemyCom.speed = 200.f * defaultResolution.scaling;
	auto& hunterCom = registry.enemyHunters.get(entity);
	hunterCom.currentState = hunterCom.searchingMode;
	hunterCom.huntingRange = hunterCom.huntingRange * defaultResolution.scaling;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMYHUNTER,
			EFFECT_ASSET_ID::TEXTURED,
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
	enemyCom.hp = 5;
	enemyCom.loot = 4;
	enemyCom.speed = 200.f * defaultResolution.scaling;
	auto& bacteria = registry.enemyBacterias.get(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMYBACTERIA,
			EFFECT_ASSET_ID::TEXTURED,
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
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createProjectile(RenderSystem* renderer, vec2 pos, vec2 velocity, Entity playerEntity) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);
	Mesh& hitbox = renderer->getMesh(GEOMETRY_BUFFER_ID::FIREBALL);
	registry.hitboxes.emplace(entity, &hitbox);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = velocity;
	motion.position = pos;

	// Setting initial values
	motion.scale = vec2({ FIREBALL_BB_WIDTH * defaultResolution.scaling, FIREBALL_BB_HEIGHT * defaultResolution.scaling });

	// fireball stuff
	registry.projectiles.emplace(entity);
	registry.projectiles.get(entity).belongToPlayer = playerEntity;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::FIREBALL,
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
	if (defaultResolution.scaling == 2) {
		motion.scale = vec2({ HELP_BB_WIDTH * 0.7, HELP_BB_HEIGHT*0.7 });
	}
	else {
		motion.scale = vec2({ HELP_BB_WIDTH * defaultResolution.scaling, HELP_BB_HEIGHT * defaultResolution.scaling });
	}

	registry.helpModes.emplace(entity);

	return entity;
}