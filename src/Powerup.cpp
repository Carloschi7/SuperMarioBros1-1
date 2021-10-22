#include "Powerup.h"
#include "Renderer.h"

Powerup::Powerup(PowerUpType type, const glm::vec2& pos)
	:pos(pos),
	speed(0.0f, 0.0f),
	bForward(true),
	bOnGround(true),
	bExpired(false),
	m_Type(type),
	fTextureOffset(0.0f)
{
	if (m_Type == PowerUpType::FIRE_FLOWER)
		fTextureOffset = 0.5f;
}

Powerup::Powerup(Powerup&& p) noexcept :
	pos(p.pos),
	speed(p.speed),
	bForward(p.bForward),
	bOnGround(p.bOnGround),
	bExpired(p.bExpired),
	m_Type(p.m_Type),
	m_Timer(p.m_Timer),
	fTextureOffset(p.fTextureOffset)
{
}

Powerup::~Powerup()
{
}

void Powerup::Draw(Shader& shd, const Texture& tex)
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos, 1.0f));
	Renderer::DrawRect(shd, tex, model, { fTextureOffset, 0.0f });
}

bool Powerup::IsSpawning() const
{
	return !m_Timer.HavePassedAtLeast(1.5f);
}

void Powerup::Update()
{
	if (m_Type == PowerUpType::MUSHROOM)
	{
		bOnGround = (speed.y == 0.0f);
		speed += glm::vec2(0.0f, 0.01f);

		if (speed.y > 0.1f) speed.y = 0.1f;

		float dir = (bForward) ? 0.05f : -0.05f;
		speed.x = (bOnGround) ? dir : 0.0f;
	}
}

