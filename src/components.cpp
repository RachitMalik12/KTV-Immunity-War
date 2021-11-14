#include "components.hpp"
#include "render_system.hpp" // for gl_has_errors
#include "tiny_ecs_registry.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../ext/stb_image/stb_image.h"
#include "../ext/json/dist/json/json.h" 

// stlib
#include <iostream>
#include <sstream>

Debug debugging;
LevelFileLoader levelFileLoader; 
GameSaveDataManager dataManager; 
float death_timer_counter_ms = 3000;


// Very, VERY simple OBJ loader from https://github.com/opengl-tutorials/ogl tutorial 7
// (modified to also read vertex color and omit uv and normals)
bool Mesh::loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size)
{
	// disable warnings about fscanf and fopen on Windows
#ifdef _MSC_VER
	#pragma warning(disable:4996)
#endif

	printf("Loading OBJ file %s...\n", obj_path.c_str());
	// Note, normal and UV indices are currently not used
	std::vector<uint16_t> out_uv_indices, out_normal_indices;
	std::vector<glm::vec2> out_uvs;
	std::vector<glm::vec3> out_normals;

	FILE* file = fopen(obj_path.c_str(), "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while (1) {
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		if (strcmp(lineHeader, "v") == 0) {
			ColoredVertex vertex;
			fscanf(file, "%f %f %f %f %f %f\n", &vertex.position.x, &vertex.position.y, &vertex.position.z,
				                                &vertex.color.x, &vertex.color.y, &vertex.color.z);
			out_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			out_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			out_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], normalIndex[3]; // , uvIndex[3]
			//int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
			if (matches != 6) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				fclose(file);
				return false;
			}
			// -1 since .obj starts counting at 1 and OpenGL starts at 0
			out_vertex_indices.push_back((uint16_t)vertexIndex[0] - 1);
			out_vertex_indices.push_back((uint16_t)vertexIndex[1] - 1);
			out_vertex_indices.push_back((uint16_t)vertexIndex[2] - 1);
			//out_uv_indices.push_back(uvIndex[0] - 1);
			//out_uv_indices.push_back(uvIndex[1] - 1);
			//out_uv_indices.push_back(uvIndex[2] - 1);
			out_normal_indices.push_back((uint16_t)normalIndex[0] - 1);
			out_normal_indices.push_back((uint16_t)normalIndex[1] - 1);
			out_normal_indices.push_back((uint16_t)normalIndex[2] - 1);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}
	fclose(file);

	// Compute bounds of the mesh
	vec3 max_position = { -99999,-99999,-99999 };
	vec3 min_position = { 99999,99999,99999 };
	for (ColoredVertex& pos : out_vertices)
	{
		max_position = glm::max(max_position, pos.position);
		min_position = glm::min(min_position, pos.position);
	}
	min_position.z = 0;// don't scale z direction
	max_position.z = 1;
	vec3 size3d = max_position - min_position;
	out_size = size3d;

	// Normalize mesh to range -0.5 ... 0.5
	for (ColoredVertex& pos : out_vertices)
		pos.position = ((pos.position - min_position) / size3d) - vec3(0.5f, 0.5f, 0.f);

	return true;
}

void LevelFileLoader::readFile() {
	// Source: https://stackoverflow.com/questions/4273056/jsoncpp-not-reading-files-correctly
	Json::Value root;   // will contain the root value after parsing.
	Json::CharReaderBuilder builder;
	std::ifstream file(data_path() + "/levels/level_design.json",std::ifstream::binary);
	std::string errs;
	bool parsedSuccessfully = Json::parseFromStream(builder, file, &root, &errs);
	if (!parsedSuccessfully)
	{   // Print error
		std::cout << errs << "\n";
	}
	auto jsonLevel = root.get("levels", -1); 

    // Help from: https://stackoverflow.com/questions/44442932/iterate-over-an-array-of-json-objects-with-jsoncpp
	for (Json::Value::ArrayIndex i = 0; i != jsonLevel.size(); i++) {
		auto enemy_types = jsonLevel[i]["enemy_types"];
		Level output_level; 
		auto current_file_level = jsonLevel[i]; 
		// enemies
		auto enemies = current_file_level["enemies"];
		for (Json::Value::ArrayIndex i = 0; i != enemies.size(); i++) {
			output_level.enemies.push_back(enemies[i].asInt()); 
		}
		auto num_enemy_types = jsonLevel[i]["num_enemy_types"];
		for (Json::Value::ArrayIndex i = 0; i != enemy_types.size(); i++) {
			output_level.enemy_types.push_back(enemy_types[i].asInt());
		}
		auto enemyPositions = current_file_level["enemy_positions"];
		int size = enemyPositions.size();
		for (Json::Value::ArrayIndex i = 0; i < size; i++) {
			std::vector<vec2> curEnemyPosition;
			int curSize = enemyPositions[i].size();
			for (Json::Value::ArrayIndex j = 0; j < curSize ; j++) {
				vec2 curPosition = vec2(enemyPositions[i][j]["x"].asFloat(), enemyPositions[i][j]["y"].asFloat());
				curEnemyPosition.push_back(curPosition);
			}
			output_level.enemyPositions.push_back(curEnemyPosition);
		}

		//blocks
		output_level.num_blocks = current_file_level["blocks"].asInt(); 
		auto block_positions = jsonLevel[i]["block_positions"]; 
		for (Json::Value::ArrayIndex i = 0; i != block_positions.size(); i++) {
			vec2 position_i = vec2(block_positions[i]["x"].asFloat(), block_positions[i]["y"].asFloat()); 
			output_level.block_positions.push_back(position_i);
		}

		// player positions
		auto player_position = jsonLevel[i]["player_position"]; 
		output_level.player_position = vec2(player_position["x"].asFloat(), player_position["y"].asFloat()); 
		auto player2_position = jsonLevel[i]["player2_position"];
		output_level.player2_position = vec2(player2_position["x"].asFloat(), player2_position["y"].asFloat());

		levels.push_back(output_level); 
	}
}

