// internal
#include "ai_system.hpp"

void AISystem::step(float elapsed_ms, float width, float height) {
	stepEnemyHunter(elapsed_ms);
	stepEnemyBacteria(elapsed_ms, width, height);
	stepEnemyChase(elapsed_ms);
	stepEnemySwarm(elapsed_ms);
	stepEnemyCoord(elapsed_ms, width, height);
	stepEnemyGerm(elapsed_ms);
	stepEnemyAStar(elapsed_ms, width, height);
	stepEnemyBoss(elapsed_ms);
}

void AISystem::stepEnemyHunter(float elapsed_ms) {
	for (Entity hunterEntity : registry.enemyHunters.entities) {
		EnemyHunter& hunter = registry.enemyHunters.get(hunterEntity);
		Enemy& hunterStatus = registry.enemies.get(hunterEntity);
		if (!hunterStatus.isDead) {
			if (hunterStatus.hp <= 2) {
				hunter.currentState = hunter.fleeingMode;
			}
			if (hunter.currentState == hunter.fleeingMode && hunter.isFleeing == false) {
				registry.motions.get(hunterEntity).velocity = vec2(2.0f * hunterStatus.speed, 0);
				hunter.isFleeing = true;
				registry.renderRequests.remove(hunterEntity);
				registry.renderRequests.insert(
					hunterEntity,
					{ TEXTURE_ASSET_ID::ENEMYHUNTERFLEE,
						EFFECT_ASSET_ID::ENEMY,
						GEOMETRY_BUFFER_ID::SPRITE });
				hunter.isAnimatingHurt = false;
			}
			else {
				if (hunter.timeToUpdateAi) {
					if (hunter.currentState == hunter.searchingMode) {
						if (isEnemyInRangeOfThePlayers(hunterEntity)) {
							hunter.currentState = hunter.huntingMode;
							registry.renderRequests.remove(hunterEntity);
							registry.renderRequests.insert(
								hunterEntity,
								{ TEXTURE_ASSET_ID::ENEMYHUNTERMAD,
									EFFECT_ASSET_ID::ENEMY,
									GEOMETRY_BUFFER_ID::SPRITE });
							hunter.isAnimatingHurt = false;
						}
						else {
							setEnemyWonderingRandomly(hunterEntity);
						}
					}
					if (hunter.currentState == hunter.huntingMode) {
						setEnemyChasingThePlayer(hunterEntity);
					}
					hunter.timeToUpdateAi = false;
					hunter.aiUpdateTimer = hunter.aiUpdateTime;
				}
				else {
					hunter.aiUpdateTimer -= elapsed_ms;
					if (hunter.aiUpdateTimer < 0) {
						hunter.timeToUpdateAi = true;
					}
				}
			}
			resolveHunterAnimation(hunterEntity, hunterStatus, hunter);
		}
	}
}

void AISystem::resolveHunterAnimation(Entity hunterEntity, Enemy& hunterStatus, EnemyHunter& hunter) {
	if (hunter.isAnimatingHurt && !hunterStatus.isInvin) {
		if (hunter.currentState == hunter.searchingMode) {
			registry.renderRequests.remove(hunterEntity);
			registry.renderRequests.insert(
				hunterEntity,
				{ TEXTURE_ASSET_ID::ENEMYHUNTER,
					EFFECT_ASSET_ID::ENEMY,
					GEOMETRY_BUFFER_ID::SPRITE });
			hunter.isAnimatingHurt = false;
		}
		else if (hunter.currentState == hunter.huntingMode) {
			registry.renderRequests.remove(hunterEntity);
			registry.renderRequests.insert(
				hunterEntity,
				{ TEXTURE_ASSET_ID::ENEMYHUNTERMAD,
					EFFECT_ASSET_ID::ENEMY,
					GEOMETRY_BUFFER_ID::SPRITE });
			hunter.isAnimatingHurt = false;
		}
		else {
			// hunter.fleeingMode
			registry.renderRequests.remove(hunterEntity);
			registry.renderRequests.insert(
				hunterEntity,
				{ TEXTURE_ASSET_ID::ENEMYHUNTERFLEE,
					EFFECT_ASSET_ID::ENEMY,
					GEOMETRY_BUFFER_ID::SPRITE });
			hunter.isAnimatingHurt = false;
		}
	}
}

enum class BTState {
	Running,
	Success,
	Failure
};

// The base class representing any node in our behaviour tree
class BTNode {
public:
	virtual void init(Entity e) {};
	virtual BTState process(Entity e) = 0;
};

// A composite node that loops through all children and exits when one fails
class BTRunPair : public BTNode {
private:
	int m_index;
	BTNode* m_children[2];

public:
	BTRunPair(BTNode* c0, BTNode* c1)
		: m_index(0) {
		m_children[0] = c0;
		m_children[1] = c1;
	}

	void init(Entity e) override
	{
		m_index = 0;
		// initialize the first child
		const auto& child = m_children[m_index];
		child->init(e);
	}

	BTState process(Entity e) override {
		if (m_index >= 2)
			return BTState::Success;

		// process current child
		BTState state = m_children[m_index]->process(e);

		// select a new active child and initialize its internal state
		if (state == BTState::Success) {
			++m_index;
			if (m_index >= 2) {
				return BTState::Success;
			}
			else {
				m_children[m_index]->init(e);
				return BTState::Running;
			}
		}
		else {
			return state;
		}
	}
};

// A general decorator with lambda condition
// TAKES IN BTNode and CONDITION.
// If condition passes, it will run the child process. If not, finish by returning "success" and do not run child process.
class BTIfCondition : public BTNode
{
public:
	BTIfCondition(BTNode* child, std::function<bool(Entity)> condition)
		: m_child(child), m_condition(condition) {
	}

	virtual void init(Entity e) override {
		m_child->init(e);
	}

	virtual BTState process(Entity e) override {
		if (m_condition(e))
			return m_child->process(e);
		else
			return BTState::Success;
	}

private:
	BTNode* m_child;
	std::function<bool(Entity)> m_condition;
};


// LEAF NODE - has a prrocess that will be run if this node is met
class ChasePlayer : public BTNode {
private:
	void init(Entity e) override {
	}

