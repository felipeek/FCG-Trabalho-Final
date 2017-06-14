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
}