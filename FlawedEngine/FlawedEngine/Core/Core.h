#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace FlawedEngine
{
	typedef unsigned int uint32_t;
#define MAX_BONE_INFLUENCE 4

	struct Transform
	{
		glm::mat4 Projection = glm::mat4(1.0f);
		glm::mat4 View = glm::mat4(1.0f);
		glm::mat4 Model = glm::mat4(1.0f);
	};

	struct sVertex
	{
		glm::vec3 Postion;
		glm::vec3 Normal;
		glm::vec2 TexCoords;

		glm::vec3 Tangent;
		glm::vec3 BiTangent;

		int mBoneIDs[MAX_BONE_INFLUENCE];
		float mWeights[MAX_BONE_INFLUENCE];
	};

	struct sTexture
	{
		uint32_t ID;
		std::string Type;
		std::string Path;
	};
}