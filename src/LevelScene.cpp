#include <chrono>
#include "LevelScene.h"

LevelScene::LevelScene(Player& ref)
	:m_PlayerRef(&ref)
{
}

LevelScene::~LevelScene()
{
}

DeathScene::DeathScene(Player& ref)
	:LevelScene(ref), m_bExpiring(false)
{
}

bool DeathScene::PlayScene()
{
	//Simulating mario's death according to the NES fashion
	if (m_Timer.HavePassedAtLeast(1.0f))
	{
		if (!m_bExpiring)
		{
			m_bExpiring = true;
			m_PlayerRef->Speed().x = 0.0f;
			m_PlayerRef->Speed().y = -0.4f;
		}
		else
		{
			if (!m_Timer.HavePassedAtLeast(5.0f))
			{
				m_PlayerRef->Speed() += m_PlayerRef->Acceleration();
				m_PlayerRef->Position() += m_PlayerRef->Speed();
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}

FlagPoleScene::FlagPoleScene(Player& player_ref, FlagPole& flag_ref)
	:LevelScene(player_ref), m_FlagPoleRef(&flag_ref)
{
}

bool FlagPoleScene::PlayScene()
{
	if (m_Timer.state == 0)
	{
		if (m_PlayerRef->Position().y < m_FlagPoleRef->Position().y - m_PlayerRef->GetHeight() + 0.5f)
		{
			if (std::abs(m_PlayerRef->Position().x - m_FlagPoleRef->Position().x) < 0.5f)
				m_PlayerRef->Position().x = m_FlagPoleRef->Position().x;
			else 
				m_PlayerRef->Position().x = m_FlagPoleRef->Position().x + 1.0f;

			m_PlayerRef->Position().y += 0.1f;
			return true;
		}
		else
		{
			m_Timer.SetCurrentTimePointAsBegin();
			m_Timer.state = 1;
			m_PlayerRef->bIsJumping = false;

			//If mario is on the left side of the flag, we make him go to the other side
			if(m_PlayerRef->Position().x == m_FlagPoleRef->Position().x)
				m_PlayerRef->Position().x += 1.0f;
		}
	}
	if (m_Timer.state == 1)
	{
		if (!m_Timer.HavePassedAtLeast(1.0f))
		{
			return true;
		}
		else
		{
			m_Timer.SetCurrentTimePointAsBegin();
			m_PlayerRef->Speed().x = 0.1f;
			m_PlayerRef->Speed().y = 0.0f;
			m_Timer.state = 2;
		}
	}
	if(m_Timer.state == 2)
	{
		if (!m_Timer.HavePassedAtLeast(2.0f))
		{
			m_PlayerRef->Speed() += glm::vec2(0.0f, 0.05f);
			if (m_PlayerRef->Speed().y > 1.0f) m_PlayerRef->Speed().y = 1.0f;
			m_PlayerRef->Position() += m_PlayerRef->Speed();
			return true;
		}
	}

	return false;
}

PipeDownScene::PipeDownScene(Player& ref)
	:LevelScene(ref)
{
}

bool PipeDownScene::PlayScene()
{
	//Time is proportional to the player height
	while (!m_Timer.HavePassedAtLeast(m_PlayerRef->GetHeight()))
	{
		m_PlayerRef->Speed().y = 0.02f;
		m_PlayerRef->Position().y += m_PlayerRef->Speed().y;
		m_PlayerRef->bIsJumping = false;
		return true;
	}

	return false;
}

PipeUpScene::PipeUpScene(Player& ref)
	:LevelScene(ref)
{
}

bool PipeUpScene::PlayScene()
{
	constexpr float fSpeed = -0.04f;

	//Time is proportional to the player height
	while (!m_Timer.HavePassedAtLeast(1.0f))
	{
		m_PlayerRef->Position().y += fSpeed;
		m_PlayerRef->bIsJumping = false;
		return true;
	}

	return false;
}
