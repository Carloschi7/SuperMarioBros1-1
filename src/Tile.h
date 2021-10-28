#pragma once
#include "glm/glm.hpp"
#include "Shader.h"
#include "VertexManager.h"
#include "Texture.h"

enum class TypeOfTile
{
	SOLID = 0,
	COIN,
	QUESTION_MARK_EMPTY,
	QUESTION_MARK_MUSHROOM,
	QUESTION_MARK_FIRE_FLOWER,
	BRICK,
	STONE,
	UNDERGROUND_SOLID,
	UNDERGROUND_BRICK,
	INVISIBLE //Used to give collisions to solid entities like pipes
};

class Tile;

static constexpr float fTileCount = 8.0f;

class Tile
{
public:
	Tile(TypeOfTile type, glm::vec2 pos);
	Tile(const Tile&) noexcept;
	Tile(Tile&& t) noexcept;
	~Tile();

	Tile& operator=(const Tile&);

	void Draw(Shader& shd, const Texture& tex);
	void SetPosition(const glm::vec2& pos);
	void SetIfPopup(bool true_or_false);
	inline bool IsPopup() const { return bPopup; }
	void UpdateIfTemporary(float fSpeed);
	TypeOfTile Type() const;
	const glm::vec2& Position() const;

public:
	bool bExpired;
	float fTextureOffset;

	//Variables for Popup tiles
	std::shared_ptr<glm::vec2> m_pStartingPos;

private:
	bool bPopup;
	TypeOfTile m_Type;
	glm::vec2 m_Pos;
};