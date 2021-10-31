// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>

#include "physics_system.hpp"

// Game configuration
const size_t MAX_ENEMIES = 5;
const size_t MAX_ENEMIESRUN = 2;
const size_t ENEMY_DELAY_MS = 1000;
const size_t PLAYER_SPEED = 150;
const size_t DEFAULT_HEIGHT = 800;
const int WALL_THICKNESS = 40;
const int SHOP_WALL_THICKNESS = 100;

// Create the fish world
WorldSystem::WorldSystem()
	: spawnPowerup(true), 
	  isLevelOver(false),
	  initial_level_load(true), 
	  level_number(1)
{
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
	setupWindowScaling();
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
	window = glfwCreateWindow(width, height, "Project KTV", nullptr, nullptr);
	if (window == nullptr) {
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}
	glfwSetWindowAspectRatio(window, 1200, 800);
	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	glfwSetKeyCallback(window, key_redirect);
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	auto mouse_button_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_click(_0, _1, _2); };
	glfwSetMouseButtonCallback(window, mouse_button_redirect);


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

	// set player speed base on resolution scaling
	playerSpeed = PLAYER_SPEED * defaultResolution.scaling;
	// Load level information 
	levelFileLoader.readFile(); 
	levels = levelFileLoader.getLevels(); 

	return window;
}

void WorldSystem::setupLevel(bool firstTime, bool restart) {
	int screen_width, screen_height;
	glfwGetFramebufferSize(window, &screen_width, &screen_height);
	if (!firstTime) {
		// Remove all players, enemies, blocks 
	
		if (!restart) {
			while (registry.enemies.entities.size() > 0)
				registry.remove_all_components_of(registry.enemies.entities.back());
			while (registry.blocks.entities.size() > 0)
				registry.remove_all_components_of(registry.blocks.entities.back());
			level_number += 1;
		} else if (restart) {
			while (registry.players.entities.size() > 0)
				registry.remove_all_components_of(registry.players.entities.back());
			while (registry.projectiles.entities.size() > 0)
				registry.remove_all_components_of(registry.projectiles.entities.back()); 
			// Add new players back immediately to prevent weird collision bug 
			int ind = level_number - 1;
			Level lvl = levels[ind];
			player_wizard = createWizard(renderer, lvl.player_position);
			if (twoPlayer.inTwoPlayerMode) {
				player2_wizard = createWizard(renderer, { screen_width / 10, screen_height * 0.66f });
			}
			while (registry.enemies.entities.size() > 0)
				registry.remove_all_components_of(registry.enemies.entities.back());
			while (registry.blocks.entities.size() > 0)
				registry.remove_all_components_of(registry.blocks.entities.back());
		}
	
	}
	int index = level_number - 1; 
	Level firstLevel = levels[index];
	auto enemies = firstLevel.enemies;
	auto enemy_types = firstLevel.enemy_types;
	auto num_enemy_types = firstLevel.num_enemy_types;
	for (int i = 0; i < enemy_types.size(); i++) {
		// enemy blob 
		if (enemy_types[i] == 1) {
			// Spawn enemies number of enemies for type enemy_type[i]
			for (int j = 0; j < enemies[i]; j++) {
				vec2 position = vec2(uniform_dist(rng) * (screen_width - (screen_width / 6.f)), screen_height / 16.f);
				vec2 velocity = vec2(0.f, 200.f * defaultResolution.scaling);
				createEnemyBlob(renderer, position, velocity);
			}
		}

		if (enemy_types[i] == 2) {
			for (int j = 0; j < enemies[i]; j++) {
				// Create enemy run
				vec2 position = vec2(uniform_dist(rng) * (screen_width - (screen_width / 6.f)), screen_height / 16.f);
				vec2 velocity = vec2(uniform_dist(rng) * 200.f * defaultResolution.scaling, uniform_dist(rng) * 200.f * defaultResolution.scaling);
				createEnemyRun(renderer, position, velocity);
			}
		}

		if (enemy_types[i] == 3) {
			for (int j = 0; j < enemies[i]; j++) {
				createEnemyHunter(renderer, vec2(screen_width * 0.66f, screen_height * 0.5f), vec2(1, 0));
			}
		}
	}

	// Blocks 
	for (int b = 0; b < firstLevel.block_positions.size(); b++) {
		vec2 block_pos_i = firstLevel.block_positions[b];
		std::string block_color_i = firstLevel.color; 
		createBlock(renderer, block_pos_i, block_color_i);
	}
	if (firstTime) {
		player_wizard = createWizard(renderer, firstLevel.player_position);
		if (twoPlayer.inTwoPlayerMode) {
			player2_wizard = createWizard(renderer, { screen_width / 10, screen_height * 0.66f });
		}
	}
	// Update state 
	isLevelOver = false;
}


void WorldSystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;
	// Playing background music indefinitely
	Mix_PlayMusic(background_music, -1);
    restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	// Get the screen dimensions
	int screen_width, screen_height;
	glfwGetFramebufferSize(window, &screen_width, &screen_height);

	// Updating window title with money
	std::stringstream title_ss;
	// Get hp of player 1 and player 2 
	int hp_p1 = 0; 
	int hp_p2 = 0;
	title_ss << "Level: " << level_number; 
	if (!registry.deathTimers.has(player_wizard) && registry.players.has(player_wizard)) {
		hp_p1 = registry.players.get(player_wizard).hp; 
	}
	if (twoPlayer.inTwoPlayerMode && !registry.deathTimers.has(player2_wizard)) {
		hp_p2 = registry.players.get(player2_wizard).hp; 
	}
	if (twoPlayer.inTwoPlayerMode) {
		title_ss << " Money P1: " << registry.players.get(player_wizard).money << " Health P1 " << hp_p1
			     << " & Money P2: " << registry.players.get(player2_wizard).money  << " Health P2 " << hp_p2;
	} else {
		title_ss << " Money: " << registry.players.get(player_wizard).money << " & Health P1 " << hp_p1;
	}
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
	    registry.remove_all_components_of(registry.debugComponents.entities.back());

	// Removing out of screen entities
	auto& motions_registry = registry.motions;
	auto& destinations_registry = registry.mouseDestinations;
	int nextLevel = level_number + 1; 
	if (isLevelOver && nextLevel <= levels.size()) {
		// Only if we have levels left we need to change level 
		initial_level_load = false; 
		setupLevel(initial_level_load, false); 
	}

	// Check level completion 
	if (registry.enemies.size() == 0) {
		isLevelOver = true; 
	}
	//// Spawning new enemy
	//next_enemyblob_spawn -= elapsed_ms_since_last_update * current_speed;
	//if (registry.enemyBlobs.components.size() <= MAX_ENEMIES && next_enemyblob_spawn < 0.f) {
	//	// Reset timer
	//	next_enemyblob_spawn = (ENEMY_DELAY_MS / 2) + uniform_dist(rng) * (ENEMY_DELAY_MS / 2);
	//	// Create enemyBlob
	//	vec2 position = vec2(uniform_dist(rng) * (screen_width - (screen_width / 6.f)), screen_height / 16.f);
	//	vec2 velocity = vec2(0.f, 200.f * defaultResolution.scaling);
	//	createEnemyBlob(renderer, position, velocity);
	//}

	//// Spawning new enemy run
	//// TODO: Make so that enemy cannot MOVE outside of first room
	//next_enemyrun_spawn -= elapsed_ms_since_last_update * current_speed;
	//if (registry.enemiesrun.components.size() <= MAX_ENEMIESRUN && next_enemyrun_spawn < 0.f) {
	//	// Reset timer
	//	next_enemyrun_spawn = (ENEMY_DELAY_MS / 2) + uniform_dist(rng) * (ENEMY_DELAY_MS / 2);
	//	// Create enemy run
	//	vec2 position = vec2(uniform_dist(rng) * (screen_width - (screen_width / 6.f)), screen_height / 16.f);
	//	vec2 velocity = vec2(uniform_dist(rng) * 200.f * defaultResolution.scaling, uniform_dist(rng) * 200.f * defaultResolution.scaling);
	//	createEnemyRun(renderer, position, velocity);
	//}

	if (twoPlayer.inTwoPlayerMode && destinations_registry.has(player2_wizard)) {
		Motion& motion = motions_registry.get(player2_wizard);
		MouseDestination& mouseDestination = destinations_registry.get(player2_wizard);

		if (abs(motion.position.x - mouseDestination.position.x) < 1.f && abs(motion.position.y - mouseDestination.position.y) < 1.f) {
			destinations_registry.remove(player2_wizard);
			motion.velocity = vec2(0,0);
		}
	}

	// update Stuck timers and remove if time drops below zero, similar to the death counter
	for (Entity entity : registry.stuckTimers.entities) {
		StuckTimer& counter = registry.stuckTimers.get(entity);
		// remove timer if current position is the different from "stuck" position
		if (registry.motions.get(entity).position != counter.stuck_pos) {
			registry.stuckTimers.remove(entity);
		}
		// else if entity is "stuck" in same position, progress timer
		else {
			// progress timer
			counter.counter_ms -= elapsed_ms_since_last_update;
			// remove entity (enemies/enemies run) when timer expires
			if (counter.counter_ms < 0) {
				registry.motions.get(entity).position = vec2(screen_width / 2.f, screen_height / 2.f);
			}
		}
	}

	for (Entity playerEntity : registry.players.entities) {
		Player& player = registry.players.get(playerEntity);
		if (player.isInvin) {
			player.invinTimerInMs -= elapsed_ms_since_last_update;
			if (player.invinTimerInMs < 0) {
				player.isInvin = false;
			}

		}
	}

	// handle player1 projectiles
	next_projectile_fire_player1 -= elapsed_ms_since_last_update * current_speed;
	Player& player1 = registry.players.get(player_wizard);
	Motion playerMotion = motions_registry.get(player_wizard);
	if (player1.isFiringProjectile && next_projectile_fire_player1 < 0.f) {
		next_projectile_fire_player1 = player1.PROJECTILE_FIRE_RATE;
		switch (player1.firingDirection) {
			case 0: // up
				createProjectile(renderer, playerMotion.position, { 0, -1.f * player1.PROJECTILE_SPEED * defaultResolution.scaling }, player_wizard);
				break;
			case 1: // right
				createProjectile(renderer, playerMotion.position, { player1.PROJECTILE_SPEED * defaultResolution.scaling, 0 }, player_wizard);
				break;
			case 2: // down
				createProjectile(renderer, playerMotion.position, { 0, player1.PROJECTILE_SPEED * defaultResolution.scaling }, player_wizard);
				break;
			case 3: // left
				createProjectile(renderer, playerMotion.position, { -1.f * player1.PROJECTILE_SPEED * defaultResolution.scaling, 0 }, player_wizard);
				break;
		}
	}

	// handle player2 projectile
	if (twoPlayer.inTwoPlayerMode) {
		next_projectile_fire_player2 -= elapsed_ms_since_last_update * current_speed;
		Motion player2Motion = motions_registry.get(player2_wizard);
		Player& player2 = registry.players.get(player2_wizard);
		if (player2.isFiringProjectile && next_projectile_fire_player2 < 0.f) {
			next_projectile_fire_player2 = player2.PROJECTILE_FIRE_RATE;
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			if (registry.inShops.has(player2_wizard)) {
				y += DEFAULT_HEIGHT * defaultResolution.scaling;
			}
			float dx = (float)x - player2Motion.position.x;
			float dy = (float)y - player2Motion.position.y;
			float h = sqrtf(powf(dx, 2) + powf(dy, 2));
			float scale = player2.PROJECTILE_SPEED * defaultResolution.scaling / h;
			createProjectile(renderer, player2Motion.position, { dx * scale, dy * scale }, player2_wizard);
		}
	}


	return true;
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
	// Debugging for memory/component leaks

	registry.list_all_components();
	// Restart game starts from level 1 always 
	initial_level_load = true; 
	level_number = 1; 
	spawnPowerup = true; 
	// Set all states to default
	// Reset the game speed
	current_speed = 1.f;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all fish, turtles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
	    registry.remove_all_components_of(registry.motions.entities.back());

	// Debugging for memory/component leaks
	registry.list_all_components();

	// Create walls and doors
	createWalls(screenWidth, screenHeight);
	createADoor(screenWidth, screenHeight);
	setupLevel(initial_level_load, false); 
	initial_level_load = false; 

}

