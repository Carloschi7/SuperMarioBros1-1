#pragma once
#include "Tile.h"
#include "Enemy.h"
#include "Player.h"
#include "Powerup.h"
#include "Counter.h"
#include "Flagpole.h"
#include "Pipe.h"
#include "LevelScene.h"
#include "Background.h"

//Defines the current area of the level. Can either be the main level or 
//a bonus area
struct LevelArea
{
	LevelArea() :
		m_LevelWidth(0), m_LevelHeight(0),
		m_ScreenPos(0.0f, 0.0f),
		m_OriginalScreenPos(0.0f, 0.0f)
	{
	}

	LevelArea(const std::string& level,
		uint32_t width, uint32_t height,
		glm::vec2 screen_pos) :
		m_Level(level), m_LevelWidth(width), m_LevelHeight(height),
		m_ScreenPos(screen_pos), m_OriginalScreenPos(screen_pos)
	{
	}

	std::string m_Level;
	uint32_t m_LevelWidth, m_LevelHeight;
	std::vector<Tile> m_Tiles;
	std::vector<Enemy> m_Enemies;
	std::vector<Powerup> m_Powerups;
	std::vector<Pipe> m_Pipes;
	glm::vec2 m_ScreenPos, m_OriginalScreenPos;
	Background m_Background;
};

class Level
{
public:
	Level();
	Level(const Level&) = delete;
	Level(const std::string& levelmap, uint32_t width, uint32_t height, glm::vec2 camera_offset,
		uint32_t widthInBlocks, uint32_t heightInBlocks, const PipesProperties& func);
	~Level();

	static std::string LoadStringInputFromFile(const std::string& str);

public:
	//Upload level instance
	void PushArea(const std::string& levelmap, uint32_t width, uint32_t height, glm::vec2 camera_offset,
		uint32_t widthInBlocks, uint32_t heightInBlocks, const PipesProperties& func);

	void ReloadArea(uint32_t index, const std::string& levelmap, uint32_t width, uint32_t height, glm::vec2 camera_offset,
		uint32_t widthInBlocks, uint32_t heightInBlocks, const PipesProperties& func);

	//Initializes pipes connections
	void InitPipes();

	//Draw current level
	void DrawLevel(Shader& scene_shader, Shader& overlay_shader, const std::vector<Texture>& textures);

	//Draw level background of the current area
	void DrawLevelBackground(Shader& scene_shader, const std::vector<Texture>& textures);

	//Handles collisions for every entity in the scope of the level
	void LevelLogic(Player& p, Camera& c, float fElapsedTime, const Window* wnd);

	//Scrolls screen according to the player's position
	void ScrollScreen(Player& p, Camera& c, float fPlayerSpeed);

	//Reloads assets into a new instance
	void ResetCurrentLevel(Player& p, Camera& c);

	//Function which creates a vector of pointers to the subvectors of pipes
	//Each one defined for each area
	//It will always called each iterations, so there won't be problems with
	//dangling pointers(which can be generated with the reset of an area)
	std::vector<std::vector<Pipe>*> MergedPipes();

	//Select level area
	inline void SwitchArea(uint32_t area_index) { m_AreaIndex = area_index; }

	//Retrieves vectors
	inline const std::vector<Tile>& GetTileVector() const { return Cur().m_Tiles; }
	inline std::vector<Tile>& GetTileVector() { return Cur().m_Tiles; }
	inline const std::vector<Enemy>& GetEnemyVector() const { return Cur().m_Enemies; }
	inline std::vector<Enemy>& GetEnemyVector() { return Cur().m_Enemies; }
	inline bool PlayerExitPipes() const { return m_ExitPipeScene != nullptr; }
	inline void SetScreenPos(uint32_t index, glm::vec2 pos) { m_Areas[index].m_ScreenPos = pos; }

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

	//Utility func to make the code cleaner
	inline LevelArea& Cur() { return m_Areas[m_AreaIndex]; }
	inline const LevelArea& Cur() const { return m_Areas[m_AreaIndex]; }

private:
	std::vector<LevelArea> m_Areas;
	uint32_t m_AreaIndex = 0;

	uint32_t m_WidthInBlocks, m_HeightInBlocks;
	FlagPole m_Flagpole;
	Counter m_CoinCounter;
	LevelScene* m_DeathScene, *m_FlagpoleScene, *m_EnterPipeScene, *m_ExitPipeScene;
	PipesProperties m_PipeProps;
};