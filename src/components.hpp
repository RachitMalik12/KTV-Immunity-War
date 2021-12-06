#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"
#include "../ext/json/dist/json/json.h" 
#include <queue>
#include <stack>

enum AttackDirection {
	UP,
	LEFT,
	DOWN,
	RIGHT,
};

struct Player
{
	int hp = 0;
	float invinFrame = 2000.f;
	float invinTimerInMs = 0;
	bool isInvin = false;
	bool isFiringProjectile = false;
	AttackDirection attackDirection = UP;
	bool isDead = false;
	Entity playerStat;
};

struct PlayerStat
{
	int playerMoneyLimit = 99;
	float projectileSpeed = 300.f;
	float attackDelay = 800.f;
	float movementSpeed = 150.f;
	int maxHp = 3;
	int money = 0;
	float damage = 1.f;
};

struct DeadPlayer {
	float deathAnimationTime = 500.f;
	float deathTimer = 0.f;
};

// The projectile shot by the wizard character.
struct Projectile
{
	Entity belongToPlayer;
};

struct EnemyProjectile {
	Entity belongToEnemy;
};

struct Block
{

};


// Enemy that will be attacked by wizard using projectile
struct Enemy
{
	float hp;
	float max_hp;
	int damage;
	int loot;
	float speed;
	float invinFrame = 500.f;
	float invinTimerInMs = 0;
	bool isInvin = false;
	vec2 velocityOfPlayerHit = vec2(0, 0);
	int damageOfPlayerHit = 0;
	bool isDead = false;
};

struct DeadEnemy {
	bool gotCut = false;
	float deathAnimationTime = 1000.f;
	float deathTimer = 0;
};

struct EnemyBlob
{

};

struct EnemyTutorial {

};
// Enemy that will chase the wizard but not using BFS
struct EnemyChase
{
	// counter for enemy chase 
	float counter_ms = 2000;
	// mark encounter with other enemies and counter for changing direction
	uint encounter = 0;
	float counter_other_en_chase_ms = 800;
	uint enemy_chase_max_dist_sq = 200 * 200;
	int counter_value = 2000;
	int counter_other_en_chase_value = 800;
	float aiUpdateTime = 100.f;
	float aiUpdateTimer = 0;
	bool timeToUpdateAi = true;
};

// Enemy that will be attacked by wizard using projectile and tries to run away from wizard
// counter for lighting up the enemy when it is hit by the player
struct EnemyRun
{
	int max_dist_wz_en_run = 50 * 50;
};

// State machine feature
struct EnemyHunter
{
	int currentState;
	int searchingMode = 0;
	int huntingMode = 1;
	int fleeingMode = 2;
	float aiUpdateTime = 1000.f;
	float aiUpdateTimer = 0;
	bool timeToUpdateAi = true;
	bool isFleeing = false;
	float huntingRange = 500.f;
	bool isAnimatingHurt = false;
};

// BFS Enemy
struct EnemyBacteria
{
	bool huntingMode = true;
	float bfsUpdateTime = 3000.f;
	float pathUpdateTime = 500.f;
	float finX = 0;
	float finY = 0;
	std::stack<std::pair<int, int>> traversalStack;
	std::queue<std::pair<int, int>> adjacentsQueue;
	float next_bacteria_BFS_calculation;
	float next_bacteria_PATH_calculation;
};

// Behaviour Tree Enemy
struct EnemyGerm
{
	float germBehaviourUpdateTime = 1500.f;
	float next_germ_behaviour_calculation;
	float mode;
	float explosionCountDown = 0;
	float explosionCountInit = 5;
	float playerChaseThreshold = 5;
};

// A* Enemy
struct EnemyAStar
{
	float movementUpdateTime = 200.f;
	float AStarBehaviourUpdateTime = 1500.f;
	float next_AStar_behaviour_calculation;
	float next_bacteria_movement;
	float stepSizes = 200.f;
	float distanceCloseToPlayer = 100.f;
	std::queue<std::pair<int, int>> traversalQueue;
	bool finishedPathCalculation = false;
	bool isXCalculationFinished = false;
	bool isYCalculationFinished = false;
};

