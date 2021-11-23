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

Entity createWizard(RenderSystem* renderer, vec2 position) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
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
	enemyCom.loot = 1;
	enemyCom.speed = 200.f * defaultResolution.scaling;
	motion.velocity = vec2(0.f, enemyCom.speed);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMY,
			EFFECT_ASSET_ID::ENEMY,
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
	enemyCom.max_hp = enemyCom.hp;
	enemyCom.loot = 1;
	enemyCom.speed = 200.f * defaultResolution.scaling;
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
	enemyCom.hp = 5;
	enemyCom.max_hp = enemyCom.hp;
	enemyCom.loot = 2;
	enemyCom.speed = 200.f * defaultResolution.scaling;
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
	enemyCom.hp = 5;
	enemyCom.max_hp = enemyCom.hp;
	enemyCom.loot = 4;
	enemyCom.speed = 200.f * defaultResolution.scaling;
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
	enemyCom.hp = 5;
	enemyCom.loot = 4;
	enemyCom.speed = 200.f * defaultResolution.scaling;
	auto& germ = registry.enemyGerms.get(entity);
	germ.mode = (rand() % 10) + 1;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::GERM,
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
	enemyCom.hp = 3;
	enemyCom.max_hp = enemyCom.hp;
	enemyCom.loot = 1;
	enemyCom.speed = 100.f * defaultResolution.scaling;
	motion.velocity = vec2(0, 0);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMYSWARM,
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
		  EFFECT_ASSET_ID::TEXTURED,
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
		  EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = vec2({ DAMAGE_POWERUP_WIDTH * defaultResolution.scaling, DAMAGE_POWERUP_HEIGHT * defaultResolution.scaling });

	registry.damagePowerUp.emplace(entity);
	Powerup& powerup = registry.powerups.emplace(entity);
	powerup.cost = 10;

	return entity;
}
Entity createAttackSpeedPowerup(vec2 position) {
	Entity entity = Entity();

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ATTACKPOWERUP,
		  EFFECT_ASSET_ID::TEXTURED,
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
		  EFFECT_ASSET_ID::TEXTURED,
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
	* Render a number between 0 and 99 inclusive on screen at the desierd position. If input number isn't in the correct range, render 0 instead.
	*
	* @param  singleDigitNumber   the number to be displayed on screen, non-negative single digit or double digits only, meaning 0 - 99
	* @return					  the newly created entity
	*/
std::vector<Entity> createNumber(RenderSystem* renderer, vec2 position, int number) {
	std::vector<Entity> numberEntity;
	if (number < 0 || number > 99) {
		numberEntity.push_back(createSingleDigitNumber(renderer, position, 0));
		return numberEntity;
	}
	if (number < 10) {
		numberEntity.push_back(createSingleDigitNumber(renderer, position, number));
		return numberEntity;
	}
	else {
		return createDoubleDigitNumber(renderer, position, number);
	}
}

Entity createSingleDigitNumber(RenderSystem* renderer, vec2 position, int singleDigitNumber) {
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

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

std::vector<Entity> createDoubleDigitNumber(RenderSystem* renderer, vec2 position, int doubleDigitNumber) {
	std::vector<Entity> numberEntity;
	Entity firstNumberEntity = createSingleDigitNumber(renderer, vec2(position.x + (NUMBER_BB_WIDTH * defaultResolution.scaling / 2), position.y), doubleDigitNumber % 10);
	Entity secondNumberEntity = createSingleDigitNumber(renderer, vec2(position.x - (NUMBER_BB_WIDTH * defaultResolution.scaling / 2), position.y), doubleDigitNumber / 10);
	numberEntity.push_back(firstNumberEntity);
	numberEntity.push_back(secondNumberEntity);
	return numberEntity;
}