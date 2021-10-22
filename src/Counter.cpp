#include "Counter.h"
#include "Renderer.h"

Counter::Counter()
	:m_FirstDigit(0), m_SecondDigit(0)
{
	m_FirstDigitModel = glm::translate(glm::mat4(1.0f), glm::vec3(-0.8f, 0.8f, 0.0f));
	m_FirstDigitModel = glm::scale(m_FirstDigitModel, glm::vec3(0.05f, 0.1f, 1.0f));

	m_SecondDigitModel = glm::translate(glm::mat4(1.0f), glm::vec3(-0.76f, 0.8f, 0.0f));
	m_SecondDigitModel = glm::scale(m_SecondDigitModel, glm::vec3(0.05f, 0.1f, 1.0f));
}

void Counter::Draw(Shader& shd, const Texture& tex)
{
	float offset = 0.0f;

	offset = (float)m_FirstDigit / 10.0f;
	Renderer::DrawRect(shd, tex, m_FirstDigitModel, { offset, 0.0f });

	offset = (float)m_SecondDigit / 10.0f;
	Renderer::DrawRect(shd, tex, m_SecondDigitModel, { offset, 0.0f });
}

void Counter::Increase()
{
	if (m_FirstDigit == 9 && m_SecondDigit == 9)
	{
		m_FirstDigit = 0;
		m_SecondDigit = 0;
	}
	else if (m_SecondDigit == 9)
	{
		m_FirstDigit++;
		m_SecondDigit = 0;
	}
	else
	{
		m_SecondDigit++;
	}
}

void Counter::Reset()
{
	m_FirstDigit = 0;
	m_SecondDigit = 0;
}