struct EnemySwarm {
	float aiUpdateTime = 3000.f;
	// Wait 1000ms before update AI for the first time so it doesn't fire at the player right after level loads
	float aiUpdateTimer = 1000.f;
	bool timeToUpdateAi = false;
	float projectileSpeed = 150.f;
	float spreadOutDistance = 200.f;
	bool isAnimatingHurt = false;
};

struct EnemyCoordHead {
	float aiUpdateTime = 2000.f;
	float aiUpdateTimer = 0;
	bool timeToUpdateAi = true;
	float minDistFromTail = 300.f;
	Entity belongToTail;
};

struct EnemyCoordTail {
	float minDistFromHead = 300.f;
};

struct EnemyBoss {
	float aiUpdateInterval = 1500.f;
	float aiUpdateTimer = 0;
	bool timeToUpdateAi = true;
	float projectileSpeed = 150.f;
};

struct EnemyBossHand {

};

struct Powerup 
{
	// Price rendering only supports 0 - 99 inclusive
	int cost = 5; 
	std::vector<Entity> priceNumbers;
};

struct Wall
{

};

struct Door
{

};

// All data relevant to the shape and motion of entities
struct Motion {
	vec2 position = { 0, 0 };
	float angle = 0;
	vec2 velocity = { 0, 0 };
	vec2 scale = { 10, 10 };
};

// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other) { this->other = other; };
};

// Data structure for toggling debug mode
struct Debug {
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
};
extern Debug debugging;

struct TwoPlayer {
	bool inTwoPlayerMode = false;
};
extern TwoPlayer twoPlayer;

struct HelpMode {
	bool inHelpMode = false;
	bool menuHelp = false;
	int clicked = 0;
	bool isOnTopMainMenu = false;
	bool isOnTopInGameMenu = false;
};
extern HelpMode helpMode;

struct Step {
	bool stepInProgress = false;
};
extern Step stepProgress;

struct StoryMode {
	int inStoryMode = 0;
	bool firstLoad = true;
};
extern StoryMode storyMode;

enum menuButtons {
	P1,
	P2,
	Load,
	Save,
	JoinLeave,
	Restart,
	Help,
	None,
};

struct MenuMode {
	// 0 = no menu, 1 = first, main menu // 2 = other in game menu
	bool inMenuMode = true;
	bool inHelpDrawn = false;
	bool inGameMode = false;
	int menuType = 1;
	menuButtons currentButton;
};
extern MenuMode menuMode;


struct DefaultResolution {
	int width = 1200;
	int height = 800;
	float scaling;
	float defaultHeight = 800.f;
	float wallThickness = 40.f;
	float shopWallThickness = 40.f;
	float shopBufferZone = 40.f;
};
extern DefaultResolution defaultResolution;

enum HUDLocation {
	BATTLE_ROOM,
	SHOP_ROOM,
};
struct GameHUD {
	vec2 playerOneBattleRoomLocation = vec2(50, 50);
	vec2 playerTwoBattleRoomLocation = vec2(950, 50);
	vec2 playerOneShopRoomLocation = vec2(50, 850);
	vec2 playerTwoShopRoomLocation = vec2(950, 850);
	Entity playerOneHudEntity;
	Entity playerTwoHudEntity;
	HUDLocation currentLocation = BATTLE_ROOM;
};
extern GameHUD gameHud;

// SOURCE of lighting implementation
// https://gamedev.stackexchange.com/questions/135458/combining-and-drawing-2d-lights-in-opengl
struct Lighting {
	vec3 ambient_light = vec3(0.5, 0.5, 0.5);
	vec2 light_source_pos = vec2(0, 0);
	vec3 light_col = vec3(0.999, 0.999, 0.999);
	float light_intensity = 0.9;
};

// Sets the brightness of the screen
struct ScreenState
{
	float brighten_screen_factor = -1;
	int game_over_factor = 0;
};

// A struct to refer to debugging graphics in the ECS
struct DebugComponent
{
	// Note, an empty struct has size 1
};

// Timer used in tutorial level to determine when the enemies will appear
struct TutorialTimer {
	float counter_ms = 7000.f;
};

// A timer that will be associated to level starting
struct StartLevelTimer
{
	float counter_ms = 1500;
};


