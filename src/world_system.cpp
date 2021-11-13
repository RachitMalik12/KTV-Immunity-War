// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>

#include "physics_system.hpp"

// Game configuration
const size_t DEFAULT_HEIGHT = 800;
const int WALL_THICKNESS = 40;
const int SHOP_WALL_THICKNESS = 100;

// Create the fish world
WorldSystem::WorldSystem()
	: isLevelOver(false),
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

	// Load level information 
	levelFileLoader.readFile(); 
	levels = levelFileLoader.getLevels(); 

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;
	// Playing background music indefinitely
	Mix_PlayMusic(background_music, -1);
    restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());

	// Get the screen dimensions
	int screen_width, screen_height;
	glfwGetFramebufferSize(window, &screen_width, &screen_height);

	animateStep(elapsed_ms_since_last_update);
	updateWindowTitle();
	levelCompletionCheck();
	resolveMouseControl();
	stuckTimer(elapsed_ms_since_last_update, screen_width, screen_height);
	invincibilityTimer(elapsed_ms_since_last_update);
	handlePlayerOneProjectile(elapsed_ms_since_last_update);
	handlePlayerTwoProjectile(elapsed_ms_since_last_update);
	deathHandling();

	return true;
}

void WorldSystem::deathHandling() {
	if (twoPlayer.inTwoPlayerMode) {
		Player& player1 = registry.players.get(player_wizard);
		Player& player2 = registry.players.get(player2_wizard);
		if (player1.isDead) {
			Motion& player1Motion = registry.motions.get(player_wizard);
			player1Motion.velocity = vec2(0, 0);
			registry.renderRequests.remove(player_wizard);
			// TODO: Implement player death animation
		}
		if (player2.isDead) {
			Motion& player2Motion = registry.motions.get(player2_wizard);
			player2Motion.velocity = vec2(0, 0);
			registry.renderRequests.remove(player2_wizard);
			// TODO: Implement player death animation
		}
		if (player1.isDead && player2.isDead) {
			setupLevel(level_number);
		}
	}
	else {
		Player& player1 = registry.players.get(player_wizard);
		if (player1.isDead) {
			setupLevel(level_number);
		}
	}
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
	// Debugging for memory/component leaks

	registry.list_all_components();
	// Restart game starts from level 1 always 
	level_number = 1; 

	// set help mode to false again
	helpMode.inHelpMode = false;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all fish, turtles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
	    registry.remove_all_components_of(registry.motions.entities.back());

	// reset player stats
	while (registry.playerStats.entities.size() > 0)
		registry.remove_all_components_of(registry.playerStats.entities.back());
	setPlayerStats();

	// Debugging for memory/component leaks
	registry.list_all_components();

	// Create walls and doors
	createWalls(screenWidth, screenHeight);
	createADoor(screenWidth, screenHeight);
	setupLevel(level_number); 
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

void WorldSystem::frame_counter(float elapsed_ms, Entity entity)
{
	Animation& playerOneAnimation = registry.animations.get(entity);
	playerOneAnimation.animationTimer += elapsed_ms;
	if (playerOneAnimation.animationTimer > playerOneAnimation.animationSpeed) {
		playerOneAnimation.xFrame = (playerOneAnimation.xFrame + 1) % playerOneAnimation.numOfFrames;
		playerOneAnimation.animationTimer = 0;
	}
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {
	int w, h;
	glfwGetWindowSize(window, &w, &h);
	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
		storyMode.firstLoad = true;
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
	PlayerStat& playerOneStat = registry.playerStats.get(player.playerStat);
	Animation& playerOneAnimation = registry.animations.get(registry.animations.entities.front());

	if (!player.isDead) {
		vec2 currentVelocity = vec2(player1motion.velocity.x, player1motion.velocity.y);
		if (action == GLFW_PRESS && key == GLFW_KEY_W) {
			playerOneAnimation.pressed = true;
			player1motion.velocity = vec2(currentVelocity.x, currentVelocity.y - playerOneStat.movementSpeed);
			playerOneAnimation.xFrame = 0;
			playerOneAnimation.yFrame = 0;
			registry.renderRequests.remove(player_wizard);
			registry.renderRequests.insert(
				player_wizard,
				{ TEXTURE_ASSET_ID::KNIGHT,
					EFFECT_ASSET_ID::KNIGHT,
					GEOMETRY_BUFFER_ID::SPRITE }, false);
		}
		if (action == GLFW_RELEASE && key == GLFW_KEY_W) {
			playerOneAnimation.pressed = false;
			playerOneAnimation.xFrame = 0;
			player1motion.velocity = vec2(currentVelocity.x, 0);
		}

		if (action == GLFW_PRESS && key == GLFW_KEY_S) {
			playerOneAnimation.pressed = true;
			player1motion.velocity = vec2(currentVelocity.x, currentVelocity.y + playerOneStat.movementSpeed);
			int counter = 0;
			playerOneAnimation.xFrame = 0;
			playerOneAnimation.yFrame = 2;
			registry.renderRequests.remove(player_wizard);
			registry.renderRequests.insert(
				player_wizard,
				{ TEXTURE_ASSET_ID::KNIGHT,
					EFFECT_ASSET_ID::KNIGHT,
					GEOMETRY_BUFFER_ID::SPRITE }, false);
		}
		if (action == GLFW_RELEASE && key == GLFW_KEY_S) {
			playerOneAnimation.pressed = false;
			playerOneAnimation.xFrame = 0;
			player1motion.velocity = vec2(currentVelocity.x, 0);
		}

		if (action == GLFW_PRESS && key == GLFW_KEY_A) {
			playerOneAnimation.pressed = true;
			player1motion.velocity = vec2(currentVelocity.x - playerOneStat.movementSpeed, currentVelocity.y);
			playerOneAnimation.xFrame = 0;
			playerOneAnimation.yFrame = 1;
			registry.renderRequests.remove(player_wizard);
			registry.renderRequests.insert(
				player_wizard,
				{ TEXTURE_ASSET_ID::KNIGHT,
					EFFECT_ASSET_ID::KNIGHT,
					GEOMETRY_BUFFER_ID::SPRITE }, false);
		}
		if (action == GLFW_RELEASE && key == GLFW_KEY_A) {
			playerOneAnimation.pressed = false;
			playerOneAnimation.xFrame = 0;
			player1motion.velocity = vec2(0, currentVelocity.y);

		}

		if (action == GLFW_PRESS && key == GLFW_KEY_D) {
			playerOneAnimation.pressed = true;
			player1motion.velocity = vec2(currentVelocity.x + playerOneStat.movementSpeed, currentVelocity.y);
			playerOneAnimation.xFrame = 0;
			playerOneAnimation.yFrame = 3;
			registry.renderRequests.remove(player_wizard);
			registry.renderRequests.insert(
				player_wizard,
				{ TEXTURE_ASSET_ID::KNIGHT,
					EFFECT_ASSET_ID::KNIGHT,
					GEOMETRY_BUFFER_ID::SPRITE }, false);
		}

		if (action == GLFW_RELEASE && key == GLFW_KEY_D) {
			playerOneAnimation.pressed = false;
			playerOneAnimation.xFrame = 0;
			player1motion.velocity = vec2(0, currentVelocity.y);
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
	}	

	if (action == GLFW_PRESS && key == GLFW_KEY_K) {
		// Save current level. 
		dataManager.setLevelNumber(level_number); 
		if (twoPlayer.inTwoPlayerMode) {
			dataManager.setPlayerStatEntity(player_stat, player2_stat);
			dataManager.saveFile(2);
		}
		else {
			dataManager.setPlayerStatEntity(player_stat); 
			dataManager.saveFile(1); 
		}
		
	}

	// level loading
	if (action == GLFW_PRESS && key == GLFW_KEY_L) {
		// Update player mode based on savefile
		dataManager.setPlayerModeFromFile();
		int player_mode_file = dataManager.getPlayerMode();
		if (player_mode_file == 1) {
			twoPlayer.inTwoPlayerMode = false;
		}
		else {
			twoPlayer.inTwoPlayerMode = true;
		}
		setPlayerStats(); 

		if (twoPlayer.inTwoPlayerMode) {
			dataManager.setPlayerStatEntity(player_stat, player2_stat); 
		}
		else {
			dataManager.setPlayerStatEntity(player_stat);
		}
		bool loadFile = dataManager.loadFile();
		if (!loadFile) {
			// If load failed due to file missing,load level 1 with 1 player. 
			twoPlayer.inTwoPlayerMode = false;
			setupLevel(1);
		}
		else {
			level_number = dataManager.getLevelNumber();
			int playerModeFile = dataManager.getPlayerMode();
			if (playerModeFile == 1) {
				twoPlayer.inTwoPlayerMode = false;
			}
			else {
				twoPlayer.inTwoPlayerMode = true;
			}
			setupLevel(level_number);
		}
	}
	// load level 1
	if (action == GLFW_PRESS && key == GLFW_KEY_1) {
		level_number = 1; 
		setupLevel(level_number);
	}
	// load level 2
	if (action == GLFW_PRESS && key == GLFW_KEY_2) {
		level_number = 2; 
		setupLevel(level_number);
	}
	// load level 3
	if (action == GLFW_PRESS && key == GLFW_KEY_3) {
		level_number = 3;
		setupLevel(level_number);
	}
	// load level 4
	if (action == GLFW_PRESS && key == GLFW_KEY_4) {
		level_number = 4;
		setupLevel(level_number);
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
	if (action == GLFW_PRESS && key == GLFW_KEY_X) {
		if (twoPlayer.inTwoPlayerMode) {
			twoPlayer.inTwoPlayerMode = false;
		} else {
			twoPlayer.inTwoPlayerMode = true;
		}
		restart_game();
	}

	// Control if in help mode or not
	if (action == GLFW_RELEASE && key == GLFW_KEY_P) {
		if (helpMode.inHelpMode) {
			helpMode.inHelpMode = false;
			for (Entity entity : registry.helpModes.entities) {
				registry.remove_all_components_of(entity);
			}
		}
		else {
			helpMode.inHelpMode = true;
			createHelp();
		}
	}

	// storymode
	if (action == GLFW_RELEASE && key == GLFW_KEY_SPACE && storyMode.firstLoad) {
		Entity ent;
		if (storyMode.inStoryMode ==6) {
			storyMode.inStoryMode = 0;
			for (Entity entity : registry.storyModes.entities) {
				registry.remove_all_components_of(entity);
			}
			storyMode.firstLoad = false;
		}
		else if (storyMode.inStoryMode == 0){
			storyMode.inStoryMode = 1;
			createStory();
		}
		else if (storyMode.inStoryMode == 1) {
			storyMode.inStoryMode = 2;
			ent = registry.storyModes.entities[0];
			registry.renderRequests.remove(ent);
			registry.renderRequests.insert(
				ent,
				{ TEXTURE_ASSET_ID::FRAME2,
					EFFECT_ASSET_ID::TEXTURED,
					GEOMETRY_BUFFER_ID::SPRITE }, false);
			
		}
		else if (storyMode.inStoryMode == 2) {
			storyMode.inStoryMode = 3;
			ent = registry.storyModes.entities[0];
			registry.renderRequests.remove(ent);
			registry.renderRequests.insert(
				ent,
				{ TEXTURE_ASSET_ID::FRAME3,
					EFFECT_ASSET_ID::TEXTURED,
					GEOMETRY_BUFFER_ID::SPRITE }, false);
		}
		else if (storyMode.inStoryMode == 3) {
			storyMode.inStoryMode = 4;
			ent = registry.storyModes.entities[0];
			registry.renderRequests.remove(ent);
			registry.renderRequests.insert(
				ent,
				{ TEXTURE_ASSET_ID::FRAME4,
					EFFECT_ASSET_ID::TEXTURED,
					GEOMETRY_BUFFER_ID::SPRITE }, false);
		}
		else if (storyMode.inStoryMode == 4) {
			storyMode.inStoryMode = 5;
			ent = registry.storyModes.entities[0];
			registry.renderRequests.remove(ent);
			registry.renderRequests.insert(
				ent,
				{ TEXTURE_ASSET_ID::FRAME5,
					EFFECT_ASSET_ID::TEXTURED,
					GEOMETRY_BUFFER_ID::SPRITE }, false);
		}
		else if (storyMode.inStoryMode == 5) {
			storyMode.inStoryMode = 6;
			ent = registry.storyModes.entities[0];
			registry.renderRequests.remove(ent);
			registry.renderRequests.insert(
				ent,
				{ TEXTURE_ASSET_ID::FRAME6,
					EFFECT_ASSET_ID::TEXTURED,
					GEOMETRY_BUFFER_ID::SPRITE }, false);

		}
	}
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
		Player& wizard2_player = registry.players.get(player2_wizard);
		if (!wizard2_player.isDead) {
			if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
				if (registry.mouseDestinations.has(player2_wizard))
					registry.mouseDestinations.remove(player2_wizard);
				Motion& wizard2_motion = registry.motions.get(player2_wizard);
				PlayerStat& playerTwoStat = registry.playerStats.get(wizard2_player.playerStat);
				double x, y;
				glfwGetCursorPos(window, &x, &y);
				if (registry.inShops.has(player2_wizard)) {
					y += DEFAULT_HEIGHT * defaultResolution.scaling;
				}
				float dx = (float)x - wizard2_motion.position.x;
				float dy = (float)y - wizard2_motion.position.y;
				float h = sqrtf(powf(dx, 2) + powf(dy, 2));
				float scale = playerTwoStat.movementSpeed / h;
				wizard2_motion.velocity = vec2(dx * scale, dy * scale);
				registry.mouseDestinations.emplace(player2_wizard, vec2(x, y));
			}

			if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT) {
				wizard2_player.isFiringProjectile = true;
			}

			if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_RIGHT) {
				wizard2_player.isFiringProjectile = false;
			}
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

void WorldSystem::setResolution() {
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW");
	}
	int resolutionSelection;
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	if (mode->width <= 3840 && mode->width > 2560) {
		// 4k resolution (3840 x 2160)
		defaultResolution.width = 2400;
		defaultResolution.height = 1600;
		defaultResolution.scaling = 2;
	}
	else if (mode->width <= 2560 && mode->width > 1920) {
		// 1440p resolution (2560 x 1440)
		defaultResolution.width = 1800;
		defaultResolution.height = 1200;
		defaultResolution.scaling = 1.5;
	}
	else if (mode->width <=  1920 && mode->width > 1280) {
		// 1080p resolution (1920 x 1080)
		defaultResolution.width = 1200;
		defaultResolution.height = 800;
		defaultResolution.scaling = 1;
	}
	else {
		// 720p resolution (1280 x 720)
		defaultResolution.width = 600;
		defaultResolution.height = 400;
		defaultResolution.scaling = 0.5;
	}
}

void WorldSystem::setPlayerStats() {
	auto entity = Entity();
	registry.playerStats.emplace(entity);
	player_stat = entity;
	PlayerStat& playerOneStat = registry.playerStats.get(player_stat);
	playerOneStat.damage += 1; // The sword does 1 more damage than the waterball.
	playerOneStat.movementSpeed = playerOneStat.movementSpeed * defaultResolution.scaling;
	playerOneStat.projectileSpeed = playerOneStat.projectileSpeed * defaultResolution.scaling;
	if (twoPlayer.inTwoPlayerMode) {
		auto entity2 = Entity();
		registry.playerStats.emplace(entity2);
		player2_stat = entity2;
		PlayerStat& playerTwoStat = registry.playerStats.get(player2_stat);
		playerTwoStat.movementSpeed = playerTwoStat.movementSpeed * defaultResolution.scaling;
		playerTwoStat.projectileSpeed = playerTwoStat.projectileSpeed * defaultResolution.scaling;
	}
}

void WorldSystem::handlePlayerTwoProjectile(float elapsed_ms_since_last_update) {
	// handle player2 projectile
	if (twoPlayer.inTwoPlayerMode) {
		next_projectile_fire_player2 -= elapsed_ms_since_last_update;
		Motion player2Motion = registry.motions.get(player2_wizard);
		Player& player2 = registry.players.get(player2_wizard);
		PlayerStat& playerTwoStat = registry.playerStats.get(player2.playerStat);
		if (player2.isFiringProjectile && next_projectile_fire_player2 < 0.f) {
			next_projectile_fire_player2 = playerTwoStat.projectileFireRate;
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			if (registry.inShops.has(player2_wizard)) {
				y += DEFAULT_HEIGHT * defaultResolution.scaling;
			}
			float dx = (float)x - player2Motion.position.x;
			float dy = (float)y - player2Motion.position.y;
			float h = sqrtf(powf(dx, 2) + powf(dy, 2));
			float scale = playerTwoStat.projectileSpeed / h;
			float th = atan2(dy, dx);
			createProjectile(renderer, player2Motion.position, { dx * scale, dy * scale }, th, player2_wizard);
		}
	}
}

void WorldSystem::handlePlayerOneProjectile(float elapsed_ms_since_last_update) {
	// handle player1 projectiles
	next_projectile_fire_player1 -= elapsed_ms_since_last_update;
	Player& player1 = registry.players.get(player_wizard);
	PlayerStat& playerOneStat = registry.playerStats.get(player1.playerStat);
	Motion playerMotion = registry.motions.get(player_wizard);
	if (player1.isFiringProjectile && next_projectile_fire_player1 < 0.f) {
		next_projectile_fire_player1 = playerOneStat.projectileFireRate;

		switch (player1.firingDirection) {
		case 0: // up
			createSword(renderer, M_PI * 3 / 2 - M_PI / 3, player_wizard);
			break;
		case 1: // right
		    createSword(renderer, -M_PI / 3, player_wizard);
			break;
		case 2: // down
			createSword(renderer, M_PI / 2 - M_PI / 3, player_wizard);
			break;
		case 3: // left
			createSword(renderer, M_PI - M_PI / 3, player_wizard);
			break;
		}
	}
}

void WorldSystem::invincibilityTimer(float elapsed_ms_since_last_update) {
	for (Entity playerEntity : registry.players.entities) {
		Player& player = registry.players.get(playerEntity);
		if (player.isInvin) {
			player.invinTimerInMs -= elapsed_ms_since_last_update;
			if (player.invinTimerInMs < 0) {
				player.isInvin = false;
			}

		}
	}
}

void WorldSystem::stuckTimer(float elapsed_ms_since_last_update, int screen_width, int screen_height) {
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
}

void WorldSystem::resolveMouseControl() {
	if (twoPlayer.inTwoPlayerMode && registry.mouseDestinations.has(player2_wizard)) {
		Motion& motion = registry.motions.get(player2_wizard);
		MouseDestination& mouseDestination = registry.mouseDestinations.get(player2_wizard);

		if (abs(motion.position.x - mouseDestination.position.x) < 1.f && abs(motion.position.y - mouseDestination.position.y) < 1.f) {
			registry.mouseDestinations.remove(player2_wizard);
			motion.velocity = vec2(0, 0);
		}
	}
}

void WorldSystem::levelCompletionCheck() {
	// Check level completion 
	if (registry.enemies.size() == 0) {
		isLevelOver = true;
	}
	int nextLevel = level_number + 1;
	if (isLevelOver && nextLevel <= levels.size()) {
		// Only if we have levels left we need to change level 
		level_number = nextLevel;
		setupLevel(level_number);
	}
}

void WorldSystem::updateWindowTitle() {
	// Updating window title with money
	std::stringstream title_ss;
	// Get hp of player 1 and player 2 
	int hp_p1 = 0;
	int hp_p2 = 0;
	title_ss << "Level: " << level_number;
	hp_p1 = registry.players.get(player_wizard).hp;
	if (twoPlayer.inTwoPlayerMode) {
		hp_p2 = registry.players.get(player2_wizard).hp;
	}
	if (twoPlayer.inTwoPlayerMode) {
		title_ss << " P1 Money: " << registry.playerStats.get(registry.players.get(player_wizard).playerStat).money << " Health: " << hp_p1
			<< " & P2 Money: " << registry.playerStats.get(registry.players.get(player2_wizard).playerStat).money << " Health: " << hp_p2;
	}
	else {
		title_ss << " Money: " << registry.playerStats.get(registry.players.get(player_wizard).playerStat).money << " & Health P1 " << hp_p1;
	}
	glfwSetWindowTitle(window, title_ss.str().c_str());
}

void WorldSystem::animateStep(float elapsed_ms_since_last_update) {
	//animate
	for (Entity entity : registry.animations.entities) {
		Animation& animation = registry.animations.get(entity);
		if (animation.pressed) {
			float prev_frame = animation.xFrame;
			frame_counter(elapsed_ms_since_last_update, entity);
			if (entity.getId() == player_wizard.getId() && animation.xFrame == 0)
				animation.xFrame = 1;
			if (entity.getId() == player_wizard.getId()) {
				registry.renderRequests.remove(entity);
				registry.renderRequests.insert(
					entity,
					{ TEXTURE_ASSET_ID::KNIGHT,
						EFFECT_ASSET_ID::KNIGHT,
						GEOMETRY_BUFFER_ID::SPRITE }, false);
			}
		}
	}
}

void WorldSystem::setupLevel(int levelNum) {
	int screen_width, screen_height;
	glfwGetFramebufferSize(window, &screen_width, &screen_height);

	while (registry.players.entities.size() > 0)
		registry.remove_all_components_of(registry.players.entities.back());
	while (registry.projectiles.entities.size() > 0)
		registry.remove_all_components_of(registry.projectiles.entities.back());
	while (registry.enemyProjectiles.entities.size() > 0)
		registry.remove_all_components_of(registry.enemyProjectiles.entities.back());
	while (registry.enemies.entities.size() > 0)
		registry.remove_all_components_of(registry.enemies.entities.back());
	while (registry.blocks.entities.size() > 0)
		registry.remove_all_components_of(registry.blocks.entities.back());

	int index = levelNum - 1;
	Level level = levels[index];
	auto enemies = level.enemies;
	auto enemy_types = level.enemy_types;
	auto enemyPositions = level.enemyPositions;
	for (int i = 0; i < enemyPositions.size(); i++) {
		for (int j = 0; j < enemyPositions[i].size(); j++) {
			createEnemy(renderer, enemyPositions[i][j] * defaultResolution.scaling, enemy_types[i]);
		}
	}

	// Blocks 
	for (int b = 0; b < level.block_positions.size(); b++) {
		vec2 block_pos_i = level.block_positions[b];
		std::string block_color_i;
		if (uniform_dist(rng) < 0.33) {
			block_color_i = "red";
		}
		else if (uniform_dist(rng) >= 0.33 && uniform_dist(rng) < 0.66) {
			block_color_i = "orange";
		}
		else {
			block_color_i = "yellow";
		}
		createBlock(renderer, block_pos_i * defaultResolution.scaling, block_color_i);
	}

	player_wizard = createKnight(renderer, level.player_position * defaultResolution.scaling);
	Player& player1 = registry.players.get(player_wizard);
	player1.playerStat = player_stat;
	PlayerStat& playerOneStat = registry.playerStats.get(player_stat);
	player1.hp = playerOneStat.maxHp;
	if (twoPlayer.inTwoPlayerMode) {
		player2_wizard = createWizard(renderer, level.player2_position * defaultResolution.scaling);
		Player& player2 = registry.players.get(player2_wizard);
		player2.playerStat = player2_stat;
		PlayerStat& playerTwoStat = registry.playerStats.get(player2_stat);
		player2.hp = playerTwoStat.maxHp;
	}

	// Reset player position on level transition
	Motion& player1Motion = registry.motions.get(player_wizard);
	player1Motion.position = level.player_position * defaultResolution.scaling;
	if (twoPlayer.inTwoPlayerMode) {
		Motion& player2Motion = registry.motions.get(player2_wizard);
		player2Motion.position = level.player2_position * defaultResolution.scaling;
	}
	// Update state 
	isLevelOver = false;
}