	BTState process(Entity e) override {
		// modify world
		float finX = registry.motions.get(registry.players.entities[0]).position.x;
		float finY = registry.motions.get(registry.players.entities[0]).position.y;
		if (registry.players.entities.size() > 1 && registry.enemyGerms.get(e).mode <= registry.enemyGerms.get(e).playerChaseThreshold) {
			finX = registry.motions.get(registry.players.entities[1]).position.x;
			finY = registry.motions.get(registry.players.entities[1]).position.y;
		}
		float initX = registry.motions.get(e).position.x;
		float initY = registry.motions.get(e).position.y;

		vec2 diff = vec2(finX, finY) - vec2(initX, initY);
		float angle = atan2(diff.y, diff.x);
		registry.motions.get(e).velocity = vec2(cos(angle) * registry.enemies.get(e).speed, sin(angle) * registry.enemies.get(e).speed);
		// return progress
		return BTState::Success;
	}
};

// LEAF NODE - has a prrocess that will be run if this node is met
class Explode : public BTNode {
private:
	void init(Entity e) override {
	}
	BTState process(Entity e) override {
		// modify world
		if (registry.enemyGerms.get(e).explosionCountDown == 0) {
			registry.enemyGerms.get(e).explosionCountDown = registry.enemyGerms.get(e).explosionCountInit;
			float randomizedSpeedX = (rand() % 6) - 5; // randomized number for randomized velocity multiplier
			float randomizedSpeedY = (rand() % 6) - 5; // randomized number for randomized velocity multiplier
			if (registry.enemyGerms.get(e).mode <= registry.enemyGerms.get(e).playerChaseThreshold) {
				registry.motions.get(e).velocity.y = registry.enemies.get(e).speed * randomizedSpeedY;
				registry.motions.get(e).velocity.x = registry.enemies.get(e).speed * randomizedSpeedX;
			}
			else {
				registry.motions.get(e).velocity.x = registry.enemies.get(e).speed * randomizedSpeedX;
				registry.motions.get(e).velocity.y = registry.enemies.get(e).speed * randomizedSpeedY;
			}
		}
		else {
			registry.enemyGerms.get(e).explosionCountDown--;
		}
		// return progress
		return BTState::Success;
	}
};


void AISystem::stepEnemyGerm(float elapsed_ms) {
	for (Entity germEntity : registry.enemyGerms.entities) {
		EnemyGerm& germ = registry.enemyGerms.get(germEntity);
		germ.next_germ_behaviour_calculation -= elapsed_ms;
		if (germ.next_germ_behaviour_calculation < 0.f) {
			germ.next_germ_behaviour_calculation = germ.germBehaviourUpdateTime;

			// BTNode (leaf node, a process that will be run if reached)
			ChasePlayer chasePlayer;

			// creating condition for chasing player, if player(s) is/are alive
			std::function<bool(Entity)> conditionChasePlayer = [](Entity e)
			{
				if (registry.players.entities.size() > 1) {
					return !registry.players.get(registry.players.entities[0]).isDead && !registry.players.get(registry.players.entities[1]).isDead;
				}
				else {
					return !registry.players.get(registry.players.entities[0]).isDead;
				}
			};

			// BTNode (node that is a condition, and if condition is met, will run the child node that is passed in)
			BTIfCondition chase = BTIfCondition(&chasePlayer, conditionChasePlayer);
			// BTNode (leaf node, a process that will be run if reached)
			Explode explode;

			// creating condition for exploding, if one player has died
			std::function<bool(Entity)> conditionExplosion = [](Entity e)
			{
				if (registry.players.entities.size() > 1) {
					return registry.players.get(registry.players.entities[0]).isDead || registry.players.get(registry.players.entities[1]).isDead;
				}
				else {
					return registry.players.get(registry.players.entities[0]).isDead;
				}
			};

			// BTNode (node that is a condition, and if condition is met, will run the child node that is passed in)
			BTIfCondition explosion = BTIfCondition(&explode, conditionExplosion);

			// run the two BTNodes 
			BTRunPair root = BTRunPair(&chase, &explosion);
			root.init(germEntity);

			// iterate through all the different steps
			for (int i = 0; i < 2; i++) {
				// run processes (0 to 1 because there is 2 steps (BTIFCondition for chase and for explode)
				BTState state = root.process(germEntity);
				if (state != BTState::Running) {
					break;
				}
			}
		}
	}
}

// separate map into an 8x8 "grid"
void AISystem::createAdj() {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			visited[i][j] = false;
			pred[i][j] = { -1, -1 };
			adj[i][j] = { i, j };
		}
	}
	blocksInitialized = true;
}

