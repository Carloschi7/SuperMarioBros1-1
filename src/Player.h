#pragma once
#include "glm/glm.hpp"
#include "MainIncl.h"
#include "Tile.h"
#include "Enemy.h"
#include "Timer.h"

class Pipe;

enum class PlayerState
{
	MARIO = 0, SUPER_MARIO, FIRE_MARIO, 
};

struct FireBall
{
	FireBall(const glm::vec2& pos, const glm::vec2& speed);
	FireBall(const FireBall&) = default;

	glm::vec2 m_Pos, m_Speed;
	void Draw(Shader& shd, const Texture& tex);
	bool Update(const std::vector<Tile>& tiles, std::vector<Enemy>& enemies);
private:
	Timer m_Timer;
	Timer m_RotationTimer;
};

class Player
{
public:
	Player();
	Player(const glm::vec2& pos);
	Player(Player&& p) noexcept;
	~Player();

	void Update(Window& wnd, const std::vector<Tile>& tiles,
		std::vector<Enemy>& enemies,
		float fSpeed, float down, float right);

	void Draw(Shader& shd, const Texture& player_texture, const Texture& fire_ball_texture);
	void SetPlayerState(const PlayerState& p);
	PlayerState GetPlayerState() { return m_PlayerState; }
	float GetHeight();

	glm::vec2& Position();
	inline const glm::vec2& Position() const { return m_Pos; }
	glm::vec2& Speed();
	glm::vec2& Acceleration();
	bool CanBeDamaged();
	void SetVulnerability(bool true_or_false);
	inline const std::vector<FireBall>& GetMarioFireballs() const { return m_FireBalls; }

	bool bIsJumping, bDead, bCrouched, bCanStand, bTouchesFlagpole;
	std::pair<bool, const Pipe*> EnteringPipe;
private:

	glm::vec2 m_Pos;
	glm::vec2 m_Speed;
	glm::vec2 m_PrevSpeed;
	glm::vec2 m_Acceleration;
	bool bFacingRight, bIsVulnerable;
	PlayerState m_PlayerState;
	std::vector<FireBall> m_FireBalls;
	Timer m_VulnerabilityTimer, m_FireBallTimer;
};