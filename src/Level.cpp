#include "Level.h"
#include "LevelScene.h"
#include "Renderer.h"

Level::Level()
	:m_ScreenPos(0.0f, 0.0f), m_LevelWidth(0), m_LevelHeight(0), m_OriginalScreenPos({0.0f, 0.0f}),
	m_WidthInBlocks(0), m_HeightInBlocks(0), m_DeathScene(nullptr), m_FlagpoleScene(nullptr),
	m_EnterPipeScene(nullptr)
{
}

Level::Level(const std::string& levelmap, uint32_t width, uint32_t height, glm::vec2 camera_offset,
	uint32_t widthInBlocks, uint32_t heightInBlocks, const pipe_func& func)
	: Level()
{
	LoadLevel(levelmap, width, height, camera_offset, widthInBlocks, heightInBlocks, func);
}

Level::~Level()
{
	DestroyAnimations();
}

std::string Level::LoadStringInputFromFile(const std::string& str)
{
	std::ifstream file(str);
	if (!file.is_open())
		return "";

	std::string line;
	std::string ret;
	while (std::getline(file, line))
	{
		ret += line;
	}

	return ret;
}

void Level::LoadLevel(const std::string& levelmap, uint32_t width, uint32_t height, glm::vec2 camera_offset,
	uint32_t widthInBlocks, uint32_t heightInBlocks, const pipe_func& func)
{
	m_Level = levelmap;
	m_LevelWidth = width;
	m_LevelHeight = height;
	m_ScreenPos = camera_offset;
	m_OriginalScreenPos = camera_offset;
	m_WidthInBlocks = widthInBlocks;
	m_HeightInBlocks = heightInBlocks;
	m_PipeBindingFunc = func;
	
	m_Tiles.clear();
	m_Enemies.clear();
	m_Powerups.clear();
	m_Pipes.clear();

	for (uint32_t y = 0; y < height; y++)
	{
		for (uint32_t x = 0; x < width; x++)
		{
			glm::vec2 pos((float)x, (float)y);
			switch (levelmap[y * width + x])
			{
			case '.':
				break;
			case '#':
				m_Tiles.emplace_back(TypeOfTile::SOLID, pos);
				break;
			case '$':
				m_Tiles.emplace_back(TypeOfTile::COIN, pos);
				break;
			case '?':
				m_Tiles.emplace_back(TypeOfTile::QUESTION_MARK_EMPTY, pos);
				break;
			case 'm':
				m_Tiles.emplace_back(TypeOfTile::QUESTION_MARK_MUSHROOM, pos);
				break;
			case 'f':
				m_Tiles.emplace_back(TypeOfTile::QUESTION_MARK_FIRE_FLOWER, pos);
				break;
			case 'F':
				m_Flagpole.SetPosition(pos);
				m_Tiles.emplace_back(TypeOfTile::STONE, pos);//To make the flagpole base solid
				break;
			case 'b':
				m_Tiles.emplace_back(TypeOfTile::BRICK, pos);
				break;
			case 's':
				m_Tiles.emplace_back(TypeOfTile::STONE, pos);
				break;
			case 'i':
				m_Tiles.emplace_back(TypeOfTile::INVISIBLE, pos);
				break;
			case 'G':
				m_Enemies.emplace_back(TypeOfEnemy::GOOMBA, pos, "assets/textures/goomba.png");
				break;
			case 'K':
				m_Enemies.emplace_back(TypeOfEnemy::KOOPA, pos, "assets/textures/koopa.png");
				break;
			case 'P':
			case 'p':
				//When loading a pipe obj, the character right next to the p letter is a number
				//between 1 to 9, which tells the height of the pipe's body
				//Ans from the level files this value is guaranteed to be on the same line
				int8_t pipe_height = levelmap[y * width + x + 1] - '0';
				if (pipe_height < 1 || pipe_height > 9)
					pipe_height = 1;

				m_Pipes.emplace_back(pos, pipe_height);
				m_Pipes.back().bEnterable = (levelmap[y * width + x] == 'P') ? true : false;

				//Creating pipe's solid body
				for (int8_t i = pipe_height; i >= 0; i--)
				{
					m_Tiles.emplace_back(TypeOfTile::INVISIBLE, glm::vec2(pos.x, pos.y - i));
					m_Tiles.emplace_back(TypeOfTile::INVISIBLE, glm::vec2(pos.x + 1, pos.y - i));
				}
				break;
			}
		}
	}

	//Custom pipe connection
	m_PipeBindingFunc(m_Pipes);
}

