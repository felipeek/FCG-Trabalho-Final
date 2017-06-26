#pragma once

#include <GL\glew.h>
#include <vector>

namespace raw
{
	class Texture
	{
	public:
		void bind(GLenum slot) const;
		void unbind(GLenum slot) const;
		const char* getPath() const;
		void increaseReferences();
		void decreaseReferences();
		static Texture* load(const char* texturePath);
		static void destroy(const Texture* texture);
		static void destroyAll();
	private:
		Texture(const char* texturePath);
		~Texture();
		GLuint textureId;
		char* path;
		int references;
		static std::vector<Texture*> loadedTextures;
	};

	struct CubeMapTexturePath
	{
		char* right;
		char* left;
		char* top;
		char* bottom;
		char* back;
		char* front;
	};

	class CubeMapTexture
	{
	public:
		CubeMapTexture(const CubeMapTexturePath& texturePaths);
		~CubeMapTexture();
		void bind(GLenum slot) const;
		void unbind(GLenum slot) const;
	private:
		GLuint textureId;
	};
}