void WorldSystem::createADoor(int screenWidth, int screenHeight) {
	vec2 doorPosition = { screenWidth / 2 , screenHeight };
	vec2 doorScale = { screenWidth * doorWidthScale, SHOP_WALL_THICKNESS };
	createDoor(doorPosition, doorScale);
}


// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {
	int w, h;
	glfwGetWindowSize(window, &w, &h);
	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
        restart_game();
	}

	// Debugging
	if (key == GLFW_KEY_BACKSLASH) {
		if (action == GLFW_RELEASE) {
			debugging.in_debug_mode = !debugging.in_debug_mode;
		}
	}
	
	Motion& player1motion = registry.motions.get(player_wizard);
	Player& player = registry.players.get(player_wizard);

	vec2 currentVelocity = vec2(player1motion.velocity.x, player1motion.velocity.y);
	if (action == GLFW_PRESS && key == GLFW_KEY_W) {
		player1motion.velocity = vec2(currentVelocity.x, currentVelocity.y - playerSpeed);
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_W) {
		player1motion.velocity = vec2(currentVelocity.x, currentVelocity.y + playerSpeed);
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_S) {
		player1motion.velocity = vec2(currentVelocity.x, currentVelocity.y + playerSpeed);
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_S) {
		player1motion.velocity = vec2(currentVelocity.x, currentVelocity.y - playerSpeed);
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_A) {
		if (!registry.flips.has(player_wizard)){
				registry.flips.emplace(player_wizard);
				Flip& flipped = registry.flips.get(player_wizard);
				flipped.left = true;
				player1motion.scale = vec2({ -WIZARD_BB_WIDTH * defaultResolution.scaling, WIZARD_BB_HEIGHT * defaultResolution.scaling });
		}

		player1motion.velocity = vec2(currentVelocity.x - playerSpeed, currentVelocity.y);
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_A) {
		player1motion.velocity = vec2(currentVelocity.x + playerSpeed, currentVelocity.y);

	}

	if (action == GLFW_PRESS && key == GLFW_KEY_D) {
		if (registry.flips.has(player_wizard))
		{
			Flip& flipped = registry.flips.get(player_wizard);
			flipped.left = false;
			registry.flips.remove(player_wizard);
			player1motion.scale = vec2({ WIZARD_BB_WIDTH * defaultResolution.scaling, WIZARD_BB_HEIGHT * defaultResolution.scaling });
		}
		player1motion.velocity = vec2(currentVelocity.x + playerSpeed, currentVelocity.y);
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_D) {
		player1motion.velocity = vec2(currentVelocity.x - playerSpeed, currentVelocity.y);
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_T) {
		player.isFiringProjectile = true;
		player.firingDirection = 0;
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_G) {
		player.isFiringProjectile = true;
		player.firingDirection = 2;
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_H) {
		player.isFiringProjectile = true;
		player.firingDirection = 1;

	}

	if (action == GLFW_PRESS && key == GLFW_KEY_F) {
		player.isFiringProjectile = true;
		player.firingDirection = 3;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_L) {
		setupLevel(initial_level_load, true); 
	}
	
	if (action == GLFW_RELEASE && (key == GLFW_KEY_F || key == GLFW_KEY_H || key == GLFW_KEY_G || key == GLFW_KEY_T)) {
		player.isFiringProjectile = false;
	}

	// Open/close door
	if (action == GLFW_PRESS && key == GLFW_KEY_O) {
		if (registry.doors.entities.size() == 0) {
			createADoor(w, h);
		} else {
			Entity door = registry.doors.entities.front();
			registry.remove_all_components_of(door);
		}
	}

	// Debugging
	if (key == GLFW_KEY_Z) {
		if (action == GLFW_RELEASE)
			debugging.in_debug_mode = false;
		else
			debugging.in_debug_mode = true;
	}

	// Switch between one player/two player
	if (action == GLFW_PRESS && key == GLFW_KEY_2) {
		if (twoPlayer.inTwoPlayerMode) {
			twoPlayer.inTwoPlayerMode = false;
		} else {
			twoPlayer.inTwoPlayerMode = true;
		}
		restart_game();
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
	// JA: adding flip left action to wizard 2
	if (twoPlayer.inTwoPlayerMode) {
		Motion& motion = registry.motions.get(player2_wizard);
		float deltaX = mouse_position.x - motion.position.x;
		if (deltaX < 0 && !registry.flips.has(player2_wizard))
		{
			registry.flips.emplace(player2_wizard);
			Flip& flipped = registry.flips.get(player2_wizard);
			flipped.left = true;
			motion.scale = vec2({ -WIZARD_BB_WIDTH * defaultResolution.scaling, WIZARD_BB_HEIGHT * defaultResolution.scaling });
		}
		else
		{
			if (deltaX > 0 && registry.flips.has(player2_wizard))
			{
				Flip& flipped = registry.flips.get(player2_wizard);
				flipped.left = false;
				registry.flips.remove(player2_wizard);
				motion.scale = vec2({ WIZARD_BB_WIDTH * defaultResolution.scaling, WIZARD_BB_HEIGHT * defaultResolution.scaling });
			}
		}
	}
}