void Level::DrawLevel(Shader& scene_shader, Shader& overlay_shader, const std::vector<Texture>& textures)
{
	Renderer::SetTextureBoundary({ 0.5f, 1.0f });
	for (auto& t : m_Powerups)
	{
		if (!t.bExpired && t.pos.x + 0.5f > m_ScreenPos.x&& t.pos.x - 0.5f < m_ScreenPos.x + m_WidthInBlocks
			&& t.pos.y + 0.5f > m_ScreenPos.y&& t.pos.y - 0.5f < m_ScreenPos.y + m_HeightInBlocks)
			t.Draw(scene_shader, textures[5]);
	}

	//This is done because a single block is defined within a sixth of the whole tiles.png texture width
	Renderer::SetTextureBoundary({ 1.0f / 6.0f, 1.0f });
	for (auto& t : m_Tiles)
	{
		if (!t.bExpired && t.Position().x + 0.5f > m_ScreenPos.x&& t.Position().x - 0.5f < m_ScreenPos.x + m_WidthInBlocks
			&& t.Position().y + 0.5f > m_ScreenPos.y&& t.Position().y - 0.5f < m_ScreenPos.y + m_HeightInBlocks)
			t.Draw(scene_shader, textures[0]);
	}

	for (auto& t : m_Enemies)
	{
		if (t.pos.x + 0.5f > m_ScreenPos.x&& t.pos.x - 0.5f < m_ScreenPos.x + m_WidthInBlocks
			&& t.pos.y + 0.5f > m_ScreenPos.y&& t.pos.y - 0.5f < m_ScreenPos.y + m_HeightInBlocks)
		{
			if (t.Type() == TypeOfEnemy::GOOMBA)
			{
				Renderer::SetTextureBoundary({ 0.33f,1.0f });
				t.Draw(scene_shader, textures[7]);
			}
			else
			{
				Renderer::SetTextureBoundary({ 0.2f, 1.0f });
				t.Draw(scene_shader, textures[8]);
			}
		}
	}

	//Flagpole
	Renderer::SetTextureBoundary({ 1.0f, 1.0f });
	m_Flagpole.Draw(scene_shader, textures[10]);

	//Pipes
	Renderer::SetTextureBoundary({ 1.0f, 0.5f });
	for (auto& pipe : m_Pipes)
	{
		pipe.Draw(scene_shader, textures[11]);
	}

	//Coin counter
	Renderer::SetTextureBoundary({ 0.1f, 1.0f });
	m_CoinCounter.Draw(overlay_shader, textures[4]);
}

