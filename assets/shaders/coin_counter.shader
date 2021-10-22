#shader vertex
#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTexCoords;

out vec2 TexCoords;
uniform mat4 model;

void main()
{
	TexCoords = inTexCoords;
	gl_Position = model * vec4(inPos, 1.0f);
}

#shader fragment
#version 330 core

in vec2 TexCoords;

out vec4 color;
uniform sampler2D texture1;
uniform float offset;

void main()
{
	vec2 tex = TexCoords;
	tex.x += offset;
	vec4 output = texture(texture1, tex);
	color = output;
}