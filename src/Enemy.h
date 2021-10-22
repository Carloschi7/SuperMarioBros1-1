#pragma once
#include <glm/glm.hpp>
#include "Timer.h"
#include "Shader.h"
#include "Texture.h"

enum class TypeOfEnemy
{
	GOOMBA = 1, KOOPA
};

enum class TypeOfEnemyDeath
{
	STOMP,
	FIREBALL_HIT_FROM_LEFT,
	FIREBALL_HIT_FROM_RIGHT
};

class Enemy
{
public:
	using time_point = std::chrono::system_clock::time_point;
	using duration = std::chrono::duration<float>;

	Enemy(TypeOfEnemy type, glm::vec2 pos, std::string texpath);
	Enemy(const Enemy& e);
	Enemy(Enemy&& e) noexcept;
	~Enemy();

	Enemy& operator=(const Enemy&) = default;

	void Draw(Shader& shd, const Texture& tex);
	void Update(float fElapsedTime);
	TypeOfEnemy Type() const;
	inline void SetDeathType(TypeOfEnemyDeath type) { m_DeathType = type; }
	inline TypeOfEnemyDeath GetDeathType() const { return m_DeathType; }
public:
	bool bForward, bExpiring, bExpired;
	glm::vec2 pos, speed;

private:
	bool bOnGround, bLeftPhase;
	TypeOfEnemy m_Type;
	TypeOfEnemyDeath m_DeathType;
	Timer m_Timer;
};