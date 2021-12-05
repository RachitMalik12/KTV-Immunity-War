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
	void clearLevel();
	void setupLevel(int levelNum);
	void setPlayersStats();
	void setPlayerOneStats();
	void setPlayerTwoStats();
	void setupTutorial();
	void createShopHint();
	void waitAndMakeEnemiesVisible(float elapsed_ms); 
	// create correct background for level
	void createLevelBackground(int levelNum);
	// OpenGL window handle
	GLFWwindow* window;
	int level_number;
	std::vector<Level> levels;  
	bool isLevelOver;
	bool devMode = false;
	bool isTransitionOver;
	bool firstEntranceToShop; 
	bool startingNewLevel = false;
	bool tutorialEnemyTransition = true; 
	bool tutorialEnemyFinishTransition = false; 
	bool isGameOver = false;
	bool shopHintCreated = false; 
	// Game state
	RenderSystem* renderer;
	float next_projectile_fire_player1;
	float next_projectile_fire_player2;
	float step_interval = 600.0f;
	Entity player_knight;
	Entity player2_wizard;
	Entity player_stat;
	Entity player2_stat;
	// music references
	int volume = 20;
	int fade_duration = 500;
	Mix_Chunk* menu_click_sound;
	Mix_Chunk* swing_sound;
	Mix_Chunk* zap_sound;
	Mix_Chunk* level_start_sound;
	Mix_Chunk* level_end_sound;
	Mix_Music* battle0_bgm;
	Mix_Music* battle1_bgm;
	Mix_Music* battle2_bgm;
	Mix_Music* battle3_bgm;
	Mix_Music* battle4_bgm;
	Mix_Music* battle5_bgm;
	Mix_Music* battle6_bgm;
	Mix_Music* battle7_bgm;
	Mix_Music* shop_bgm;
	Mix_Music* final_boss_bgm;
	void setLevelMusic(int level);
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
	void stopPlayerAtMouseDestination();
	void levelCompletionCheck(float elapsed_ms_since_last_update);
	void advanceToShopOrStage();
	void animateKnight(float elapsed_ms_since_last_update);
	void animateWizard(float elpased_ms_since_last_update);
	void checkIfPlayersAreMoving();
	void removeDeadPlayersAndEnemies(float elapsed_ms);
	// misc
	void playerTwoJoinOrLeave();
	void setFinalLevelStages(Level level, BossPhase phase);
	void createEnemyFilteredByType(Level level, int enemyFilter);
	// utils 
	float scaleCoordinate(float coordinate);
	// animation
	void knightFrameSetter(float elapsed_ms, KnightAnimation& knightAnimation);
	void wizardAttackFrameSetter(float elapsed_ms, WizardAnimation& wizardAnimation);
	void wizardWalkFrameSetter(float elapsed_ms, WizardAnimation& wizardAnimation);
	void wizardIdleFrameSetter(float elapsed_ms, WizardAnimation& wizardAnimation);
	void attachAndRenderPriceNumbers(Entity powerUp, vec2 pos);
	void scaleGameHUD();
	void removeWizardHud();
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
	Entity chooseFixedPowerUp(vec2 pos, int index); 
	void attachAndRenderPowerupDescription(vec2 pos, std::string type);
	void drawTutorialTextInShop(); 
	void reviveWizard(Player& p1, PlayerStat& p1Stat); 
	void reviveKnight(Player& p2, PlayerStat& p2Stat); 
	void progressBrightenScreen(float elapsed_ms_since_last_update);
}; 