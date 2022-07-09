#pragma once

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <string>
#include <vector>

namespace FlawedEngine
{
	typedef unsigned int uint32_t;
#define MAX_BONE_INFLUENCE 4

	struct sMaterial
	{
		glm::vec3 Color = glm::vec3(0.5);
		glm::vec3 Diffuse = glm::vec3(0.5f);
		glm::vec3 Specular = glm::vec3(0.5f);
		float Shininess = 32.0f;
		float Roughness;
	};

	struct sLight
	{
		glm::vec3 position;

		float constant;
		float linear;
		float quadratic;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	struct sModel
	{
		glm::vec3 Translation = glm::vec3(0.0f);
		glm::vec3 Rotation = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);
	};

	struct Transform
	{
		glm::vec3 Position;
		glm::vec3 Front;
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