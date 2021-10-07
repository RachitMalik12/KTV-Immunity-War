// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>

#include "physics_system.hpp"

// Game configuration
const size_t MAX_TURTLES = 15;
const size_t MAX_FISH = 5;
const size_t TURTLE_DELAY_MS = 2000 * 3;
const size_t FISH_DELAY_MS = 5000 * 3;
const size_t PLAYER_SPEED = 150;
const int WINDOW_WIDTH_PX = 1200;
const int WINDOW_HEIGHT_PX = 800;
const int WALL_THICCNESS = 40;

// Create the fish world
WorldSystem::WorldSystem()
	: points(0) {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (salmon_dead_sound != nullptr)
		Mix_FreeChunk(salmon_dead_sound);
	if (salmon_eat_sound != nullptr)
		Mix_FreeChunk(salmon_eat_sound);
	Mix_CloseAudio();

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char *desc) {
		fprintf(stderr, "%d: %s", error, desc);
	}
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow* WorldSystem::create_window(int width, int height) {
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(width, height, "Salmon Game Assignment", nullptr, nullptr);
	if (window == nullptr) {
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);

	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}

	background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
	salmon_dead_sound = Mix_LoadWAV(audio_path("salmon_dead.wav").c_str());
	salmon_eat_sound = Mix_LoadWAV(audio_path("salmon_eat.wav").c_str());

	if (background_music == nullptr || salmon_dead_sound == nullptr || salmon_eat_sound == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("music.wav").c_str(),
			audio_path("salmon_dead.wav").c_str(),
			audio_path("salmon_eat.wav").c_str());
		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;
	// Playing background music indefinitely
	Mix_PlayMusic(background_music, -1);
	fprintf(stderr, "Loaded music\n");

	// Set all states to default
    restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	// Get the screen dimensions
	int screen_width, screen_height;
	glfwGetFramebufferSize(window, &screen_width, &screen_height);

	// Updating window title with points
	std::stringstream title_ss;
	title_ss << "Points: " << points;
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
	    registry.remove_all_components_of(registry.debugComponents.entities.back());

	// Remove graybox info from the last step
	while (registry.grayboxComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.grayboxComponents.entities.back());

	// Removing out of screen entities
	auto& motions_registry = registry.motions;

	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current)
	for (int i = (int)motions_registry.components.size()-1; i>=0; --i) {
	    Motion& motion = motions_registry.components[i];
		if (motion.position.x + abs(motion.scale.x) < 0.f) {
		    registry.remove_all_components_of(motions_registry.entities[i]);
		}
	}

	// Spawning new enemy
	next_enemy_spawn -= elapsed_ms_since_last_update * current_speed;
	if (registry.enemies.components.size() <= MAX_FISH && next_enemy_spawn < 0.f) {
		// !!!  TODO A1: Create new fish with createFish({0,0}), as for the Turtles above
		// Reset timer
		next_enemy_spawn = (FISH_DELAY_MS / 2) + uniform_dist(rng) * (FISH_DELAY_MS / 2);
		// Create fish
		Entity entity = createEnemy(renderer, { 0,0 }, { 0,0 });
		// Setting random initial position (to the right of screen) and constant velocity
		Motion& motion = registry.motions.get(entity);
		motion.position =
			vec2(screen_width - 200.f,
				50.f + uniform_dist(rng) * (screen_height - 100.f));
		motion.velocity = vec2(-200.f, 0.f);
	}

	// Processing the salmon state
	assert(registry.screenStates.components.size() <= 1);
    ScreenState &screen = registry.screenStates.components[0];

    float min_counter_ms = 3000.f;
	for (Entity entity : registry.deathTimers.entities) {
		// progress timer
		DeathTimer& counter = registry.deathTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		if(counter.counter_ms < min_counter_ms){
		    min_counter_ms = counter.counter_ms;
		}

		// restart the game once the death timer expired
		if (counter.counter_ms < 0) {
			registry.deathTimers.remove(entity);
			screen.darken_screen_factor = 0;
            restart_game();
			return true;
		}
	}
	// reduce window brightness if any of the present salmons is dying
	screen.darken_screen_factor = 1 - min_counter_ms / 3000;

	// !!! TODO A1: update LightUp timers and remove if time drops below zero, similar to the death counter

	return true;
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");

	// Reset the game speed
	current_speed = 1.f;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all fish, turtles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
	    registry.remove_all_components_of(registry.motions.entities.back());

	// Debugging for memory/component leaks
	registry.list_all_components();

	// Create walls
	vec2 leftWallPos = { 0, WINDOW_HEIGHT_PX / 2 };
	vec2 rightWallPos = { WINDOW_WIDTH_PX, WINDOW_HEIGHT_PX / 2 };
	vec2 topWallPos = { WINDOW_WIDTH_PX / 2, 0 };
	vec2 bottomWallPos = { WINDOW_WIDTH_PX / 2, WINDOW_HEIGHT_PX };
	vec2 verticalWallScale = { WALL_THICCNESS, WINDOW_HEIGHT_PX };
	vec2 horizontalWallScale = { WINDOW_WIDTH_PX, WALL_THICCNESS };
	createWall(leftWallPos, verticalWallScale, true);
	createWall(rightWallPos, verticalWallScale, true);
	createWall(topWallPos, horizontalWallScale, false);
	createWall(bottomWallPos, horizontalWallScale, false);

	// Create a new player character
	player_wizard = createWizard(renderer, { 100, 200 });

	// Create some blocks
	createBlock(renderer, { 700, 600 }, "red");
	createBlock(renderer, { 700, 300 }, "orange");
	createBlock(renderer, { 700, 100 }, "yellow");

	// !! TODO A3: Enable static pebbles on the ground
	// Create pebbles on the floor for reference
	/*
	for (uint i = 0; i < 20; i++) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		float radius = 30 * (uniform_dist(rng) + 0.3f); // range 0.3 .. 1.3
		Entity pebble = createPebble({ uniform_dist(rng) * w, h - uniform_dist(rng) * 20 }, 
			         { radius, radius });
		float brightness = uniform_dist(rng) * 0.5 + 0.5;
		registry.colors.insert(pebble, { brightness, brightness, brightness});
	}
	*/
}

