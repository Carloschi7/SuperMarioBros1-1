#pragma once
#include "Tile.h"
#include "Enemy.h"
#include "Player.h"
#include "Powerup.h"
#include "Counter.h"
#include "Flagpole.h"
#include "Pipe.h"
#include "LevelScene.h"

class Level
{
public:
	//Function type which stores pipe binding functions
	using pipe_func = std::function<void(std::vector<Pipe>&)>;

	Level();
	Level(const Level&) = delete;
	Level(const std::string& levelmap, uint32_t width, uint32_t height, glm::vec2 camera_offset,
		uint32_t widthInBlocks, uint32_t heightInBlocks, const pipe_func& func);
	~Level();

	static std::string LoadStringInputFromFile(const std::string& str);

public:
	//Upload level instance
	void LoadLevel(const std::string& levelmap, uint32_t width, uint32_t height, glm::vec2 camera_offset,
		uint32_t widthInBlocks, uint32_t heightInBlocks, const pipe_func& func);

	//Draw current level
	void DrawLevel(Shader& scene_shader, Shader& overlay_shader, const std::vector<Texture>& textures);

	//Handles collisions for every entity in the scope of the level
	void LevelLogic(Player& p, Camera& c, float fElapsedTime, const Window* wnd);

	//Scrolls screen according to the player's position
	void ScrollScreen(Player& p, Camera& c, float fPlayerSpeed);

	//Reloads assets into a new instance
	void ResetCurrentLevel(Player& p, Camera& c);

	//Retrieves vectors
	inline const std::vector<Tile>& GetTileVector() const { return m_Tiles; }
	inline std::vector<Tile>& GetTileVector() { return m_Tiles; }
	inline const std::vector<Enemy>& GetEnemyVector() const { return m_Enemies; }
	inline std::vector<Enemy>& GetEnemyVector() { return m_Enemies; }
	inline bool PlayerExitPipes() const { return m_ExitPipeScene != nullptr; }

	//Retrieves level width and height
	glm::ivec2 GetLevelSize() const;
	void DestroyAnimations();
private:
	//Handles collisions between moving entities and static tiles
	void EntityBlockCollision(glm::vec2& pos, glm::vec2& sp, const glm::vec2& tilepos, float fEntityTileHeight,
		float fElapsedTime);

	//Functions to handle scenes(death, flagpole animations and more)
	//The return type determines whether the animation type requires or not to
	//Skip level collisions to work effectively
	bool HandleAnimations(Player& p, Camera& c);

	bool IsTile(const glm::vec2& position) const;

private:
	std::string m_Level;
	uint32_t m_LevelWidth, m_LevelHeight;
	uint32_t m_WidthInBlocks, m_HeightInBlocks;
	std::vector<Tile> m_Tiles;
	std::vector<Enemy> m_Enemies;
	std::vector<Powerup> m_Powerups;
	std::vector<Pipe> m_Pipes;
	FlagPole m_Flagpole;
	glm::vec2 m_ScreenPos, m_OriginalScreenPos;
	Counter m_CoinCounter;
	LevelScene* m_DeathScene, *m_FlagpoleScene, *m_EnterPipeScene, *m_ExitPipeScene;
	pipe_func m_PipeBindingFunc;
};