#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"

using namespace raw;

// Create a new mesh using:
// vertices: an array containing all vertices that define the mesh.
// indices: an array containing all indices thar define the mesh.
// diffuseMap: a texture map defining the diffuse color of the object.
// specularMap: a texture map defining the specular color of the object.
// normalMap: a texture map defining the normals of the object. If used, it will replace the vertice normals.
// specularShineness: a value defining the shineness of the specular color.
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices,
	Texture* diffuseMap, Texture* specularMap, Texture* normalMap, float specularShineness)
{
	this->vertices = vertices;
	this->indices = indices;
	this->setDiffuseMap(diffuseMap);
	this->setSpecularMap(specularMap);
	this->setNormalMap(normalMap);
	this->specularShineness = specularShineness;
	this->renderMode = MeshRenderMode::TRIANGLES;
	this->visible = true;
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
void Mesh::render(const Shader& shader, bool useNormalMap) const
{
	if (!this->visible) return;

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

		if (this->normalMap != 0 && useNormalMap)
		{
			this->getNormalMap()->bind(GL_TEXTURE2);
			GLuint materialNormalMapLocation = glGetUniformLocation(shader.getProgram(), "material.normalMap");
			GLuint materialUseNormalMapLocation = glGetUniformLocation(shader.getProgram(), "material.useNormalMap");
			glUniform1i(materialNormalMapLocation, 2);
			glUniform1i(materialUseNormalMapLocation, true);
		}
		else
		{
			GLuint materialUseNormalMapLocation = glGetUniformLocation(shader.getProgram(), "material.useNormalMap");
			glUniform1i(materialUseNormalMapLocation, false);
		}
	}
	else if (shader.getType() == ShaderType::FIXED || shader.getType() == ShaderType::TEXTURE)
	{
		this->getDiffuseMap()->bind(GL_TEXTURE0);
		GLuint fixedTextureLocation = glGetUniformLocation(shader.getProgram(), "fixedTexture");
		glUniform1i(fixedTextureLocation, 0);
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

// Set the mesh's diffuseMap. If there was already a diffuseMap set, decrease the number of references
// and call destroy().
// Also, increase the number of references of new diffuseMap.
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

// Set the mesh's specularMap. If there was already a specularMap set, decrease the number of references
// and call destroy().
// Also, increase the number of references of new specularMap.
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

Texture* Mesh::getNormalMap() const
{
	return this->normalMap;
}

// Set the mesh's normalMap. If there was already a normalMap set, decrease the number of references
// and call destroy().
// Also, increase the number of references of new normalMap.
void Mesh::setNormalMap(Texture* normalMap)
{
	if (this->normalMap != 0)
	{
		this->normalMap->decreaseReferences();
		Texture::destroy(this->normalMap);
	}

	if (normalMap != 0)
	{
		this->normalMap = normalMap;
		this->normalMap->increaseReferences();
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

bool Mesh::isVisible() const
{
	return this->visible;
}

void Mesh::setVisible(bool visible)
{
	this->visible = visible;
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

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)(4 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)(10 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

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
const char* Mesh::defaultDiffuseMapPath = ".\\res\\art\\defaultDiffuseMap.png";
const char* Mesh::defaultSpecularMapPath = ".\\res\\art\\defaultSpecularMap.png";

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

glm::vec4 Mesh::getTangentVector(glm::vec2 diffUV1, glm::vec2 diffUV2, glm::vec4 edge1, glm::vec4 edge2)
{
	float f = 1.0f / (diffUV1.x * diffUV2.y - diffUV1.y * diffUV2.x);
	glm::vec4 tangentVector = f * (edge1 * diffUV2.y - edge2 * diffUV1.y);
	return tangentVector;
}

// Quad: A quad is a special type of Mesh which its vertices are defined as a simple square.
// Useful to render 2D entities.
Quad::Quad() : Mesh(Quad::quadVertices, Quad::quadIndices, 0, 0, 0, 128.0f)
{

}

Quad::Quad(Texture* diffuseMap) : Mesh(Quad::quadVertices, Quad::quadIndices,
	diffuseMap, 0, 0, 128.0f)
{

}

Quad::Quad(Texture* diffuseMap, Texture* specularMap, Texture* normalMap, float specularShineness) : 
	Mesh(Quad::quadVertices, Quad::quadIndices, diffuseMap, specularMap, normalMap, 128.0f)
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