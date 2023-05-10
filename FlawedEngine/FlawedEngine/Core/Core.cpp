#include "Core.h"

namespace FlawedEngine
{
	glm::vec3 DirectionalLightPos = glm::vec3(-2.0f, 10.0f, -1.0f);


	void SetDirectionalLightPos(const glm::vec3& Position)
	{
		DirectionalLightPos = Position;
	}

	glm::vec3& GetDirectionalLightPos()
	{
		return DirectionalLightPos;
	}
}