// An entity that is currently in the item shop. For mouse-controlled characters.
struct InShop 
{

};

// An entity in motion that will stop at a certain point
struct MouseDestination
{
	vec2 position = { 0,0 };
	MouseDestination(const vec2 pos) : position(pos) {};
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & salmon.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Mesh datastructure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = {1,1};
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};

struct Flip {
	bool left = false;
};

struct Level {
	std::vector<int> enemies; 
	std::vector<std::vector<vec2>> enemyPositions;
	int num_blocks = 0; 
	std::vector<vec2> block_positions; 
	std::vector<int> enemy_types; 
	// 0 means spawn all enemies at once, 1 time based, 2 waves. 
	int level_type = 0; 
	vec2 player_position = vec2(50, 200); 
	vec2 player2_position = vec2(50, 200);
};

struct KnightAnimation {
	int xFrame = 0;
	int yFrame = 0;
	bool moving = 0;
	int numOfFrames = 8;
	float msPerFrame = 100.f;
	float animationTimer = 0.f;
};

struct WizardAnimation {
	int frameIdle = 0;
	int frameWalk = 0;
	int frameAttack = 0;
	int numOfIdleFrames = 3;
	int numOfWalkFrames = 2;
	int numOfAttackFrames = 3;
	float idleMsPerFrame = 200.f;
	float idleTimer = 0.f;
	float walkMsPerFrame = 150.f;
	float walkTimer = 0.f;
	float attackMsPerFrame = 100.f;
	float attackTimer = 0.f;
	int animationMode = 0;
	int idleMode = 0;
	int walkMode = 1;
	int attackMode = 2;
	int hurtMode = 3;
	bool isAnimatingHurt = false;
};

struct Number {
	int frame;
};

struct Letter {
	int frame;
};

enum PlayerCharacter {
	KNIGHT,
	WIZARD,
};

struct HUD {
	Entity headShot;
	Entity coin;
	std::vector<Entity> hps;
	std::vector<Entity> coinCount;
};

struct HUDElement {

};

struct Sword {
	Entity belongToPlayer;
	float max_distance_modifier = 2.f / 3.f;
	float max_distance = M_PI * max_distance_modifier;
	float distance_traveled = 0;
	float angular_velocity = M_PI * 3;
	mat3 rotation;
};

struct Background {

};

struct MovementSpeedPowerUp {
	float movementSpeedUpFactor = 50.f; 
};

struct HpPowerUp {
	int hpUpFactor = 1; 
};

struct AtackSpeedPowerUp {
	float delayReductionFactor = 0.1; 
	float projectileSpeedUpFactor = 50.f; 
};

struct DamagePowerUp {
	float damageUpFactor = 0.5; 
};

struct MovementAndAttackTutInst {

};

struct Arrow {

};

enum BossPhase {
	NONE,
	STAGE1,
	STAGE2,
	STAGE3,
	STAGE4
};

