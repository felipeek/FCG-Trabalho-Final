#include "Skybox.h"
#include "Model.h"
#include "StaticModels.h"

using namespace raw;

Skybox::Skybox()
{
	const float skyBoxSize = 50.0f;

	CubeMapTexturePath cubeMapDayTexturePath;
	CubeMapTexturePath cubeMapNightTexturePath;
	cubeMapDayTexturePath.right = ".\\res\\skybox\\right.png";
	cubeMapDayTexturePath.left = ".\\res\\skybox\\left.png";
	cubeMapDayTexturePath.top = ".\\res\\skybox\\top.png";
	cubeMapDayTexturePath.bottom = ".\\res\\skybox\\bottom.png";
	cubeMapDayTexturePath.front = ".\\res\\skybox\\front.png";
	cubeMapDayTexturePath.back = ".\\res\\skybox\\back.png";
	cubeMapNightTexturePath.right = ".\\res\\skybox\\dright.png";
	cubeMapNightTexturePath.left = ".\\res\\skybox\\dleft.png";
	cubeMapNightTexturePath.top = ".\\res\\skybox\\dtop.png";
	cubeMapNightTexturePath.bottom = ".\\res\\skybox\\dbottom.png";
	cubeMapNightTexturePath.front = ".\\res\\skybox\\dfront.png";
	cubeMapNightTexturePath.back = ".\\res\\skybox\\dback.png";
	this->skyboxDayTexture = new CubeMapTexture(cubeMapDayTexturePath);
	this->skyboxNightTexture = new CubeMapTexture(cubeMapNightTexturePath);

	this->cubeModel = new Model(std::vector<Mesh*>({ StaticModels::getCubeMesh(skyBoxSize, 0, 0, 0, 0.0f) }));
	this->cube = new Entity(this->cubeModel);

	this->nightSkyColor = glm::vec4(30.0f / 255, 33.0f / 255, 37.0f / 255, 1.0f);
	this->daySkyColor = glm::vec4(90.0f / 255, 121.0f / 255, 144.0f / 255, 1.0f);

	this->bNight = true;
}

Skybox::~Skybox()
{
	delete this->skyboxDayTexture;
	delete this->skyboxNightTexture;
	delete this->cubeModel;
	delete this->cube;
}

void Skybox::render(const Shader& shader, const Camera& camera) const
{
	shader.useProgram();

	if (this->bNight)
		this->skyboxNightTexture->bind(GL_TEXTURE0);
	else
		this->skyboxDayTexture->bind(GL_TEXTURE0);

	GLuint cubeMapLocation = glGetUniformLocation(shader.getProgram(), "cubeMap");
	glUniform1i(cubeMapLocation, 0);

	this->cube->render(shader, camera);
}

glm::vec4 Skybox::getSkyColor() const
{
	if (this->bNight)
		return this->nightSkyColor;
	else
		return this->daySkyColor;
}

bool Skybox::isNight() const
{
	return this->bNight;
}

void Skybox::setNight(bool night)
{
	this->bNight = night;
}