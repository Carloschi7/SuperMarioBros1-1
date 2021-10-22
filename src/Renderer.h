#pragma once
#include "MainIncl.h"

//Local renderer class for tile rendering
class Renderer
{
private:
	Renderer();
	~Renderer();
	Renderer(const Renderer&) = delete;
public:
	static void SetTextureBoundary(glm::vec2 offset_from_bottom_left);
	static void DrawRect(Shader& shd, const Texture& tex, const glm::mat4& model, glm::vec2 texture_offset);
private:
	static Renderer* GetInstance();
	void ISetTextureBoundary(glm::vec2 offset_from_bottom_left);
	//The shader used is required to have a texture1 uniform for the main texture rendering and a
	//offset float uniform to perform texture parsing
	void IDrawRect(Shader& shd, const Texture& tex, const glm::mat4& model, glm::vec2 texture_offset);
private:
	VertexManager m_VertexManager;
};