void AISystem::stepEnemyBacteria(float elapsed_ms, float width, float height) {
	for (Entity bacteriaEntity : registry.enemyBacterias.entities) {
		Enemy& enemy = registry.enemies.get(bacteriaEntity);
		if (!enemy.isDead) {
			registry.enemyBacterias.get(bacteriaEntity).next_bacteria_BFS_calculation -= elapsed_ms;
			registry.enemyBacterias.get(bacteriaEntity).next_bacteria_PATH_calculation -= elapsed_ms;
			auto& motions_registry = registry.motions;
			if (registry.enemyBacterias.get(bacteriaEntity).next_bacteria_BFS_calculation < 0.f) {
				EnemyBacteria& bacteria = registry.enemyBacterias.get(bacteriaEntity);
				Motion& player1Motion = motions_registry.get(registry.players.entities[0]);

				// if bacteria is hunting, it will do BFS to find player
				if (bacteria.huntingMode) {
					bacteria.huntingMode = false;

					createAdj();

					// twoPlayerMode ? select random player to follow
					if (twoPlayer.inTwoPlayerMode) {
						registry.enemyBacterias.get(bacteriaEntity).next_bacteria_BFS_calculation = bacteria.bfsUpdateTime;
						Motion player2Motion = motions_registry.get(registry.players.entities[1]);
						float pickPlayer = rand() % 2 + 1;

						if (pickPlayer != 1 && !registry.players.get(registry.players.entities[1]).isDead) {
							registry.enemyBacterias.get(bacteriaEntity).finX = player2Motion.position.x;
							registry.enemyBacterias.get(bacteriaEntity).finY = player2Motion.position.y;

							handlePath(width, height, bacteriaEntity);
						}
						else {
							registry.enemyBacterias.get(bacteriaEntity).finX = player1Motion.position.x;
							registry.enemyBacterias.get(bacteriaEntity).finY = player1Motion.position.y;

							handlePath(width, height, bacteriaEntity);
						}
					}
					else {
						registry.enemyBacterias.get(bacteriaEntity).next_bacteria_BFS_calculation = bacteria.bfsUpdateTime;
						registry.enemyBacterias.get(bacteriaEntity).finX = player1Motion.position.x;
						registry.enemyBacterias.get(bacteriaEntity).finY = player1Motion.position.y;

						handlePath(width, height, bacteriaEntity);
					}
				}
			}
		}
	}
	for (Entity bacteriaEntity : registry.enemyBacterias.entities) {
		Enemy& enemy = registry.enemies.get(bacteriaEntity);
		if (!enemy.isDead) {
			if (registry.enemyBacterias.get(bacteriaEntity).next_bacteria_PATH_calculation < 0.f) {
				EnemyBacteria& bacteria = registry.enemyBacterias.get(bacteriaEntity);
				registry.enemyBacterias.get(bacteriaEntity).next_bacteria_PATH_calculation = bacteria.pathUpdateTime;
				findPath(bacteriaEntity);
			}
		}
	}

}

void AISystem::stepEnemyChase(float elapsed_ms) {
	// update enemy chase so it chases the player
	for (Entity entity : registry.enemyChase.entities) {
		EnemyChase& chase = registry.enemyChase.get(entity);
		Enemy& enemy = registry.enemies.get(entity);
		if (chase.timeToUpdateAi && !enemy.isDead) {
			auto& enemyCom = registry.enemies.get(entity);
			Motion& motion = registry.motions.get(entity);
			Entity playerEntity = pickAPlayer();
			Motion& playerMotion = registry.motions.get(playerEntity);

			// check if it is close to any other enemyChase
			// if yes, make it move in opposite direction for a certain time
			for (Entity other_enemy_chase : registry.enemyChase.entities) {
				if (other_enemy_chase != entity) {
					Motion& motion_other_en_chase = registry.motions.get(other_enemy_chase);
					vec2 dp = motion_other_en_chase.position - motion.position;
					float dist_squared = dot(dp, dp);
					if (dist_squared < registry.enemyChase.get(entity).enemy_chase_max_dist_sq) {
						// set encounter to true
						registry.enemyChase.get(entity).encounter = 1;
						motion.velocity = vec2{ dp.x * -1.f, dp.y * -1.f };
					}
					if (registry.enemyChase.get(entity).encounter == 1) {
						registry.enemyChase.get(entity).counter_ms -= elapsed_ms;
						if (registry.enemyChase.get(entity).counter_ms < 0) {
							vec2 chase_to_wz = vec2(playerMotion.position.x - motion.position.x, playerMotion.position.y - motion.position.y);
							float radians_for_angle = atan2f(-chase_to_wz.y, -chase_to_wz.x);
							float radians = atan2f(chase_to_wz.y, chase_to_wz.x);
							motion.angle = radians_for_angle;
							motion.velocity = vec2(enemyCom.speed * cos(-radians), enemyCom.speed * sin(radians));
							registry.enemyChase.get(entity).encounter == 0;
							registry.enemyChase.get(entity).counter_other_en_chase_ms = registry.enemyChase.get(entity).counter_other_en_chase_value;
						}
					}
				}
				else {
					registry.enemyChase.get(entity).counter_ms -= elapsed_ms;
					// reset timer and encounter variable when timer expires and
					// recalculate direction turtle is facing
					if (registry.enemyChase.get(entity).counter_ms < 0) {
						registry.enemyChase.get(entity).counter_ms = registry.enemyChase.get(entity).counter_value;
						vec2 chase_to_wz = vec2(playerMotion.position.x - motion.position.x, playerMotion.position.y - motion.position.y);
						float radians_for_angle = atan2f(-chase_to_wz.y, -chase_to_wz.x);
						float radians = atan2f(chase_to_wz.y, chase_to_wz.x);
						motion.angle = radians_for_angle;
						motion.velocity = vec2(enemyCom.speed * cos(-radians), enemyCom.speed * sin(radians));
					}
				}
				chase.timeToUpdateAi = false;
				chase.aiUpdateTimer = chase.aiUpdateTime;
			}
		}
		else {
			chase.aiUpdateTimer -= elapsed_ms;
			if (chase.aiUpdateTimer < 0) {
				chase.timeToUpdateAi = true;
			}
		}
	}
}

