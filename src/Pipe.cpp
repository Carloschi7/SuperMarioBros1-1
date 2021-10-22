#include "Pipe.h"
#include "Renderer.h"
#include "Player.h"

Pipe::Pipe(const glm::vec2& position, uint8_t body_size)
	:m_Pos(position), m_BodyHeight(body_size), m_pPipeBodyModels(nullptr),
	m_ConnectedPipe(nullptr)
{
	//Pipe position defined at the bottom left of the object
	
	//We can define model matrices as class members beacuse pipes are static entities
	m_PipeExitModel = glm::translate(glm::mat4(1.0f), glm::vec3(m_Pos.x + 1.0f, m_Pos.y - m_BodyHeight + 0.5f, 0.0f));
	m_PipeExitModel = glm::scale(m_PipeExitModel, glm::vec3(2.0f, 1.0f, 1.0f));

	m_pPipeBodyModels = new glm::mat4[m_BodyHeight];
	for (int32_t i = -(int8_t)m_BodyHeight + 1, j = 0; i <= 0 && j < m_BodyHeight; i++, j++)
	{
		glm::mat4& model = m_pPipeBodyModels[j];
		model = glm::translate(glm::mat4(1.0f), glm::vec3(m_Pos.x + 1.0f, m_Pos.y + i + 0.5f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 1.0f, 1.0f));
	}
}

Pipe::Pipe(Pipe&& p) noexcept :
	m_pPipeBodyModels(std::exchange(p.m_pPipeBodyModels, nullptr)),
	m_PipeExitModel(p.m_PipeExitModel),
	m_Pos(p.m_Pos),
	m_BodyHeight(p.m_BodyHeight),
	m_ConnectedPipe(p.m_ConnectedPipe),
	bEnterable(p.bEnterable)
{
}

Pipe::~Pipe()
{
	if(m_pPipeBodyModels)
		delete[] m_pPipeBodyModels;
}

void Pipe::Draw(Shader& shd, const Texture& tex) const
{
	//Draw exit
	Renderer::DrawRect(shd, tex, m_PipeExitModel, {0.0f, 0.0f});

	//Draw body
	for (uint32_t i = 0; i < m_BodyHeight; i++)
	{
		Renderer::DrawRect(shd, tex, m_pPipeBodyModels[i], { 0.0f, 0.5f });
	}
}

bool Pipe::CheckEntrance(const Player& p, bool IsDownKeyPressed) const
{
	//Defined as the minimum distance from the pipe ledge required for mario to enter the pipe
	float fPipeBorderOffset = 0.75f;
	const glm::vec2& pos = p.Position();

	if (bEnterable && IsDownKeyPressed && !p.bIsJumping)
	{
		if (pos.y < m_Pos.y - m_BodyHeight - 0.4f && pos.y > (float)m_Pos.y - m_BodyHeight - 0.6f &&
			pos.x - fPipeBorderOffset > m_Pos.x && pos.x + fPipeBorderOffset < m_Pos.x + 2.0f)
		{
			return true;
		}
	}

	return false;
}
