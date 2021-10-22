#include "Enemy.h"
#include "Renderer.h"

Enemy::Enemy(TypeOfEnemy type, glm::vec2 pos, std::string texpath)
	:m_Type(type),
	pos(pos + glm::vec2(0.5f, 0.5f)),
	speed(glm::vec2(0.0f, 0.005f)),
	bOnGround(false),
	bForward(false), 
	bExpired(false),
	bExpiring(false),
	bLeftPhase(true),
	m_DeathType(TypeOfEnemyDeath::STOMP) //By default
{
}

Enemy::Enemy(const Enemy& e) : 
	pos(e.pos),
	m_Type(e.m_Type),
	m_DeathType(e.m_DeathType),
	speed(e.speed),
	bForward(e.bForward),
	bOnGround(e.bOnGround),
	bExpired(e.bExpired),
	bExpiring(e.bExpiring),
	bLeftPhase(e.bLeftPhase),
	m_Timer(e.m_Timer)
{
}

Enemy::Enemy(Enemy&& e) noexcept :
	pos(e.pos),
	m_Type(e.m_Type),
	m_DeathType(e.m_DeathType),
	speed(e.speed),
	bForward(e.bForward),
	bOnGround(e.bOnGround), 
	bExpired(e.bExpired),
	bExpiring(e.bExpiring),
	bLeftPhase(e.bLeftPhase),
	m_Timer(e.m_Timer)
{
}

Enemy::~Enemy()
{
}

void Enemy::Draw(Shader& shd, const Texture& tex)
{
	if (bExpired) return;
	float fTextureOffset = 0.0f;

	if (m_Type == TypeOfEnemy::GOOMBA)
	{
		if (bExpiring && m_DeathType == TypeOfEnemyDeath::STOMP)
			fTextureOffset = 0.67f;
		else if (!bLeftPhase)
			fTextureOffset = 0.33f;
	}
	else
	{
		if (bExpiring && m_DeathType == TypeOfEnemyDeath::STOMP)
			fTextureOffset = 0.8f;
		else if (bForward && !bLeftPhase)
			fTextureOffset = 0.4f;
		else if (bForward)
			fTextureOffset = 0.6f;
		else if (!bLeftPhase)
			fTextureOffset = 0.2f;
	}


	glm::mat4 model(1.0f);

	switch (m_Type)
	{
	case TypeOfEnemy::GOOMBA:
		model = glm::translate(model, glm::vec3(pos, 0.0f));
		break;
	case TypeOfEnemy::KOOPA:
		//Sligtly adjusted considering the koopa is a little big higher than a goomba
		model = glm::translate(model, glm::vec3(glm::vec2(pos.x, pos.y - 0.3f), 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.5f, 1.0f));
		break;
	}

	Renderer::DrawRect(shd, tex, model, { fTextureOffset, 0.0f });
}

void Enemy::Update(float fElapsedTime)
{
	//Giving a second for the enemy to expire
	if (bExpiring)
	{
		switch (m_DeathType)
		{
		case TypeOfEnemyDeath::STOMP:
			if (m_Timer.state == 0)
			{
				m_Timer.state = 1;
				m_Timer.SetCurrentTimePointAsBegin();
			}

			if (m_Timer.HavePassedAtLeast(1.0f))
				bExpired = true;

			return;
		case TypeOfEnemyDeath::FIREBALL_HIT_FROM_LEFT:
		case TypeOfEnemyDeath::FIREBALL_HIT_FROM_RIGHT:
			if (m_Timer.state == 0)
			{
				m_Timer.state = 1;
				m_Timer.SetCurrentTimePointAsBegin();

				speed.x = (m_DeathType == TypeOfEnemyDeath::FIREBALL_HIT_FROM_LEFT) ?
					8.0f * fElapsedTime : 
					-8.0f * fElapsedTime;
				
				speed.y = -8.0f * fElapsedTime;
			}

			speed += glm::vec2(0.0f, 0.01f);
			pos += speed;

			if (m_Timer.HavePassedAtLeast(2.0f))
				bExpired = true;
			return;
		}
	}

	pos += speed;

	if (m_Timer.HavePassedAtLeast(0.5f))
	{
		bLeftPhase = !bLeftPhase;
		m_Timer.SetCurrentTimePointAsBegin();
	}

	bOnGround = (speed.y == 0.0f);
	speed += glm::vec2(0.0f, 0.01f);//Gravity for enemies

	if (speed.y > 0.1f) speed.y = 0.1f;

	float dir = (bForward) ? 0.05f : -0.05f;
	speed.x = bOnGround ? dir : dir * 0.2f;
}

TypeOfEnemy Enemy::Type() const
{
	return m_Type;
}