bool AISystem::handlePath(float width, float height, Entity& bacteriaEntity) {
	int positionX = registry.enemyBacterias.get(bacteriaEntity).finX;
	int positionY = registry.enemyBacterias.get(bacteriaEntity).finY;
	Motion playerMotion = registry.motions.get(registry.players.entities[0]);
	Motion bacteriaMotion = registry.motions.get(bacteriaEntity);

	// bacteria initial position (with respect to 8x8 grid)
	int resIndexX = bacteriaMotion.position.x / (width / 8);
	int resIndexY = bacteriaMotion.position.y / (height / 8);

	// final position (player position, with respect to 8x8 grid)
	int finXPosition = positionX / (width / 8);
	int finYPosition = positionY / (height / 8);

	// initialize first position, it will be visited later, add it to the queue
	visited[resIndexX][resIndexY] = false;
	registry.enemyBacterias.get(bacteriaEntity).adjacentsQueue.push({ resIndexX, resIndexY });
	std::pair<int, int> currPosition = registry.enemyBacterias.get(bacteriaEntity).adjacentsQueue.front();

	while (!registry.enemyBacterias.get(bacteriaEntity).adjacentsQueue.empty()) {
		// get first element in queue
		currPosition = registry.enemyBacterias.get(bacteriaEntity).adjacentsQueue.front();
		registry.enemyBacterias.get(bacteriaEntity).adjacentsQueue.pop();

		// check if it's out of bounds or has been visited. if so, skip
		if (currPosition.first < 8 && currPosition.second < 8 && visited[currPosition.first][currPosition.second] == false && currPosition.first >= 0 && currPosition.second >= 0) {

			// visited
			visited[currPosition.first][currPosition.second] = true;


			// We stop BFS when we find destination.
			if (currPosition.first == finXPosition && currPosition.second == finYPosition) {
				bfsSearchPath(resIndexX, resIndexY, finXPosition, finYPosition, bacteriaEntity, width, height);
				registry.enemyBacterias.get(bacteriaEntity).huntingMode = true;
				return true;
			}

			// add adjacent "grid" block above to queue
			if (currPosition.second - 1 >= 0) {
				if (visited[currPosition.first][currPosition.second - 1] == false) {
					registry.enemyBacterias.get(bacteriaEntity).adjacentsQueue.push({ currPosition.first, currPosition.second - 1 });
				}
				if (pred[currPosition.first][currPosition.second - 1].first == -1 && pred[currPosition.first][currPosition.second - 1].second == -1) {
					pred[currPosition.first][currPosition.second - 1] = { currPosition.first, currPosition.second };
				}
			}

			// add adjacent "grid" block below to queue
			if (currPosition.second + 1 < 8) {
				if (visited[currPosition.first][currPosition.second + 1] == false) {
					registry.enemyBacterias.get(bacteriaEntity).adjacentsQueue.push({ currPosition.first, currPosition.second + 1 });
				}
				if (pred[currPosition.first][currPosition.second + 1].first == -1 && pred[currPosition.first][currPosition.second + 1].second == -1) {
					pred[currPosition.first][currPosition.second + 1] = { currPosition.first, currPosition.second };
				}
			}

			// add adjacent "grid" block left to queue
			if (currPosition.first - 1 >= 0) {
				if (visited[currPosition.first - 1][currPosition.second] == false) {
					registry.enemyBacterias.get(bacteriaEntity).adjacentsQueue.push({ currPosition.first - 1, currPosition.second });
				}
				if (pred[currPosition.first - 1][currPosition.second].first == -1 && pred[currPosition.first - 1][currPosition.second].second == -1) {
					pred[currPosition.first - 1][currPosition.second] = { currPosition.first, currPosition.second };
				}
			}

			// add adjacent "grid" block right to queue
			if (currPosition.first + 1 < 8) {
				if (visited[currPosition.first + 1][currPosition.second] == false) {
					registry.enemyBacterias.get(bacteriaEntity).adjacentsQueue.push({ currPosition.first + 1, currPosition.second });
				}
				if (pred[currPosition.first + 1][currPosition.second].first == -1 && pred[currPosition.first + 1][currPosition.second].second == -1) {
					pred[currPosition.first + 1][currPosition.second] = { currPosition.first, currPosition.second };
				}
			}
		}

	}
	return false;

}

void AISystem::findPath(Entity& bacteriaEntity) {
	float finX = registry.enemyBacterias.get(bacteriaEntity).finX;
	float finY = registry.enemyBacterias.get(bacteriaEntity).finY;
	std::pair<int, int> currPosition = { finX , finY };
	// go through traversal stack. it should have the path now.
	// if it's empty, don't do anything.
	if (!registry.enemyBacterias.get(bacteriaEntity).traversalStack.empty()) {
		// get current position of traversal stack
		currPosition = registry.enemyBacterias.get(bacteriaEntity).traversalStack.top();
		registry.enemyBacterias.get(bacteriaEntity).traversalStack.pop();
		int bacteriaPositionX = registry.motions.get(bacteriaEntity).position.x;
		int bacteriaPositionY = registry.motions.get(bacteriaEntity).position.y;

		// from the current bacteria position, go to 
		moveToSpot(bacteriaPositionX, bacteriaPositionY, currPosition.first, currPosition.second, bacteriaEntity);
	}
}

void AISystem::bfsSearchPath(float initX, float initY, float finX, float finY, Entity& bacteriaEntity, float width, float height) {
	std::pair<int, int> currPosition = { finX , finY };

	// traverse from the final destination "grid" block
	// turn it back into the actually screen size using * (width/8) or * (height/8)
	// push into our traversalStack -- stack because we are now going BACKWARDS from the end to the beginning, using the predecessor to find our path from the player to the bacteria.
	while (currPosition.first != initX || currPosition.second != initY) {
		std::pair<int, int> temp = { pred[currPosition.first][currPosition.second].first * (width / 8),  pred[currPosition.first][currPosition.second].second * (height / 8) };
		registry.enemyBacterias.get(bacteriaEntity).traversalStack.push(temp);
		currPosition = pred[currPosition.first][currPosition.second];
	}

	while (!registry.enemyBacterias.get(bacteriaEntity).adjacentsQueue.empty())
	{
		registry.enemyBacterias.get(bacteriaEntity).adjacentsQueue.pop();
	}
}

void AISystem::moveToSpot(float initX, float initY, float finalX, float finalY, Entity& bacteriaEntity) {
	vec2 diff = vec2(finalX, finalY) - vec2(initX, initY);
	float angle = atan2(diff.y, diff.x);
	registry.motions.get(bacteriaEntity).velocity = vec2(cos(angle) * registry.enemies.get(bacteriaEntity).speed, sin(angle) * registry.enemies.get(bacteriaEntity).speed);
}

bool AISystem::isEnemyInRangeOfThePlayers(Entity enemyEntity) {
	Motion& enemyMotion = registry.motions.get(enemyEntity);
	float distance;
	if (twoPlayer.inTwoPlayerMode) {
		Motion& player1Motion = registry.motions.get(registry.players.entities.front());
		Motion& player2Motion = registry.motions.get(registry.players.entities.back());
		float distFromPlayer1 = enemyDistanceFromPlayer(player1Motion, enemyMotion);
		float distFromPlayer2 = enemyDistanceFromPlayer(player2Motion, enemyMotion);
		distance = std::min(distFromPlayer1, distFromPlayer2);
	}
	else {
		Motion& player1Motion = registry.motions.get(registry.players.entities.front());
		distance = enemyDistanceFromPlayer(player1Motion, enemyMotion);
	}
	if (distance < registry.enemyHunters.get(enemyEntity).huntingRange) {
		return true;
	}
	return false;
}

