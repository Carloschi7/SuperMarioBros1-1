#include "Renderer.h"

Renderer::Renderer()
{
	//Default square block data
	float verts[]
	{
		-0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f, 0.5f,  1.0f, 1.0f,

		 0.5f, 0.5f,  1.0f, 1.0f,
		-0.5f, 0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 0.0f,
	};

	Layout lyt;
	lyt.PushAttribute({ 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0 });
	lyt.PushAttribute({ 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 2 * sizeof(float) });
	m_VertexManager.SendDataToOpenGLArray(verts, sizeof(verts), lyt);
}

Renderer::~Renderer()
{
}

void Renderer::SetTextureBoundary(glm::vec2 offset_from_bottom_left)
{
	GetInstance()->ISetTextureBoundary(offset_from_bottom_left);
}

void Renderer::DrawRect(Shader& shd, const Texture& tex, const glm::mat4& model, glm::vec2 texture_offset)
{
	GetInstance()->IDrawRect(shd, tex, model, texture_offset);
}

Renderer* Renderer::GetInstance()
{
	static Renderer instance;
	return &instance;
}

void Renderer::ISetTextureBoundary(glm::vec2 offset_from_bottom_left)
{
	float verts[]
	{
		-0.5f, -0.5f, 0.0f,						 0.0f,
		 0.5f, -0.5f, offset_from_bottom_left.x, 0.0f,
		 0.5f, 0.5f,  offset_from_bottom_left.x, offset_from_bottom_left.y,

		 0.5f, 0.5f,  offset_from_bottom_left.x, offset_from_bottom_left.y,
		-0.5f, 0.5f,  0.0f,						 offset_from_bottom_left.y,
		-0.5f, -0.5f, 0.0f,						 0.0f,
	};

	Layout lyt;
	lyt.PushAttribute({ 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0 });
	lyt.PushAttribute({ 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 2 * sizeof(float) });
	m_VertexManager.SendDataToOpenGLArray(verts, sizeof(verts), lyt);
}

void Renderer::IDrawRect(Shader& shd, const Texture& tex, const glm::mat4& model, glm::vec2 texture_offset)
{
	tex.Bind();
	shd.Uniform1i(0, "texture1");
	shd.Uniform1f(texture_offset.x, "offset");
	
	//Cause the overlay shader soes not have one
	if(shd.IsUniformDefined("vertical_offset"))
		shd.Uniform1f(texture_offset.y, "vertical_offset");
	
	shd.UniformMat4f(model, "model");

	m_VertexManager.BindVertexArray();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	if(texture_offset.x != 0.0f)
		shd.Uniform1f(0.0f, "offset");
	if (texture_offset.y != 0.0f)
		shd.Uniform1f(0.0f, "vertical_offset");
}
