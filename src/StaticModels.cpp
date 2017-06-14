#include "StaticModels.h"

// Vertices:
// 4 - position
// 4 - color
// 4 - normal

using namespace raw;

/*Mesh* StaticModels::getCubeMesh(Texture* diffuseMap, Texture* specularMap, float specularShineness)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	Vertex v;

	// Bottom
	v = { glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(0.0f, -1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) };
	vertices.push_back(v);
	v = { glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(0.0f, -1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) };
	vertices.push_back(v);
	v = { glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(0.0f, -1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f) };
	vertices.push_back(v);
	v = { glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(0.0f, -1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f) };
	vertices.push_back(v);

	// Top
	v = { glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) };
	vertices.push_back(v);
	v = { glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) };
	vertices.push_back(v);
	v = { glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f) };
	vertices.push_back(v);
	v = { glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f) };
	vertices.push_back(v);

	// Left
	v = { glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) };
	vertices.push_back(v);
	v = { glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) };
	vertices.push_back(v);
	v = { glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f) };
	vertices.push_back(v);
	v = { glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f) };
	vertices.push_back(v);

	// Right
	v = { glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) };
	vertices.push_back(v);
	v = { glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) };
	vertices.push_back(v);
	v = { glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f) };
	vertices.push_back(v);
	v = { glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f) };
	vertices.push_back(v);

	// Front
	v = { glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) };
	vertices.push_back(v);
	v = { glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f) };
	vertices.push_back(v);
	v = { glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f) };
	vertices.push_back(v);
	v = { glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) };
	vertices.push_back(v);

	// Back
	v = { glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(0.0f, 0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f) };
	vertices.push_back(v);
	v = { glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(0.0f, 0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f) };
	vertices.push_back(v);
	v = { glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(0.0f, 0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f) };
	vertices.push_back(v);
	v = { glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(0.0f, 0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f) };
	vertices.push_back(v);

	// Bottom
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);

	// Top
	indices.push_back(4);
	indices.push_back(5);
	indices.push_back(6);
	indices.push_back(4);
	indices.push_back(6);
	indices.push_back(7);

	// Left
	indices.push_back(8);
	indices.push_back(9);
	indices.push_back(10);
	indices.push_back(9);
	indices.push_back(10);
	indices.push_back(11);

	// Right
	indices.push_back(12);
	indices.push_back(13);
	indices.push_back(14);
	indices.push_back(13);
	indices.push_back(14);
	indices.push_back(15);

	// Front
	indices.push_back(16);
	indices.push_back(17);
	indices.push_back(18);
	indices.push_back(17);
	indices.push_back(18);
	indices.push_back(19);

	// Back
	indices.push_back(21);
	indices.push_back(20);
	indices.push_back(23);
	indices.push_back(20);
	indices.push_back(22);
	indices.push_back(23);

	return new Mesh(vertices, indices, diffuseMap, specularMap, specularShineness);
}

Mesh* StaticModels::getTriangleMesh(Texture* diffuseMap, Texture* specularMap, float specularShineness)
{
	std::vector<raw::Vertex> vertices = std::vector<raw::Vertex>();

	raw::Vertex v1;
	v1.position = glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
	v1.normal = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	v1.textureCoordinates = glm::vec2(0.0f, 0.0f);

	raw::Vertex v2;
	v2.position = glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
	v2.normal = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	v2.textureCoordinates = glm::vec2(1.0f, 0.0f);

	raw::Vertex v3;
	v3.position = glm::vec4(0.0f, 0.5f, 0.0f, 1.0f);
	v3.normal = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	v3.textureCoordinates = glm::vec2(0.5f, 1.0f);

	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);

	std::vector<unsigned int> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	return new Mesh(vertices, indices, diffuseMap, specularMap, specularShineness);
}*/

