// internal
#include "ai_system.hpp"
#include <iostream>

void AISystem::step(float elapsed_ms, float width, float height) {
	stepEnemyHunter(elapsed_ms);
	stepEnemyBacteria(elapsed_ms, width, height);
	stepEnemyChase(elapsed_ms);
}

void AISystem::stepEnemyHunter(float elapsed_ms) {
	for (Entity hunterEntity : registry.enemyHunters.entities) {
		EnemyHunter& hunter = registry.enemyHunters.get(hunterEntity);
		Enemy& hunterStatus = registry.enemies.get(hunterEntity);
		if (hunterStatus.hp <= 2) {
			hunter.currentState = hunter.fleeingMode;
		}
		if (hunter.currentState == hunter.fleeingMode && hunter.isFleeing == false) {
			registry.motions.get(hunterEntity).velocity = vec2(2.0f * hunterStatus.speed, 0);
			hunter.isFleeing = true;
		}
		else {
			if (hunter.timeToUpdateAi) {
				if (hunter.currentState == hunter.searchingMode) {
					if (isHunterInRangeOfThePlayers(hunterEntity)) {
						hunter.currentState = hunter.huntingMode;
					}
					else {
						setHunterWonderingRandomly(hunterEntity);
					}
				}
				if (hunter.currentState == hunter.huntingMode) {
					setHunterChasingThePlayer(hunterEntity);
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
	next_bacteria_BFS_calculation -= elapsed_ms;
	auto& motions_registry = registry.motions;
	if (next_bacteria_BFS_calculation < 0.f) {
		for (Entity bacteriaEntity : registry.enemyBacterias.entities) {
			EnemyBacteria& bacteria = registry.enemyBacterias.get(bacteriaEntity);
			Motion& player1Motion = motions_registry.get(registry.players.entities[0]);

			// if bacteria is hunting, it will do BFS to find player
			if (bacteria.huntingMode) {
				bacteria.huntingMode = false;

				createAdj();

				// twoPlayerMode ? select random player to follow
				if (twoPlayer.inTwoPlayerMode) {
					next_bacteria_BFS_calculation = bacteria.bfsUpdateTime;
					Motion player2Motion = motions_registry.get(registry.players.entities[1]);
					float pickPlayer = rand() % 2 + 1;

					if (pickPlayer != 1 && !registry.players.get(registry.players.entities[1]).isDead) {
						handlePath(player2Motion.position.x, player2Motion.position.y, width, height, bacteriaEntity);
					}
					else {
						handlePath(player1Motion.position.x, player1Motion.position.y, width, height, bacteriaEntity);
					}
				}
				else {
					next_bacteria_BFS_calculation = bacteria.bfsUpdateTime;
					handlePath(player1Motion.position.x, player1Motion.position.y, width, height, bacteriaEntity);
				}
			}
		}
	}
}

void AISystem::stepEnemyChase(float elapsed_ms) {
	// update enemy chase so it chases the player
	for (Entity entity : registry.enemyChase.entities) {
		auto& enemyCom = registry.enemies.get(entity);
		Motion& motion = registry.motions.get(entity);
		Motion& motion_wz = *new Motion();
		for (uint k = 0; k < registry.players.size(); k++) {
			if (!registry.players.get(registry.players.entities[k]).isDead) {
				motion_wz = registry.motions.get(registry.players.entities[k]);
				break;
			}
		}
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
						vec2 chase_to_wz = vec2(motion_wz.position.x - motion.position.x, motion_wz.position.y - motion.position.y);
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
					vec2 chase_to_wz = vec2(motion_wz.position.x - motion.position.x, motion_wz.position.y - motion.position.y);
					float radians_for_angle = atan2f(-chase_to_wz.y, -chase_to_wz.x);
					float radians = atan2f(chase_to_wz.y, chase_to_wz.x);
					motion.angle = radians_for_angle;
					motion.velocity = vec2(enemyCom.speed * cos(-radians), enemyCom.speed * sin(radians));
				}
			}

		}

	}
}

bool AISystem::handlePath(int positionX, int positionY, float width, float height, Entity& bacteriaEntity) {
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
	adjacentsQueue.push({ resIndexX, resIndexY });
	std::pair<int, int> currPosition = adjacentsQueue.front();

	while (!adjacentsQueue.empty()) {
		// get first element in queue
		currPosition = adjacentsQueue.front();
		adjacentsQueue.pop();

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
					adjacentsQueue.push({ currPosition.first, currPosition.second - 1 });
				}
				if (pred[currPosition.first][currPosition.second - 1].first == -1 && pred[currPosition.first][currPosition.second - 1].second == -1) {
					pred[currPosition.first][currPosition.second - 1] = { currPosition.first, currPosition.second };
				}
			}

			// add adjacent "grid" block below to queue
			if (currPosition.second + 1 < 8) {
				if (visited[currPosition.first][currPosition.second + 1] == false) {
					adjacentsQueue.push({ currPosition.first, currPosition.second + 1 });
				}
				if (pred[currPosition.first][currPosition.second + 1].first == -1 && pred[currPosition.first][currPosition.second + 1].second == -1) {
					pred[currPosition.first][currPosition.second + 1] = { currPosition.first, currPosition.second };
				}
			}

			// add adjacent "grid" block left to queue
			if (currPosition.first - 1 >= 0) {
				if (visited[currPosition.first - 1][currPosition.second] == false) {
					adjacentsQueue.push({ currPosition.first - 1, currPosition.second });
				}
				if (pred[currPosition.first - 1][currPosition.second].first == -1 && pred[currPosition.first - 1][currPosition.second].second == -1) {
					pred[currPosition.first - 1][currPosition.second] = { currPosition.first, currPosition.second };
				}
			}

			// add adjacent "grid" block right to queue
			if (currPosition.first + 1 < 8) {
				if (visited[currPosition.first + 1][currPosition.second] == false) {
					adjacentsQueue.push({ currPosition.first + 1, currPosition.second });
				}
				if (pred[currPosition.first + 1][currPosition.second].first == -1 && pred[currPosition.first + 1][currPosition.second].second == -1) {
					pred[currPosition.first + 1][currPosition.second] = { currPosition.first, currPosition.second };
				}
			}
		}

	}
	return false;

}