float AISystem::enemyDistanceFromPlayer(const Motion& player, const Motion& hunter) {
	vec2 dp = player.position - hunter.position;
	return sqrt(dot(dp, dp));
}

void AISystem::setEnemyWonderingRandomly(Entity enemyEntity) {
	Enemy& enemyStatus = registry.enemies.get(enemyEntity);
	float randomNumBetweenNegativeOneAndOne = (uniform_dist(rng) - 0.5) * 2;
	float anotherRandomNumBetweenNegativeOneAndOne = (uniform_dist(rng) - 0.5) * 2;
	vec2 randomVelocity =
		vec2(1.0f * enemyStatus.speed * randomNumBetweenNegativeOneAndOne,
			1.0f * enemyStatus.speed * anotherRandomNumBetweenNegativeOneAndOne);
	registry.motions.get(enemyEntity).velocity = randomVelocity;
}

void AISystem::setEnemyChasingThePlayer(Entity enemyEntity) {
	Enemy& enemyStatus = registry.enemies.get(enemyEntity);
	Motion& enemyMotion = registry.motions.get(enemyEntity);
	Entity playerToChase;
	if (twoPlayer.inTwoPlayerMode) {
		playerToChase = determineWhichPlayerToChase(enemyEntity);
	}
	else {
		playerToChase = registry.players.entities.front();
	}
	Motion& playerMotion = registry.motions.get(playerToChase);
	vec2 diff = playerMotion.position - enemyMotion.position;
	float angle = atan2(diff.y, diff.x);
	enemyMotion.velocity = vec2(cos(angle) * enemyStatus.speed, sin(angle) * enemyStatus.speed);
}

Entity AISystem::determineWhichPlayerToChase(Entity enemyEntity) {
	Motion& enemyMotion = registry.motions.get(enemyEntity);
	Motion& player1Motion = registry.motions.get(registry.players.entities.front());
	Motion& player2Motion = registry.motions.get(registry.players.entities.back());
	float distFromPlayer1 = enemyDistanceFromPlayer(player1Motion, enemyMotion);
	float distFromPlayer2 = enemyDistanceFromPlayer(player2Motion, enemyMotion);
	if (distFromPlayer1 < distFromPlayer2) {
		return registry.players.entities.front();
	}
	else {
		return registry.players.entities.back();
	}
}

void AISystem::stepEnemySwarm(float elapsed_ms) {
	for (Entity swarmEntity : registry.enemySwarms.entities) {
		EnemySwarm& swarm = registry.enemySwarms.get(swarmEntity);
		Enemy& swarmStatus = registry.enemies.get(swarmEntity);
		if (!swarmStatus.isDead) {
			if (swarm.timeToUpdateAi) {
				if (bossMode.currentBossLevel != STAGE2) {
					swarmSpreadOut(swarmEntity);
				}
				swarmFireProjectileAtPlayer(swarmEntity);
				swarm.timeToUpdateAi = false;
				swarm.aiUpdateTimer = 0;
			}
			else {
				swarm.aiUpdateTimer += elapsed_ms;
				if (swarm.aiUpdateTimer > swarm.aiUpdateTime) {
					swarm.timeToUpdateAi = true;
				}
			}

			if (swarm.isAnimatingHurt && !swarmStatus.isInvin) {
				registry.renderRequests.remove(swarmEntity);
				if (bossMode.currentBossLevel == STAGE1) {
					registry.renderRequests.insert(
						swarmEntity,
						{ TEXTURE_ASSET_ID::MINION,
							EFFECT_ASSET_ID::ENEMY,
							GEOMETRY_BUFFER_ID::SPRITE });
				}
				else {
					registry.renderRequests.insert(
						swarmEntity,
						{ TEXTURE_ASSET_ID::ENEMYSWARM,
							EFFECT_ASSET_ID::ENEMY,
							GEOMETRY_BUFFER_ID::SPRITE });
				}
				swarm.isAnimatingHurt = false;
			}
		}
	}
}

void AISystem::swarmSpreadOut(Entity swarmEntity) {
	if (registry.enemySwarms.entities.size() == 1) {
		setEnemyWonderingRandomly(swarmEntity);
	}
	Entity closestSwarmEntity = findClosestSwarm(swarmEntity);
	moveAwayfromOtherSwarm(swarmEntity, closestSwarmEntity);
}


void AISystem::moveAwayfromOtherSwarm(Entity enemyEntity, Entity otherEnemyEntity) {
	if (registry.motions.has(otherEnemyEntity)) {
		Motion& enemyMotion = registry.motions.get(enemyEntity);
		Motion& otherEnemyMotion = registry.motions.get(otherEnemyEntity);
		EnemySwarm& enemySwarm = registry.enemySwarms.get(enemyEntity);
		float distance = sqrt(pow(enemyMotion.position.x - otherEnemyMotion.position.x, 2) +
			pow(enemyMotion.position.y - otherEnemyMotion.position.y, 2));
		if (distance < enemySwarm.spreadOutDistance) {
			vec2 directionFromEnemyToOtherEnemy =
				vec2(otherEnemyMotion.position.x - enemyMotion.position.x, otherEnemyMotion.position.y - enemyMotion.position.y);
			vec2 oppositeOfDirection = vec2(directionFromEnemyToOtherEnemy.x * -1.f, directionFromEnemyToOtherEnemy.y * -1.f);
			vec2 normalizedDirection = vec2(oppositeOfDirection.x / sqrt(pow(oppositeOfDirection.x, 2) + pow(oppositeOfDirection.y, 2)),
				oppositeOfDirection.y / sqrt(pow(oppositeOfDirection.x, 2) + pow(oppositeOfDirection.y, 2)));
			Enemy& enemyStatus = registry.enemies.get(enemyEntity);
			enemyMotion.velocity = vec2(normalizedDirection.x * enemyStatus.speed, normalizedDirection.y * enemyStatus.speed);
		}
		else {
			setEnemyWonderingRandomly(enemyEntity);
		}
	}
	else {
		setEnemyWonderingRandomly(enemyEntity);
	}
}