std::vector<Level> LevelFileLoader::getLevels() {
	return levels;
}
// Some reading logic inspired from here: https://stackoverflow.com/questions/27486538/jsoncpp-writing-back-to-the-json-file
void GameSaveDataManager::setPlayerModeFromFile() {
	std::string path = data_path() + "/saveData/saveFile.json";
	Json::Value root;
	Json::Reader reader;
	std::ifstream saveFileRead(path);
	if (!saveFileRead.good()) {
		std::cout << "No save file found, set player mode to 1";
		playerModeFromFile = 1;
	}

	bool success = reader.parse(saveFileRead, root);
	if (!success) {
		std::cout << "Failed to parse save file :( : " << reader.getFormatedErrorMessages();
	}
	
	if (root.isMember("playerStat_p1") && root.isMember("playerStat_p2")) {
		playerModeFromFile = 2; 
	}
	else if (root.isMember("playerStat_p1")) {
		playerModeFromFile = 1; 
	}
}
// Some reading logic inspired from here: https://stackoverflow.com/questions/27486538/jsoncpp-writing-back-to-the-json-file
bool GameSaveDataManager::loadFile() {
	std::string path = data_path() + "/saveData/saveFile.json";
	Json::Value root;
	Json::Reader reader;
	std::ifstream saveFileRead(path);
	if (!saveFileRead.good()) {
		std::cout << "No save file found, loading level 1 by default";
		return false; 
	}
	bool success = reader.parse(saveFileRead, root);
	if (!success) {
		std::cout << "Failed to parse save file :( : " << reader.getFormatedErrorMessages();
	}
	this->levelNumber = root["levelNumber"].asInt(); 
	// Load p1 and p2 player information
	setPlayerModeFromFile(); 
	if (playerModeFromFile == 1) {
		loadPlayerStats(root, 1); 
	} 
	if (playerModeFromFile == 2) {
		loadPlayerStats(root, 1); 
		loadPlayerStats(root, 2); 
	}
	saveFileRead.close();
	return true; 
}

void GameSaveDataManager::loadPlayerStats(Json::Value& root, int playerMode) {
	std::string playerStatKey = "playerStat_p" + std::to_string(playerMode); 
	Entity playerStatEntity = playerMode == 1 ? playerStatEntity1 : playerStatEntity2; 
	PlayerStat& playerStat = registry.playerStats.get(playerStatEntity);
	playerStat.damage = root[playerStatKey]["damage"].asInt();
	playerStat.maxHp = root [playerStatKey]["maxHp"].asInt();
	playerStat.money = root[playerStatKey]["money"].asInt();
	playerStat.movementSpeed = root[playerStatKey]["movementSpeed"].asFloat();
	playerStat.attackDelay = root[playerStatKey]["attackDelay"].asFloat(); 
	playerStat.projectileSpeed = root[playerStatKey]["projectileSpeed"].asFloat(); 
}
// Some inspired from here: https://stackoverflow.com/questions/27486538/jsoncpp-writing-back-to-the-json-file
void GameSaveDataManager::saveFile(int playerMode) {
	std::string path = data_path() + "/saveData/saveFile.json";
	Json::Value root;
	Json::Value levelNum(this->levelNumber);
	Json::StyledStreamWriter writer;
	root["levelNumber"] = levelNum;
	// Always write player 1 
	savePlayerStats(root, playerStatEntity1, 1);
	if (playerMode == 2) {
		savePlayerStats(root, playerStatEntity2, playerMode);
	}
	std::ofstream saveFileWrite(path);
	writer.write(saveFileWrite, root);
	saveFileWrite.close();
}

void GameSaveDataManager::savePlayerStats(Json::Value& root, Entity playerStatEntity,  int playerNum) {
	PlayerStat& playerStat = registry.playerStats.get(playerStatEntity); 
	std::string playerKey = "playerStat_p" + std::to_string(playerNum); 
	Json::Value damage(playerStat.damage);
	root[playerKey]["damage"] = damage; 
	Json::Value maxHp(playerStat.maxHp);
	root[playerKey]["maxHp"] = maxHp; 
	Json::Value money(playerStat.money);
	root[playerKey]["money"] = money; 
	Json::Value movementSpeed(playerStat.movementSpeed);
	root[playerKey]["movementSpeed"] = movementSpeed; 
	Json::Value attackDelay(playerStat.attackDelay);
	root[playerKey]["attackDelay"] = attackDelay;
	Json::Value projectileSpeed(playerStat.projectileSpeed);
	root[playerKey]["projectileSpeed"] = projectileSpeed;

}

void GameSaveDataManager::setPlayerStatEntity(Entity playerStatEntity1, Entity playerStatEntity2) {
	this->playerStatEntity1 = playerStatEntity1; 
	this->playerStatEntity2 = playerStatEntity2; 
}

void GameSaveDataManager::setPlayerStatEntity(Entity playerStatEntity1) {
	this->playerStatEntity1 = playerStatEntity1; 
}

void GameSaveDataManager::setLevelNumber(int levelNumber) {
	this->levelNumber = levelNumber;
}

int GameSaveDataManager::getLevelNumber() {
	return this->levelNumber; 
}

int GameSaveDataManager::getPlayerMode() {
	return this->playerModeFromFile;
}
