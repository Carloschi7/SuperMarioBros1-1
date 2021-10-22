#include "Player.h"
#include "Renderer.h"

FireBall::FireBall(const glm::vec2& pos, const glm::vec2& speed) :
	m_Pos(pos), m_Speed(speed)
{
}

void FireBall::Draw(Shader& shd, const Texture& tex)
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(m_Pos, 1.0f));

	//Simulating fireball rotation
	float fElapsedTime = m_RotationTimer.ElapsedTime();
	if (fElapsedTime >= 0.1f && fElapsedTime < 0.2f)
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	else if(fElapsedTime >= 0.2f && fElapsedTime < 0.3f)
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	else if (fElapsedTime >= 0.3f && fElapsedTime < 0.4f)
		model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));

	if (fElapsedTime >= 0.4f)
		m_RotationTimer.SetCurrentTimePointAsBegin();

	Renderer::DrawRect(shd, tex, model, { 1.0f, 1.0f });
}

bool FireBall::Update(const std::vector<Tile>& tiles, std::vector<Enemy>& enemies)
{
	for (const auto& tile : tiles)
	{
		float dist = glm::distance(tile.Position(), m_Pos);

		if (tile.Type() == TypeOfTile::COIN ||
			dist > glm::sqrt(2.0f) ||
			tile.bExpired)
			continue;

		//Computing the actual distance between the block and the fireball
		constexpr float h = 0.5f;
		float d = glm::min(glm::abs(tile.Position().x - m_Pos.x),
			glm::abs(tile.Position().y - m_Pos.y));

		float r = glm::sqrt(glm::pow(h, 2) + glm::pow(d, 2));

		//Center of the fireball has hit a block
		if (dist <= r)
		{
			//To ease fireball bouncing on the edge of tiles
			constexpr float fCorrection = 0.06f;
			
			if (glm::abs(tile.Position().x - m_Pos.x) <=
				glm::abs(tile.Position().y - m_Pos.y) + fCorrection &&
				m_Speed.y > 0.0f)
			{
				m_Speed.y = -1.0f;
				break;
			}
			else
			{
				return false;
			}
		}
	}

	for (auto iter = enemies.begin(); iter != enemies.end(); ++iter)
	{
		const glm::vec2& pos = iter->pos;
		if (m_Pos.x > pos.x - 0.5f && m_Pos.x < pos.x + 0.5f &&
			m_Pos.y > pos.y - 0.5f && m_Pos.y < pos.y + 0.5f)
		{
			if (iter->bExpiring || iter->bExpired)
				continue;

			iter->bExpiring = true;
			iter->SetDeathType(
				m_Speed.x > 0.0f ? 
				TypeOfEnemyDeath::FIREBALL_HIT_FROM_LEFT :
				TypeOfEnemyDeath::FIREBALL_HIT_FROM_RIGHT);
			return false;
		}
	}

	//Fireball gravity
	m_Speed.y += 0.1f;
	
	if (m_Speed.y > 1.0f) 
		m_Speed.y = 1.0f;

	m_Pos += m_Speed * 0.1f;
	auto tp = std::chrono::steady_clock::now();
	//2s lifespan
	return !m_Timer.HavePassedAtLeast(2.0f);
}

Player::Player()
	:Player(glm::vec2(0.0f))
{
}

Player::Player(const glm::vec2& pos)
	: m_Pos(pos + glm::vec2(0.5f, 0.5f)),
	m_Speed(0.0f, 0.0f),
	m_Acceleration(0.0f, 0.02f),
	bIsJumping(false),
	bDead(false),
	bCrouched(false),
	bCanStand(true),
	bIsVulnerable(true),
	m_PrevSpeed(0.0f, 0.0f),
	bFacingRight(true),
	bTouchesFlagpole(false),
	m_PlayerState(PlayerState::MARIO)
{
	EnteringPipe.first = false;
	EnteringPipe.second = nullptr;
}

Player::Player(Player&& p) noexcept :
	m_Pos(p.m_Pos),
	m_Speed(p.m_Speed),
	bFacingRight(p.bFacingRight),
	bIsJumping(p.bIsJumping),
	bDead(p.bDead),
	bCrouched(p.bCrouched),
	bCanStand(p.bCanStand),
	bIsVulnerable(p.bIsVulnerable),
	bTouchesFlagpole(p.bTouchesFlagpole),
	EnteringPipe(p.EnteringPipe),
	m_FireBalls(std::move(p.m_FireBalls)),
	m_Acceleration(p.m_Acceleration),
	m_PrevSpeed(p.m_PrevSpeed),
	m_VulnerabilityTimer(p.m_VulnerabilityTimer),
	m_FireBallTimer(p.m_FireBallTimer),
	m_PlayerState(p.m_PlayerState)
{
}

Player::~Player()
{
}