void Level::LevelLogic(Player& p, Camera& c, float fElapsedTime, const Window* wnd)
{
	//Checking for scenes to be played
	if (HandleAnimations(p, c))
		return;

	float fPlayerHeight = p.GetHeight();

	//PLAYER COLLISION DETECTION

	static auto MarioStandUpCollision = [&]()
	{
		//Determines whether mario is sandwiched between two blocks above and below
		uint8_t bBlockSandwich = 0;
		for (auto iter = m_Tiles.begin(); iter != m_Tiles.end(); ++iter)
		{
			Tile& t = *iter;
			if (glm::length(p.Position() - t.Position()) > 2.0f) continue;

			if (t.Type() == TypeOfTile::COIN || t.bExpired) continue;

			//if player is crouched we discard the tile
			if (p.GetPlayerState() != PlayerState::MARIO &&
				((glm::ivec2)p.Position() == glm::ivec2(t.Position().x, t.Position().y + 1.0f) ||
				(glm::ivec2)p.Position() == glm::ivec2(t.Position().x - 1.0f, t.Position().y + 1.0f) ||
					(glm::ivec2)p.Position() == glm::ivec2(t.Position().x + 1.0f, t.Position().y + 1.0f)))
			{
				bBlockSandwich |= 0x01;
			}

			if (p.GetPlayerState() != PlayerState::MARIO &&
				((glm::ivec2)p.Position() == glm::ivec2(t.Position().x, t.Position().y - 1.0f) ||
				(glm::ivec2)p.Position() == glm::ivec2(t.Position().x - 1.0f, t.Position().y - 1.0f) ||
					(glm::ivec2)p.Position() == glm::ivec2(t.Position().x + 1.0f, t.Position().y - 1.0f)))
			{
				bBlockSandwich |= 0x02;
			}
		}

		if (bBlockSandwich == 0x03)
		{
			p.bCrouched = true;
			p.bCanStand = false;
		}
		else
		{
			p.bCanStand = true;
		}
	};

	if (p.bCrouched)
	{
		MarioStandUpCollision();
	}

	for (auto iter = m_Tiles.begin(); iter != m_Tiles.end(); ++iter)
	{
		Tile& t = *iter;
		bool bExit = false;
		//Dont bother calculating collisions with distant tiles
		if (t.bExpired) continue;

		if (t.IsPopup())
		{
			t.UpdateIfTemporary(0.05f);
			continue;
		}

		//Top-left coords
		glm::vec2 playerpos = p.Position() - glm::vec2(0.5f, 0.5f);
		glm::vec2 tilepos = t.Position() - glm::vec2(0.5f, 0.5f);

		//the +- 0.05f is a small adjustment which allows the player to fall into tight pits more easily
		float fAdj = 0.05f;
		if (playerpos.x - fAdj > tilepos.x - 1.0f && playerpos.x + fAdj < tilepos.x + 1.0f &&
			playerpos.y - fAdj > tilepos.y - fPlayerHeight && playerpos.y + fAdj < tilepos.y + 1.0f)
		{
			glm::vec2& sp = p.Speed();
			glm::vec2& pos = p.Position();

			if (t.Type() != TypeOfTile::COIN)
			{
				EntityBlockCollision(pos, sp, t.Position(), fPlayerHeight, fElapsedTime);
			}
				

			switch (t.Type())
			{
			case TypeOfTile::SOLID:
				
				break;

			case TypeOfTile::COIN:
				t.bExpired = true;
				m_CoinCounter.Increase();
				break;

			case TypeOfTile::QUESTION_MARK_EMPTY:

				//Checking for mario's fist to hit the block
				if (pos.y > tilepos.y + 0.9f && pos.x > t.Position().x - 0.9f && 
					pos.x < t.Position().x + 0.9f && t.fTextureOffset == 2.0f / 6.0f)
				{
					t.fTextureOffset += 1.0f / 6.0f;
					m_CoinCounter.Increase();
					m_Tiles.emplace_back(TypeOfTile::COIN, tilepos);
					m_Tiles.back().SetIfPopup(true);
					bExit = true;
				}
				break;

			case TypeOfTile::QUESTION_MARK_MUSHROOM:
				if (pos.y > tilepos.y + 0.9f && pos.x > t.Position().x - 0.9f &&
					pos.x < t.Position().x + 0.9f && t.fTextureOffset == 2.0f / 6.0f)
				{
					t.fTextureOffset += 1.0f / 6.0f;
					m_Powerups.emplace_back(PowerUpType::MUSHROOM, t.Position());
					bExit = true;
				}
				break;

			case TypeOfTile::QUESTION_MARK_FIRE_FLOWER:
				if (pos.y > tilepos.y + 0.9f && pos.x > t.Position().x - 0.9f &&
					pos.x < t.Position().x + 0.9f && t.fTextureOffset == 2.0f / 6.0f)
				{
					t.fTextureOffset += 1.0f / 6.0f;
					m_Powerups.emplace_back(PowerUpType::FIRE_FLOWER, t.Position());
					bExit = true;
				}
				break;

			case TypeOfTile::BRICK:
				if (p.Speed().y == 0.0f && t.Position().y < p.Position().y)
				{
					t.bExpired = true;
					bExit = true;
				}
				break;
			}
		}

		if (bExit) break;
	}

	//ENEMY COLLSION DETECTION

	for (auto iter = m_Enemies.begin(); iter != m_Enemies.end(); ++iter)
	{
		Enemy& e = *iter;

		//We dont update off-screen enemies
		if (e.bExpired || e.pos.x + 0.5f < m_ScreenPos.x || e.pos.x - 0.5f > m_ScreenPos.x + m_WidthInBlocks
			|| e.pos.y < m_ScreenPos.y)
			continue;
		

		e.Update(fElapsedTime);

		//If the enemy does not get stomped, we dont care about the enemy collisions
		//with tiles or players
		if (e.GetDeathType() != TypeOfEnemyDeath::STOMP)
			continue;

		//Collision with other enemies
		for (auto iter2 = m_Enemies.begin(); iter2 != m_Enemies.end(); ++iter2)
		{
			Enemy& e2 = *iter2;
			float dist = glm::distance(e.pos, e2.pos);

			if (iter != iter2 && !e2.bExpiring && !e.bExpiring && dist < 1.0f)
			{
				if (dist < 0.3f)
				{
					e.bForward = !e2.bForward;
				}

				if ((e.pos.x > e2.pos.x && !e.bForward) ||
					(e.pos.x < e2.pos.x && e.bForward))
				{
					e.bForward = !e.bForward;
					e2.bForward = !e2.bForward;
				}
			}
		}

		//Collision with tiles
		for (auto iter2 = m_Tiles.begin(); iter2 != m_Tiles.end(); ++iter2)
		{
			Tile& t = *iter2;
			if (glm::length(t.Position() - e.pos) > glm::sqrt(2.0f) || t.Type() == TypeOfTile::COIN ||
				t.bExpired) continue;

			glm::vec2 tilepos = t.Position() - glm::vec2(0.5f, 0.5f);
			glm::vec2 enemypos = e.pos - glm::vec2(0.5f, 0.5f);

			//Collision with blocks while falling
			if (enemypos.x + 1.0f > tilepos.x&& enemypos.x < tilepos.x + 1.0f &&
				enemypos.y + 1.0f > tilepos.y&& enemypos.y < tilepos.y + 1.0f)
			{
				EntityBlockCollision(e.pos, e.speed, t.Position(), 1.0f, fElapsedTime);
				e.speed.x = (e.bForward) ? 0.05f : -0.05f;
				enemypos = e.pos - glm::vec2(0.5f, 0.5f);
			}
			//Enemy hitting a wall
			if (tilepos.y > enemypos.y - 0.1f && tilepos.y < enemypos.y + 0.1f)
			{
				e.speed.x += (e.bForward) ? -0.1f : 0.1f;
				e.bForward = !e.bForward;
			}
		}
		
		//Collision with player
		//If the player hits the enemy from above, it initiates the expiring phase
		//and the player gets a little boost in the y axis
		//Obv we do not consider enemies which are too far from the player location
		if (glm::length(p.Position() - e.pos) <= sqrt(2.0f))
		{
			float fPlayerGroundOffset = fPlayerHeight - 1.0f;

			const glm::vec2& pos = p.Position();
			const glm::vec2& sp = p.Speed();
			if (p.CanBeDamaged())
			{
				if (!e.bExpiring && pos.x > e.pos.x - 0.5f && pos.x < e.pos.x + 0.5f &&
					pos.y > e.pos.y - 0.5f - fPlayerGroundOffset
					&& pos.y < e.pos.y + 0.5f - fPlayerGroundOffset)
				{
					if (p.bIsJumping && pos.x > e.pos.x - 0.5f && pos.x < e.pos.x + 0.5f &&
						pos.y - fPlayerGroundOffset < e.pos.y - 0.1f)
					{
						p.Speed().y = -0.3f;
						e.SetDeathType(TypeOfEnemyDeath::STOMP);
						e.bExpiring = true;
					}
					else
					{
						p.SetVulnerability(false);
						if (p.GetPlayerState() != PlayerState::MARIO)
						{
							p.SetPlayerState(PlayerState::MARIO);
							p.bCrouched = false;
						}
						else
						{
							//Implement death
							p.bDead = true;
						}
					}
				}
			}
		}

	}

	//POWERUPS
	for (auto iter = m_Powerups.begin(); iter != m_Powerups.end(); ++iter)
	{
		Powerup& pw = *iter;
		if (pw.bExpired) continue;

		if (pw.IsSpawning())
		{
			pw.pos.y -= 0.01f;
			continue;
		}
		
		pw.Update();
		//Cause the mushroom is the only powerup which can move for now
		if (pw.speed == glm::vec2(0.0f, 0.0f) && pw.Type() == PowerUpType::MUSHROOM)
		{
			pw.speed.x = 0.5f;
		}

		//Checking collision with player
		glm::vec2& plpos = p.Position();
		if (plpos.x + 1.0f > pw.pos.x&& plpos.x < pw.pos.x + 1.0f &&
			plpos.y + fPlayerHeight > pw.pos.y&& plpos.y < pw.pos.y + 1.0f)
		{
			pw.bExpired = true;
			if (pw.Type() == PowerUpType::MUSHROOM && p.GetPlayerState() == PlayerState::MARIO)
			{
				p.SetPlayerState(PlayerState::SUPER_MARIO);
				MarioStandUpCollision();
			}
			if (pw.Type() == PowerUpType::FIRE_FLOWER)
			{
				p.SetPlayerState(PlayerState::FIRE_MARIO);
				MarioStandUpCollision();
			}
		}

		//POWERUP against tiles in case of moving powerup
		if (pw.Type() == PowerUpType::MUSHROOM)
		{
			for (auto iter2 = m_Tiles.begin(); iter2 != m_Tiles.end(); ++iter2)
			{
				Tile& t = *iter2;
				if (glm::length(t.Position() - pw.pos) > sqrt(2.0f) || t.Type() == TypeOfTile::COIN
					|| t.bExpired) continue;

				glm::vec2 tilepos = t.Position() - glm::vec2(0.5f, 0.5f);
				glm::vec2 poweruppos = pw.pos - glm::vec2(0.5f, 0.5f);

				//Collision with blocks while falling
				if (poweruppos.x + 1.0f > tilepos.x&& poweruppos.x < tilepos.x + 1.0f &&
					poweruppos.y + 1.0f > tilepos.y&& poweruppos.y < tilepos.y + 1.0f)
				{
					EntityBlockCollision(pw.pos, pw.speed, t.Position(), 1.0f, fElapsedTime);
					pw.speed.x = (pw.bForward) ? -0.05f : 0.05f;
					poweruppos = pw.pos - glm::vec2(0.5f, 0.5f);
				}
				if (tilepos.y > poweruppos.y - 0.1f && tilepos.y < poweruppos.y + 0.1f)
				{
					pw.speed.x += (pw.bForward) ? -0.1f : 0.1f;
					pw.bForward = !pw.bForward;
				}
			}

			pw.pos += pw.speed;
		}
	}

	//Check pipe entrance
	for (const auto& pipe : m_Pipes)
	{
		if (pipe.CheckEntrance(p, wnd->IsKeyboardEvent(InputEvent(GLFW_KEY_S, GLFW_PRESS))))
			p.EnteringPipe = std::make_pair(true, &pipe);
	}

	//Flagpole collision
	m_Flagpole.PoleCollision(p);
}

