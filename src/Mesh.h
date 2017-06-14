#pragma once
#include <vector>
#include "MathIncludes.h"
#include <GL\glew.h>

namespace raw
{
	class Texture;
	class Shader;

	#pragma pack(push, 1)
	struct Vertex
	{
		glm::vec4 position;
		glm::vec4 normal;
		glm::vec2 textureCoordinates;
	};
	#pragma pack(pop)

	enum class MeshRenderMode
	{
		TRIANGLES,
		LINES
	};

	class Mesh
	{
	public:
		Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices,
			Texture* diffuseMap, Texture* specularMap, float specularShineness);
		~Mesh();
		void render(const Shader& shader) const;
		Texture* getDiffuseMap() const;
		void setDiffuseMap(Texture* diffuseMap);
		Texture* getSpecularMap() const;
		void setSpecularMap(Texture* diffuseMap);
		float getSpecularShineness() const;
		void setSpecularShineness(float specualrShineness);
		MeshRenderMode getRenderMode() const;
		void setRenderMode(MeshRenderMode renderMode);

		static Texture* getDefaultDiffuseMap();
		static Texture* getDefaultSpecularMap();
	private:
		void createVAO();
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		Texture* diffuseMap;
		Texture* specularMap;
		float specularShineness;
		MeshRenderMode renderMode;
		GLuint VAO;

		static Texture* defaultDiffuseMap;
		static Texture* defaultSpecularMap;
		static const char* defaultDiffuseMapPath;
		static const char* defaultSpecularMapPath;
	};

	class Quad : public Mesh
	{
	public:
		Quad();
		Quad(Texture* diffuseMap);
		Quad(Texture* diffuseMap, Texture* specularMap, float specularShineness);
		~Quad();
	private:
		static const std::vector<Vertex> quadVertices;
		static const std::vector<unsigned int> quadIndices;
		static const std::vector<Vertex> generateQuadVertices();
		static const std::vector<unsigned int> generateQuadIndices();
	};
}