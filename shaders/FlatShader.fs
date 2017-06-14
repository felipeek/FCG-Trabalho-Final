#version 330 core

flat in vec4 fragmentColor;
out vec4 finalColor;

void main()
{
	finalColor = fragmentColor;
}