void Level::ScrollScreen(Player& p, Camera& c, float fPlayerSpeed)
{
	if (m_ScreenPos.x <= m_LevelWidth - m_WidthInBlocks && p.Position().x > m_ScreenPos.x + m_WidthInBlocks * 0.7f)
	{
		c.MoveX(-fPlayerSpeed);
		m_ScreenPos.x += fPlayerSpeed;
	}
	if (m_ScreenPos.x > 0.1f && p.Position().x < m_ScreenPos.x + m_WidthInBlocks * 0.3f)
	{
		c.MoveX(fPlayerSpeed);
		m_ScreenPos.x -= fPlayerSpeed;
	}

	if (m_ScreenPos.y <= m_LevelHeight - m_HeightInBlocks - 0.5f &&
		p.Position().y > m_ScreenPos.y + m_HeightInBlocks * 0.7f)
	{
		c.MoveY(-p.Speed().y);
		m_ScreenPos.y += p.Speed().y;
	}
	if (m_ScreenPos.y > 0.0f && p.Position().y < m_ScreenPos.y + m_HeightInBlocks * 0.2f)
	{
		c.MoveY(fPlayerSpeed);
		m_ScreenPos.y -= fPlayerSpeed;

		if (m_ScreenPos.y < 0.0f) // Clamping camera movement
		{
			c.MoveY(m_ScreenPos.y);
			m_ScreenPos.y = 0.0f;
		}
	}
}

