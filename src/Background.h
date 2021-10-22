#pragma once
#include "glm/glm.hpp"
#include <string>

#include "Texture.h"
#include "Shader.h"

class Background
{
public:
	Background();
	Background(glm::vec2 bottom_right);
	Background(Background&& b) noexcept;
	~Background();

	void SetSize(glm::vec2 bottom_right);
	void Draw(Shader& shd, Texture& tex);

private:
	uint32_t m_Width, m_Height;
	glm::mat4 m_Model;
};