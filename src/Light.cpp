#include "Light.h"

using namespace raw;

Light::Light()
{
}

Light::Light(glm::vec4 ambientColor, glm::vec4 diffuseColor, glm::vec4 specularColor)
{
	this->ambientColor = ambientColor;
	this->diffuseColor = diffuseColor;
	this->specularColor = specularColor;
}

Light::~Light()
{

}

void Light::setAmbientColor(const glm::vec4& ambientColor)
{
	this->ambientColor = ambientColor;
}

glm::vec4 Light::getAmbientColor() const
{
	return this->ambientColor;
}

void Light::setDiffuseColor(const glm::vec4& diffuseColor)
{
	this->diffuseColor = diffuseColor;
}

glm::vec4 Light::getDiffuseColor() const
{
	return this->diffuseColor;
}

void Light::setSpecularColor(const glm::vec4& specularColor)
{
	this->specularColor = specularColor;
}

glm::vec4 Light::getSpecularColor() const
{
	return this->specularColor;
}

LightType Light::getType() const
{
	return this->type;
}

// Send all information about the light to the shader, so it can be used when rendering.
void Light::updateUniforms(const Shader& shader, unsigned int arrayPosition) const
{
	char locationBuffer[1024];

	glm::vec4 lightAmbient = this->ambientColor;
	glm::vec4 lightDiffuse = this->diffuseColor;
	glm::vec4 lightSpecular = this->specularColor;
	LightType lightType = this->getType();

	this->getShaderLocationString("ambientColor", locationBuffer, arrayPosition);
	GLuint lightAmbientLocation = glGetUniformLocation(shader.getProgram(), locationBuffer);
	this->getShaderLocationString("diffuseColor", locationBuffer, arrayPosition);
	GLuint lightDiffuseLocation = glGetUniformLocation(shader.getProgram(), locationBuffer);
	this->getShaderLocationString("specularColor", locationBuffer, arrayPosition);
	GLuint lightSpecularLocation = glGetUniformLocation(shader.getProgram(), locationBuffer);
	this->getShaderLocationString("type", locationBuffer, arrayPosition);
	GLuint lightTypeLocation = glGetUniformLocation(shader.getProgram(), locationBuffer);

	glUniform4f(lightAmbientLocation, lightAmbient.x, lightAmbient.y, lightAmbient.z, lightAmbient.w);
	glUniform4f(lightDiffuseLocation, lightDiffuse.x, lightDiffuse.y, lightDiffuse.z, lightDiffuse.w);
	glUniform4f(lightSpecularLocation, lightSpecular.x, lightSpecular.y, lightSpecular.z, lightSpecular.w);
	glUniform1i(lightTypeLocation, lightType);
}

// This function will generate a string with the following format: "lights[arrayPosition].attribute"
// Where arrayPosition is a number, received as a paramater and attribute is a string, also received as a
// parameter. The result is stored in the buffer received as a parameter.
// The purpose of this function is to ease the creation of those strings, since they must be generated when
// sending light uniforms to the fragment shader.
// The shader has an array, named "lights", which each element is a LightDescriptor, a struct defined in
// the fragment shader that contains all informations about each light. The strings created with this
// function are in the correct format to access such array.
void Light::getShaderLocationString(char* attribute, char* buffer, unsigned int arrayPosition) const
{
	unsigned int bufferPosition = 0;
	const char shaderLightArray[] = "lights";
	char arrayPositionStr[32];
	sprintf_s(arrayPositionStr, "%d", arrayPosition);

	// Manually builds the location string.
	strcpy(buffer + bufferPosition, shaderLightArray);
	bufferPosition += sizeof(shaderLightArray) - 1;
	*(buffer + bufferPosition) = '[';
	++bufferPosition;
	strcpy(buffer + bufferPosition, arrayPositionStr);
	bufferPosition += strlen(arrayPositionStr);
	*(buffer + bufferPosition) = ']';
	*(buffer + bufferPosition + 1) = '.';
	bufferPosition += 2;
	strcpy(buffer + bufferPosition, attribute);
	bufferPosition += strlen(attribute);
	*(buffer + bufferPosition) = 0;
}