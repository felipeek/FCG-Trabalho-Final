#version 330 core

struct Material
{
	sampler2D diffuseMap;
	sampler2D specularMap;
	float shineness;
};

in vec4 ambientColor;
in vec4 diffuseColor;
in vec4 specularColor;
in vec2 fragmentTextureCoords;

out vec4 finalColor;

uniform Material material;

void main()
{
	finalColor = ambientColor * texture(material.diffuseMap, fragmentTextureCoords) +
		diffuseColor * texture(material.diffuseMap, fragmentTextureCoords) + 
		specularColor * texture(material.specularMap, fragmentTextureCoords);
}