void Level::ResetCurrentLevel(Player& p, Camera& c)
{
	p.Position() = { 3.0f, 18.0f };
	c.ResetPosition();
	c.MoveY(-m_OriginalScreenPos.y);
	m_CoinCounter.Reset();

	LoadLevel(m_Level, m_LevelWidth, m_LevelHeight, m_OriginalScreenPos,
		m_WidthInBlocks, m_HeightInBlocks, m_PipeBindingFunc);
}

glm::ivec2 Level::GetLevelSize() const
{
	return glm::ivec2(m_LevelWidth, m_LevelHeight);
}

void Level::EntityBlockCollision(glm::vec2& pos, glm::vec2& sp, const glm::vec2& tilepos,
	float fEntityTileHeight,float fElapsedTime)
{
	glm::vec2 playerpos = (pos + sp * fElapsedTime) - glm::vec2(0.5f, 0.5f);
	glm::vec2 tp = tilepos - glm::vec2(0.5f, 0.5f);

	//Diagonal movements (DOWN-RIGHT)
	if (sp.x >= 0.0f && sp.y >= 0.0f)
	{
		if (playerpos.x - tp.x < (playerpos.y - tp.y) / fEntityTileHeight)
		{
			pos.x = tilepos.x - 1.0f;
		}
		else { pos.y = tilepos.y - (fEntityTileHeight - 0.05f); sp.y = 0.0f; }
	}
	//DOWN-LEFT
	else if (sp.x < 0.0f && sp.y >= 0.0f)
	{
		if (playerpos.x - tp.x > (tp.y - playerpos.y) / fEntityTileHeight)
		{
			pos.x = tilepos.x + 1.0f;
		}
		else 
		{ 
			pos.y = tilepos.y - (fEntityTileHeight - 0.05f); sp.y = 0.0f;
		}
	}
	//UP-RIGHT
	else if (sp.x >= 0.0f && sp.y < 0.0f)
	{
		//TEMP 0.1f
		if (tp.x - playerpos.x + 0.1f < playerpos.y - tp.y - 0.01f)
		{
			pos.y = tilepos.y + 1.01f; sp.y = 0.0f;
		}
		else pos.x = tilepos.x - 1.0f;
	}
	//UP-LEFT
	else if (sp.x < 0.0f && sp.y < 0.0f)
	{
		//TEMP 0.05f
		if (playerpos.y - tp.y - 0.05f < playerpos.x - tp.x)
		{
			pos.x = tilepos.x + 1.0f;
		}
		else if (playerpos.y - tp.y > playerpos.x - tp.x + 0.01f)
		{
			pos.y = tilepos.y + 1.01f; sp.y = 0.0f;
		}
	}
}

