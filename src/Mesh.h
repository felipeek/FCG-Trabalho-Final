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
		glm::vec4 tangent;
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
			Texture* diffuseMap, Texture* specularMap, Texture* normalMap, float specularShineness);
		~Mesh();
		void render(const Shader& shader, bool useNormalMap) const;
		Texture* getDiffuseMap() const;
		void setDiffuseMap(Texture* diffuseMap);
		Texture* getSpecularMap() const;
		void setSpecularMap(Texture* diffuseMap);
		Texture* getNormalMap() const;
		void setNormalMap(Texture* normalMap);
		float getSpecularShineness() const;
		void setSpecularShineness(float specualrShineness);
		MeshRenderMode getRenderMode() const;
		void setRenderMode(MeshRenderMode renderMode);
		bool isVisible() const;
		void setVisible(bool visible);

		static Texture* getDefaultDiffuseMap();
		static Texture* getDefaultSpecularMap();
		static glm::vec4 getTangentVector(glm::vec2 diffUV1, glm::vec2 diffUV2, glm::vec4 edge1, glm::vec4 edge2);
	private:
		void createVAO();
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		Texture* diffuseMap;
		Texture* specularMap;
		Texture* normalMap;
		float specularShineness;
		MeshRenderMode renderMode;
		bool visible;
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
		Quad(Texture* diffuseMap, Texture* specularMap, Texture* normalMap, float specularShineness);
		~Quad();
	private:
		static const std::vector<Vertex> quadVertices;
		static const std::vector<unsigned int> quadIndices;
		static const std::vector<Vertex> generateQuadVertices();
		static const std::vector<unsigned int> generateQuadIndices();
	};
}