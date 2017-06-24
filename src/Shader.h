#pragma once

#include <GL\glew.h>

const char fixedVertexShaderPath[] = ".\\shaders\\FixedShader.vs";
const char fixedFragmentShaderPath[] = ".\\shaders\\FixedShader.fs";
const char basicVertexShaderPath[] = ".\\shaders\\BasicShader.vs";
const char basicFragmentShaderPath[] = ".\\shaders\\BasicShader.fs";
const char phongVertexShaderPath[] = ".\\shaders\\PhongShader.vs";
const char phongFragmentShaderPath[] = ".\\shaders\\PhongShader.fs";
const char gouradVertexShaderPath[] = ".\\shaders\\GouradShader.vs";
const char gouradFragmentShaderPath[] = ".\\shaders\\GouradShader.fs";
const char flatVertexShaderPath[] = ".\\shaders\\FlatShader.vs";
const char flatFragmentShaderPath[] = ".\\shaders\\FlatShader.fs";

namespace raw
{
	enum class ShaderType
	{
		FIXED,
		BASIC,
		FLAT,
		GOURAD,
		PHONG
	};

	class Shader
	{
	public:
		Shader(ShaderType type);
		~Shader();
		GLuint getProgram() const;
		ShaderType getType() const;
		void useProgram() const;
	private:
		GLuint shaderProgram;
		ShaderType type;
	};
}