struct BossMode {
	int finalLevelNum = 8;
	// which stage in the final boss we are in.
	BossPhase currentBossLevel = NONE;
	Level level;
};
extern BossMode bossMode;

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID {
	TREE_RED = 0,
	TREE_ORANGE = TREE_RED + 1,
	TREE_YELLOW = TREE_ORANGE + 1,
	WATERBALL = TREE_YELLOW + 1,
	WIZARD = WATERBALL + 1,
	WIZARDHURT = WIZARD + 1,
	ENEMY = WIZARDHURT + 1,
	ENEMYRUN = ENEMY + 1,
	ENEMYHUNTER = ENEMYRUN + 1,
	ENEMYHUNTERMAD = ENEMYHUNTER + 1,
	ENEMYHUNTERHURT = ENEMYHUNTERMAD + 1,
	ENEMYHUNTERFLEE = ENEMYHUNTERHURT + 1,
	HELPPANEL = ENEMYHUNTERFLEE + 1,
	ENEMYBACTERIA = HELPPANEL + 1,
	ENEMYCHASE = ENEMYBACTERIA + 1,
	KNIGHT = ENEMYCHASE + 1,
	FRAME1 = KNIGHT + 1,
	FRAME2 = FRAME1 + 1,
	FRAME3 = FRAME2 + 1,
	FRAME4 = FRAME3 + 1,
	FRAME5 = FRAME4 + 1,
	FRAME6 = FRAME5 + 1,
	ENEMYSWARM = FRAME6 + 1,
	ENEMYSWARMHURT = ENEMYSWARM + 1,
	FIREBALL = ENEMYSWARMHURT + 1,
	SWORD = FIREBALL + 1,
	WIZARDATTACK = SWORD + 1,
	WIZARDIDLE = WIZARDATTACK + 1,
	WIZARDWALK = WIZARDIDLE + 1,
	GERM = WIZARDWALK + 1,
	MENU = GERM + 1,
	INGAMEMENU = MENU + 1,
	HPPOWERUP = INGAMEMENU + 1,
	ATTACKPOWERUP = HPPOWERUP + 1,
	MOVEMENTSPEEDPOWERUP = ATTACKPOWERUP + 1,
	DAMAGEPOWERUP = MOVEMENTSPEEDPOWERUP + 1,
	BACKGROUND = DAMAGEPOWERUP + 1,
	NUMBER = BACKGROUND + 1,
	COIN = NUMBER + 1,
	HP = COIN + 1,
	KNIGHTICON = HP + 1,
	WIZARDICON = KNIGHTICON + 1,
	CAPSLETTER = WIZARDICON + 1,
	SMALLETTER = CAPSLETTER + 1,
	TUTINSTRUCTIONS = SMALLETTER + 1,
	ARROW = TUTINSTRUCTIONS + 1,
	ENEMYASTAR = ARROW + 1,
	FINALBACKGROUND = ENEMYASTAR + 1,
	HAND = FINALBACKGROUND + 1,
	MINION = HAND + 1,
	MINIONCRAZY = MINION + 1,
	BOSSFIREBALL = MINIONCRAZY + 1,
	BOSS = BOSSFIREBALL + 1,
	ENEMYHEAD = BOSS + 1,
	ENEMYTAIL = ENEMYHEAD + 1,
	END1 = ENEMYTAIL +1,
	END2 = END1 + 1,
	TEXTURE_COUNT = END2 + 1
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	COLOURED = 0,
	LINE = COLOURED + 1,
	TEXTURED = LINE + 1,
	WATER = TEXTURED + 1,
	KNIGHT = WATER + 1,
	WIZARD = KNIGHT + 1,
	ENEMY = WIZARD + 1,
	NUMBER = ENEMY + 1,
	POWERUP = NUMBER + 1,
	LETTER = POWERUP + 1, 
	BOSS = LETTER + 1,
	EFFECT_COUNT = BOSS + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	SPRITE = 0,
	DEBUG_LINE = SPRITE + 1,
	SCREEN_TRIANGLE = DEBUG_LINE + 1,
	WALLS = SCREEN_TRIANGLE + 1,
	DOOR = WALLS + 1,
	WIZARD = DOOR + 1,
	HUNTER = WIZARD + 1,
	BLOBBER = HUNTER + 1,
	RUNNER = BLOBBER + 1,
	WATERBALL = RUNNER + 1,
	TREE = WATERBALL + 1,
	BACTERIA = TREE + 1,
	SWORD = BACTERIA + 1,
	GERM = SWORD + 1,
	GEOMETRY_COUNT = GERM + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};

class LevelFileLoader {
private: 
	std::vector<Level> levels; 
public:
	void readFile();
	std::vector<Level> getLevels(); 
};

extern LevelFileLoader levelFileLoader; 

class GameSaveDataManager {
private: 
	int levelNumber; 
	Entity playerStatEntity1; 
	Entity playerStatEntity2; 
	int playerModeFromFile = 1; 
	void loadPlayerStats(Json::Value& root, int playerMode);
	void savePlayerStats(Json::Value& root, Entity playerStatEntity, int playerNum);
public: 
	void saveFile(int playerMode); 
	void setPlayerModeFromFile(); 
	bool loadFile(); 
	int getLevelNumber(); 
	int getPlayerMode(); 
	void setLevelNumber(int levelNumber);
	void setPlayerStatEntity(Entity playerStatEntity1, Entity playerStatEntity2); 
	void setPlayerStatEntity(Entity playerStatEntity1); 
};

extern GameSaveDataManager dataManager; 