// Compute collisions between entities
void WorldSystem::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions; // TODO: @Tim, is the reference here needed?
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		// Checking collision of projectiles with other entities
		if (registry.projectiles.has(entity)) {
			if (registry.enemies.has(entity_other)) {
				// remove enemy, fireball, count points
				registry.remove_all_components_of(entity_other);
				registry.remove_all_components_of(entity);
				++points;
			}
		}

		// For now, we are only interested in collisions that involve the salmon
		if (registry.players.has(entity)) {
			//Player& player = registry.players.get(entity);

			//// Checking Player - HardShell collisions
			//if (registry.hardShells.has(entity_other)) {
			//	// initiate death unless already dying
			//	if (!registry.deathTimers.has(entity)) {
			//		// Scream, reset timer, and make the salmon sink
			//		registry.deathTimers.emplace(entity);
			//		Mix_PlayChannel(-1, salmon_dead_sound, 0);
			//		registry.motions.get(entity).angle = 3.1415f;
			//		registry.motions.get(entity).velocity = { 0, 80 };

			//		// !!! TODO A1: change the salmon color on death
			//	}
			//}
			//// Checking Player - SoftShell collisions
			//else if (registry.softShells.has(entity_other)) {
			//	if (!registry.deathTimers.has(entity)) {
			//		// chew, count points, and set the LightUp timer
			//		registry.remove_all_components_of(entity_other);
			//		Mix_PlayChannel(-1, salmon_eat_sound, 0);
			//		++points;

			//		// !!! TODO A1: create a new struct called LightUp in components.hpp and add an instance to the salmon entity by modifying the ECS registry
			//	}
			//}
		}
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {
	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);

        restart_game();
	}

	// Debugging
	if (key == GLFW_KEY_BACKSLASH) {
		if (action == GLFW_RELEASE) {
			debugging.in_debug_mode = !debugging.in_debug_mode;
		}
	}

	// Grayboxing
	if (key == GLFW_KEY_RIGHT_BRACKET) {
		if (action == GLFW_RELEASE) {
			debugging.in_graybox_mode = !debugging.in_graybox_mode;
		}
	}

	Motion& salmonMotion = registry.motions.get(player_wizard);
	vec2 currentVelocity = vec2(salmonMotion.velocity.x, salmonMotion.velocity.y);
	if (action == GLFW_PRESS && key == GLFW_KEY_W) {
		salmonMotion.velocity = vec2(currentVelocity.x, currentVelocity.y - (float)PLAYER_SPEED);
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_W) {
		salmonMotion.velocity = vec2(currentVelocity.x, currentVelocity.y + (float)PLAYER_SPEED);
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_S) {
		salmonMotion.velocity = vec2(currentVelocity.x, currentVelocity.y + (float)PLAYER_SPEED);
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_S) {
		salmonMotion.velocity = vec2(currentVelocity.x, currentVelocity.y - (float)PLAYER_SPEED);
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_A) {
		salmonMotion.velocity = vec2(currentVelocity.x - (float)PLAYER_SPEED, currentVelocity.y);
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_A) {
		salmonMotion.velocity = vec2(currentVelocity.x + (float)PLAYER_SPEED, currentVelocity.y);
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_D) {
		salmonMotion.velocity = vec2(currentVelocity.x + (float)PLAYER_SPEED, currentVelocity.y);
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_D) {
		salmonMotion.velocity = vec2(currentVelocity.x - (float)PLAYER_SPEED, currentVelocity.y);
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_T) {
		createFireball(renderer, salmonMotion.position, { 0, -300.f });
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_G) {
		createFireball(renderer, salmonMotion.position, { 0, 300.f });
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_H) {
		createFireball(renderer, salmonMotion.position, { 300.f, 0});
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_F) {
		createFireball(renderer, salmonMotion.position, { -300.f, 0 });
	}

	// Debugging
	if (key == GLFW_KEY_Z) {
		if (action == GLFW_RELEASE)
			debugging.in_debug_mode = false;
		else
			debugging.in_debug_mode = true;
	}


	// Control the current speed with `<` `>`
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA) {
		current_speed -= 0.1f;
		printf("Current speed = %f\n", current_speed);
	}
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD) {
		current_speed += 0.1f;
		printf("Current speed = %f\n", current_speed);
	}
	current_speed = fmax(0.f, current_speed);
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE SALMON ROTATION HERE
	// xpos and ypos are relative to the top-left of the window, the salmon's
	// default facing direction is (1, 0)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	(vec2)mouse_position; // dummy to avoid compiler warning
}
