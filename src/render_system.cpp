// internal
#include "render_system.hpp"
#include "world_system.hpp"
#include <SDL.h>

#include "tiny_ecs_registry.hpp"
#include "world_init.hpp"

void RenderSystem::drawTexturedMesh(Entity entity,
									const mat3 &projection)
{
	Motion &motion = registry.motions.get(entity);

	assert(registry.renderRequests.has(entity));
	const RenderRequest &render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED)
	{
		textureEffectSetup(program,entity);
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::LINE)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();

		int size = 3;
		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, size, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, size, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void *)sizeof(vec3));
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::KNIGHT)
	{
		textureEffectSetup(program, entity);
		KnightAnimation& knightAnimation = registry.knightAnimations.get(registry.players.entities.front());
		GLint xFrame = glGetUniformLocation(program, "xFrame");
		GLint yFrame = glGetUniformLocation(program, "yFrame");
		glUniform1i(xFrame, knightAnimation.xFrame);
		glUniform1i(yFrame, knightAnimation.yFrame);
		playerEffects(program, entity);
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::WIZARD) {
		textureEffectSetup(program, entity);
		WizardAnimation& wizardAnimation = registry.wizardAnimations.get(registry.players.entities.back());
		GLint frameWalk = glGetUniformLocation(program, "frameWalk");
		GLint frameIdle = glGetUniformLocation(program, "frameIdle");
		GLint frameAttack = glGetUniformLocation(program, "frameAttack");
		GLint animationMode = glGetUniformLocation(program, "animationMode");
		glUniform1i(frameWalk, wizardAnimation.frameWalk);
		glUniform1i(frameIdle, wizardAnimation.frameIdle);
		glUniform1i(frameAttack, wizardAnimation.frameAttack);
		glUniform1i(animationMode, wizardAnimation.animationMode);
		playerEffects(program, entity);
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::ENEMY || render_request.used_effect == EFFECT_ASSET_ID::BOSS)
	{
		textureEffectSetup(program, entity);
		gl_has_errors();
		enemyEffects(program, entity);
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::NUMBER)
	{
		textureEffectSetup(program, entity);
		GLint frame = glGetUniformLocation(program, "frame");
		glUniform1i(frame, registry.numbers.get(entity).frame);
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::LETTER)
	{
		textureEffectSetup(program, entity);
		GLint frame = glGetUniformLocation(program, "frame");
		glUniform1i(frame, registry.letters.get(entity).frame);
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::POWERUP)
	{
		textureEffectSetup(program, entity);
		GLfloat time_loc = glGetUniformLocation(program, "time");
		glUniform1f(time_loc, (float)(glfwGetTime() * 10.0f));
		gl_has_errors();
	}
	else
	{
		assert(false && "Type of render request not supported");
	}

	// Getting lighting information and condition
	GLint in_shop = glGetUniformLocation(program, "in_shop");
	if (twoPlayer.inTwoPlayerMode) {
		if (registry.inShops.has(registry.players.entities[0]) || registry.inShops.has(registry.players.entities[1])) {
			glUniform1i(in_shop, 1);
		}
		else {
			glUniform1i(in_shop, 0);
		}
	}
	else {
		if (registry.inShops.has(registry.players.entities[0])) {
			glUniform1i(in_shop, 1);
		}
		else {
			glUniform1i(in_shop, 0);
		}
	}
	GLint ambient_light = glGetUniformLocation(program, "ambient_light");
	GLint light_source_pos = glGetUniformLocation(program, "light_source_pos");
	GLint light_col = glGetUniformLocation(program, "light_col");
	GLint light_intensity = glGetUniformLocation(program, "light_intensity");
	Lighting lighting = Lighting();
	glUniform3fv(ambient_light, 1, (float*)& lighting.ambient_light);
	glUniform2fv(light_source_pos, 1, (float*)& lighting.light_source_pos);
	glUniform3fv(light_col, 1, (float*)& lighting.light_col);
	glUniform1f(light_intensity, lighting.light_intensity);
	gl_has_errors();

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float *)&color);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

	Transform transformSeparate;
	transformSeparate.translate(motion.position);
	GLuint translate_loc = glGetUniformLocation(currProgram, "translate");
	glUniformMatrix3fv(translate_loc, 1, GL_FALSE, (float *)&transformSeparate.mat);
	transformSeparate.reset();

	transformSeparate.rotate(motion.angle);
	GLuint rotation_loc = glGetUniformLocation(currProgram, "rotation");
	glUniformMatrix3fv(rotation_loc, 1, GL_FALSE, (float *)&transformSeparate.mat);
	transformSeparate.reset();

	transformSeparate.scale(motion.scale);
	GLuint scale_loc = glGetUniformLocation(currProgram, "scale");
	glUniformMatrix3fv(scale_loc, 1, GL_FALSE, (float *)&transformSeparate.mat);

	Transform transform;
	transform.translate(motion.position);
	transform.rotate(motion.angle);
	transform.scale(motion.scale);

	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform.mat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
	gl_has_errors();
	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}