void WorldSystem::on_mouse_click(int button, int action, int mods) {
	if (twoPlayer.inTwoPlayerMode) {
		if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
			if (registry.mouseDestinations.has(player2_wizard))
				registry.mouseDestinations.remove(player2_wizard);
			Motion& wizard2_motion = registry.motions.get(player2_wizard);
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			if (registry.inShops.has(player2_wizard)) {
				y += DEFAULT_HEIGHT * defaultResolution.scaling;
			}
			float dx = (float)x - wizard2_motion.position.x;
			float dy = (float)y - wizard2_motion.position.y;
			float h = sqrtf(powf(dx, 2) + powf(dy, 2));
			float scale = playerSpeed / h;
			wizard2_motion.velocity = vec2(dx * scale, dy * scale);
			registry.mouseDestinations.emplace(player2_wizard, vec2(x, y));
		}

		if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT) {
			Motion& wizard2_motion = registry.motions.get(player2_wizard);
			Player& player = registry.players.get(player2_wizard);
			player.isFiringProjectile = true;
		}

		if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_RIGHT) {
			Player& player = registry.players.get(player2_wizard);
			player.isFiringProjectile = false;
		}

	}
}

void WorldSystem::setupWindowScaling() {
	gameHeightScale = 1600.f / (float)defaultResolution.height;
	doorWidthScale = 200.f / (float)defaultResolution.width;
}