void Player::Update(Window& wnd, const std::vector<Tile>& tiles,
	std::vector<Enemy>& enemies,
	float fSpeed, float down, float right)
{
	glm::vec2 dir(0.0f, 0.0f);

	if (wnd.IsKeyboardEvent({ GLFW_KEY_A, GLFW_PRESS }))
	{
		dir.x -= 1.0f * fSpeed;
		bFacingRight = false;
	}
	if (wnd.IsKeyboardEvent({ GLFW_KEY_D, GLFW_PRESS }))
	{
		dir.x += 1.0f * fSpeed;
		bFacingRight = true;
	}

	if (wnd.IsKeyboardEvent({ GLFW_KEY_S, GLFW_PRESS }) && m_PlayerState != PlayerState::MARIO)
	{
		if (!bCrouched)
		{
			bCrouched = true;
			m_Pos.y += 1.0f;
		}
	}
	else if(bCanStand && bCrouched && !bIsJumping)
	{
		m_Pos.y -= 1.0f;
		bCrouched = false;
	}


	if (wnd.IsKeyboardEvent({ GLFW_KEY_SPACE, GLFW_PRESS }) && !bIsJumping)
	{
		dir.y += -4.5f * fSpeed;
		bIsJumping = true;
	}

	//Spawn fireballs if fire mario
	if (wnd.IsKeyboardEvent({ GLFW_KEY_E, GLFW_PRESS }) && 
		m_PlayerState == PlayerState::FIRE_MARIO &&
		m_FireBalls.size() < 2)
	{
		//A little delay between fireball spawning
		if (m_FireBallTimer.HavePassedAtLeast(0.2f))
		{
			m_FireBalls.emplace_back(
				m_Pos + glm::vec2(bFacingRight ? 0.5f : -0.5f, 0.5f),
				glm::vec2(bFacingRight ? 2.0f : -2.0f, 1.0f));

			m_FireBallTimer.SetCurrentTimePointAsBegin();
		}
	}

	for (auto iter = m_FireBalls.begin(); iter != m_FireBalls.end(); ++iter)
	{
		FireBall& ball = *iter;
		if (!ball.Update(tiles, enemies))
		{
			m_FireBalls.erase(iter);
			break;
		}
	}
	

	m_Speed += dir;
	if (fabs(m_Speed.x) < 0.01f) m_Speed.x = 0.0f;
	if (m_Speed.x > 0.1f) m_Speed.x = 0.1f;
	if (m_Speed.x < -0.1f) m_Speed.x = -0.1f;
	if (m_Speed.y > 0.4f) m_Speed.y = 0.4f;
	if (m_Speed.y < -4.5f) m_Speed.y = -4.5f;
	m_Pos += m_Speed;
	m_Speed.x *= 0.7f;


	//If the player lands on the ground, he regains the ability to jump
	if (m_PrevSpeed.y > 0.0f && m_Speed.y == 0.0f) bIsJumping = false;

	m_Speed += m_Acceleration;
	//Not allowing the player to perform a jump when falling
	if (m_PrevSpeed.y > 0.05f && m_Speed.y > 0.05f && m_Speed.y > m_PrevSpeed.y) bIsJumping = true;
	//Setting prev speed
	m_PrevSpeed = m_Speed;

	glm::vec2 to_clamp = m_Pos - glm::vec2(0.5f, 0.5f);

	if (to_clamp.x < 0.0f) m_Pos.x = 0.5f;
	if (to_clamp.y < 0.0f) m_Pos.y = 0.5f;
	if (to_clamp.x + 1.0f > right) m_Pos.x = right - 0.5f;
	if (to_clamp.y + 1.0f > down)
	{
		bDead = true;
	}
}

void Player::Draw(Shader& shd, const Texture& player_texture, const Texture& fire_ball_texture)
{
	Renderer::SetTextureBoundary({ 0.2f, 1.0f });
	//Sprite offseting
	float fTextureOffset = 0.0f;

	if (bDead || bCrouched) fTextureOffset = 0.8f;
	else if (!bFacingRight && bIsJumping) fTextureOffset = 0.6f;
	else if (!bFacingRight) fTextureOffset = 0.4f;
	else if (bIsJumping) fTextureOffset = 0.2f;

	
	glm::mat4 model(1.0f);

	switch (m_PlayerState)
	{
	case PlayerState::MARIO:
		model = glm::translate(model, glm::vec3(m_Pos, 0.0f));
		break;
	case PlayerState::SUPER_MARIO:
	case PlayerState::FIRE_MARIO:
		model = glm::translate(model, glm::vec3(m_Pos.x, m_Pos.y + 0.5f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 2.0f, 1.0f));
		break;
	}
	Renderer::DrawRect(shd, player_texture, model, { fTextureOffset, 0.0f });

	//Draw fireballs
	shd.Uniform1f(0.0f, "offset");

	/*fire_ball_texture.Bind(0);
	shd.Uniform1i(0, "texture1");*/
	Renderer::SetTextureBoundary({ 1.0f, 1.0f });
	for (auto& ball : m_FireBalls)
	{
		ball.Draw(shd, fire_ball_texture);
	}
}

void Player::SetPlayerState(const PlayerState& p)
{
	m_PlayerState = p;
}

float Player::GetHeight()
{
	switch (m_PlayerState)
	{
	case PlayerState::MARIO:
		return 1.0f;
	case PlayerState::SUPER_MARIO:
	case PlayerState::FIRE_MARIO:
		if (!bCrouched)
			return 2.0f;
		else
			return 1.0f;
	}
}

glm::vec2& Player::Position()
{
	return m_Pos;
}

glm::vec2& Player::Speed()
{
	return m_Speed;
}

glm::vec2& Player::Acceleration()
{
	return m_Acceleration;
}

bool Player::CanBeDamaged()
{
	if (bIsVulnerable)
	{
		m_VulnerabilityTimer.SetCurrentTimePointAsBegin();
		return true;
	}
	else
	{
		if (m_VulnerabilityTimer.HavePassedAtLeast(2.0f))
		{
			bIsVulnerable = true;
			return true;
		}
		else
			return false;
	}
}

void Player::SetVulnerability(bool true_or_false)
{
	bIsVulnerable = true_or_false;
}