// draw the intermediate texture to the screen, with some distortion to simulate
// water
void RenderSystem::drawToScreen()
{
	// Setting shaders
	// get the water texture, sprite mesh, and program
	glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::WATER]);
	gl_has_errors();
	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glDisable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER,
		index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
																	 // indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();
	const GLuint water_program = effects[(GLuint)EFFECT_ASSET_ID::WATER];
	// Set clock
	GLuint time_uloc = glGetUniformLocation(water_program, "time");
	GLuint brighten_timer_uloc = glGetUniformLocation(water_program, "brighten_screen_factor");
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	ScreenState& screen = registry.screenStates.get(screen_state_entity);
	glUniform1f(brighten_timer_uloc, screen.brighten_screen_factor);
	gl_has_errors();
	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(water_program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	gl_has_errors();
	// Draw
	glDrawElements(
		GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
		nullptr); // one triangle = 3 vertices; nullptr indicates that there is
				  // no offset from the bound index buffer
	gl_has_errors();

	// get game over information
	GLint game_over_uloc = glGetUniformLocation(water_program, "game_over_factor");

	//if (screen.game_over_factor == 0 || screen.game_over_factor == 1) {
	glUniform1i(game_over_uloc, screen.game_over_factor);
	
	gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();
	// Clearing backbuffer

	glViewport(0, 0, w, h);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
							  // and alpha blending, one would have to sort
							  // sprites back to front
	gl_has_errors();

	mat3 projection_2D = createProjectionMatrix(0.f, 0.f);

	// Draw all textured meshes that have a position and size component
	for (Entity entity : registry.renderRequests.entities)
	{
		if (!registry.motions.has(entity))
			continue;
		// Note, its not very efficient to access elements indirectly via the entity
		// albeit iterating through all Sprites in sequence. A good point to optimize
		drawTexturedMesh(entity, projection_2D);
	}

	// Truely render to the screen
	drawToScreen();

	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();
}

void RenderSystem::playerOneTransition(bool leaveShop) {
	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
	vec2 player2Pos = registry.motions.get(registry.players.entities[1]).position;
	Entity player2Entity = registry.players.entities[1];
	if (leaveShop) {
		registry.inShops.remove(player2Entity);
		gameHud.currentLocation = BATTLE_ROOM;
		HUDLocationSwitch(gameHud.playerOneHudEntity);
		HUDLocationSwitch(gameHud.playerTwoHudEntity);
	} else {
		registry.inShops.emplace(player2Entity);
		gameHud.currentLocation = SHOP_ROOM;
		HUDLocationSwitch(gameHud.playerOneHudEntity);
		HUDLocationSwitch(gameHud.playerTwoHudEntity);
	}
	registry.motions.get(player2Entity).velocity = vec2(0, 0);
	if (registry.mouseDestinations.has(player2Entity))
		registry.mouseDestinations.get(player2Entity).position = player2Pos;
	if (leaveShop) {
		registry.motions.get(player2Entity).position = vec2(screenWidth + defaultResolution.shopBufferZone, screenHeight - defaultResolution.shopBufferZone * 3);
	} else {
		registry.motions.get(player2Entity).position = vec2(screenWidth + defaultResolution.shopBufferZone, screenHeight + defaultResolution.shopBufferZone * 3);
	}
}