void AISystem::bfsSearchPath(float initX, float initY, float finX, float finY, Entity& bacteriaEntity, float width, float height) {
	std::pair<int, int> currPosition = { finX , finY };

	// traverse from the final destination "grid" block
	// turn it back into the actually screen size using * (width/8) or * (height/8)
	// push into our traversalStack -- stack because we are now going BACKWARDS from the end to the beginning, using the predecessor to find our path from the player to the bacteria.
	while (currPosition.first != initX || currPosition.second != initY) {
		std::pair<int, int> temp = { pred[currPosition.first][currPosition.second].first * (width / 8),  pred[currPosition.first][currPosition.second].second * (height / 8) };
		traversalStack.push(temp);
		currPosition = pred[currPosition.first][currPosition.second];
	}

	// go through traversal stack. it should have the path now.
	// if it's empty, stop.
	while (!traversalStack.empty()) {
		// get current position of traversal stack
		currPosition = traversalStack.top();
		traversalStack.pop();
		int bacteriaPositionX = registry.motions.get(bacteriaEntity).position.x;
		int bacteriaPositionY = registry.motions.get(bacteriaEntity).position.y;

		// from the current bacteria position, go to 
		moveToSpot(bacteriaPositionX, bacteriaPositionY, currPosition.first, currPosition.second, bacteriaEntity);
	}
	while (!adjacentsQueue.empty())
	{
		adjacentsQueue.pop();
	}
}

void AISystem::moveToSpot(float initX, float initY, float finalX, float finalY, Entity& bacteriaEntity) {
	vec2 diff = vec2(finalX, finalY) - vec2(initX, initY);
	float angle = atan2(diff.y, diff.x);
	registry.motions.get(bacteriaEntity).velocity = vec2(cos(angle) * registry.enemies.get(bacteriaEntity).speed, sin(angle) * registry.enemies.get(bacteriaEntity).speed);

}

bool AISystem::isHunterInRangeOfThePlayers(Entity hunterEntity) {
	Motion& hunterMotion = registry.motions.get(hunterEntity);
	float distance;
	if (twoPlayer.inTwoPlayerMode) {
		Motion& player1Motion = registry.motions.get(registry.players.entities.front());
		Motion& player2Motion = registry.motions.get(registry.players.entities.back());
		float distFromPlayer1 = enemyDistanceFromPlayer(player1Motion, hunterMotion);
		float distFromPlayer2 = enemyDistanceFromPlayer(player2Motion, hunterMotion);
		distance = std::min(distFromPlayer1, distFromPlayer2);
	}
	else {
		Motion& player1Motion = registry.motions.get(registry.players.entities.front());
		distance = enemyDistanceFromPlayer(player1Motion, hunterMotion);
	}
	if (distance < registry.enemyHunters.get(hunterEntity).huntingRange) {
		return true;
	}
	return false;
}

float AISystem::enemyDistanceFromPlayer(const Motion& player, const Motion& hunter) {
	vec2 dp = player.position - hunter.position;
	return sqrt(dot(dp, dp));
}

void AISystem::setHunterWonderingRandomly(Entity hunterEntity) {
	Enemy& hunterStatus = registry.enemies.get(hunterEntity);
	float randomNumBetweenNegativeOneAndOne = (uniform_dist(rng) - 0.5) * 2;
	float anotherRandomNumBetweenNegativeOneAndOne = (uniform_dist(rng) - 0.5) * 2;
	vec2 randomVelocity =
		vec2(1.0f * hunterStatus.speed * randomNumBetweenNegativeOneAndOne,
			1.0f * hunterStatus.speed * anotherRandomNumBetweenNegativeOneAndOne);
	registry.motions.get(hunterEntity).velocity = randomVelocity;
}

void AISystem::setHunterChasingThePlayer(Entity hunterEntity) {
	Enemy& hunterStatus = registry.enemies.get(hunterEntity);
	Motion& hunterMotion = registry.motions.get(hunterEntity);
	Entity playerToChase;
	if (twoPlayer.inTwoPlayerMode) {
		playerToChase = determineWhichPlayerToChase(hunterEntity);
	}
	else {
		playerToChase = registry.players.entities.front();
	}
	Motion& playerMotion = registry.motions.get(playerToChase);
	vec2 diff = playerMotion.position - hunterMotion.position;
	float angle = atan2(diff.y, diff.x);
	hunterMotion.velocity = vec2(cos(angle) * hunterStatus.speed, sin(angle) * hunterStatus.speed);
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