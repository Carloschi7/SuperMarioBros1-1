#pragma once
#include <glm/glm.hpp>
#include "Shader.h"
#include "VertexManager.h"
#include "Player.h"

class FlagPole
{
public:
	FlagPole();
	FlagPole(const glm::vec2& pos);
	FlagPole(FlagPole&& right) noexcept;
	inline void SetPosition(const glm::vec2& pos) { m_Pos = pos; }
	inline const glm::vec2& Position() const { return m_Pos; }
	void Draw(Shader& shd, const Texture& tex);
	void PoleCollision(Player& p);
private:
	glm::vec2 m_Pos;
	static constexpr uint32_t m_PoleHeight = 9;
};