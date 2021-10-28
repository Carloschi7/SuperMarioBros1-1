#include "../application/Application.h"
#include "Timer.h"
#include "Level.h"
#include "Player.h"
#include "Renderer.h"

class UserVars
{
public:
	friend class Application;
	static UserVars* GetInstance()
	{
		static UserVars instance;
		return &instance;
	}
private:
	UserVars() {}
	~UserVars() {}
	Player m_Player;
	Level m_Level;
};

Application::Application(const Window& window)
{
	m_Window = (Window*)::operator new(sizeof(Window));
	memcpy(m_Window, &window, sizeof(Window));
}

Application::~Application()
{
	m_VertexManagers.clear();
	m_SceneObjs.clear();
	m_Textures.clear();
	m_CubeMaps.clear();
	m_Shaders.clear();
	m_Models.clear();
	m_CustomFrameBuffers.clear();
	::operator delete (m_Window);
}


void Application::OnUserCreate()
{
	//Instance setup
	Player& player = UserVars::GetInstance()->m_Player;
	Level& level = UserVars::GetInstance()->m_Level;
	//Camera setup
	float fAspectRatio = (float)m_Window->Width() / (float)m_Window->Height();
	m_Camera.SetVectors(glm::vec3(0.0f), glm::vec3(0.0f));
	m_Camera.SetOrthographicValues(0.0f, 10.0f * fAspectRatio, 10.0f, 0.0f);

	//Shader instantiation
	m_Shaders.emplace_back("assets/shaders/basic_2D_texture.shader");
	m_Shaders[0].UniformMat4f(m_Camera.GetProjMatrix(), "proj");
	m_Shaders.emplace_back("assets/shaders/coin_counter.shader");

	m_Textures.emplace_back("assets/textures/tiles.png", false);
	m_Textures.emplace_back("assets/textures/mario.png", false);
	m_Textures.emplace_back("assets/textures/super_mario.png", false);
	m_Textures.emplace_back("assets/textures/fire_mario.png", false);
	m_Textures.emplace_back("assets/textures/numbers.png", true);
	m_Textures.emplace_back("assets/textures/powerups.png", false);
	m_Textures.emplace_back("assets/textures/fireball.png", false);
	m_Textures.emplace_back("assets/textures/goomba.png", false);
	m_Textures.emplace_back("assets/textures/koopa.png", false);
	m_Textures.emplace_back("assets/textures/background.png", false);
	m_Textures.emplace_back("assets/textures/flagpole_final.png", false);
	m_Textures.emplace_back("assets/textures/pipe.png", false);

	//Creating level layout
	glm::vec2 CameraOffset = { 0.0f, 10.0f };

	//Definitions
	uint32_t WidthInBlocks = 18;
	uint32_t HeightInBlocks = 10;

	player.Position() = { 3.0f, 18.0f };
	m_Camera -= CameraOffset;

	//Customized level warp settings with pipes
	PipesProperties props;
	props.SetConnectionFunc(
		[](std::vector<std::vector<Pipe>*> vec)
		{
			//Pipe 3 of area 0
			//Set to nullptr because the exit is not a pipe, simply a free spawn
			vec[0]->at(3).SetPipeConnection(nullptr);
			vec[0]->at(3).nWarpArea = 1;

			vec[1]->at(0).SetPipeConnection(&vec[0]->at(4));
			vec[1]->at(0).nWarpArea = 0;
		}
	);
	props.SetWarpAreaFunc(
		[](std::vector<std::vector<Pipe>*> vec, const Pipe* sel, Level& lvl, Player& p)
		{
			if (&vec[0]->at(3) == sel)
			{
				lvl.SwitchArea(vec[0]->at(3).nWarpArea);
				p.Position() = { 3.0f, 3.0f };
				lvl.SetScreenPos(1, { 0.0f, 0.0f });
			}

			if (&vec[1]->at(0) == sel)
			{
				lvl.SwitchArea(vec[1]->at(0).nWarpArea);
			}
		}
	);


	//Main level
	level.PushArea(Level::LoadStringInputFromFile("assets/levels/level1-1.txt"),
		210,
		20,
		CameraOffset,
		WidthInBlocks,
		HeightInBlocks,
		props);

	//Bonus area
	level.PushArea(Level::LoadStringInputFromFile("assets/levels/level1-1sub.txt"),
		WidthInBlocks,
		HeightInBlocks,
		{0.0f, 0.0f},
		WidthInBlocks,
		HeightInBlocks,
		props
	);
	//Connecting pipes to loaded areas
	level.InitPipes();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_Window->SetVsync(true);
}

void Application::OnUserRun()
{
	Player& player = UserVars::GetInstance()->m_Player;
	Level& level = UserVars::GetInstance()->m_Level;

	//Useful game constants
	constexpr float fPlayerSpeed = 0.1f;

	while (!m_Window->ShouldClose())
	{
		//Elapsed time management
		Timer elapsed_time_timer;
		static float fElapsedTime = 0.0f;

		glClear(GL_COLOR_BUFFER_BIT);
		m_Shaders[0].UniformMat4f(m_Camera.GetViewMatrix(), "view");

		if (m_Window->IsKeyboardEvent(InputEvent(GLFW_KEY_R, GLFW_PRESS)))
		{
			level.DestroyAnimations();
			player.bDead = false;
			player.bTouchesFlagpole = false;
			level.ResetCurrentLevel(player, m_Camera);
			player.SetPlayerState(PlayerState::MARIO);
		}

		if (!player.bDead && !player.bTouchesFlagpole && !player.EnteringPipe.first && 
			!level.PlayerExitPipes())
		{
			player.Update(*m_Window, level.GetTileVector(),
				level.GetEnemyVector(),
				fPlayerSpeed,
				level.GetLevelSize().y,
				level.GetLevelSize().x);
		}
	
	
		//Screen scrolling
		level.ScrollScreen(player, m_Camera, fPlayerSpeed);

		//Collision Detection, handles almost all of the game logic

		if (m_Window->IsKeyboardEvent({ GLFW_KEY_Q, GLFW_PRESS }))
		{
			__debugbreak();
		}

		level.LevelLogic(player, m_Camera, fElapsedTime, m_Window);

		level.DrawLevelBackground(m_Shaders[0], m_Textures);
		player.Draw(m_Shaders[0], m_Textures[(int)player.GetPlayerState() + 1], m_Textures[6]);
		level.DrawLevel(m_Shaders[0], m_Shaders[1], m_Textures);

		m_Window->Update();

		fElapsedTime = elapsed_time_timer.ElapsedTime();
	}
}









