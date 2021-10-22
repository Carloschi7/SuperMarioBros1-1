#pragma once
#include <glm/glm.hpp>
#include <chrono>
#include "Shader.h"
#include "Texture.h"
#include "Timer.h"

enum class PowerUpType
{
	MUSHROOM = 0, FIRE_FLOWER
};

class Powerup
{
public:
	Powerup(PowerUpType type, const glm::vec2& pos);
	Powerup(Powerup&& p) noexcept;
	~Powerup();

	void Draw(Shader& shd, const Texture& tex);
	bool IsSpawning() const;
	void Update();
	PowerUpType Type() const { return m_Type; }
public:
	glm::vec2 pos, speed;
	bool bForward, bExpired;
private:
	float fTextureOffset;
	PowerUpType m_Type;
	bool bOnGround;
	Timer m_Timer;
};