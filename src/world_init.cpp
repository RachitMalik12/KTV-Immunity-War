#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"


Entity createWizard(RenderSystem* renderer, vec2 position) {
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
	motion.scale = vec2({ WIZARD_BB_WIDTH * defaultResolution.scaling, WIZARD_BB_HEIGHT * defaultResolution.scaling });

	registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::WIZARD,
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
			EFFECT_ASSET_ID::PEBBLE,
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
			EFFECT_ASSET_ID::PEBBLE,
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

Entity createEnemyBlob(RenderSystem* renderer, vec2 position, vec2 velocity)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = velocity;
	motion.position = position;

	motion.scale = vec2({ ENEMY_BB_WIDTH * defaultResolution.scaling, ENEMY_BB_HEIGHT * defaultResolution.scaling });

	registry.enemies.emplace(entity);
	registry.enemyBlobs.emplace(entity);
	// Set enemy attributes
	auto& enemyCom = registry.enemies.get(entity);
	enemyCom.damage = 1;
	enemyCom.hp = 3;
	enemyCom.loot = 1;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMY,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createEnemyRun(RenderSystem* renderer, vec2 position, vec2 velocity)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = velocity;
	motion.position = position;

	motion.scale = vec2({ ENEMYRUN_BB_WIDTH * defaultResolution.scaling, ENEMYRUN_BB_HEIGHT * defaultResolution.scaling });

	registry.enemies.emplace(entity);
	registry.enemiesrun.emplace(entity);
	// Set enemy attributes
	auto& enemyCom = registry.enemies.get(entity);
	enemyCom.damage = 1;
	enemyCom.hp = 2;
	enemyCom.loot = 1;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMYRUN,
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

Entity createPowerup(RenderSystem* renderer, vec2 position)
{   // Reserve an entity
	auto entity = Entity();

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = position;

	motion.scale = vec2({ POWERUP_BB_WIDTH * defaultResolution.scaling, POWERUP_BB_HEIGHT * defaultResolution.scaling });

	registry.powerups.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{  TEXTURE_ASSET_ID::POWERUP,
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
		 EFFECT_ASSET_ID::PEBBLE,
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

Entity createBox(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::PEBBLE,
		 GEOMETRY_BUFFER_ID::GRAYBOX });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = scale;

	registry.grayboxComponents.emplace(entity);
	return entity;
}