/*GLfloat Models::triangleVertices[30] = {
	 -0.5f, -0.5f, 0.0f, 1.0f ,  0.0f, 0.0f, 1.0f, 0.0f , 0.0f, 0.0f,
	 0.5f, -0.5f, 0.0f, 1.0f ,  0.0f, 0.0f, 1.0f, 0.0f , 1.0f, 0.0f,
	 0.0f, 0.5f, 0.0f, 1.0f ,  0.0f, 0.0f, 1.0f, 0.0f , 0.5f, 1.0f
};

GLuint Models::triangleIndices[1][3] = {
	{ 0, 1, 2 }
};

GLfloat Models::cubeVertices[24*10] = {
	// Bottom
	 -1.0f, -1.0f, 1.0f, 1.0f , 0.0f, -1.0f, 0.0f, 0.0f ,	0.0f, 0.0f,
	 1.0f, -1.0f, 1.0f, 1.0f , 0.0f, -1.0f, 0.0f, 0.0f ,	1.0f, 0.0f,
	 1.0f, -1.0f, -1.0f, 1.0f , 0.0f, -1.0f, 0.0f, 0.0f ,	0.0f, 1.0f,
	 -1.0f, -1.0f, -1.0f, 1.0f , 0.0f, -1.0f, 0.0f, 0.0f ,	1.0f, 1.0f,

	// Top
	 -1.0f, 1.0f, 1.0f, 1.0f , 0.0f, 1.0f, 0.0f, 0.0f , 0.0f, 0.0f ,
	 1.0f, 1.0f, 1.0f, 1.0f , 0.0f, 1.0f, 0.0f, 0.0f , 1.0f, 0.0f ,
	 1.0f, 1.0f, -1.0f, 1.0f , 0.0f, 1.0f, 0.0f, 0.0f , 0.0f, 1.0f ,
	 -1.0f, 1.0f, -1.0f, 1.0f , 0.0f, 1.0f, 0.0f, 0.0f ,1.0f, 1.0f,

	// Left
	 -1.0f, -1.0f, 1.0f, 1.0f , -1.0f, 0.0f, 0.0f, 0.0f , 0.0f, 0.0f ,
	 -1.0f, -1.0f, -1.0f, 1.0f , -1.0f, 0.0f, 0.0f, 0.0f ,1.0f, 0.0f,
	 -1.0f, 1.0f, 1.0f, 1.0f , -1.0f, 0.0f, 0.0f, 0.0f , 0.0f, 1.0f ,
	 -1.0f, 1.0f, -1.0f, 1.0f , -1.0f, 0.0f, 0.0f, 0.0f , 1.0f, 1.0f ,

	// Right
	 1.0f, -1.0f, 1.0f, 1.0f , 1.0f, 0.0f, 0.0f, 0.0f , 0.0f, 0.0f ,
	 1.0f, -1.0f, -1.0f, 1.0f , 1.0f, 0.0f, 0.0f, 0.0f ,1.0f, 0.0f,
	 1.0f, 1.0f, 1.0f, 1.0f , 1.0f, 0.0f, 0.0f, 0.0f , 0.0f, 1.0f ,
	 1.0f, 1.0f, -1.0f, 1.0f , 1.0f, 0.0f, 0.0f, 0.0f , 1.0f, 1.0f ,

	// Front
	 -1.0f, -1.0f, 1.0f, 1.0f , 0.0f, 0.0f, 1.0f, 0.0f , 0.0f, 0.0f ,
	 1.0f, -1.0f, 1.0f, 1.0f , 0.0f, 0.0f, 1.0f, 0.0f , 1.0f, 0.0f ,
	 -1.0f, 1.0f, 1.0f, 1.0f , 0.0f, 0.0f, 1.0f, 0.0f , 0.0f, 1.0f ,
	 1.0f, 1.0f, 1.0f, 1.0f , 0.0f, 0.0f, 1.0f, 0.0f , 1.0f, 1.0f ,

	// Back
	 1.0f, -1.0f, -1.0f, 1.0f , 0.0f, 0.0f, -1.0f, 0.0f , 0.0f, 0.0f ,
	 -1.0f, -1.0f, -1.0f, 1.0f , 0.0f, 0.0f, -1.0f, 0.0f ,1.0f, 0.0f,
	 1.0f, 1.0f, -1.0f, 1.0f , 0.0f, 0.0f, -1.0f, 0.0f , 0.0f, 1.0f ,
	 -1.0f, 1.0f, -1.0f, 1.0f , 0.0f, 0.0f, -1.0f, 0.0f ,1.0f, 1.0f
};

GLuint Models::cubeIndices[12][3] = {
	{ 0, 1, 2 },{ 0, 2, 3 },			// bottom
	{ 4, 5, 6 },{ 4, 6, 7 },			// top
	{ 8, 9, 10 },{ 9, 10, 11 },		// left
	{ 12, 13, 14 },{ 13, 14, 15 },		// right
	{ 16, 17, 18 },{ 17, 18, 19 },		// front
	{ 21, 20, 23 },{ 20, 22, 23 }		// back
};

GLfloat Models::lightVertices[24*10] = {
	// Bottom
	-1.0f, -1.0f, 1.0f, 1.0f , 0.0f, -1.0f, 0.0f, 0.0f ,	0.0f, 0.0f,
	1.0f, -1.0f, 1.0f, 1.0f , 0.0f, -1.0f, 0.0f, 0.0f ,	1.0f, 0.0f,
	1.0f, -1.0f, -1.0f, 1.0f , 0.0f, -1.0f, 0.0f, 0.0f ,	1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f, 1.0f , 0.0f, -1.0f, 0.0f, 0.0f ,	0.0f, 1.0f,

	// Top
	-1.0f, 1.0f, 1.0f, 1.0f , 0.0f, 1.0f, 0.0f, 0.0f , 0.0f, 0.0f ,
	1.0f, 1.0f, 1.0f, 1.0f , 0.0f, 1.0f, 0.0f, 0.0f , 1.0f, 0.0f ,
	1.0f, 1.0f, -1.0f, 1.0f , 0.0f, 1.0f, 0.0f, 0.0f , 1.0f, 1.0f ,
	-1.0f, 1.0f, -1.0f, 1.0f , 0.0f, 1.0f, 0.0f, 0.0f , 0.0f, 1.0f,

	// Left
	-1.0f, -1.0f, 1.0f, 1.0f , -1.0f, 0.0f, 0.0f, 0.0f , 1.0f, 0.0f ,
	-1.0f, -1.0f, -1.0f, 1.0f , -1.0f, 0.0f, 0.0f, 0.0f ,0.0f, 0.0f,
	-1.0f, 1.0f, 1.0f, 1.0f , -1.0f, 0.0f, 0.0f, 0.0f , 1.0f, 1.0f ,
	-1.0f, 1.0f, -1.0f, 1.0f , -1.0f, 0.0f, 0.0f, 0.0f , 0.0f, 1.0f ,

	// Right
	1.0f, -1.0f, 1.0f, 1.0f , 1.0f, 0.0f, 0.0f, 0.0f , 1.0f, 0.0f ,
	1.0f, -1.0f, -1.0f, 1.0f , 1.0f, 0.0f, 0.0f, 0.0f ,0.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f , 1.0f, 0.0f, 0.0f, 0.0f , 1.0f, 1.0f ,
	1.0f, 1.0f, -1.0f, 1.0f , 1.0f, 0.0f, 0.0f, 0.0f , 0.0f, 1.0f ,

	// Front
	-1.0f, -1.0f, 1.0f, 1.0f , 0.0f, 0.0f, 1.0f, 0.0f , 0.0f, 0.0f ,
	1.0f, -1.0f, 1.0f, 1.0f , 0.0f, 0.0f, 1.0f, 0.0f , 1.0f, 0.0f ,
	-1.0f, 1.0f, 1.0f, 1.0f , 0.0f, 0.0f, 1.0f, 0.0f , 0.0f, 1.0f ,
	1.0f, 1.0f, 1.0f, 1.0f , 0.0f, 0.0f, 1.0f, 0.0f , 1.0f, 1.0f ,

	// Back
	1.0f, -1.0f, -1.0f, 1.0f , 0.0f, 0.0f, -1.0f, 0.0f , 1.0f, 0.0f ,
	-1.0f, -1.0f, -1.0f, 1.0f , 0.0f, 0.0f, -1.0f, 0.0f ,0.0f, 0.0f,
	1.0f, 1.0f, -1.0f, 1.0f , 0.0f, 0.0f, -1.0f, 0.0f , 1.0f, 1.0f ,
	-1.0f, 1.0f, -1.0f, 1.0f , 0.0f, 0.0f, -1.0f, 0.0f ,0.0f, 1.0f
};

GLuint Models::lightIndices[12][3] = {
	{ 0, 1, 2 },{ 0, 2, 3 },			// bottom
	{ 4, 5, 6 },{ 4, 6, 7 },			// top
	{ 8, 9, 10 },{ 9, 10, 11 },		// left
	{ 12, 13, 14 },{ 13, 14, 15 },		// right
	{ 16, 17, 18 },{ 17, 18, 19 },		// front
	{ 21, 20, 23 },{ 20, 22, 23 }		// back
};
GLfloat Models::lightLineVertices[2 * 10] = {
	0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
	0.2f, 1.0f, 0.3f, 1, 0, 0, 0, 0, 0, 0
};

GLuint Models::lightLineIndices[2] = {
	0, 1
};

GLfloat Models::spotLightVertices[5 * 10] = {
	-1.0f, -1.0f, 1.0f, 1.0f , 0.0f, 0.0f, 0.0f, 0.0f ,	0.0f, 0.0f,
	1.0f, -1.0f, 1.0f, 1.0f , 0.0f, 0.0f, 0.0f, 0.0f ,	0.0f, 0.0f,
	-1.0f, 1.0f, 1.0f, 1.0f , 0.0f, 0.0f, 0.0f, 0.0f ,	0.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f , 0.0f, 0.0f, 0.0f, 0.0f ,	0.0f, 0.0f,
	0.0f, 0.0f, -1.0f, 1.0f , 0.0f, 0.0f, 0.0f, 0.0f ,	0.0f, 0.0f,
};

GLuint Models::spotLightIndices[6][3] = {
	{ 0, 1, 2 },{ 1, 3, 2 },
	{ 0, 4, 2 },{2 , 4, 3},
	{4, 1, 3}, {0, 1, 4}
};*/