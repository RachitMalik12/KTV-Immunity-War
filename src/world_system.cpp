// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>

#include "physics_system.hpp"

// Create the fish world
WorldSystem::WorldSystem()
	: isLevelOver(false),
	  isTransitionOver(false),
	  firstEntranceToShop(true),
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
	auto entity = Entity();
	registry.titles.emplace(entity);
	registry.titles.get(entity).window = window;
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

	progressBrightenScreen(elapsed_ms_since_last_update);
	levelCompletionCheck(elapsed_ms_since_last_update);
	stopPlayerAtMouseDestination();
	stuckTimer(elapsed_ms_since_last_update, screen_width, screen_height);
	invincibilityTimer(elapsed_ms_since_last_update);
	checkIfPlayersAreMoving();
	animateKnight(elapsed_ms_since_last_update);
	animateWizard(elapsed_ms_since_last_update);
	handlePlayerOneAttack(elapsed_ms_since_last_update);
	handlePlayerTwoProjectile(elapsed_ms_since_last_update);
	deathHandling();
	return true;
}

void WorldSystem::deathHandling() {
	ScreenState& screen = registry.screenStates.components[0];
	if (twoPlayer.inTwoPlayerMode) {
		Player& player1 = registry.players.get(player_knight);
		Player& player2 = registry.players.get(player2_wizard);
		if (player1.isDead) {
			Motion& player1Motion = registry.motions.get(player_knight);
			player1Motion.velocity = vec2(0, 0);
			registry.renderRequests.remove(player_knight);
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
		Player& player1 = registry.players.get(player_knight);
		if (player1.isDead) {
			setupLevel(level_number);
		}
	}
}

void WorldSystem::progressGameEndEffect(float elapsed_ms_since_last_update) {
	// Check level completion 
	if (isGameOver == true) {
		isGameOver = false;
		auto entity1 = Entity();
		registry.deathTimers.emplace(entity1);
	}

	ScreenState& screen = registry.screenStates.components[0];
	float min_counter_ms = 3000.f;
	for (Entity entity : registry.deathTimers.entities) {
		// progress timer
		DeathTimer& counter = registry.deathTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		if (counter.counter_ms < min_counter_ms) {
			min_counter_ms = counter.counter_ms;
		}

		// stop shake effect and move onto next level once timer expires
		if (counter.counter_ms < 0) {
			registry.deathTimers.remove(entity);
			setupLevel(level_number);
			screen.game_over_factor = 0;
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
	setPlayersStats();

	// Debugging for memory/component leaks
	registry.list_all_components();

	// Create background texture
	createBackground(renderer, vec2(defaultResolution.width / 2, defaultResolution.height));

	// Setup level
	setupLevel(level_number);
}

void WorldSystem::createADoor(int screenWidth, int screenHeight) {
	vec2 doorPosition = { screenWidth / 2 , screenHeight };
	vec2 doorScale = { screenWidth * doorWidthScale, defaultResolution.shopWallThickness };
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
	
	Motion& player1motion = registry.motions.get(player_knight);
	Player& player = registry.players.get(player_knight);
	PlayerStat& playerOneStat = registry.playerStats.get(player.playerStat);
	KnightAnimation& playerOneAnimation = registry.knightAnimations.get(registry.knightAnimations.entities.front());

	if (!player.isDead) {
		vec2 currentVelocity = vec2(player1motion.velocity.x, player1motion.velocity.y);
		if (action == GLFW_PRESS && key == GLFW_KEY_W) {
			playerOneAnimation.moving = true;
			player1motion.velocity = vec2(currentVelocity.x, currentVelocity.y - playerOneStat.movementSpeed);
			playerOneAnimation.xFrame = 1;
			playerOneAnimation.yFrame = 0;
			registry.renderRequests.remove(player_knight);
			registry.renderRequests.insert(
				player_knight,
				{ TEXTURE_ASSET_ID::KNIGHT,
					EFFECT_ASSET_ID::KNIGHT,
					GEOMETRY_BUFFER_ID::SPRITE }, false);
		}
		if (action == GLFW_RELEASE && key == GLFW_KEY_W) {
			player1motion.velocity = vec2(currentVelocity.x, 0);
		}

		if (action == GLFW_PRESS && key == GLFW_KEY_S) {
			playerOneAnimation.moving = true;
			player1motion.velocity = vec2(currentVelocity.x, currentVelocity.y + playerOneStat.movementSpeed);
			int counter = 0;
			playerOneAnimation.xFrame = 1;
			playerOneAnimation.yFrame = 2;
			registry.renderRequests.remove(player_knight);
			registry.renderRequests.insert(
				player_knight,
				{ TEXTURE_ASSET_ID::KNIGHT,
					EFFECT_ASSET_ID::KNIGHT,
					GEOMETRY_BUFFER_ID::SPRITE }, false);
		}
		if (action == GLFW_RELEASE && key == GLFW_KEY_S) {
			player1motion.velocity = vec2(currentVelocity.x, 0);
		}

		if (action == GLFW_PRESS && key == GLFW_KEY_A) {
			playerOneAnimation.moving = true;
			player1motion.velocity = vec2(currentVelocity.x - playerOneStat.movementSpeed, currentVelocity.y);
			playerOneAnimation.xFrame = 1;
			playerOneAnimation.yFrame = 1;
			registry.renderRequests.remove(player_knight);
			registry.renderRequests.insert(
				player_knight,
				{ TEXTURE_ASSET_ID::KNIGHT,
					EFFECT_ASSET_ID::KNIGHT,
					GEOMETRY_BUFFER_ID::SPRITE }, false);
		}
		if (action == GLFW_RELEASE && key == GLFW_KEY_A) {
			player1motion.velocity = vec2(0, currentVelocity.y);

		}

		if (action == GLFW_PRESS && key == GLFW_KEY_D) {
			playerOneAnimation.moving = true;
			player1motion.velocity = vec2(currentVelocity.x + playerOneStat.movementSpeed, currentVelocity.y);
			playerOneAnimation.xFrame = 1;
			playerOneAnimation.yFrame = 3;
			registry.renderRequests.remove(player_knight);
			registry.renderRequests.insert(
				player_knight,
				{ TEXTURE_ASSET_ID::KNIGHT,
					EFFECT_ASSET_ID::KNIGHT,
					GEOMETRY_BUFFER_ID::SPRITE }, false);
		}

		if (action == GLFW_RELEASE && key == GLFW_KEY_D) {
			player1motion.velocity = vec2(0, currentVelocity.y);
		}

		if (action == GLFW_PRESS && key == GLFW_KEY_T) {
			player.isFiringProjectile = true;
			player.firingDirection = 0;
			if (!playerOneAnimation.moving) {
				playerOneAnimation.yFrame = 0;
			}
		}

		if (action == GLFW_PRESS && key == GLFW_KEY_G) {
			player.isFiringProjectile = true;
			player.firingDirection = 2;
			if (!playerOneAnimation.moving) {
				playerOneAnimation.yFrame = 2;
			}
		}

		if (action == GLFW_PRESS && key == GLFW_KEY_H) {
			player.isFiringProjectile = true;
			player.firingDirection = 3;
			if (!playerOneAnimation.moving) {
				playerOneAnimation.yFrame = 3;
			}

		}

		if (action == GLFW_PRESS && key == GLFW_KEY_F) {
			player.isFiringProjectile = true;
			player.firingDirection = 1;
			if (!playerOneAnimation.moving) {
				playerOneAnimation.yFrame = 1;
			}
		}
	}	

	if (action == GLFW_PRESS && key == GLFW_KEY_K) {
		saveGame();
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
		setPlayersStats(); 

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

	if (action == GLFW_PRESS && key == GLFW_KEY_5) {
		level_number = 5;
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
	// Debug mode to get to level transition
	if (action == GLFW_RELEASE && key == GLFW_KEY_B) {
		while (registry.enemies.entities.size() > 0)
			registry.remove_all_components_of(registry.enemies.entities.back());
	}
	// Debug mode - give player 1000 dollars.
	if (action == GLFW_RELEASE && key == GLFW_KEY_N) {
		PlayerStat& ps1 = registry.playerStats.get(player_stat);
		if (twoPlayer.inTwoPlayerMode) {
			PlayerStat& ps2 = registry.playerStats.get(player2_stat); 
			ps1.money += 1000.f; 
			ps2.money += 1000.f; 
		}
		else {
			ps1.money += 1000.f; 
		}
		updateTitle(level_number); 
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
		playerTwoJoinOrLeave();
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
		storyClicker();
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE && !helpMode.isOnTopInGameMenu) {
		toggleInGameMenu();
	}
}

void WorldSystem::toggleInGameMenu() {
	// if menu has no been created or menu is back from help
		// 
	if (menuMode.menuType == 0 && !menuMode.inHelpDrawn) {
		menuMode.menuType = 2;
		menuMode.inGameMode = true;

		//if in shop
		if (registry.inShops.has(player_knight) || registry.inShops.has(player2_wizard)) {
			Entity ent = createInGameMenu();
			Motion& motion = registry.motions.get(ent);
			motion.position = { defaultResolution.width / 2 , defaultResolution.height / 2 + defaultResolution.defaultHeight };
		}
		else {
			createInGameMenu();
		}
	}
	else if (menuMode.inGameMode && menuMode.menuType != 1) {
		for (Entity entity : registry.menuModes.entities) {
			registry.remove_all_components_of(entity);
		}
		menuMode.menuType = 0;

	}
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
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
	// menu is main menu
	if (menuMode.menuType == 1) {
		createTitleScreen(mouse_position);
	}

	// menu is in game
	if (menuMode.menuType == 2) {
		createInGameScreen(mouse_position);
	}
}

void WorldSystem::on_mouse_click(int button, int action, int mods) {
	if (twoPlayer.inTwoPlayerMode && storyMode.inStoryMode==0 && menuMode.menuType ==0) {
		Player& wizard2_player = registry.players.get(player2_wizard);
		WizardAnimation& animation = registry.wizardAnimations.get(player2_wizard);
		if (!wizard2_player.isDead) {
			if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
				if (registry.mouseDestinations.has(player2_wizard))
					registry.mouseDestinations.remove(player2_wizard);
				Motion& wizard2_motion = registry.motions.get(player2_wizard);
				PlayerStat& playerTwoStat = registry.playerStats.get(wizard2_player.playerStat);
				double x, y;
				glfwGetCursorPos(window, &x, &y);
				if (registry.inShops.has(player2_wizard)) {
					y += defaultResolution.defaultHeight;
				}
				float xPixelToDestination = (float)x - wizard2_motion.position.x;
				float yPixelToDestination = (float)y - wizard2_motion.position.y;
				float pixelToDestination = sqrtf(powf(xPixelToDestination, 2) + powf(yPixelToDestination, 2));
				// (pixel / second) / (pixel) = (1 / second) 
				float oneOverTime = playerTwoStat.movementSpeed / pixelToDestination;
				wizard2_motion.velocity = vec2(xPixelToDestination * oneOverTime, yPixelToDestination * oneOverTime);
				registry.mouseDestinations.emplace(player2_wizard, vec2(x, y));
				if (!animation.isAnimatingHurt && animation.animationMode != animation.attackMode) {
					animation.animationMode = animation.walkMode;
					animation.frameWalk = 0;
					animation.walkTimer = 0;
					registry.renderRequests.remove(player2_wizard);
					registry.renderRequests.insert(
						player2_wizard,
						{ TEXTURE_ASSET_ID::WIZARDWALK,
							EFFECT_ASSET_ID::WIZARD,
							GEOMETRY_BUFFER_ID::SPRITE });
				}
			}

			if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT) {
				wizard2_player.isFiringProjectile = true;
				if (!animation.isAnimatingHurt) {
					animation.animationMode = animation.attackMode;
					animation.frameAttack = 0;
					animation.attackTimer = 0;
					registry.renderRequests.remove(player2_wizard);
					registry.renderRequests.insert(
						player2_wizard,
						{ TEXTURE_ASSET_ID::WIZARDATTACK,
							EFFECT_ASSET_ID::WIZARD,
							GEOMETRY_BUFFER_ID::SPRITE });
				}
			}

			if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_RIGHT) {
				wizard2_player.isFiringProjectile = false;
				if (!animation.isAnimatingHurt && !animation.animationMode != animation.walkMode) {
					animation.animationMode = animation.idleMode;
					animation.frameIdle = 0;
					animation.idleTimer = 0;
					registry.renderRequests.remove(player2_wizard);
					registry.renderRequests.insert(
						player2_wizard,
						{ TEXTURE_ASSET_ID::WIZARDIDLE,
							EFFECT_ASSET_ID::WIZARD,
							GEOMETRY_BUFFER_ID::SPRITE });
				}
			}
		}
	}
	
	bool left_clicked = (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT);
	// menu is in-game menu
	if (helpMode.clicked == 1) {
		helpMode.clicked = 2;
		
	}
	else if (helpMode.clicked == 2) {

		menuMode.inHelpDrawn = false;
		// possible bug/edge case
		menuMode.currentButton = None;
		for (Entity entity : registry.helpModes.entities) {
			registry.remove_all_components_of(entity);
		}

		if (helpMode.isOnTopMainMenu) {
			// bring main menu back
			menuMode.menuType = 1;
			// help is no longer displayed over main menu
			helpMode.isOnTopMainMenu = false;
			// make the menu
			createMenu();
		}
		else if (helpMode.isOnTopInGameMenu) {
			menuMode.menuType = 2;
			helpMode.isOnTopInGameMenu = false;
			Entity entity = createInGameMenu();
			if (registry.inShops.has(player_knight) || registry.inShops.has(player2_wizard)) {
				Motion& motion = registry.motions.get(entity);
				motion.position = { defaultResolution.width / 2 , defaultResolution.height / 2 + defaultResolution.defaultHeight };
			}
		}
		helpMode.inHelpMode = false;
		helpMode.clicked = 0;
	}

	// menu is main menu
	if (menuMode.menuType == 1) {
		if (left_clicked) {
			menuLogic(1);
		}
	}
	// menu is in game
	else if (menuMode.menuType == 2) {
		if (left_clicked) {
			menuLogic(2);
		}
	}
	// story click to progress
	else if (storyMode.firstLoad && left_clicked&& menuMode.menuType==0) {
		storyClicker();
	}
}

void WorldSystem::storyClicker() {
	Entity ent;
	if (storyMode.inStoryMode == 6) {
		storyMode.inStoryMode = 0;
		for (Entity entity : registry.storyModes.entities) {
			registry.remove_all_components_of(entity);
		}
		storyMode.firstLoad = false;
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

void WorldSystem::menuLogic(int menuType) {
	if (menuType == 1) {
		// 1P 
		if (menuMode.currentButton == P1) {
			menuMode.menuType = 0;
			for (Entity entity : registry.menuModes.entities) {
				registry.remove_all_components_of(entity);
			}
			if (storyMode.inStoryMode == 0) {
				storyMode.inStoryMode = 1;
				createStory();
			}
		}
		// 2P
		if (menuMode.currentButton == P2) {
			menuMode.menuType = 0;
			for (Entity entity : registry.menuModes.entities) {
				registry.remove_all_components_of(entity);
			}
			// insert code here
			playerTwoJoinOrLeave();
			if (storyMode.inStoryMode == 0) {
				storyMode.inStoryMode = 1;
				createStory();
			}
		}
		// Load
		if (menuMode.currentButton == Load) {
			menuMode.menuType = 0;
			for (Entity entity : registry.menuModes.entities) {
				registry.remove_all_components_of(entity);
			}
			// No story on Load
			storyMode.firstLoad = false;
			// Load the game from last save
			loadGame();
		}
		// Help
		if (menuMode.currentButton == Help) {
			menuMode.menuType = 0;
			createHelp();
			menuMode.inHelpDrawn = true;
			helpMode.inHelpMode = true;
			helpMode.menuHelp = true;
			helpMode.isOnTopMainMenu = true;
			helpMode.clicked = 1;
		}
	}

	if (menuType == 2) {
		// Save
		if (menuMode.currentButton == Save) {
			menuMode.menuType = 0;
			for (Entity entity : registry.menuModes.entities) {
				registry.remove_all_components_of(entity);
			}
			saveGame();
		}
		// 2P join/leave
		if (menuMode.currentButton == JoinLeave) {
			menuMode.menuType = 0;
			for (Entity entity : registry.menuModes.entities) {
				registry.remove_all_components_of(entity);
			}
			playerTwoJoinOrLeave();
		}
		// Restart
		if (menuMode.currentButton == Restart) {
			menuMode.menuType = 0;
			for (Entity entity : registry.menuModes.entities) {
				registry.remove_all_components_of(entity);
			}
			restart_game();
		}
		// Load
		if (menuMode.currentButton == Load) {
			menuMode.menuType = 0;
			for (Entity entity : registry.menuModes.entities) {
				registry.remove_all_components_of(entity);
			}
			// No story on Load
			storyMode.firstLoad = false;
			// Load the game from last save
			loadGame();
		}
		// Help
		if (menuMode.currentButton == Help) {
			menuMode.menuType = 0;
			Entity help = createHelp();
			if (registry.inShops.has(player_knight) || registry.inShops.has(player2_wizard)) {
				Motion& motion = registry.motions.get(help);
				motion.position.y = defaultResolution.height/2 + defaultResolution.defaultHeight;
			}
			menuMode.inHelpDrawn = true;
			helpMode.inHelpMode = true;
			helpMode.menuHelp = true;
			helpMode.isOnTopInGameMenu = true;
			helpMode.clicked = 1;
			
		}
	}
	

}

void WorldSystem::loadGame() {
	dataManager.setPlayerModeFromFile();
	int player_mode_file = dataManager.getPlayerMode();
	if (player_mode_file == 1) {
		twoPlayer.inTwoPlayerMode = false;
	}
	else {
		twoPlayer.inTwoPlayerMode = true;
	}
	setPlayersStats();

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

void WorldSystem::saveGame() {
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
	vec2 verticalWallScale = { defaultResolution.wallThickness, screenHeight * gameHeightScale };
	vec2 horizontalWallScale = { screenWidth, defaultResolution.wallThickness };
	createWall(leftWallPos, verticalWallScale);
	createWall(rightWallPos, verticalWallScale);
	createWall(topWallPos, horizontalWallScale);
	createWall(bottomWallPos, horizontalWallScale);

	// Create middle shop walls
	vec2 middleWallLeftPos = { 0, screenHeight };
	vec2 middleWallRightPos = { screenWidth, screenHeight };
	vec2 shopWallScale = { screenWidth - (screenWidth * doorWidthScale), defaultResolution.shopWallThickness };
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
	defaultResolution.defaultHeight *= defaultResolution.scaling;
	defaultResolution.wallThickness *= defaultResolution.scaling;
	defaultResolution.shopWallThickness *= defaultResolution.scaling;
	defaultResolution.shopBufferZone *= defaultResolution.scaling;
}

void WorldSystem::setPlayersStats() {
	setPlayerOneStats();
	if (twoPlayer.inTwoPlayerMode) {
		setPlayerTwoStats();
	}
}

void WorldSystem::setPlayerOneStats() {
	auto entity = Entity();
	player_stat = entity;
	PlayerStat& playerOneStat = registry.playerStats.emplace(entity);
	int swordDefaultDamage = 2;
	playerOneStat.damage = swordDefaultDamage;
	playerOneStat.movementSpeed = playerOneStat.movementSpeed * defaultResolution.scaling;
	playerOneStat.projectileSpeed = playerOneStat.projectileSpeed * defaultResolution.scaling;
}

void WorldSystem::setPlayerTwoStats() {
	auto entity = Entity();
	PlayerStat& playerTwoStat = registry.playerStats.emplace(entity);
	player2_stat = entity;
	playerTwoStat.movementSpeed = playerTwoStat.movementSpeed * defaultResolution.scaling;
	playerTwoStat.projectileSpeed = playerTwoStat.projectileSpeed * defaultResolution.scaling;
}

void WorldSystem::handlePlayerTwoProjectile(float elapsed_ms_since_last_update) {
	// handle player2 projectile
	if (twoPlayer.inTwoPlayerMode) {
		next_projectile_fire_player2 -= elapsed_ms_since_last_update;
		Motion player2Motion = registry.motions.get(player2_wizard);
		Player& player2 = registry.players.get(player2_wizard);
		PlayerStat& playerTwoStat = registry.playerStats.get(player2.playerStat);
		if (!player2.isDead && player2.isFiringProjectile && next_projectile_fire_player2 < 0.f) {
			next_projectile_fire_player2 = playerTwoStat.attackDelay;
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			if (registry.inShops.has(player2_wizard)) {
				y += defaultResolution.defaultHeight;
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

void WorldSystem::handlePlayerOneAttack(float elapsed_ms_since_last_update) {
	// handle player1 projectiles
	float angle = 0;
	float offset = -M_PI / 3.f;
	next_projectile_fire_player1 -= elapsed_ms_since_last_update;
	Player& player1 = registry.players.get(player_knight);
	PlayerStat& playerOneStat = registry.playerStats.get(player1.playerStat);
	Motion playerMotion = registry.motions.get(player_knight);
	if (player1.isFiringProjectile && next_projectile_fire_player1 < 0.f) {
		next_projectile_fire_player1 = playerOneStat.attackDelay;
		
		switch (player1.firingDirection) {
		case 0: // up
			angle = M_PI * 3 / 2;
			break;
		case 1: // left
			angle = M_PI;
			break;
		case 2: // down
			angle = M_PI / 2;
			break;
		case 3: // right
			// no action
			break;
		}
		createSword(renderer, angle + offset, player_knight);
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
	for (Entity enemyEntity : registry.enemies.entities) {
		Enemy& enemy = registry.enemies.get(enemyEntity);
		if (enemy.isInvin) {
			enemy.invinTimerInMs -= elapsed_ms_since_last_update;
			if (enemy.invinTimerInMs < 0) {
				enemy.isInvin = false;
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


void WorldSystem::stopPlayerAtMouseDestination() {
	if (twoPlayer.inTwoPlayerMode && registry.mouseDestinations.has(player2_wizard)) {
		Motion& motion = registry.motions.get(player2_wizard);
		MouseDestination& mouseDestination = registry.mouseDestinations.get(player2_wizard);

		float pixelThreshold = 10.f;
		bool not_out_bound_x = abs(motion.position.x - mouseDestination.position.x) < (pixelThreshold * defaultResolution.scaling);
		bool not_out_bound_y = abs(motion.position.y - mouseDestination.position.y) < (pixelThreshold * defaultResolution.scaling);
		if (not_out_bound_x && not_out_bound_y) {
			registry.mouseDestinations.remove(player2_wizard);
			motion.velocity = vec2(0, 0);
		}
	}
}

void WorldSystem::levelCompletionCheck(float elapsed_ms_since_last_update) {
	// Check level completion 
	if (registry.enemies.size() == 0) {
		transitionToShop();
		isLevelOver = true;
	}
	if (isLevelOver && isTransitionOver) {
		int nextLevel = level_number + 1;
		if (nextLevel <= levels.size()) {
			ScreenState& screen = registry.screenStates.components[0];
			float min_counter_ms = 3000.f;
			for (Entity entity : registry.endLevelTimers.entities) {
				// progress timer
				EndLevelTimer& counter = registry.endLevelTimers.get(entity);
				counter.counter_ms -= elapsed_ms_since_last_update;
				if (counter.counter_ms < min_counter_ms) {
					min_counter_ms = counter.counter_ms;
				}

				// move on to next level the game once the end level timer expired
				if (counter.counter_ms < 0) {
					registry.endLevelTimers.remove(entity);
					screen.darken_screen_factor = 0;
					level_number = nextLevel;
					setupLevel(level_number);
				}
				else {
					screen.darken_screen_factor = 1 - min_counter_ms / 3000;
				}
				
			}
		}
	}
}

void WorldSystem::transitionToShop() {
	if (registry.doors.entities.size() > 0) {
		Entity door = registry.doors.entities.front();
		registry.remove_all_components_of(door);
	}
	if (!twoPlayer.inTwoPlayerMode) {
		setTransitionFlag(player_knight);
	}
	else {
		setTransitionFlag(player2_wizard);
	}
}

void WorldSystem::spawnPowerups(int n) {
	float width = static_cast<float>(defaultResolution.width); 
	float height = static_cast<float>(defaultResolution.height);
	// Same yPos for all powerUps. 
	float yPos = (height / 2) + (defaultResolution.defaultHeight);
	// We want to divide the screen width into n+1 equal columns to space the powerups
	float numCols = static_cast<float>(n + 1); 
	float colWidth = width / numCols; 
	// Clear all the old powerups from the previous level. 
	while (registry.powerups.entities.size() > 0)
		registry.remove_all_components_of(registry.powerups.entities.back());

	for (int i = 0; i < n; i++) {
		float xPos = colWidth * (i + 1);
		chooseRandomPowerUp({ xPos, yPos}); 	
	}
}

Entity WorldSystem::chooseRandomPowerUp(vec2 pos) {
	float random_choice = uniform_dist(rng);
	if (random_choice < 0.25f) {
		return createHpPowerup(pos);
	}
	else if (random_choice >= 0.25f && random_choice <= 0.5f) {
		return createDamagePowerup(pos);
	}
	else if (random_choice > 0.5f && random_choice <= 0.75f) {
		return createAttackSpeedPowerup(pos);
	}
	else {
		return createMovementSpeedPowerup(pos); 
	}
}
void WorldSystem::reviveKnight(Player& p1, PlayerStat& p1Stat) {
	p1.isDead = false;
	p1.hp = p1Stat.maxHp;
	registry.renderRequests.insert(
		player_knight,
		{ TEXTURE_ASSET_ID::KNIGHT,
			EFFECT_ASSET_ID::KNIGHT,
			GEOMETRY_BUFFER_ID::SPRITE }, false);
}

void WorldSystem::reviveWizard(Player& p2, PlayerStat& p2Stat) {
	p2.isDead = false;
	p2.hp = p2Stat.maxHp;
	WizardAnimation& animation = registry.wizardAnimations.get(player2_wizard);
	animation.animationMode = animation.idleMode;
	registry.renderRequests.insert(
		player2_wizard,
		{ TEXTURE_ASSET_ID::WIZARDIDLE,
			EFFECT_ASSET_ID::WIZARD,
			GEOMETRY_BUFFER_ID::SPRITE });
	animation.frameIdle = 0;
	animation.idleTimer = 0;
}

void WorldSystem::progressBrightenScreen(float elapsed_ms_since_last_update) {
	// Check level completion 
	if (startingNewLevel == true) {
		startingNewLevel = false;
		auto entity1 = Entity();
		registry.startLevelTimers.emplace(entity1);
	}

	ScreenState& screen = registry.screenStates.components[0];
	float min_counter_ms = 3000.f;
	for (Entity entity : registry.startLevelTimers.entities) {
		// progress timer
		StartLevelTimer& counter = registry.startLevelTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		if (counter.counter_ms < min_counter_ms) {
			min_counter_ms = counter.counter_ms;
		}

		// move on to next level the game once the end level timer expired
		if (counter.counter_ms < 0) {
			registry.startLevelTimers.remove(entity);
			screen.brighten_screen_factor = 0;
		}
		// increase window brightness if the level is starting
		screen.brighten_screen_factor = 0 + min_counter_ms / 3000;
	}
}
void WorldSystem::reviveDeadPlayerInShop() {
	if (twoPlayer.inTwoPlayerMode) {
		Player& p1 = registry.players.get(player_knight);
		Player& p2 = registry.players.get(player2_wizard);
		PlayerStat& p1Stat = registry.playerStats.get(player_stat);
		PlayerStat& p2Stat = registry.playerStats.get(player2_stat);
		// Restore the dead player so they can buy stuff. 
		if (p1.isDead) {
			reviveKnight(p1, p1Stat);
		}
		if (p2.isDead) {
			reviveWizard(p2, p2Stat);
		}
		updateTitle(level_number);
	}
}

void WorldSystem::setTransitionFlag(Entity player) {

	if (registry.inShops.has(player) && firstEntranceToShop) {
		firstEntranceToShop = false;
		reviveDeadPlayerInShop();
		int num_powerUps = 4; 
		spawnPowerups(num_powerUps); 
	}
	if (!registry.inShops.has(player) && !firstEntranceToShop) {
		isTransitionOver = true;
		if (registry.endLevelTimers.entities.size() == 0) {
			Entity entity1 = Entity();
			registry.endLevelTimers.emplace(entity1);
		}
	}
	else {
		isTransitionOver = false;
	}
}



void WorldSystem::animateKnight(float elapsed_ms_since_last_update) {
	KnightAnimation& animation = registry.knightAnimations.get(player_knight);
	if (animation.moving) {
		float prev_frame = animation.xFrame;
		knightFrameSetter(elapsed_ms_since_last_update, animation);
		registry.renderRequests.remove(player_knight);
		registry.renderRequests.insert(
			player_knight,
			{ TEXTURE_ASSET_ID::KNIGHT,
				EFFECT_ASSET_ID::KNIGHT,
				GEOMETRY_BUFFER_ID::SPRITE }, false);
	}
}

void WorldSystem::animateWizard(float elpased_ms_since_last_update) {
	if (twoPlayer.inTwoPlayerMode) {
		WizardAnimation& animation = registry.wizardAnimations.get(player2_wizard);
		Player& player = registry.players.get(player2_wizard);
		if (animation.isAnimatingHurt && !player.isInvin) {
			animation.animationMode = animation.idleMode;
			registry.renderRequests.remove(player2_wizard);
			registry.renderRequests.insert(
				player2_wizard,
				{ TEXTURE_ASSET_ID::WIZARDIDLE,
					EFFECT_ASSET_ID::WIZARD,
					GEOMETRY_BUFFER_ID::SPRITE });
			animation.frameIdle = 0;
			animation.idleTimer = 0;
			animation.isAnimatingHurt = false;
		}

		if (animation.animationMode == animation.attackMode) {
			wizardAttackFrameSetter(elpased_ms_since_last_update, animation);
		}
		else if (animation.animationMode == animation.walkMode) {
			wizardWalkFrameSetter(elpased_ms_since_last_update, animation);
		}
		else {
			// animation.animationMode = animation.idleMode
   			wizardIdleFrameSetter(elpased_ms_since_last_update, animation);
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
	while (registry.walls.entities.size() > 0)
		registry.remove_all_components_of(registry.walls.entities.back());
	while (registry.doors.entities.size() > 0)
		registry.remove_all_components_of(registry.doors.entities.back());

	// Close the door at the start of every level after player leaves the shop. 
	createADoor(screen_width, screen_height);
	createWalls(screen_width, screen_height);
	
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

	player_knight = createKnight(renderer, level.player_position * defaultResolution.scaling);
	Player& player1 = registry.players.get(player_knight);
	player1.playerStat = player_stat;
	PlayerStat& playerOneStat = registry.playerStats.get(player_stat);
	player1.hp = playerOneStat.maxHp; 
	if (twoPlayer.inTwoPlayerMode) {
		player2_wizard = createWizard(renderer, level.player2_position * defaultResolution.scaling);
		Player& player2 = registry.players.get(player2_wizard);
		if (!registry.playerStats.has(player2_stat)) {
			setPlayerTwoStats();
		}
		player2.playerStat = player2_stat;
		PlayerStat& playerTwoStat = registry.playerStats.get(player2_stat);
		player2.hp = playerTwoStat.maxHp;
	}

	// Reset player position on level transition
	Motion& player1Motion = registry.motions.get(player_knight);
	player1Motion.position = level.player_position * defaultResolution.scaling;
	if (twoPlayer.inTwoPlayerMode) {
		Motion& player2Motion = registry.motions.get(player2_wizard);
		player2Motion.position = level.player2_position * defaultResolution.scaling;
	}
	// Update state 
	startingNewLevel = true;
	isLevelOver = false;
	isTransitionOver = false;
	firstEntranceToShop = true; 
	updateTitle(levelNum);
}

void WorldSystem::playerTwoJoinOrLeave() {
	helpMode.inHelpMode = true;
	while (stepProgress.stepInProgress);
	if (twoPlayer.inTwoPlayerMode) {
		twoPlayer.inTwoPlayerMode = false;
		registry.remove_all_components_of(player2_wizard);
		registry.remove_all_components_of(player2_stat);
		updateTitle(level_number);
	}
	else {
		twoPlayer.inTwoPlayerMode = true;
		setupLevel(level_number);
	}
	helpMode.inHelpMode = false;
}

void WorldSystem::checkIfPlayersAreMoving() {
	Motion& knightMotion = registry.motions.get(player_knight);
	KnightAnimation& knightAnimation = registry.knightAnimations.get(player_knight);
	if (knightAnimation.moving && knightMotion.velocity.x == 0 && knightMotion.velocity.y == 0) {
		knightAnimation.moving = false;
		knightAnimation.xFrame = 0;
	}
	if (twoPlayer.inTwoPlayerMode) {
		Motion& wizardMotion = registry.motions.get(player2_wizard);
		WizardAnimation& wizardAnimation = registry.wizardAnimations.get(player2_wizard);
		if (wizardAnimation.animationMode == wizardAnimation.walkMode && wizardMotion.velocity.x == 0 && wizardMotion.velocity.y == 0) {
			wizardAnimation.animationMode = wizardAnimation.idleMode;
			wizardAnimation.frameIdle = 0;
			wizardAnimation.idleTimer = 0;
			registry.renderRequests.remove(player2_wizard);
			registry.renderRequests.insert(
				player2_wizard,
				{ TEXTURE_ASSET_ID::WIZARDIDLE,
					EFFECT_ASSET_ID::WIZARD,
					GEOMETRY_BUFFER_ID::SPRITE });
		}
	}
}

void WorldSystem::updateTitle(int level) {
	Title& title = registry.titles.components[0];
	title.level = level; 
	title.p1hp = registry.players.get(player_knight).hp;
	title.p1money = registry.playerStats.get(registry.players.get(player_knight).playerStat).money;
	if (twoPlayer.inTwoPlayerMode) {
		title.p2hp = registry.players.get(player2_wizard).hp;
		title.p2money = registry.playerStats.get(registry.players.get(player2_wizard).playerStat).money;
	}
	title.updateWindowTitle();
}

void WorldSystem::knightFrameSetter(float elapsed_ms, KnightAnimation& knightAnimation)
{
	knightAnimation.animationTimer += elapsed_ms;
	if (knightAnimation.animationTimer > knightAnimation.msPerFrame) {
		knightAnimation.xFrame = (knightAnimation.xFrame + 1) % knightAnimation.numOfFrames;
		if (knightAnimation.xFrame == 0) {
			knightAnimation.xFrame = knightAnimation.numOfFrames;
		}
		knightAnimation.animationTimer = 0;
	}
}

void WorldSystem::wizardAttackFrameSetter(float elapsed_ms, WizardAnimation& wizardAnimation) {
	wizardAnimation.attackTimer += elapsed_ms;
	if (wizardAnimation.attackTimer > wizardAnimation.attackMsPerFrame) {
		wizardAnimation.frameAttack = (wizardAnimation.frameAttack + 1) % wizardAnimation.numOfAttackFrames;
		wizardAnimation.attackTimer = 0;
	}

}

void WorldSystem::wizardWalkFrameSetter(float elapsed_ms, WizardAnimation& wizardAnimation) {
	wizardAnimation.walkTimer += elapsed_ms;
	if (wizardAnimation.walkTimer > wizardAnimation.walkMsPerFrame) {
		wizardAnimation.frameWalk = (wizardAnimation.frameWalk + 1) % wizardAnimation.numOfWalkFrames;
		wizardAnimation.walkTimer = 0;
	}
}

void WorldSystem::wizardIdleFrameSetter(float elapsed_ms, WizardAnimation& wizardAnimation) {
	wizardAnimation.idleTimer += elapsed_ms;
	if (wizardAnimation.idleTimer > wizardAnimation.idleMsPerFrame) {
		wizardAnimation.frameIdle = (wizardAnimation.frameIdle + 1) % wizardAnimation.numOfIdleFrames;
		wizardAnimation.idleTimer = 0;
	}
}


Entity WorldSystem::createMenu() {
	Entity entity = Entity();

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::MENU,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = { defaultResolution.width / 2, defaultResolution.height / 2};
	motion.scale = vec2({ STORY_BB_WIDTH * defaultResolution.scaling, STORY_BB_HEIGHT * defaultResolution.scaling });

	registry.menuModes.emplace(entity);

	return entity;
}

Entity WorldSystem::createInGameMenu() {
	Entity entity = Entity();

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::INGAMEMENU,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = { defaultResolution.width / 2, defaultResolution.height / 2 };
	motion.scale = vec2({ STORY_BB_WIDTH * defaultResolution.scaling, STORY_BB_HEIGHT * defaultResolution.scaling });

	registry.menuModes.emplace(entity);

	return entity;
}

void WorldSystem::createTitleScreen(vec2 mouse_position) {
	if(registry.menuModes.size()>0){
		Entity entity = registry.menuModes.entities[0];
		Motion &motion = registry.motions.get(entity);
		float xnum = motion.position.x + TL_BUTTONPOS.x * defaultResolution.scaling;
		vec2 xpos = { xnum - BUTTON_BB_WIDTH * defaultResolution.scaling , xnum };
		vec2 ypos = inShopAdjustPosition(TL_BUTTONPOS.y, motion);
		// 1P
		if (withinButtonBounds(mouse_position.x, xpos)) {
			if (withinButtonBounds(mouse_position.y, ypos)) {
				menuMode.currentButton = P1;
			}
			// 2P
			ypos = inShopAdjustPosition(BL_BUTTONPOS.y, motion);
			if (withinButtonBounds(mouse_position.y, ypos)) {
				menuMode.currentButton = P2;
			}
		}
		else {
			xnum = motion.position.x + TR_BUTTONPOS.x * defaultResolution.scaling;
			xpos = { xnum - BUTTON_BB_WIDTH * defaultResolution.scaling , xnum };

			if (withinButtonBounds(mouse_position.x, xpos)) {
				// Load
				ypos = inShopAdjustPosition(TR_BUTTONPOS.y, motion);
				if (withinButtonBounds(mouse_position.y, ypos)) {
					menuMode.currentButton = Load;
				}
				
				// Help
				ypos = inShopAdjustPosition(BR_BUTTONPOS.y, motion);
				if (withinButtonBounds(mouse_position.y, ypos)) {
					menuMode.currentButton = Help;
				}
			}
			else {
				menuMode.currentButton = None;
				
			}
		}
	}
}

void WorldSystem::createInGameScreen(vec2 mouse_position) {
	bool inShop = registry.inShops.has(player_knight) || registry.inShops.has(player2_wizard);
	if (registry.menuModes.size() > 0) {
		Entity entity = registry.menuModes.entities[0];
		Motion& motion = registry.motions.get(entity);
		float xnum = motion.position.x + TL_BUTTONPOS.x * defaultResolution.scaling;

		vec2 xpos = { xnum - BUTTON_BB_WIDTH * defaultResolution.scaling , xnum };
		vec2 ypos = inShopAdjustPosition(TL_BUTTONPOS.y, motion);
		
		if (withinButtonBounds(mouse_position.x, xpos)) {
			// 2P on/off
			if (withinButtonBounds(mouse_position.y, ypos)) {
				menuMode.currentButton = JoinLeave;
			}
			// Restart
			ypos = inShopAdjustPosition(BL_BUTTONPOS.y, motion);
			if (withinButtonBounds(mouse_position.y, ypos)) {
				menuMode.currentButton = Restart;
			}
			
		}
		else {
			xnum = motion.position.x + TR_BUTTONPOS.x * defaultResolution.scaling;
			xpos = { xnum - BUTTON_BB_WIDTH * defaultResolution.scaling , xnum };
			
			if (withinButtonBounds(mouse_position.x, xpos)) {
				// Load
				ypos = inShopAdjustPosition(TR_BUTTONPOS.y, motion);
				if (withinButtonBounds(mouse_position.y, ypos)) {
					menuMode.currentButton = Load;
				}
				
				// Help
				ypos = inShopAdjustPosition(BR_BUTTONPOS.y, motion);
				if (withinButtonBounds(mouse_position.y, ypos)) {
					menuMode.currentButton = Help;
				}
					
				// Save
				ypos = inShopAdjustPosition(TTR_BUTTONPOS.y, motion);
				if (withinButtonBounds(mouse_position.y, ypos)) {
					menuMode.currentButton = Save;
				}
				
			}
			else {
				menuMode.currentButton = None;
			}
		}
	}
}

vec2 WorldSystem::inShopAdjustPosition(float button_pos, Motion& motion) {
	bool inShop = registry.inShops.has(player_knight) || registry.inShops.has(player2_wizard);
	float ynum;
	if (inShop) {
		ynum = motion.position.y + button_pos * defaultResolution.scaling - defaultResolution.defaultHeight;
	}
	else {
		ynum = motion.position.y + button_pos * defaultResolution.scaling;
	}
	return { ynum, ynum + BUTTON_BB_HEIGHT * defaultResolution.scaling };
}

bool WorldSystem::withinButtonBounds(float mouse_position, vec2 bounds) {

	return mouse_position > bounds[0] && mouse_position < bounds[1];

}