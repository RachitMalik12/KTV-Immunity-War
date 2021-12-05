#pragma once

#include <array>
#include <utility>

#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
	/**
	 * The following arrays store the assets the game will use. They are loaded
	 * at initialization and are assumed to not be modified by the render loop.
	 *
	 * Whenever possible, add to these lists instead of creating dynamic state
	 * it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count> texture_dimensions;

	// Make sure these paths remain in sync with the associated enumerators.
	// Associated id with .obj path
	const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
	{
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::WIZARD, mesh_path("wizard_hitbox.obj")),
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::HUNTER, mesh_path("hunter_hitbox.obj")),
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::BLOBBER, mesh_path("blob_hitbox.obj")),
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::RUNNER, mesh_path("runner_hitbox.obj")),
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::WATERBALL, mesh_path("waterball_hitbox.obj")),
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::TREE, mesh_path("TREE_hitbox.obj")),
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::BACTERIA, mesh_path("yellow-bacteria_hitbox.obj")),
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::SWORD, mesh_path("sword_hitbox.obj")),
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::GERM, mesh_path("BTEnemy_hitbox.obj"))

		 
		  // specify meshes of other assets here
	};

	// Make sure these paths remain in sync with the associated enumerators.
	// SOURCE for enemyrun.png: https://store.line.me/stickershop/product/1014536/en
	// SOURCE for enemy.png: https://www.klipartz.com/ru/search?q=%D0%B2%D0%B8%D1%80%D1%83%D1%81
	// SOURCE for enemychase.png: https://commons.wikimedia.org/wiki/File:Average_prokaryote_cell-_unlabled.svg
	// SOURCE for keys on help.png: https://support.apple.com/en-us/HT201236
	// SOURCE for tail: https://roblox.fandom.com/wiki/Catalog:Earth_Dragon_Tail
	const std::array<std::string, texture_count> texture_paths = {
			textures_path("tree_red.png"),
			textures_path("tree_orange.png"),
			textures_path("tree_yellow.png"),
			textures_path("waterball.png"),
			textures_path("wizard.png"),
			textures_path("wizard_disgust.png"),
			textures_path("enemy.png"),
			textures_path("enemyrun.png"),
			textures_path("hunter1.png"),
			textures_path("hunter2.png"),
			textures_path("hunter3.png"),
			textures_path("hunter4.png"),
			textures_path("help.png"),
			textures_path("yellow-bacteria.png"),
			textures_path("enemychase.png"),
			textures_path("knight.png"),
			textures_path("Frame_1.png"),
			textures_path("Frame_2.png"),
			textures_path("Frame_3.png"),
			textures_path("Frame_4.png"),
			textures_path("Frame_5.png"),
			textures_path("Frame_6.png"),
			textures_path("swarm1.png"),
			textures_path("swarm2.png"),
			textures_path("fireball.png"),
			textures_path("sword.png"),
			textures_path("wizard_attack.png"),
			textures_path("wizard_idle.png"),
			textures_path("wizard_walk.png"),
			textures_path("BTEnemy.png"),
			textures_path("main_menu.png"),
			textures_path("in_game_menu.png"),
			textures_path("HpUp.png"), 
			textures_path("attackSpeedUp.png"),
			textures_path("movementUp.png"), 
			textures_path("dmgUp.png"),
			textures_path("background_new.png"),
			textures_path("numbers.png"),
			textures_path("coin.png"),
			textures_path("hp.png"),
			textures_path("knightIcon.png"),
			textures_path("wizard_earring.png"), 
			textures_path("caps_letters.png"), 
			textures_path("small_letters.png"), 
			textures_path("tutorial.png"), 
			textures_path("shop-arrow.png"),
			textures_path("AStarEnemy.png"),
			textures_path("bg_final.png"),
			textures_path("hand.png"),
			textures_path("minion.png"),
			textures_path("minioncrazy.png"),
			textures_path("bossfireball.png"),
			textures_path("boss.png"),
			textures_path("enemyhead.png"),
			textures_path("enemytail.png"),
			textures_path("end_1.png"),
			textures_path("end_2.png")
	};

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("coloured"),
		shader_path("line"),
		shader_path("textured"),
		shader_path("water"),
		shader_path("knight"),
		shader_path("wizard"),
		shader_path("enemy"),
		shader_path("number"),
		shader_path("powerup"),
		shader_path("letter"),
		shader_path("boss")
	};

	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;
	std::array<Mesh, geometry_count> meshes;

public:
	// Initialize the window
	bool init(int width, int height, GLFWwindow* window);

	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void initializeGlTextures();

	void initializeGlEffects();

	void initializeGlMeshes();
	Mesh& getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int)id]; };

	void initializeGlGeometryBuffers();
	// Initialize the screen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the water
	// shader
	bool initScreenTexture();

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw();

	mat3 createProjectionMatrix(float left, float top);

private:
	// Internal drawing functions for each entity type
	void drawTexturedMesh(Entity entity, const mat3& projection);
	void drawToScreen();
	void playerOneTransition(bool leaveShop);
	void playerTwoTransition(bool leaveShop, vec2 player2Pos);
	void textureEffectSetup(const GLuint program, Entity entity);
	void enemyEffects(const GLuint program, Entity entity);
	void playerEffects(const GLuint program, Entity entity);

	// Window handle
	GLFWwindow* window;
	float screen_scale;  // Screen to pixel coordinates scale factor (for apple
						 // retina display?)

	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;

	Entity screen_state_entity;
};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program);
