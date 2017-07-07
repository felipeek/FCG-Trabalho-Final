#include "Scoreboard.h"
#include "Model.h"

using namespace raw;

const glm::vec4 Scoreboard::leftScorePosition = glm::vec4(-0.1f, 0.92f, 0.0f, 1.0f);
const float Scoreboard::leftScoreScale = 0.1f;
const glm::vec4 Scoreboard::rightScorePosition = glm::vec4(0.1f, 0.92f, 0.0f, 1.0f);
const float Scoreboard::rightScoreScale = 0.1f;
const glm::vec4 Scoreboard::separatorPosition = glm::vec4(0.0f, 0.92f, 0.0f, 1.0f);
const float Scoreboard::separatorScale = 0.1f;

Scoreboard::Scoreboard()
{
	// Create models
	this->leftModels.push_back(this->createScoreboardModel(".\\res\\score\\b0.png"));
	this->leftModels.push_back(this->createScoreboardModel(".\\res\\score\\b1.png"));
	this->leftModels.push_back(this->createScoreboardModel(".\\res\\score\\b2.png"));
	this->leftModels.push_back(this->createScoreboardModel(".\\res\\score\\b3.png"));
	this->leftModels.push_back(this->createScoreboardModel(".\\res\\score\\b4.png"));
	this->leftModels.push_back(this->createScoreboardModel(".\\res\\score\\b5.png"));
	this->rightModels.push_back(this->createScoreboardModel(".\\res\\score\\g0.png"));
	this->rightModels.push_back(this->createScoreboardModel(".\\res\\score\\g1.png"));
	this->rightModels.push_back(this->createScoreboardModel(".\\res\\score\\g2.png"));
	this->rightModels.push_back(this->createScoreboardModel(".\\res\\score\\g3.png"));
	this->rightModels.push_back(this->createScoreboardModel(".\\res\\score\\g4.png"));
	this->rightModels.push_back(this->createScoreboardModel(".\\res\\score\\g5.png"));
	Model* separatorModel = this->createScoreboardModel(".\\res\\score\\separator.png");

	// Create entities
	this->leftScore = new Entity(this->leftModels[0]);
	this->leftScore->getTransform().setWorldPosition(leftScorePosition);
	this->leftScore->getTransform().setWorldScale(glm::vec3(leftScoreScale));
	this->rightScore = new Entity(this->rightModels[0]);
	this->rightScore->getTransform().setWorldPosition(rightScorePosition);
	this->rightScore->getTransform().setWorldScale(glm::vec3(rightScoreScale));
	this->separator = new Entity(separatorModel);
	this->separator->getTransform().setWorldPosition(separatorPosition);
	this->separator->getTransform().setWorldScale(glm::vec3(separatorScale));
}

Scoreboard::~Scoreboard()
{
	// Delete models
	for (unsigned int i = 0; i < this->leftModels.size(); ++i)
		delete this->leftModels[i];
	for (unsigned int i = 0; i < this->rightModels.size(); ++i)
		delete this->rightModels[i];

	delete this->separator->getModel();

	// Delete entities
	delete this->leftScore;
	delete this->rightScore;
	delete this->separator;
}

void Scoreboard::render(const Shader& shader, float windowRatio) const
{
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	this->leftScore->render(shader, windowRatio);
	this->separator->render(shader, windowRatio);
	this->rightScore->render(shader, windowRatio);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void Scoreboard::update(float deltaTime)
{

}

void Scoreboard::changeLeftScore(int score)
{
	if (score >= 0 && score < this->leftModels.size())
		this->leftScore->setModel(this->leftModels[score]);
}

void Scoreboard::changeRightScore(int score)
{
	if (score >= 0 && score < this->rightModels.size())
		this->rightScore->setModel(this->rightModels[score]);
}

Model* Scoreboard::createScoreboardModel(char* texPath)
{
	return new Model(std::vector<Mesh*>({ new Quad(Texture::load(texPath)) }));
}