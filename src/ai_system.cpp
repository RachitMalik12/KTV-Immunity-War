// internal
#include "ai_system.hpp"

void AISystem::step(float elapsed_ms)
{
	for (Entity hunterEntity: registry.enemyHunters.entities) {
		EnemyHunter& hunter = registry.enemyHunters.get(hunterEntity);
		Enemy& hunterStatus = registry.enemies.get(hunterEntity);
		if (hunterStatus.hp <= 2) {
			hunter.currentState = hunter.fleeingMode;
		}
		if (hunter.currentState == hunter.fleeingMode && hunter.isFleeing == false) {
			registry.motions.get(hunterEntity).velocity = vec2(2.0f * hunterStatus.speed, 0);
			hunter.isFleeing = true;
		} else {
			if (hunter.timeToUpdateAi) {
				if (hunter.currentState == hunter.searchingMode) {
					if (isHunterInRangeOfThePlayers(hunterEntity)) {
						hunter.currentState = hunter.huntingMode;
					} else {
						setHunterWonderingRandomly(hunterEntity);
					}
				}
				if (hunter.currentState == hunter.huntingMode) {
					setHunterChasingThePlayer(hunterEntity);
				}
				hunter.timeToUpdateAi = false;
				hunter.aiUpdateTimer = hunter.aiUpdateTime;
			} else {
				hunter.aiUpdateTimer -= elapsed_ms;
				if (hunter.aiUpdateTimer < 0) {
					hunter.timeToUpdateAi = true;
				}
			}
		}
	}
}

bool AISystem::isHunterInRangeOfThePlayers(Entity hunterEntity) {
	Motion& hunterMotion = registry.motions.get(hunterEntity);
	float distance;
	if (twoPlayer.inTwoPlayerMode) {
		Motion& player1Motion = registry.motions.get(registry.players.entities.front());
		Motion& player2Motion = registry.motions.get(registry.players.entities.back());
		float distFromPlayer1 = hunterDistanceFromPlayer(player1Motion, hunterMotion);
		float distFromPlayer2 = hunterDistanceFromPlayer(player2Motion, hunterMotion);
		distance = std::min(distFromPlayer1, distFromPlayer2);
	}
	else {
		Motion& player1Motion = registry.motions.get(registry.players.entities.front());
		distance = hunterDistanceFromPlayer(player1Motion, hunterMotion);
	}
	if (distance < registry.enemyHunters.get(hunterEntity).huntingRange) {
		return true;
	}
	return false;
}

float AISystem::hunterDistanceFromPlayer(const Motion& player, const Motion& hunter) {
	vec2 dp = player.position - hunter.position;
	return sqrt(dot(dp, dp));
}

void AISystem::setHunterWonderingRandomly(Entity hunterEntity) {
	EnemyHunter& hunter = registry.enemyHunters.get(hunterEntity);
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
	} else {
		playerToChase = registry.players.entities.front();
	}
	Motion& playerMotion = registry.motions.get(playerToChase);
	vec2 diff = playerMotion.position - hunterMotion.position;
	float angle = atan2(diff.y, diff.x);
	hunterMotion.velocity = vec2(cos(angle) * hunterStatus.speed, sin(angle) * hunterStatus.speed);
}

Entity AISystem::determineWhichPlayerToChase(Entity hunterEntity) {
	Motion& hunterMotion = registry.motions.get(hunterEntity);
	Motion& player1Motion = registry.motions.get(registry.players.entities.front());
	Motion& player2Motion = registry.motions.get(registry.players.entities.back());
	float distFromPlayer1 = hunterDistanceFromPlayer(player1Motion, hunterMotion);
	float distFromPlayer2 = hunterDistanceFromPlayer(player2Motion, hunterMotion);
	if (distFromPlayer1 < distFromPlayer2) {
		return registry.players.entities.front();
	}
	else {
		return registry.players.entities.back();
	}
}