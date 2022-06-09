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

namespace FlawedEngine
{
	typedef unsigned int uint32_t;

	class cModelRenderer
	{
	public:
		cModelRenderer(std::vector<sVertex> Vertecies, std::vector<sTexture> Textures);

		std::vector<sVertex> mVertecies;
		std::vector<sTexture> mTextures;
		uint32_t VAO; 
		void Draw();
	private:
		
		uint32_t VBO, EBO;

		void Setup();

	
	};
}

