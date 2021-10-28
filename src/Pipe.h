#pragma once
#include <glm/glm.hpp>
#include "MainIncl.h"

class Level;
class Player;
class Pipe;

class PipesProperties
{
private:
	//the parameter std::vector<std::vector<Pipe>*> may not be a safe choice but it works pretty 
	//well in this instance
	using connect_func = std::function<void(std::vector<std::vector<Pipe>*>)>;
	using warp_func = std::function<void(std::vector<std::vector<Pipe>*>, const Pipe*, Level&, Player&)>;
	connect_func m_ConnectFunc;
	warp_func m_WarpAreaFunc;
public:
	PipesProperties() = default;
	inline void SetConnectionFunc(const connect_func& f) { m_ConnectFunc = f; }
	inline void ConnectPipes(std::vector<std::vector<Pipe>*> vec) { m_ConnectFunc(vec); }
	inline void SetWarpAreaFunc(const warp_func& f) { m_WarpAreaFunc = f; }
	inline void WarpAreaPipes(std::vector<std::vector<Pipe>*> vec,const Pipe* sel, Level& lvl, Player& pl) 
	{ 
		m_WarpAreaFunc(vec, sel, lvl, pl);
	}
};

class Pipe 
{
public:
	Pipe(const glm::vec2& position, uint8_t body_size);
	Pipe(Pipe&& p) noexcept;
	~Pipe();
	void Draw(Shader& shd, const Texture& tex) const;
	bool CheckEntrance(const Player& p, bool IsDownKeyPressed) const;
	inline void SetPipeConnection(const Pipe* p) { m_ConnectedPipe = p; }
	inline const Pipe* Connection() const { return m_ConnectedPipe; }
	inline const glm::vec2& Position() const { return m_Pos; }
	inline const uint8_t BodyHeight() const { return m_BodyHeight; }
public:
	bool bEnterable = false;
	//Warps into the main level by default
	uint32_t nWarpArea = 0;
private:
	glm::vec2 m_Pos;
	glm::mat4 m_PipeExitModel;
	glm::mat4* m_pPipeBodyModels;
	const Pipe* m_ConnectedPipe;
	uint8_t m_BodyHeight;
};