Entity AISystem::findClosestSwarm(Entity swarmEntity) {
	Motion& swarmMotion = registry.motions.get(swarmEntity);
	float shortestDistance = std::numeric_limits<float>::max();
	Entity closestSwarmEntity;
	for (Entity otherSwarmEntity : registry.enemySwarms.entities) {
		if (swarmEntity.getId() != otherSwarmEntity.getId()) {
			Motion& otherSwarmMotion = registry.motions.get(otherSwarmEntity);
			float distance = sqrt(pow(swarmMotion.position.x - otherSwarmMotion.position.x, 2) +
				pow(swarmMotion.position.y - otherSwarmMotion.position.y, 2));
			if (distance != 0 && distance < shortestDistance) {
				shortestDistance = distance;
				closestSwarmEntity = otherSwarmEntity;
			}
		}
	}
	return closestSwarmEntity;
}

void AISystem::swarmFireProjectileAtPlayer(Entity swarmEntity) {
	EnemySwarm& swarm = registry.enemySwarms.get(swarmEntity);
	Motion& swarmMotion = registry.motions.get(swarmEntity);
	Motion& playerMotion = registry.motions.get(pickAPlayer());
	vec2 diff = playerMotion.position - swarmMotion.position;
	float angle = atan2(diff.y, diff.x);
	vec2 velocity = vec2(cos(angle) * swarm.projectileSpeed, sin(angle) * swarm.projectileSpeed);
	if (bossMode.currentBossLevel == STAGE2) {
		createHandProjectile(renderer, swarmMotion.position, velocity, angle, swarmEntity);
	}
	else {
		createEnemyProjectile(renderer, swarmMotion.position, velocity, angle, swarmEntity);
	}
}

void AISystem::stepEnemyCoord(float elapsed_ms, float width, float height) {
	for (Entity headEntity : registry.enemyCoordHeads.entities) {
		EnemyCoordHead& head = registry.enemyCoordHeads.get(headEntity);
		if (head.timeToUpdateAi) {
			Enemy& headStatus = registry.enemies.get(headEntity);
			Entity tailEntity = head.belongToTail;
			if (!headStatus.isDead) {
				moveAwayfromOtherCoord(headEntity, tailEntity, elapsed_ms);
			}
			head.aiUpdateTimer = 0;
			head.timeToUpdateAi = false;
		}
		else {
			head.aiUpdateTimer += elapsed_ms;
			if (head.aiUpdateTimer > head.aiUpdateTime) {
				head.timeToUpdateAi = true;
			}
		}
	}
}

void AISystem::moveAwayfromOtherCoord(Entity enemyEntity, Entity otherEnemyEntity, float elapsed_ms) {
	if (registry.motions.has(otherEnemyEntity)) {
		Motion& enemyMotion = registry.motions.get(enemyEntity);
		Motion& otherEnemyMotion = registry.motions.get(otherEnemyEntity);
		EnemyCoordHead& enemyHead = registry.enemyCoordHeads.get(enemyEntity);
		float distance = sqrt(pow(enemyMotion.position.x - otherEnemyMotion.position.x, 2) +
			pow(enemyMotion.position.y - otherEnemyMotion.position.y, 2));
		// if head tail too close, move away from each other to maintain distance
		if (distance < enemyHead.minDistFromTail) {
			vec2 directionFromEnemyToOtherEnemy =
				vec2(otherEnemyMotion.position.x - enemyMotion.position.x, otherEnemyMotion.position.y - enemyMotion.position.y);
			vec2 oppositeOfDirection = vec2(directionFromEnemyToOtherEnemy.x * -1.f, directionFromEnemyToOtherEnemy.y * -1.f);
			vec2 normalizedOppositeDirection = vec2(oppositeOfDirection.x / sqrt(pow(oppositeOfDirection.x, 2) + pow(oppositeOfDirection.y, 2)),
				oppositeOfDirection.y / sqrt(pow(oppositeOfDirection.x, 2) + pow(oppositeOfDirection.y, 2)));
			Enemy& enemyStatus = registry.enemies.get(enemyEntity);
			enemyMotion.velocity = vec2(normalizedOppositeDirection.x * enemyStatus.speed, normalizedOppositeDirection.y * enemyStatus.speed);
			vec2 normalizedDirection = vec2(directionFromEnemyToOtherEnemy.x / sqrt(pow(directionFromEnemyToOtherEnemy.x, 2) + pow(directionFromEnemyToOtherEnemy.y, 2)),
				directionFromEnemyToOtherEnemy.y / sqrt(pow(directionFromEnemyToOtherEnemy.x, 2) + pow(directionFromEnemyToOtherEnemy.y, 2)));
			Enemy& otherEnemyStatus = registry.enemies.get(otherEnemyEntity);
			otherEnemyMotion.velocity = vec2(normalizedDirection.x * otherEnemyStatus.speed, normalizedDirection.y * otherEnemyStatus.speed);
		}
		else {
			Entity playerOneEntity = registry.players.entities.front();
			if (twoPlayer.inTwoPlayerMode) {
				Entity playerTwoEntity = registry.players.entities.back();
				Player& player1 = registry.players.get(playerOneEntity);
				Player& player2 = registry.players.get(playerTwoEntity);
				if (!player1.isDead) {
					Motion& player1Motion = registry.motions.get(registry.players.entities.front());
					handleCoordEnemyUpdate(player1Motion, enemyMotion, otherEnemyMotion, enemyEntity, otherEnemyEntity);
				}
				else {
					Motion& player2Motion = registry.motions.get(registry.players.entities.back());
					handleCoordEnemyUpdate(player2Motion, enemyMotion, otherEnemyMotion, enemyEntity, otherEnemyEntity);
				}
			}
			else {
				Entity playerOneEntity = registry.players.entities.front();
				Player& player1 = registry.players.get(playerOneEntity);
				// head running away from player
				Motion& player1Motion = registry.motions.get(registry.players.entities.front());
				// if head too close to player, then tail chases player, otherwise they both move randomly
				float distance = sqrt(pow(enemyMotion.position.x - player1Motion.position.x, 2) +
					pow(enemyMotion.position.y - player1Motion.position.y, 2));
				if (distance < 300.f) {
					handleCoordEnemyUpdate(player1Motion, enemyMotion, otherEnemyMotion, enemyEntity, otherEnemyEntity);
				}
				else {
					setEnemyWonderingRandomly(enemyEntity);
					setEnemyWonderingRandomly(otherEnemyEntity);
				}
			}
		}
	}
}

