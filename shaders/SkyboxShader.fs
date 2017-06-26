#version 330 core

in vec3 fragmentTextureCoords;

out vec4 finalColor;

uniform samplerCube cubeMap;

void main()
{
	finalColor = texture(cubeMap, fragmentTextureCoords);
}