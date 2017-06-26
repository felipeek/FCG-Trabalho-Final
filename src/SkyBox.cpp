#include "Skybox.h"
#include "Model.h"
#include "StaticModels.h"

using namespace raw;

Skybox::Skybox()
{
	const float skyBoxSize = 50.0f;

	CubeMapTexturePath cubeMapTexturePath;
	//cubeMapTexturePath.right = ".\\res\\skybox\\right.png";
	//cubeMapTexturePath.left = ".\\res\\skybox\\left.png";
	//cubeMapTexturePath.top = ".\\res\\skybox\\top.png";
	//cubeMapTexturePath.bottom = ".\\res\\skybox\\bottom.png";
	//cubeMapTexturePath.front = ".\\res\\skybox\\front.png";
	//cubeMapTexturePath.back = ".\\res\\skybox\\back.png";
	cubeMapTexturePath.right = ".\\res\\skybox\\dright.png";
	cubeMapTexturePath.left = ".\\res\\skybox\\dleft.png";
	cubeMapTexturePath.top = ".\\res\\skybox\\dtop.png";
	cubeMapTexturePath.bottom = ".\\res\\skybox\\dbottom.png";
	cubeMapTexturePath.front = ".\\res\\skybox\\dfront.png";
	cubeMapTexturePath.back = ".\\res\\skybox\\dback.png";
	this->skyboxTexture = new CubeMapTexture(cubeMapTexturePath);
	
	Model* cubeModel = new Model(std::vector<Mesh*>({ StaticModels::getCubeMesh(skyBoxSize, 0, 0, 0, 0.0f) }));
	this->cube = new Entity(cubeModel);
}

Skybox::~Skybox()
{
	delete this->skyboxTexture;
	delete this->cube->getModel();
	delete this->cube;
}

void Skybox::render(const Shader& shader, const Camera& camera) const
{
	shader.useProgram();

	this->skyboxTexture->bind(GL_TEXTURE0);
	GLuint cubeMapLocation = glGetUniformLocation(shader.getProgram(), "cubeMap");
	glUniform1i(cubeMapLocation, 0);

	this->cube->render(shader, camera);

}