void AISystem::handleCoordEnemyUpdate(Motion& playerMotion, Motion& enemyMotion, Motion& otherEnemyMotion, Entity enemyEntity, Entity otherEnemyEntity) {
	// head moving away from player
	vec2 directionHeadToPlayer =
		vec2(playerMotion.position.x - enemyMotion.position.x, playerMotion.position.y - enemyMotion.position.y);
	vec2 oppositeOfDirection = vec2(directionHeadToPlayer.x * -1.f, directionHeadToPlayer.y * -1.f);
	vec2 normalizedOppositeDirection = vec2(oppositeOfDirection.x / sqrt(pow(oppositeOfDirection.x, 2) + pow(oppositeOfDirection.y, 2)),
		oppositeOfDirection.y / sqrt(pow(oppositeOfDirection.x, 2) + pow(oppositeOfDirection.y, 2)));
	Enemy& enemyStatus = registry.enemies.get(enemyEntity);
	enemyMotion.velocity = vec2(normalizedOppositeDirection.x * enemyStatus.speed, normalizedOppositeDirection.y * enemyStatus.speed);
	// tail running towards player
	vec2 directionTailToPlayer =
		vec2(playerMotion.position.x - otherEnemyMotion.position.x, playerMotion.position.y - otherEnemyMotion.position.y);
	vec2 normalizedDirection = vec2(directionTailToPlayer.x / sqrt(pow(directionTailToPlayer.x, 2) + pow(directionTailToPlayer.y, 2)),
		directionTailToPlayer.y / sqrt(pow(directionTailToPlayer.x, 2) + pow(directionTailToPlayer.y, 2)));
	Enemy& otherEnemyStatus = registry.enemies.get(otherEnemyEntity);
	otherEnemyMotion.velocity = vec2(normalizedDirection.x * otherEnemyStatus.speed, normalizedDirection.y * otherEnemyStatus.speed);
}

Entity AISystem::pickAPlayer() {
	Entity playerOneEntity = registry.players.entities.front();
	Entity playerEntity = playerOneEntity;
	if (twoPlayer.inTwoPlayerMode) {
		Entity playerTwoEntity = registry.players.entities.back();
		Player& player1 = registry.players.get(playerOneEntity);
		Player& player2 = registry.players.get(playerTwoEntity);
		if (uniform_dist(rng) > 0.5) {
			if (!player2.isDead) {
				playerEntity = playerTwoEntity;
			}
		}
		else {
			if (player1.isDead) {
				playerEntity = playerTwoEntity;
			}
		}
	}
	return playerEntity;
}

int AISystem::findMin(vec2 upSumCost, vec2 rightSumCost, vec2 downSumCost, vec2 leftSumCost) {
	float currMin = INT_MAX;
	int currMinPosition = -1;
	// UP
	if (upSumCost.x + upSumCost.y < currMin) {
		currMin = upSumCost.x + upSumCost.y;
		currMinPosition = 0;
	}	// RIGHT
	if (rightSumCost.x + rightSumCost.y < currMin) {
		currMin = rightSumCost.x + rightSumCost.y;
		currMinPosition = 1;
	}	// DOWN
	if (downSumCost.x + downSumCost.y < currMin) {
		currMin = downSumCost.x + downSumCost.y;
		currMinPosition = 2;
	}	// LEFT
	if (leftSumCost.x + leftSumCost.y < currMin) {
		currMin = leftSumCost.x + leftSumCost.y;
		currMinPosition = 3;
	}
	return currMinPosition;
}

vec2 AISystem::findFinalPosition(vec2 currNode, Entity& enemy, float width, float height, int currMinPosition) {
	int finX = currNode.x;
	int finY = currNode.y;

	// depending on the min direction, choose that one, and made it the finX, finY
	switch (currMinPosition) {
	case 0: // up
		if (currNode.y - registry.enemyAStars.get(enemy).stepSizes > 0.f) {
			finY = currNode.y - registry.enemyAStars.get(enemy).stepSizes;
		}
		break;
	case 1: // right
		if (currNode.x + registry.enemyAStars.get(enemy).stepSizes < width) {
			finX = currNode.x + registry.enemyAStars.get(enemy).stepSizes;
		}
		break;
	case 2: // down
		if (currNode.y + registry.enemyAStars.get(enemy).stepSizes < height) {
			finY = currNode.y + registry.enemyAStars.get(enemy).stepSizes;
		}
		break;
	case 3: // left
		if (currNode.x - registry.enemyAStars.get(enemy).stepSizes > 0.f) {
			finX = currNode.x - registry.enemyAStars.get(enemy).stepSizes;
		}
		break;
	default:
		break;
	}

	return { finX, finY };

}

