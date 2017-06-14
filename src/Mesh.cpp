#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"

using namespace raw;

// Create a new mesh using:
// vertices: an array containing all vertices that define the mesh.
// indices: an array containing all indices thar define the mesh.
// diffuseMap: a texture map defining the diffuse color of the object.
// specularMap: a texture map defining the specular color of the object.
// specularShineness: a value defining the shineness of the specular color.
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices,
	Texture* diffuseMap, Texture* specularMap, float specularShineness)
{
	this->vertices = vertices;
	this->indices = indices;
	this->setDiffuseMap(diffuseMap);
	this->setSpecularMap(specularMap);
	this->specularShineness = specularShineness;
	this->renderMode = MeshRenderMode::TRIANGLES;
	this->createVAO();
}

Mesh::~Mesh()
{
	this->diffuseMap->decreaseReferences();
	Texture::destroy(this->diffuseMap);
	this->specularMap->decreaseReferences();
	Texture::destroy(this->specularMap);
}

// Render the mesh using the shader received as parameter.
void Mesh::render(const Shader& shader) const
{
	shader.useProgram();

	if (shader.getType() == ShaderType::PHONG || shader.getType() == ShaderType::GOURAD
		|| shader.getType() == ShaderType::FLAT)
	{
		this->getDiffuseMap()->bind(GL_TEXTURE0);
		this->getSpecularMap()->bind(GL_TEXTURE1);

		GLuint materialDiffuseMapLocation = glGetUniformLocation(shader.getProgram(), "material.diffuseMap");
		GLuint materialSpecularMapLocation = glGetUniformLocation(shader.getProgram(), "material.specularMap");
		GLuint materialShinenessLocation = glGetUniformLocation(shader.getProgram(), "material.shineness");

		glUniform1i(materialDiffuseMapLocation, 0);
		glUniform1i(materialSpecularMapLocation, 1);
		glUniform1f(materialShinenessLocation, this->specularShineness);
	}

	glBindVertexArray(this->VAO);

	if (this->renderMode == MeshRenderMode::TRIANGLES)
		glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	else if (this->renderMode == MeshRenderMode::LINES)
		glDrawElements(GL_LINES, this->indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

Texture* Mesh::getDiffuseMap() const
{
	return this->diffuseMap;
}

// Set the mesh's diffuseMap. If there was already a diffuseMap set, call freeTexture to notify the
// TextureFactory that there is one less reference of the old texture.
void Mesh::setDiffuseMap(Texture* diffuseMap)
{
	if (this->diffuseMap != 0)
	{
		this->diffuseMap->decreaseReferences();
		Texture::destroy(this->diffuseMap);
	}

	if (diffuseMap != 0)
	{
		this->diffuseMap = diffuseMap;
		this->diffuseMap->increaseReferences();
	}
	else
	{
		this->diffuseMap = Mesh::getDefaultDiffuseMap();
		this->diffuseMap->increaseReferences();
	}
}

Texture* Mesh::getSpecularMap() const
{
	return this->specularMap;
}

// Set the mesh's specularMap. If there was already a specularMap set, call freeTexture to notify the
// TextureFactory that there is one less reference of the old texture.
void Mesh::setSpecularMap(Texture* specularMap)
{
	if (this->specularMap != 0)
	{
		this->specularMap->decreaseReferences();
		Texture::destroy(this->specularMap);
	}

	if (specularMap != 0)
	{
		this->specularMap = specularMap;
		this->specularMap->increaseReferences();
	}
	else
	{
		this->specularMap = Mesh::getDefaultSpecularMap();
		this->specularMap->increaseReferences();
	}
}

float Mesh::getSpecularShineness() const
{
	return this->specularShineness;
}

void Mesh::setSpecularShineness(float specularShineness)
{
	this->specularShineness = specularShineness;
}

MeshRenderMode Mesh::getRenderMode() const
{
	return this->renderMode;
}

void Mesh::setRenderMode(MeshRenderMode renderMode)
{
	this->renderMode = renderMode;
}

// Create Mesh's VAO (Vertex Array Object).
void Mesh::createVAO()
{
	GLuint VBO, EBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, this->vertices.size() * sizeof(Vertex), &this->vertices[0]);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (void*)(4 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (void*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, this->indices.size() * sizeof(unsigned int), &this->indices[0]);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	this->VAO = VAO;
}

Texture* Mesh::defaultDiffuseMap = 0;
Texture* Mesh::defaultSpecularMap = 0;
const char* Mesh::defaultDiffuseMapPath = ".\\res\\defaultDiffuseMap.png";
const char* Mesh::defaultSpecularMapPath = ".\\res\\defaultSpecularMap.png";

// Returns a default diffuseMap.
Texture* Mesh::getDefaultDiffuseMap()
{
	if (!Mesh::defaultDiffuseMap)
	{
		Mesh::defaultDiffuseMap = Texture::load(Mesh::defaultDiffuseMapPath);
		Mesh::defaultDiffuseMap->increaseReferences();
	}

	return Mesh::defaultDiffuseMap;
}

// Returns a default specularMap.
Texture* Mesh::getDefaultSpecularMap()
{
	if (!Mesh::defaultSpecularMap)
	{
		Mesh::defaultSpecularMap = Texture::load(Mesh::defaultSpecularMapPath);
		Mesh::defaultSpecularMap->increaseReferences();
	}

	return Mesh::defaultSpecularMap;
}

// Quad: A quad is a special type of Mesh which its vertices are defined as a simple square.
// Useful to render 2D entities.
Quad::Quad() : Mesh(Quad::quadVertices, Quad::quadIndices, 0, 0, 128.0f)
{

}

Quad::Quad(Texture* diffuseMap) : Mesh(Quad::quadVertices, Quad::quadIndices,
	diffuseMap, 0, 128.0f)
{

}

Quad::Quad(Texture* diffuseMap, Texture* specularMap, float specularShineness) : 
	Mesh(Quad::quadVertices, Quad::quadIndices, diffuseMap, specularMap, 128.0f)
{

}

Quad::~Quad()
{

}

const std::vector<Vertex> Quad::quadVertices = Quad::generateQuadVertices();
const std::vector<unsigned int> Quad::quadIndices = Quad::generateQuadIndices();

// Manually create quad's vertices, using two triangles.
const std::vector<Vertex> Quad::generateQuadVertices()
{
	std::vector<Vertex> quadVertices;
	Vertex v;

	v = {
		glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f),		// POSITION
		glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),		// NORMAL
		glm::vec2(0.0f, 1.0f)					// TEX COORDS
	};
	quadVertices.push_back(v);

	v = {
		glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),		// POSITION
		glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),		// NORMAL
		glm::vec2(1.0f, 1.0f)					// TEX COORDS
	};
	quadVertices.push_back(v);

	v = {
		glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f),	// POSITION
		glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),		// NORMAL
		glm::vec2(0.0f, 0.0f)					// TEX COORDS
	};
	quadVertices.push_back(v);

	v = {
		glm::vec4(1.0f, -1.0f, 0.0f, 1.0f),		// POSITION
		glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),		// NORMAL
		glm::vec2(1.0f, 0.0f)					// TEX COORDS
	};
	quadVertices.push_back(v);

	return quadVertices;
}

const std::vector<unsigned int> Quad::generateQuadIndices()
{
	return std::vector<unsigned int>({
		0, 1, 2,
		1, 3, 2
	});
}