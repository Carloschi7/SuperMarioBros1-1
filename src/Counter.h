#pragma once
#include "VertexManager.h"
#include "Shader.h"
#include "Texture.h"


class Counter
{
public:
	Counter();
	void Draw(Shader& shd, const Texture& tex);
	void Increase();
	void Reset();
private:
	int m_FirstDigit, m_SecondDigit;
	glm::mat4 m_FirstDigitModel, m_SecondDigitModel;
};