vec2 AISystem::nextNode(vec2 currNode, Entity& player, Entity& enemy, float width, float height) {
	Motion& motionAStar = registry.motions.get(enemy);
	Motion& motionPlayer = registry.motions.get(player);
	// Calculate H Cost value (distance between current node and the end position) and G Cost (distance between current node and the NEXT position)
	// Add the sum of these two.
	vec2 upHCost = abs(calculateHCost(motionPlayer, { currNode.x, currNode.y - registry.enemyAStars.get(enemy).stepSizes }));
	vec2 upSumCost = abs(calculateGCost(enemy, 0)) + upHCost;

	// RIGHT
	vec2 rightHCost = abs(calculateHCost(motionPlayer, { currNode.x + registry.enemyAStars.get(enemy).stepSizes, currNode.y }));
	vec2 rightSumCost = abs(calculateGCost(enemy, 1)) + rightHCost;

	// DOWN
	vec2 downHCost = abs(calculateHCost(motionPlayer, { currNode.x, currNode.y + registry.enemyAStars.get(enemy).stepSizes }));
	vec2 downSumCost = abs(calculateGCost(enemy, 2)) + downHCost;

	// LEFT
	vec2 leftHCost = abs(calculateHCost(motionPlayer, { currNode.x - registry.enemyAStars.get(enemy).stepSizes, currNode.y }));
	vec2 leftSumCost = abs(calculateGCost(enemy, 3)) + leftHCost;
	// Find the min of all the values
	
	int currMinPosition = findMin(upSumCost, rightSumCost, downSumCost, leftSumCost);
	vec2 finalPosition = findFinalPosition(currNode, enemy, width, height, currMinPosition);

	// push the next node the queue and return the node
	std::pair<int, int> currPosition = { finalPosition.x , finalPosition.y };
	registry.enemyAStars.get(enemy).traversalQueue.push(currPosition);
	return { finalPosition.x, finalPosition.y };
}

// Distance between enemy and endgoal
vec2 AISystem::calculateHCost(const Motion& player, vec2 currNode) {
	float distanceX = abs(player.position.x - currNode.x);
	float distanceY = abs(player.position.y - currNode.y);

	return { distanceX, distanceY };
}

// Distance between enemy and next position
vec2 AISystem::calculateGCost(Entity& enemy, int dir) {
	float distanceX = 0.f;
	float distanceY = 0.f;
	if (dir == 0) {			// up
		distanceY = registry.enemyAStars.get(enemy).stepSizes;
	}
	else if (dir == 1) {	// right
		distanceX = registry.enemyAStars.get(enemy).stepSizes;
	}
	else if (dir == 2) {	// down
		distanceY = registry.enemyAStars.get(enemy).stepSizes;
	}
	else if (dir == 3) {	// left
		distanceX = registry.enemyAStars.get(enemy).stepSizes;
	}

	return { distanceX, distanceY };
}

void AISystem::handleAStarPathCalculation(Entity& player, Entity& enemy, float width, float height) {
	Motion& motionAStar = registry.motions.get(enemy);
	Motion& motionPlayer = registry.motions.get(player);
	vec2 finNode = { motionPlayer.position.x, motionPlayer.position.y };

	vec2 currNode = { motionAStar.position.x, motionAStar.position.y };
	while (abs(currNode.x - finNode.x) > registry.enemyAStars.get(enemy).distanceCloseToPlayer 
		&& abs(currNode.y - finNode.y) > registry.enemyAStars.get(enemy).distanceCloseToPlayer) {
		currNode = nextNode(currNode, player, enemy, width, height);
	}
}

void AISystem::pathCalculationInit(Entity& enemyAStar, float width, float height) {
	EnemyAStar& aStarEnemy = registry.enemyAStars.get(enemyAStar);
	aStarEnemy.finishedPathCalculation = false;
	aStarEnemy.next_AStar_behaviour_calculation = aStarEnemy.AStarBehaviourUpdateTime;
	Entity player = pickAPlayer();
	Motion& playerMotion = registry.motions.get(player);
	handleAStarPathCalculation(player, enemyAStar, width, height);
}

void AISystem::stepMovement(Entity& enemyAStar) {
	EnemyAStar& aStarEnemy = registry.enemyAStars.get(enemyAStar);
	Motion AStarMotion = registry.motions.get(enemyAStar);
	aStarEnemy.next_bacteria_movement = aStarEnemy.movementUpdateTime;
	if (!aStarEnemy.traversalQueue.empty()) {
		std::pair<int, int> currPosition = { -1 , -1 };
		currPosition = aStarEnemy.traversalQueue.front();
		aStarEnemy.traversalQueue.pop();
		moveToSpot(AStarMotion.position.x, AStarMotion.position.y, currPosition.first, currPosition.second, enemyAStar);
	}
}

void AISystem::stepEnemyAStar(float elapsed_ms, float width, float height) {
	for (Entity& entityAStar : registry.enemyAStars.entities) {  
		Enemy& enemy = registry.enemies.get(entityAStar);
		Motion AStarMotion = registry.motions.get(entityAStar);
		EnemyAStar& aStarEnemy = registry.enemyAStars.get(entityAStar);
		if (!enemy.isDead) {
			aStarEnemy.next_AStar_behaviour_calculation -= elapsed_ms;
			aStarEnemy.next_bacteria_movement -= elapsed_ms;
			if (aStarEnemy.next_AStar_behaviour_calculation < 0.f) {
				pathCalculationInit(entityAStar, width, height);
			}

			if (aStarEnemy.next_bacteria_movement < 0.f) {
				stepMovement(entityAStar);
			}
		}
	}
}

void AISystem::bossFireProjectileAtPlayer(Entity entity) {
	EnemyBoss& boss = registry.enemyBoss.get(entity);
	Motion& bossMotion = registry.motions.get(entity);
	Motion& playerMotion = registry.motions.get(pickAPlayer());
	vec2 diff = playerMotion.position - bossMotion.position;
	float angle = atan2(diff.y, diff.x);
	vec2 velocity = vec2(cos(angle) * boss.projectileSpeed, sin(angle) * boss.projectileSpeed);
	createHandProjectile(renderer, vec2(bossMotion.position.x, BOSS_BB_HEIGHT * defaultResolution.scaling), velocity, angle, entity);
}


void AISystem::stepEnemyBoss(float elapsed_ms) {
	if (bossMode.currentBossLevel == STAGE3 && registry.enemyBoss.entities.size() > 0) {
		Entity bossEntity = registry.enemyBoss.entities.front();
		EnemyBoss& boss = registry.enemyBoss.get(bossEntity);
		if (boss.timeToUpdateAi) {
			bossFireProjectileAtPlayer(bossEntity);
			boss.timeToUpdateAi = false;
			boss.aiUpdateTimer = 0;
		}
		else {
			boss.aiUpdateTimer += elapsed_ms;
			if (boss.aiUpdateTimer > boss.aiUpdateInterval) {
				boss.timeToUpdateAi = true;
			}
		}
	}
}