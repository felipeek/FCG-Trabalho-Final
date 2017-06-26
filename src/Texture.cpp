#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <cstdlib>

using namespace raw;

std::vector<Texture*> Texture::loadedTextures;

// Create a new texture, loading the image stored in texturePath.
Texture::Texture(const char* texturePath)
{
	stbi_set_flip_vertically_on_load(1);
	int imageWidth, imageHeight, imageChannels;
	unsigned char* imageData = stbi_load(texturePath, &imageWidth, &imageHeight, &imageChannels, 4);

	glGenTextures(1, &this->textureId);
	glBindTexture(GL_TEXTURE_2D, this->textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	// Anisotropic Filtering
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(imageData);

	this->path = (char*)malloc((strlen(texturePath) + 1) * sizeof(char));
	strcpy(this->path, texturePath);
	this->references = 0;
}

Texture::~Texture()
{
	free(this->path);
}

// Bind the texture to openGL, using the slot received as parameter.
void Texture::bind(GLenum slot) const
{
	glActiveTexture(slot);
	glBindTexture(GL_TEXTURE_2D, this->textureId);
}

// Unbind the texture.
void Texture::unbind(GLenum slot) const
{
	glActiveTexture(slot);
	glBindTexture(GL_TEXTURE_2D, 0);
}

const char* Texture::getPath() const
{
	return this->path;
}

void Texture::increaseReferences()
{
	++this->references;
}

void Texture::decreaseReferences()
{
	--this->references;
}

Texture* Texture::load(const char* texturePath)
{
	for (unsigned int i = 0; i < Texture::loadedTextures.size(); ++i)
		if (strcmp(Texture::loadedTextures[i]->getPath(), texturePath) == 0)
			return loadedTextures[i];

	Texture* newTexture = new Texture(texturePath);
	Texture::loadedTextures.push_back(newTexture);

	return newTexture;
}

void Texture::destroy(const Texture* texture)
{
	if (texture->references <= 0)
	{
		for (unsigned int i = 0; i < Texture::loadedTextures.size(); ++i)
			if (strcmp(Texture::loadedTextures[i]->getPath(), texture->getPath()) == 0)
				Texture::loadedTextures.erase(Texture::loadedTextures.begin() + i);
		delete texture;
	}
}

void Texture::destroyAll()
{
	for (unsigned int i = 0; i < Texture::loadedTextures.size(); ++i)
		delete Texture::loadedTextures[i];

	Texture::loadedTextures.clear();
}

CubeMapTexture::CubeMapTexture(const CubeMapTexturePath& texturePaths)
{
	int imageWidth, imageHeight, imageChannels;
	stbi_set_flip_vertically_on_load(0);	// must be done, otherwise textures appear upside down

	glGenTextures(1, &this->textureId);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureId);

	// Right
	unsigned char* imageData = stbi_load(texturePaths.right, &imageWidth, &imageHeight, &imageChannels, 4);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, imageWidth, imageHeight, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, imageData);
	stbi_image_free(imageData);

	// Left
	imageData = stbi_load(texturePaths.left, &imageWidth, &imageHeight, &imageChannels, 4);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, imageWidth, imageHeight, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, imageData);
	stbi_image_free(imageData);

	// Top
	imageData = stbi_load(texturePaths.top, &imageWidth, &imageHeight, &imageChannels, 4);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, imageWidth, imageHeight, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, imageData);
	stbi_image_free(imageData);

	// Bottom
	imageData = stbi_load(texturePaths.bottom, &imageWidth, &imageHeight, &imageChannels, 4);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, imageWidth, imageHeight, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, imageData);
	stbi_image_free(imageData);

	// Back
	imageData = stbi_load(texturePaths.back, &imageWidth, &imageHeight, &imageChannels, 4);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, imageWidth, imageHeight, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, imageData);
	stbi_image_free(imageData);

	// Front
	imageData = stbi_load(texturePaths.front, &imageWidth, &imageHeight, &imageChannels, 4);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, imageWidth, imageHeight, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, imageData);
	stbi_image_free(imageData);

	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

CubeMapTexture::~CubeMapTexture()
{

}

// Bind the texture to openGL, using the slot received as parameter.
void CubeMapTexture::bind(GLenum slot) const
{
	glActiveTexture(slot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureId);
}

// Unbind the texture.
void CubeMapTexture::unbind(GLenum slot) const
{
	glActiveTexture(slot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}