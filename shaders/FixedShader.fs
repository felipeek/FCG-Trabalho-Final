#version 330 core

in vec2 fragmentTextureCoords;

out vec4 finalColor;

uniform sampler2D fixedTexture;

void main()
{
	finalColor = texture(fixedTexture, fragmentTextureCoords);
}