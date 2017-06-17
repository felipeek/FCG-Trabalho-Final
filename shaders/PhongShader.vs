#version 330 core

layout (location = 0) in vec4 vertexPosition;
layout (location = 1) in vec4 vertexNormal;
layout (location = 2) in vec2 vertexTextureCoords;
layout (location = 3) in vec4 vertexTangent;

out vec4 fragmentPosition;
out vec4 fragmentNormal;
out vec2 fragmentTextureCoords;
out mat4 tangentMatrix;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
	vec3 normal3D = mat3(inverse(transpose(modelMatrix))) * vertexNormal.xyz;
	fragmentNormal = normalize(vec4(normal3D, 0.0));
	fragmentTextureCoords = vertexTextureCoords;
	fragmentPosition = modelMatrix * vertexPosition;
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertexPosition;

	vec4 T = modelMatrix * vertexTangent;
	vec4 N = modelMatrix * vertexNormal;
	vec4 B = vec4(cross(T.xyz, N.xyz), 0.0);
	tangentMatrix = mat4(T, B, N, vec4(0,0,0,0));
}