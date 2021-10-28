#include <chrono>
#include "Tile.h"
#include "Renderer.h"

Tile::Tile(TypeOfTile type, glm::vec2 pos)
	: m_Pos(pos + glm::vec2(0.5f, 0.5f)),
	m_pStartingPos(nullptr),
	m_Type(type),
	bExpired(false),
	bPopup(false),
	fTextureOffset(0.0f)
{
	switch (type)
	{
	case TypeOfTile::SOLID:
		fTextureOffset = 0.0f;
		break;
	case TypeOfTile::COIN:
		fTextureOffset = 1.0f / fTileCount;
		break;
	case TypeOfTile::QUESTION_MARK_EMPTY:
	case TypeOfTile::QUESTION_MARK_MUSHROOM:
	case TypeOfTile::QUESTION_MARK_FIRE_FLOWER:
		fTextureOffset = 2.0f / fTileCount;
		break;
	case TypeOfTile::BRICK:
		fTextureOffset = 4.0f / fTileCount;
		break;
	case TypeOfTile::STONE:
		fTextureOffset = 5.0f / fTileCount;
		break;
	case TypeOfTile::UNDERGROUND_SOLID:
		fTextureOffset = 6.0f / fTileCount;
		break;
	case TypeOfTile::UNDERGROUND_BRICK:
		fTextureOffset = 7.0f / fTileCount;
		break;
	case TypeOfTile::INVISIBLE:
		break;
	}
}

Tile::Tile(const Tile& t) noexcept : 
	m_Pos(t.m_Pos),
	m_pStartingPos(t.m_pStartingPos), //Just copying the address temporary, no transfer of ownership
	m_Type(t.m_Type),
	bExpired(t.bExpired),
	bPopup(t.bPopup),
	fTextureOffset(t.fTextureOffset)
{
}

Tile::Tile(Tile&& t) noexcept :
	m_Pos(t.m_Pos),
	m_Type(t.m_Type),
	bExpired(t.bExpired),
	bPopup(t.bPopup),
	fTextureOffset(t.fTextureOffset)
{
	m_pStartingPos = std::move(t.m_pStartingPos);
}

Tile::~Tile()
{
}

Tile& Tile::operator=(const Tile& t)
{
	m_Pos = t.m_Pos;
	m_pStartingPos = t.m_pStartingPos;
	m_Type = t.m_Type;
	bExpired = t.bExpired;
	bPopup = t.bPopup;
	fTextureOffset = t.fTextureOffset;
	return *this;
}

void Tile::Draw(Shader& shd, const Texture& tex)
{
	if (m_Type == TypeOfTile::INVISIBLE) 
		return;

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(m_Pos,0.0f));
	Renderer::DrawRect(shd, tex, model, { fTextureOffset, 0.0f });
}

void Tile::SetPosition(const glm::vec2& pos)
{
	m_Pos = pos + glm::vec2(0.5f, 0.5f);
}

void Tile::SetIfPopup(bool true_or_false)
{
	bPopup = true_or_false;
	if (bPopup && !m_pStartingPos)
	{
		m_pStartingPos = std::make_shared<glm::vec2>(m_Pos);
	}
}

void Tile::UpdateIfTemporary(float fSpeed)
{
	if (bPopup)
	{
		m_Pos.y -= fSpeed;

		if (glm::abs(m_pStartingPos->y - m_Pos.y) > 3.0f)
		{
			bExpired = true;
		}
	}
}

TypeOfTile Tile::Type() const
{
	return m_Type;
}

const glm::vec2& Tile::Position() const
{
	return m_Pos;
}
