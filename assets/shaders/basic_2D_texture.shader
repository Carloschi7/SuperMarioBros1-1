#shader vertex
#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tx;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 TexCoord;

void main()
{
	gl_Position = proj * view * model * vec4(pos, 0.0f, 1.0f);
	TexCoord = tx;
}

#shader fragment
#version 330 core

out vec4 color;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform float offset;
uniform float vertical_offset;

void main()
{
	vec2 tex = TexCoord;
	tex.x += offset;
	tex.y += vertical_offset;
	color = texture(texture1, tex);
}