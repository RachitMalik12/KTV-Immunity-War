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

	std::vector<std::function<void(Entity entity)>>  callbackFns;

	void attach(std::function<void(Entity entity)>);

	void hpListener(Entity entity);
	// Observer Pattern listener
	void hpCallBack(Entity entity);

	// Creates a window
	GLFWwindow* create_window(int width, int height);

	// starts the game
	void init(RenderSystem* renderer);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// Check for collisions
	void handle_collisions();

	// Should the game be over ?
	bool is_over()const;

private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_click(int button, int action, int mods);

	// restart level
	void restart_game();

	// OpenGL window handle
	GLFWwindow* window;

	// Number of fish eaten by the salmon, displayed in the window title
	int money;
	bool spawnPowerup;
	// Game state
	RenderSystem* renderer;
	float current_speed;
	float next_enemy_spawn;
	float next_enemyrun_spawn;
	Entity player_salmon;
	Entity player_wizard;
	Entity player2_wizard;

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
}; 