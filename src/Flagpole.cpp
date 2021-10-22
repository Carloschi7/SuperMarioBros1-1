#include "Flagpole.h"
#include "Renderer.h"

FlagPole::FlagPole()
	:FlagPole(glm::vec2(0.0f))
{
}

FlagPole::FlagPole(const glm::vec2& pos)
	:m_Pos(pos)
{
}

FlagPole::FlagPole(FlagPole&& right) noexcept :
	m_Pos(right.m_Pos)
{
}

void FlagPole::Draw(Shader& shd, const Texture& tex)
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(m_Pos.x, m_Pos.y - 4.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.0f, 10.0f, 1.0f));
	Renderer::DrawRect(shd, tex, model, {0.0f, 0.0f});
}

void FlagPole::PoleCollision(Player& p)
{
	const glm::vec2& pos = p.Position();
	//Simple if statement to check if mario is overlapping with the pole
	if (pos.x > m_Pos.x && pos.x < m_Pos.x + 1.0f && std::abs(pos.y - m_Pos.y) < 9.0f)
	{
		p.bTouchesFlagpole = true;
	}
}
