#include "Shader.h"
#include <iostream>
#include <istream>
#include <fstream>
#include <sstream>

using namespace raw;

// Creates a new shader based on the ShaderType received.
// The paths of the files associated with each ShaderType are defined in Shader.h
Shader::Shader(ShaderType type)
{
	const char* vertexShaderPath;
	const char* fragmentShaderPath;

	switch (type)
	{
	case ShaderType::FIXED:
		vertexShaderPath = fixedVertexShaderPath;
		fragmentShaderPath = fixedFragmentShaderPath;
		break;
	case ShaderType::BASIC:
		vertexShaderPath = basicVertexShaderPath;
		fragmentShaderPath = basicFragmentShaderPath;
		break;
	case ShaderType::TEXTURE:
		vertexShaderPath = textureVertexShaderPath;
		fragmentShaderPath = textureFragmentShaderPath;
		break;
	case ShaderType::FLAT:
		vertexShaderPath = flatVertexShaderPath;
		fragmentShaderPath = flatFragmentShaderPath;
		break;
	case ShaderType::GOURAD:
		vertexShaderPath = gouradVertexShaderPath;
		fragmentShaderPath = gouradFragmentShaderPath;
		break;
	case ShaderType::PHONG:
		vertexShaderPath = phongVertexShaderPath;
		fragmentShaderPath = phongFragmentShaderPath;
		break;
	case ShaderType::SKYBOX:
		vertexShaderPath = skyboxVertexShaderPath;
		fragmentShaderPath = skyboxFragmentShaderPath;
		break;
	case ShaderType::HPBAR:
		vertexShaderPath = hpBarVertexShaderPath;
		fragmentShaderPath = hpBarFragmentShaderPath;
		break;
	default:
		vertexShaderPath = basicVertexShaderPath;
		fragmentShaderPath = basicFragmentShaderPath;
		break;
	}

	std::ifstream vertexShaderInputStream;
	std::stringstream vertexShaderStringStream;
	vertexShaderInputStream.open(vertexShaderPath, std::ios::in);
	vertexShaderStringStream << vertexShaderInputStream.rdbuf();
	std::string vertexShaderCodeStr = vertexShaderStringStream.str();
	const char* vertexShaderCode = vertexShaderCodeStr.c_str();
	vertexShaderInputStream.close();

	std::ifstream fragmentShaderInputStream;
	std::stringstream fragmentShaderStringStream;
	fragmentShaderInputStream.open(fragmentShaderPath, std::ios::in);
	fragmentShaderStringStream << fragmentShaderInputStream.rdbuf();
	std::string fragmentShaderCodeStr = fragmentShaderStringStream.str();
	const char* fragmentShaderCode = fragmentShaderCodeStr.c_str();
	fragmentShaderInputStream.close();

	GLint success;
	GLchar infoLogBuffer[1024];
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderCode, 0);
	glShaderSource(fragmentShader, 1, &fragmentShaderCode, 0);

	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 1024, 0, infoLogBuffer);
		std::cout << "Error compiling vertex shader: " << infoLogBuffer << std::endl;
	}

	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 1024, 0, infoLogBuffer);
		std::cout << "Error compiling fragment shader: " << infoLogBuffer << std::endl;
	}

	this->shaderProgram = glCreateProgram();

	glAttachShader(this->shaderProgram, vertexShader);
	glAttachShader(this->shaderProgram, fragmentShader);
	glLinkProgram(this->shaderProgram);

	glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(this->shaderProgram, 1024, 0, infoLogBuffer);
		std::cout << "Error linking program: " << infoLogBuffer << std::endl;
	}

	this->type = type;
}

Shader::~Shader()
{

}

GLuint Shader::getProgram() const
{
	return this->shaderProgram;
}

void Shader::useProgram() const
{
	glUseProgram(this->shaderProgram);
}

ShaderType Shader::getType() const
{
	return this->type;
}