#version 460 core

//uniform sampler2D myTexture;

layout(location = 0) in vec2 TexCoords;
layout(location = 1) in vec4 Color;

layout(location = 0) out vec4 FragColor;

void main()
{
	//vec4 tex = texture(myTexture, TexCoords);

	//if (tex.a < 0.1f)
	//	discard;

	FragColor = (Color);
}