// internal
#include "physics_system.hpp"
#include "world_init.hpp"

void PhysicsSystem::step(float elapsed_ms, float window_width_px, float window_height_px)
{
	moveEntities(elapsed_ms);
	checkForCollision();
	drawDebugMode();
}

void PhysicsSystem::handle_collision() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions;
	Title& title = registry.titles.components[0];
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		// Checking collision of projectiles with other entities (enemies or enemies run)
		if (registry.projectiles.has(entity)) {
			if (registry.enemies.has(entity_other)) {
				Enemy& enemyCom = registry.enemies.get(entity_other);
				Player& playerCom = registry.players.get(registry.projectiles.get(entity).belongToPlayer);
				PlayerStat& playerStatCom = registry.playerStats.get(playerCom.playerStat);
				registry.remove_all_components_of(entity);
				if (!enemyCom.isInvin) {
					enemyCom.hp -= playerStatCom.damage;
					if (enemyCom.hp <= 0) {
						playerStatCom.money += enemyCom.loot;
						title.p2money = playerStatCom.money;
						title.updateWindowTitle();
						registry.remove_all_components_of(entity_other);
					} else {
						enemyCom.isInvin = true;
						enemyCom.invinTimerInMs = enemyCom.invinFrame;
						enemyHitHandling(entity_other);
					}
				}
			}
		}

		if (registry.swords.has(entity)) {
			if (registry.enemies.has(entity_other)) {
				Enemy& enemyCom = registry.enemies.get(entity_other);
				Player& playerCom = registry.players.get(registry.swords.get(entity).belongToPlayer);
				PlayerStat& playerStatCom = registry.playerStats.get(playerCom.playerStat);
				if (!enemyCom.isInvin) {
					enemyCom.hp -= playerStatCom.damage;
					if (enemyCom.hp <= 0) {
						playerStatCom.money += enemyCom.loot;
						title.p1money = playerStatCom.money;
						title.updateWindowTitle();
						registry.remove_all_components_of(entity_other);
					}
					else {
						enemyCom.isInvin = true;
						enemyCom.invinTimerInMs = enemyCom.invinFrame;
						enemyHitHandling(entity_other);
					}
				}
			}
		}

		// Checking collision of enemies or enemies run with walls or blocks
		if (registry.enemies.has(entity)) {
			if (registry.blocks.has(entity_other) || registry.walls.has(entity_other)) {
				// start a timer and pass the enemies/enemies run's current position
				if (!registry.stuckTimers.has(entity)) {
					registry.stuckTimers.emplace(entity);
					registry.stuckTimers.get(entity).stuck_pos = vec2(registry.motions.get(entity).position.x, registry.motions.get(entity).position.y);
				}
			}
		}

		if (registry.powerups.has(entity)) {
			if (registry.players.has(entity_other)) {
				//Deduct if money is available
				Player& playerCom = registry.players.get(entity_other);
				PlayerStat& playerStatCom = registry.playerStats.get(playerCom.playerStat);
				// TODO: Implement buying power up
				// TODO: Update player money in title when buying power up
			}
		}

		if (registry.players.has(entity)) {
			// Check Player - Enemy collisions 
			if (registry.enemies.has(entity_other)) {
				int enemyDamage = registry.enemies.get(entity_other).damage;
				resolvePlayerDamage(entity, enemyDamage);
			}
			else if (registry.enemyProjectiles.has(entity_other)) {
				Entity enemyEntity = registry.enemyProjectiles.get(entity_other).belongToEnemy;
				if (registry.enemies.has(enemyEntity)) {
					int enemyDamage = registry.enemies.get(enemyEntity).damage;
					resolvePlayerDamage(entity, enemyDamage);
					registry.remove_all_components_of(entity_other);
				}
			}
		}
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

