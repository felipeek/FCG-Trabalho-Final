#version 330 core

layout (location = 0) in vec4 vertexPosition;

out vec3 fragmentTextureCoords;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
	mat4 temp = viewMatrix;
	temp[3][0] = 0.0;
	temp[3][1] = 0.0;
	temp[3][2] = 0.0;
	gl_Position = projectionMatrix * temp * vertexPosition;
	fragmentTextureCoords = vertexPosition.xyz;
}