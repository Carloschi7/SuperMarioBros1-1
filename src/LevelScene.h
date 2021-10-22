#pragma once
#include "Player.h"
#include "Timer.h"
#include "Flagpole.h"
#include "Pipe.h"

class LevelScene
{
protected:
	Player* const m_PlayerRef;
	Timer m_Timer;
public:
	LevelScene(Player& ref);
	~LevelScene();
	virtual bool PlayScene() = 0;
};

class DeathScene final : public LevelScene
{
private:
	bool m_bExpiring;
public:
	DeathScene(Player& ref);
	virtual bool PlayScene() override;
};

class FlagPoleScene final : public LevelScene
{
private:
	FlagPole* m_FlagPoleRef;
public:
	FlagPoleScene(Player& player_ref, FlagPole& flag_ref);
	virtual bool PlayScene() override;
};

class PipeDownScene final : public LevelScene
{
public:
	PipeDownScene(Player& ref);
	virtual bool PlayScene() override;
};

class PipeUpScene final : public LevelScene
{
public:
	PipeUpScene(Player& ref);
	virtual bool PlayScene() override;
};