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

namespace FlawedEngine
{
	typedef unsigned int uint32_t;

	class cModelRenderer
	{
	public:
		void Draw(Transform& Trans, sMaterial& Mat);
		void Init(std::vector<sVertex>& Vertecies, std::vector<sTexture>& Textures, std::vector<uint32_t>& Indices);

	public:
		std::vector<sVertex> mVertecies;
		std::vector<uint32_t> mIndices;
		std::vector<sTexture> mTextures;
		uint32_t VAO; 

	private:
		void Setup();
	
	private:
		uint32_t VBO, EBO;
		cShader Shader;
	};
}