void WorldSystem::createWalls(int screenWidth, int screenHeight) {
	// Create perimeter walls
	vec2 leftWallPos = { 0, screenHeight * gameHeightScale / 2 };
	vec2 rightWallPos = { screenWidth, screenHeight * gameHeightScale / 2 };
	vec2 topWallPos = { screenWidth / 2, 0 };
	vec2 bottomWallPos = { screenWidth / 2, screenHeight * gameHeightScale };
	vec2 verticalWallScale = { WALL_THICKNESS, screenHeight * gameHeightScale };
	vec2 horizontalWallScale = { screenWidth, WALL_THICKNESS };
	createWall(leftWallPos, verticalWallScale);
	createWall(rightWallPos, verticalWallScale);
	createWall(topWallPos, horizontalWallScale);
	createWall(bottomWallPos, horizontalWallScale);

	// Create middle shop walls
	vec2 middleWallLeftPos = { 0, screenHeight };
	vec2 middleWallRightPos = { screenWidth, screenHeight };
	vec2 shopWallScale = { screenWidth - (screenWidth * doorWidthScale), SHOP_WALL_THICKNESS };
	createWall(middleWallLeftPos, shopWallScale);
	createWall(middleWallRightPos, shopWallScale);
}

void WorldSystem::setPlayerMode() {
	int playerMode;
	do {
		std::string input;
		printf("Input 1 for 1 player mode and 2 for 2 players mode.\n");
		std::cin >> input;
		try {
			playerMode = std::stoi(input);
		}
		catch (...) {
			playerMode = 0;
		}
	} while (playerMode != 1 && playerMode != 2);
	if (playerMode == 1) {
		twoPlayer.inTwoPlayerMode = false;
	}
	else {
		twoPlayer.inTwoPlayerMode = true;
	}
}

void WorldSystem::setResolution() {
	int resolutionSelection;
	do {
		std::string input;
		printf("Input 1 for 2400 by 1600, 2 for 1200 by 800 and 3 for 600 by 400.\n");
		std::cin >> input;
		try {
			resolutionSelection = std::stoi(input);
		}
		catch (...) {
			resolutionSelection = 0;
		}
	} while (resolutionSelection != 1 && resolutionSelection != 2 && resolutionSelection != 3);

	if (resolutionSelection == 1) {
		defaultResolution.width = 2400;
		defaultResolution.height = 1600;
		defaultResolution.scaling = 2;
	} else if (resolutionSelection == 2) {
		defaultResolution.width = 1200;
		defaultResolution.height = 800;
		defaultResolution.scaling = 1;
	} else {
		defaultResolution.width = 600;
		defaultResolution.height = 400;
		defaultResolution.scaling = 0.5;
	}
}

