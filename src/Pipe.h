#pragma once
#include <glm/glm.hpp>
#include "MainIncl.h"

class Player;

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
private:
	glm::vec2 m_Pos;
	glm::mat4 m_PipeExitModel;
	glm::mat4* m_pPipeBodyModels;
	const Pipe* m_ConnectedPipe;
	uint8_t m_BodyHeight;
};