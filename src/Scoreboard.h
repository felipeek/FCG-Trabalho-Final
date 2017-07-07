#pragma once

#include "Entity.h"
#include <vector>

namespace raw
{
	class Model;

	class Scoreboard
	{
	public:
		Scoreboard();
		~Scoreboard();
		void render(const Shader& shader, float windowRatio) const;
		void update(float deltaTime);
		void changeLeftScore(int score);
		void changeRightScore(int score);
	private:
		Model* createScoreboardModel(char* texPath);
		Entity* leftScore;
		Entity* rightScore;
		Entity* separator;
		std::vector<Model*> leftModels;
		std::vector<Model*> rightModels;

		const static glm::vec4 leftScorePosition;
		const static float leftScoreScale;
		const static glm::vec4 rightScorePosition;
		const static float rightScoreScale;
		const static glm::vec4 separatorPosition;
		const static float separatorScale;
	};
}