#version 330 core

in vec2 fragmentTextureCoords;

out vec4 finalColor;

uniform sampler2D fixedTexture;
uniform int playerHp;
uniform int playerMaximumHp;

void main()
{
	float texLimit = float(playerHp)/float(playerMaximumHp);
	if (fragmentTextureCoords.x > texLimit)
		finalColor = vec4(0.0, 0.0, 0.0, 0.3);
	else
		finalColor = mix(vec4(0.6, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), fragmentTextureCoords.x);
}
