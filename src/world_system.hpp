#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "render_system.hpp"

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	WorldSystem();

	std::vector<std::function<void(Entity entity)>> callbackFns;

	// Creates a window
	GLFWwindow* create_window(int width, int height);

	// starts the game
	void init(RenderSystem* renderer);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// Should the game be over ?
	bool is_over()const;

	void setResolution();

	// menu
	Entity createMenu();
	Entity createInGameMenu();

private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_click(int button, int action, int mods);
	// restart level
	void restart_game();
	void setupLevel(int levelNum);
	void setPlayersStats();
	void setPlayerOneStats();
	void setPlayerTwoStats();
	// OpenGL window handle
	GLFWwindow* window;
	int level_number;
	std::vector<Level> levels; 
	// to start with true. 
	bool isLevelOver;
	bool isTransitionOver;
	bool firstEntranceToShop; 
	bool startingNewLevel = false;
	bool isGameOver = false;
	// Game state
	RenderSystem* renderer;
	float next_projectile_fire_player1;
	float next_projectile_fire_player2;
	Entity player_knight;
	Entity player2_wizard;
	Entity player_stat;
	Entity player2_stat;
	// music references
	Mix_Music* background_music;
	Mix_Chunk* salmon_dead_sound;
	Mix_Chunk* salmon_eat_sound;
	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
	// window scaling variables setup
	void setupWindowScaling();
	// window scaling variables
	float gameHeightScale;
	float doorWidthScale;
	// create and remove walls and doors
	void createWalls(int screenWidth, int screenHeight);
	void createADoor(int screenWidth, int screenHeight);
	// world.step
	void deathHandling();
	void handlePlayerOneAttack(float elapsed_ms_since_last_update);
	void handlePlayerTwoProjectile(float elapsed_ms_since_last_update);
	void invincibilityTimer(float elapsed_ms_since_last_update);
	void stuckTimer(float elapsed_ms_since_last_update, int screen_width, int screen_height);
	void resolveMouseControl();
	void levelCompletionCheck(float elapsed_ms_since_last_update);
	void animateKnight(float elapsed_ms_since_last_update);
	void animateWizard(float elpased_ms_since_last_update);
	void checkIfPlayersAreMoving();
	// misc
	void playerTwoJoinOrLeave();
	void updateTitle(int level);
	// animation
	void knightFrameSetter(float elapsed_ms, KnightAnimation& knightAnimation);
	void wizardAttackFrameSetter(float elapsed_ms, WizardAnimation& wizardAnimation);
	void wizardWalkFrameSetter(float elapsed_ms, WizardAnimation& wizardAnimation);
	void wizardIdleFrameSetter(float elapsed_ms, WizardAnimation& wizardAnimation);
	// menu
	void menuLogic(int menuType);
	void createTitleScreen(vec2 mouse_position);
	void createInGameScreen(vec2 mouse_position);
	void toggleInGameMenu();
	vec2 inShopAdjustPosition(float button_pos, Motion& motion);
	bool withinButtonBounds(float mouse_position, vec2 bounds);
	//load
	void loadGame();
	void saveGame();
	// story
	void storyClicker();
	void transitionToShop(); 
	void setTransitionFlag(Entity player); 
	void reviveDeadPlayerInShop(); 
	void spawnPowerups(int n);
	Entity chooseRandomPowerUp(vec2 pos); 
	void reviveWizard(Player& p1, PlayerStat& p1Stat); 
	void reviveKnight(Player& p2, PlayerStat& p2Stat); 
	void progressGameEndEffect(float elapsed_ms_since_last_update);
	void progressBrightenScreen(float elapsed_ms_since_last_update);
	void lightUpEnemyRunTimer(float elapsed_ms_since_last_update);
	void lightUpEnemyBacteriaTimer(float elapsed_ms_since_last_update);
}; 