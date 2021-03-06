#include "Background.h"
#include "Renderer.h"

Background::Background()
	:m_Width(0), m_Height(0), m_Model(1.0f)
{
}

Background::Background(glm::vec2 bottom_right)
{
	SetSize(bottom_right);
}

Background::Background(const Background& b) noexcept :
	m_Width(b.m_Width), m_Height(b.m_Height), m_Model(b.m_Model)
{
}

Background::~Background()
{
}

void Background::SetSize(glm::vec2 bottom_right)
{
	m_Width = bottom_right.x;
	m_Height = bottom_right.y;
	m_Model = glm::translate(glm::mat4(1.0f), glm::vec3(m_Width / 2, m_Height / 2, 0.0f));
	m_Model = glm::scale(m_Model, glm::vec3(m_Width, m_Height, 1.0f));
}

void Background::Draw(Shader& shd, const Texture& tex)
{
	Renderer::SetTextureBoundary({ 8.0f, 1.0f });
	Renderer::DrawRect(shd, tex, m_Model, { 0.0f, 0.0f });
}