void RenderSystem::playerTwoTransition(bool leaveShop, vec2 player2Pos) {
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	Entity player1Entity = registry.players.entities[0];
	Entity player2Entity = registry.players.entities[1];
	if (leaveShop) {
		registry.motions.get(player2Entity).position.y -= defaultResolution.shopBufferZone * 3;
		registry.inShops.remove(player2Entity);
		gameHud.currentLocation = BATTLE_ROOM;
		HUDLocationSwitch(gameHud.playerOneHudEntity);
		HUDLocationSwitch(gameHud.playerTwoHudEntity);
	} else {
		registry.motions.get(player2Entity).position.y += defaultResolution.shopBufferZone * 3;
		registry.inShops.emplace(player2Entity);
		gameHud.currentLocation = SHOP_ROOM;
		HUDLocationSwitch(gameHud.playerOneHudEntity);
		HUDLocationSwitch(gameHud.playerTwoHudEntity);
	}
	registry.motions.get(player2Entity).velocity = vec2(0, 0);
	if (registry.mouseDestinations.has(player2Entity)) {
		registry.mouseDestinations.get(player2Entity).position = player2Pos;
	}
	if (leaveShop) {
		registry.motions.get(player1Entity).position = vec2((w / 2) - defaultResolution.shopBufferZone, player2Pos.y - defaultResolution.shopBufferZone * 3);
	} else {
		registry.motions.get(player1Entity).position = vec2((w / 2) - defaultResolution.shopBufferZone, player2Pos.y + defaultResolution.shopBufferZone * 3);
	}
	registry.motions.get(player2Entity).position.x = ((float)w / 2) + defaultResolution.shopBufferZone;
}

mat3 RenderSystem::createProjectionMatrix(float left, float top)
{
	// Fake projection matrix, scales with respect to window coordinates

	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	gl_has_errors();
	float right = (float)w / screen_scale;
	float bottom = (float)h / screen_scale;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	mat3 projMat = { {sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f} };;

	vec2 player1Pos = registry.motions.get(registry.players.entities[0]).position;
	if (twoPlayer.inTwoPlayerMode) {
		vec2 player2Pos = registry.motions.get(registry.players.entities[1]).position;
		if (player1Pos.y - h < defaultResolution.shopBufferZone) { // player1 is leaves the shop
			if (registry.inShops.has(registry.players.entities[1])) {
				playerOneTransition(true);
			}
			projMat = { {sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f} };
		}
		else if (player1Pos.y - h > -defaultResolution.shopBufferZone) { // player 1 enters the shop
			if (!registry.inShops.has(registry.players.entities[1])) {
				playerOneTransition(false);
			}
			projMat = { {sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty + 2, 1.f} };
		}
		else {
			projMat = { {sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f} };
		}

		if (registry.inShops.has(registry.players.entities[1])) { // player 2 is in the shop
			if (player2Pos.y - h < defaultResolution.shopBufferZone) { // player 2 leaves the shop
				playerTwoTransition(true, player2Pos);
				projMat = { {sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f} };
			}
			else {
				projMat = { {sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty + 2, 1.f} };
			}
		}
		else { // player 2 is not in the shop
			if (player2Pos.y - h > -defaultResolution.shopBufferZone) { // player 2 enters the shop
				playerTwoTransition(false, player2Pos);
				projMat = { {sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty + 2, 1.f} };
			}
			else {
				projMat = { {sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f} };
			}
		}
	}
	else {
		Entity p1 = registry.players.entities[0];
		if (player1Pos.y - h > -defaultResolution.shopBufferZone) {
			projMat = { {sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty + 2, 1.f} };
			// In the shop 
			if (!registry.inShops.has(p1)) {
				registry.inShops.emplace(p1);
				gameHud.currentLocation = SHOP_ROOM;
				HUDLocationSwitch(gameHud.playerOneHudEntity);
			}
		}
		else if (player1Pos.y - h < defaultResolution.shopBufferZone) {
			projMat = { {sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f} };
			if (registry.inShops.has(p1)) {
				registry.inShops.remove(p1);
				gameHud.currentLocation = BATTLE_ROOM;
				HUDLocationSwitch(gameHud.playerOneHudEntity);
			}
		}
		else {
			projMat = { {sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f} };
		}
	}
	return projMat;
}