bool Level::HandleAnimations(Player& p, Camera& c)
{
	if (p.EnteringPipe.first)
	{
		if (!m_EnterPipeScene)
		{
			m_EnterPipeScene = new PipeDownScene(p);
			p.Speed().x = 0.0f;
			if (p.bCrouched)
			{
				p.bCrouched = false;
				p.Position().y -= 1.0f;
			}
		}
		

		if (!m_EnterPipeScene->PlayScene()) // Scene is over
		{
			//Moving mario towards exit pipe
			const Pipe* connection = p.EnteringPipe.second->Connection();
			p.Position().x = connection->Position().x + 1.0f;
			p.Position().y = connection->Position().y - connection->BodyHeight() - p.GetHeight() + 3.0f;

			//Translating camera position
			m_ScreenPos.x = (connection->Position().x - m_WidthInBlocks / 2);
			m_ScreenPos.y = -c.GetPosition().y;
			c.ResetPosition();
			c.MoveX(-m_ScreenPos.x);
			c.MoveY(-m_ScreenPos.y);

			delete m_EnterPipeScene;
			m_EnterPipeScene = nullptr;
			p.EnteringPipe.first = false;
			p.EnteringPipe.second = nullptr;

			m_ExitPipeScene = new PipeUpScene(p);
		}
		else
			return true;
	}
	if (p.bDead)
	{
		if (!m_DeathScene) m_DeathScene = new DeathScene(p);

		if (!m_DeathScene->PlayScene()) // Scene is over
		{
			ResetCurrentLevel(p, c);
			delete m_DeathScene;
			m_DeathScene = nullptr;
			p.bDead = false;
		}
		else
			return true;
	}
	if (p.bTouchesFlagpole)
	{
		if (!m_FlagpoleScene) m_FlagpoleScene = new FlagPoleScene(p, m_Flagpole);

		if (!m_FlagpoleScene->PlayScene()) // Scene is over
		{
			delete m_FlagpoleScene;
			m_FlagpoleScene = nullptr;
			p.bTouchesFlagpole = false;
		}
	}

	if (m_ExitPipeScene)
	{
		if (!m_ExitPipeScene->PlayScene())
		{
			delete m_ExitPipeScene;
			m_ExitPipeScene = nullptr;
		}
		else
		{
			return true;
		}
	}

	return false;
}

void Level::DestroyAnimations()
{
	delete m_DeathScene;
	delete m_FlagpoleScene;
	m_DeathScene = nullptr;
	m_FlagpoleScene = nullptr;
}

bool Level::IsTile(const glm::vec2& position) const
{
	for (const Tile& tile : m_Tiles)
	{
		if (tile.Type() != TypeOfTile::COIN && tile.Position() == position)
		{
			return true;
		}
	}

	return false;
}
