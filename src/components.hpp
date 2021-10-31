#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"

// Player component
struct Player
{
	int hp = 3;
	int money = 0;
	int damage = 1;
	float invinFrame = 2000.f;
	float invinTimerInMs = 0;
	bool isInvin = false;
};

// The projectile shot by the wizard character.
struct Projectile
{
	Entity belongToPlayer;
};

struct Block
{

};


// Enemy that will be attacked by wizard using projectile
struct Enemy
{
	int hp;
	int damage;
	int loot;
	float speed;
};

struct EnemyBlob
{

};

// Enemy that will be attacked by wizard using projectile and tries to run away from wizard
struct EnemyRun
{

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
};

struct Powerup 
{

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

struct DefaultResolution {
	int width = 1200;
	int height = 800;
	float scaling;
};
extern DefaultResolution defaultResolution;

// Sets the brightness of the screen
struct ScreenState
{
	float darken_screen_factor = -1;
};

// A struct to refer to debugging graphics in the ECS
struct DebugComponent
{
	// Note, an empty struct has size 1
};

// A timer that will be associated to dying salmon
struct DeathTimer
{
	float counter_ms = 3000;
};

// A timer that will be associated to enemies/enemies run being stuck
struct StuckTimer
{
	float counter_ms = 3000;
	vec2 stuck_pos = { 0, 0 };
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
	int num_blocks = 0; 
	std::vector<vec2> block_positions; 
	std::vector<int> enemy_types; 
	int num_enemy_types = 1;
	// 0 means spawn all enemies at once, 1 time based, 2 waves. 
	int level_type = 0; 
	vec2 player_position = vec2(50, 200); 
	std::string color; 
};

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
	FIREBALL = TREE_YELLOW + 1,
	WIZARD = FIREBALL + 1,
	WIZARD_LEFT = WIZARD + 1,
	BLACK_BAR = WIZARD_LEFT + 1,
	ENEMY = BLACK_BAR + 1,
	POWERUP = ENEMY + 1,
	ENEMYRUN = POWERUP + 1,
	ENEMYHUNTER = ENEMYRUN + 1,
	TEXTURE_COUNT = ENEMYHUNTER + 1
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	COLOURED = 0,
	PEBBLE = COLOURED + 1,
	SALMON = PEBBLE + 1,
	TEXTURED = SALMON + 1,
	WATER = TEXTURED + 1,
	EFFECT_COUNT = WATER + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	SALMON = 0,
	SPRITE = SALMON + 1,
	PEBBLE = SPRITE + 1,
	DEBUG_LINE = PEBBLE + 1,
	SCREEN_TRIANGLE = DEBUG_LINE + 1,
	WALLS = SCREEN_TRIANGLE + 1,
	DOOR = WALLS + 1,
	WIZARD = DOOR + 1,
	HUNTER = WIZARD + 1,
	BLOBBER = HUNTER + 1,
	RUNNER = BLOBBER + 1,
	FIREBALL = RUNNER + 1,
	GEOMETRY_COUNT = FIREBALL + 1
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