void RenderSystem::textureEffectSetup(const GLuint program, Entity entity) {
	GLint in_position_loc = glGetAttribLocation(program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
	gl_has_errors();
	assert(in_texcoord_loc >= 0);

	int vertexSize = 3;
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, vertexSize, GL_FLOAT, GL_FALSE,
		sizeof(TexturedVertex), (void *)0);
	gl_has_errors();

	int texCoordSize = 2;
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(
		in_texcoord_loc, texCoordSize, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
		(void *)sizeof(
			vec3)); // note the stride to skip the preceeding vertex position
	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	gl_has_errors();

	assert(registry.renderRequests.has(entity));
	GLuint texture_id =
		texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

	glBindTexture(GL_TEXTURE_2D, texture_id);
	gl_has_errors();
}

void RenderSystem::enemyEffects(const GLuint program, Entity entity) {
	GLfloat colorScale = glGetUniformLocation(program, "color_scale");
	Enemy& enemy = registry.enemies.get(entity);
	float color_scale_value = enemy.max_hp - enemy.hp;
	glUniform1f(colorScale, color_scale_value);
	GLint inInvin = glGetUniformLocation(program, "inInvin");
	glUniform1i(inInvin, registry.enemies.get(entity).isInvin);
	GLfloat time_loc = glGetUniformLocation(program, "time");
	glUniform1f(time_loc, (float)(glfwGetTime() * 10.0f));
	GLint hitVelocityLoc = glGetUniformLocation(program, "velocityOfPlayerHit");
	glUniform2f(hitVelocityLoc, enemy.velocityOfPlayerHit.x, enemy.velocityOfPlayerHit.y);
	GLint playerDamageLoc = glGetUniformLocation(program, "playerDamage");
	glUniform1i(playerDamageLoc, enemy.damageOfPlayerHit);
	GLint isDeadLoc = glGetUniformLocation(program, "isDead");
	glUniform1i(isDeadLoc, enemy.isDead);
	if (enemy.isDead) {
		DeadEnemy& deadEnemy = registry.deadEnemies.get(entity);
		GLint gotCutLoc = glGetUniformLocation(program, "gotCut");
		glUniform1i(gotCutLoc, deadEnemy.gotCut);
		GLfloat animationTimeLoc = glGetUniformLocation(program, "animationTime");
		glUniform1f(animationTimeLoc, deadEnemy.deathTimer / deadEnemy.deathAnimationTime);
	}
}

void RenderSystem::playerEffects(const GLuint program, Entity entity) {
	Player& player = registry.players.get(entity);
	GLfloat colorScale = glGetUniformLocation(program, "color_scale");
	float color_scale_value = registry.playerStats.get(player.playerStat).maxHp - player.hp;
	glUniform1f(colorScale, color_scale_value);
	GLint inInvin = glGetUniformLocation(program, "inInvin");
	glUniform1i(inInvin, player.isInvin);
	GLfloat time_loc = glGetUniformLocation(program, "time");
	glUniform1f(time_loc, (float)(glfwGetTime() * 10.0f));
	GLint isDeadLoc = glGetUniformLocation(program, "isDead");
	glUniform1i(isDeadLoc, player.isDead);
	if (player.isDead) {
		DeadPlayer& deadPlayer = registry.deadPlayers.get(entity);
		GLfloat animationTimeLoc = glGetUniformLocation(program, "animationTime");
		glUniform1f(animationTimeLoc, deadPlayer.deathTimer / deadPlayer.deathAnimationTime);
	}
}
