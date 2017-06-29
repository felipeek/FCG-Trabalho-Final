#version 330 core

struct Material
{
	sampler2D diffuseMap;
	sampler2D specularMap;
	float shineness;
};

struct FogDescriptor
{
	float density;
	float gradient;
	vec4 skyColor;
	bool on;
};

flat in vec4 ambientColor;
flat in vec4 diffuseColor;
flat in vec4 specularColor;
in vec2 fragmentTextureCoords;
in float fragmentVisibility;

out vec4 finalColor;

uniform Material material;
uniform FogDescriptor fogDescriptor;

void main()
{
	finalColor = ambientColor * texture(material.diffuseMap, fragmentTextureCoords) +
		diffuseColor * texture(material.diffuseMap, fragmentTextureCoords) + 
		specularColor * texture(material.specularMap, fragmentTextureCoords);
		
	if (fogDescriptor.on)
		finalColor = mix(fogDescriptor.skyColor, finalColor, fragmentVisibility);
}