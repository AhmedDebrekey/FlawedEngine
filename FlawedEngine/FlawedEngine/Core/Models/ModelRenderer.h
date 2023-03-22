#pragma once

#ifndef OPENGL
#define OPENGL //TODO: Add this to the renderer class to be deterimned there.
#endif

#ifdef OPENGL 
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif

#include <vector>

#include "../Entity.h"
#include "../Shader.h"

#include <unordered_map>

namespace FlawedEngine
{
	typedef unsigned int uint32_t;

	class cModelRenderer
	{
	public:
		void Draw(sTransform& Trans, sMaterial& Mat, std::unordered_map<std::string, sLight>& Lights, uint32_t* SkyBox);

		void Init(std::vector<sVertex>& Vertecies, std::vector<sTexture>& Textures, std::vector<uint32_t>& Indices);

	public:
		std::vector<sVertex> mVertecies;
		std::vector<uint32_t> mIndices;
		std::vector<sTexture> mTextures;
		uint32_t VAO; 

	private:
		void Setup();
	
	private:
		uint32_t VBO, EBO, DirectionalLightUBO;
		std::vector<glm::vec4> DirectionalLights
		{
			glm::vec4(30.0f, 10.0f, 30.0f, 1.0f),    // Direction
			glm::vec4(0.2f, 0.2f, 0.2f, 1.0f),        // Ambient
			glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),        // Diffuse
			glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)         // Specular
		};
		cShader Shader;
	};
}