void PhysicsSystem::resolvePlayerDamage(Entity playerEntity, int enemyDamage) {
	Title& title = registry.titles.components[0];
	Player& player = registry.players.get(playerEntity);
	if (!player.isInvin) {
		player.hp -= enemyDamage;
		// if hp - 1 is <= 0 then initiate death unless already dying 
		if (player.hp <= 0) {
			player.hp = 0;
			player.isDead = true;
		}
		else {
			player.isInvin = true;
			player.invinTimerInMs = player.invinFrame;
			if (twoPlayer.inTwoPlayerMode && playerEntity.getId() == registry.players.entities.back().getId()) {
				registry.renderRequests.remove(playerEntity);
				registry.renderRequests.insert(
					playerEntity,
					{ TEXTURE_ASSET_ID::WIZARDHURT,
						EFFECT_ASSET_ID::TEXTURED,
						GEOMETRY_BUFFER_ID::SPRITE });
				registry.wizardAnimations.get(playerEntity).isAnimatingHurt = true;
			}
			else {
				// TODO: Implement knight hit animation with fragment shader
			}
		}
	}
	if (playerEntity.getId() == registry.players.entities[0].getId()) 
		title.p1hp = player.hp;
	else if (twoPlayer.inTwoPlayerMode)
		title.p2hp = player.hp;
	
	title.updateWindowTitle();
}

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 PhysicsSystem::get_bounding_box(const Motion& motion)
{
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

vec3 PhysicsSystem::transformVertex(Motion& motion, ColoredVertex vertex) {
	Transform transform;
	transform.translate(motion.position);
	transform.rotate(motion.angle);
	transform.scale(vec2(motion.scale.x, motion.scale.y));
	return transform.mat * vertex.position;
}

bool PhysicsSystem::doesRadiusCollide(const Motion& motion, const Motion& other_motion) {
	vec2 dp = motion.position - other_motion.position;
	float dist_squared = dot(dp, dp);
	const vec2 other_bonding_box = get_bounding_box(motion) / 2.f;
	const float other_r_squared = dot(other_bonding_box, other_bonding_box);
	const vec2 my_bonding_box = get_bounding_box(other_motion) / 2.f;
	const float my_r_squared = dot(my_bonding_box, my_bonding_box);
	const float r_squared = max(other_r_squared, my_r_squared);
	if (dist_squared < r_squared)
		return true;
	return false;
}

bool PhysicsSystem::isMeshInBoundingBox(const Entity entity, const Entity other_entity) {
	Mesh* hitbox = registry.hitboxes.get(entity);
	Motion& motion = registry.motions.get(entity);
	Motion& other_motion = registry.motions.get(other_entity);
	if (!doesRadiusCollide(motion, other_motion))
		return false;
	for (const ColoredVertex vertex : hitbox->vertices) {
		vec3 transformed_vertex = transformVertex(motion, vertex);
		const Motion& other_motion = registry.motions.get(other_entity);
		const vec2 bounding_box = get_bounding_box(other_motion);
		float left_position = other_motion.position.x - bounding_box.x / 2;
		float right_position = other_motion.position.x + bounding_box.x / 2;
		float up_position = other_motion.position.y - bounding_box.y / 2;
		float down_position = other_motion.position.y + bounding_box.y / 2;
		if (transformed_vertex.x + motion.position.x >= left_position &&
			transformed_vertex.y + motion.position.y >= up_position &&
			transformed_vertex.x + motion.position.x <= right_position &&
			transformed_vertex.y + motion.position.y <= down_position)
			return true;
	}
	return false;
}

vec2 PhysicsSystem::alignNextPositionToBoundingBox(vec2 nextPosition, const Motion& motion) {
	const vec2 bounding_box = get_bounding_box(motion) / 2.f;
	if (motion.velocity.x > 0) {
		nextPosition.x += bounding_box.x;
	}
	else if (motion.velocity.x < 0) {
		nextPosition.x -= bounding_box.x;
	}
	if (motion.velocity.y > 0) {
		nextPosition.y += bounding_box.y;
	}
	else if (motion.velocity.y < 0) {
		nextPosition.y -= bounding_box.y;
	}
	return nextPosition;
}

bool PhysicsSystem::collides(const Entity entity, const Entity other_entity)
{
	if (registry.hitboxes.has(entity)) {
		return isMeshInBoundingBox(entity, other_entity);
	}
	else if (registry.hitboxes.has(other_entity)) {
		return isMeshInBoundingBox(other_entity, entity);
	}
	const Motion& motion = registry.motions.get(entity);
	const Motion& other_motion = registry.motions.get(other_entity);
	return doesRadiusCollide(motion, other_motion);
}

bool PhysicsSystem::blockCollides(vec2 nextPosition, const Motion& block, const Motion& motion) {
	nextPosition = alignNextPositionToBoundingBox(nextPosition, motion);
	vec2 dp = nextPosition - block.position;
	float dist_squared = dot(dp, dp);
	const vec2 wall_bonding_box = get_bounding_box(block) / 2.f;
	const float r_squared = dot(wall_bonding_box, wall_bonding_box);
	if (dist_squared < r_squared)
		return true;
	return false;

}

bool PhysicsSystem::wallCollides(vec2 nextPosition, Entity wall, const Motion& motion) {
	nextPosition = alignNextPositionToBoundingBox(nextPosition, motion);
	Motion& wallMotion = registry.motions.get(wall);
	vec2 wallPos = wallMotion.position;
	vec2 wallScale = wallMotion.scale;
	float left = wallPos.x - (wallScale.x / 2);
	float right = wallPos.x + (wallScale.x / 2);
	float top = wallPos.y - (wallScale.y / 2);
	float bottom = wallPos.y + (wallScale.y / 2);
	if (nextPosition.y >= top && nextPosition.y <= bottom && nextPosition.x >= left && nextPosition.x <= right) {
		return true;
	}
	return false;
}

void PhysicsSystem::drawMeshDebug(const Entity entity) {
	Mesh* hitbox = registry.hitboxes.get(entity);
	Motion& motion = registry.motions.get(entity);
	for (const ColoredVertex vertex : hitbox->vertices) {
		vec3 transformed_vertex = transformVertex(motion, vertex);
		Entity vertex_line = createLine(vec2(transformed_vertex.x, transformed_vertex.y) + motion.position, { 4, 4 });
	}
}

void PhysicsSystem::drawBoundingBoxDebug(const Motion& motion) {
	const vec2 bounding_box = get_bounding_box(motion);
	vec2 horizontal_scale = { bounding_box.x,2 };
	vec2 vertical_scale = { 2,bounding_box.y };
	vec2 left_position = motion.position;
	left_position.x -= bounding_box.x / 2;
	vec2 right_position = motion.position;
	right_position.x += bounding_box.x / 2;
	vec2 up_position = motion.position;
	up_position.y -= bounding_box.y / 2;
	vec2 down_position = motion.position;
	down_position.y += bounding_box.y / 2;
	Entity left_line = createLine(left_position, vertical_scale);
	Entity right_line = createLine(right_position, vertical_scale);
	Entity up_line = createLine(up_position, horizontal_scale);
	Entity down_line = createLine(down_position, horizontal_scale);
}

void PhysicsSystem::bounceEnemyRun(Entity curEntity) {
	// if enemyrun within MAX_DIST_WZ_EN of either wizard
		// change enemyrun direction 
	if (registry.enemiesrun.has(curEntity)) {
		Motion& motion_en = registry.motions.get(curEntity);
		Enemy& enemyCom = registry.enemies.get(curEntity);
		for (uint k = 0; k < registry.players.size(); k++) {
			Motion& motion_wz = registry.motions.get(registry.players.entities[k]);
			vec2 dp = motion_en.position - motion_wz.position;
			float dist_squared = dot(dp, dp);
			if (dist_squared < registry.enemiesrun.get(curEntity).max_dist_wz_en_run) {
				if (motion_en.velocity.x > 0) {
					if (motion_en.velocity.y > 0) {
						motion_en.velocity.x = -1 * enemyCom.speed;
						motion_en.velocity.y = enemyCom.speed;
					}
					else {
						motion_en.velocity.x = enemyCom.speed;
						motion_en.velocity.y = enemyCom.speed;
					}
				}
				else {
					if (motion_en.velocity.y > 0) {
						motion_en.velocity.x = -1 * enemyCom.speed;
						motion_en.velocity.y = -1 * enemyCom.speed;
					}
					else {
						motion_en.velocity.x = enemyCom.speed;
						motion_en.velocity.y = -1 * enemyCom.speed;
					}
				}
			}
		}
	}
}

void PhysicsSystem::bounceEnemies(Entity curEntity, bool hitABlock) {
	// check if fireball/projectile hit a wall/block, if so remove it
		// if enemy hit a wall/block, revert moving direction
	if (hitABlock) {
		if (registry.projectiles.has(curEntity) || registry.enemyProjectiles.has(curEntity)) {
			registry.remove_all_components_of(curEntity);
		}
		else if (registry.enemyBlobs.has(curEntity)) {
			Motion& enemyMotion = registry.motions.get(curEntity);
			enemyMotion.velocity.y *= -1;
		}
		else if (registry.enemiesrun.has(curEntity) || registry.enemyChase.has(curEntity)) {
			Motion& enemyRunMotion = registry.motions.get(curEntity);
			Enemy& enemyCom = registry.enemies.get(curEntity);
			if (enemyRunMotion.velocity.x > 0) {
				if (enemyRunMotion.velocity.y > 0) {
					enemyRunMotion.velocity.x = -1 * enemyCom.speed;
					enemyRunMotion.velocity.y = enemyCom.speed;
				}
				else {
					enemyRunMotion.velocity.x = enemyCom.speed;
					enemyRunMotion.velocity.y = enemyCom.speed;
				}
			}
			else {
				if (enemyRunMotion.velocity.y > 0) {
					enemyRunMotion.velocity.x = -1 * enemyCom.speed;
					enemyRunMotion.velocity.y = -1 * enemyCom.speed;
				}
				else {
					enemyRunMotion.velocity.x = enemyCom.speed;
					enemyRunMotion.velocity.y = -1 * enemyCom.speed;
				}
			}
		}
		else if (registry.enemies.has(curEntity)) {
			Motion& motion = registry.motions.get(curEntity);
			float wallBounceVelocityDecreaseFactor = -0.8f;
			motion.velocity = vec2(motion.velocity.x * wallBounceVelocityDecreaseFactor, motion.velocity.y * wallBounceVelocityDecreaseFactor);
		}

	}
}

bool PhysicsSystem::hitBlockOrWall(vec2 nextPosition, Motion& motion) {
	bool hitABlock = false;
	for (uint j = 0; j < registry.blocks.size(); j++) {
		Entity blockEntity = registry.blocks.entities[j];
		if (blockCollides(nextPosition, registry.motions.get(blockEntity), motion)) {
			hitABlock = true;
		}
	}

	for (uint j = 0; j < registry.walls.size(); j++) {
		Entity wallEntity = registry.walls.entities[j];
		if (wallCollides(nextPosition, wallEntity, motion)) {
			hitABlock = true;
		}
	}
	return hitABlock;
}

void PhysicsSystem::moveEntities(float elapsed_ms) {
	for (uint i = 0; i < registry.motions.size(); i++)
	{
		Motion& motion = registry.motions.components[i];
		Entity entity = registry.motions.entities[i];
		float step_seconds = 1.0f * (elapsed_ms / 1000.f);
		vec2 nextPosition = vec2(motion.position.x + motion.velocity.x * step_seconds,
			motion.position.y + motion.velocity.y * step_seconds);
		bool hitABlock = hitBlockOrWall(nextPosition, motion);
		if (!hitABlock) {
			motion.position = nextPosition;
		}
		bounceEnemies(entity, hitABlock);
		bounceEnemyRun(entity);
		rotateSword(entity, elapsed_ms);
	}
}

void PhysicsSystem::rotateSword(Entity entity, float elapsed_ms) {
	float pivot_distance_modifier = 3.f / 4.f;
	float step_seconds = 1.0f * (elapsed_ms / 1000.f);
	for (Entity entity : registry.swords.entities) {
		Sword& sword = registry.swords.get(entity);
		if (registry.players.has(sword.belongToPlayer)) {
			Motion& parent_motion = registry.motions.get(sword.belongToPlayer);
			Motion& motion = registry.motions.get(entity);
			vec2 pivot = parent_motion.position;
			pivot.x += SWORD_BB_WIDTH * pivot_distance_modifier;
			motion.angle += sword.angular_velocity * step_seconds;
			Transform T;
			T.translate(parent_motion.position);
			Transform R;
			R.rotate(motion.angle);
			Transform T_inv;
			T_inv.translate(-parent_motion.position);
			mat3 matrix = T.mat * R.mat * T_inv.mat;
			vec3 world_coord = matrix * vec3(pivot.x, pivot.y, 1);
			motion.position = vec2(world_coord.x, world_coord.y);
			sword.distance_traveled += sword.angular_velocity * step_seconds;
			if (sword.distance_traveled > sword.max_distance)
				registry.remove_all_components_of(entity);
		}
		else {
			registry.remove_all_components_of(entity);
		}
		
	}
}

void PhysicsSystem::drawDebugMode() {
	// debugging of bounding boxes
	if (debugging.in_debug_mode)
	{
		uint size_before_adding_new = (uint)registry.motions.components.size();
		for (uint i = 0; i < size_before_adding_new; i++)
		{
			Motion& motion_i = registry.motions.components[i];
			Entity entity_i = registry.motions.entities[i];

			if (registry.hitboxes.has(entity_i)) {
				drawMeshDebug(entity_i);
			}
			if (!registry.walls.has(entity_i)) {
				drawBoundingBoxDebug(motion_i);
			}
		}
	}
}

void PhysicsSystem::checkForCollision() {
	// Check for collisions between all moving entities
	ComponentContainer<Motion> &motion_container = registry.motions;
	for (uint i = 0; i < motion_container.components.size(); i++)
	{
		Motion& motion = motion_container.components[i];
		Entity entity = motion_container.entities[i];
		for (uint j = 0; j < motion_container.components.size(); j++) // i+1
		{
			if (i == j)
				continue;
			Entity other_entity = motion_container.entities[j];
			Motion& other_motion = motion_container.components[j];
			if (collides(entity, other_entity))
			{
				// Create a collisions event
				// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
				registry.collisions.emplace_with_duplicates(entity, other_entity);
				registry.collisions.emplace_with_duplicates(other_entity, entity);
			}
		}
	}
}

void PhysicsSystem::enemyHitHandling(Entity enemyEntity) {
	if (registry.enemyHunters.has(enemyEntity)) {
		registry.renderRequests.remove(enemyEntity);
		registry.renderRequests.insert(
			enemyEntity,
			{ TEXTURE_ASSET_ID::ENEMYHUNTERHURT,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		registry.enemyHunters.get(enemyEntity).isAnimatingHurt = true;
	}
	else if (registry.enemySwarms.has(enemyEntity)) {
		registry.renderRequests.remove(enemyEntity);
		registry.renderRequests.insert(
			enemyEntity,
			{ TEXTURE_ASSET_ID::ENEMYSWARMHURT,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		registry.enemySwarms.get(enemyEntity).isAnimatingHurt = true;
	}
	else {
		// TODO:: Implement enemy hit handling for rest of the enemies with fragment shaders
	}
}