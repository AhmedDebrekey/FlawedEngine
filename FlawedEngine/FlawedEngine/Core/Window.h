#pragma once

#include <stdio.h>

namespace FlawedEngine
{

#define OPENGL //TODO: Add this to the renderer class to be deterimned there.

//there is going to be other API's in the future
#ifdef OPENGL 
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif

	typedef unsigned int uint32_t;
	struct sWindowSize
	{
		uint32_t Width;
		uint32_t Height;
	};

	class cWindow
	{
	public:

		void Init();
		int CreateWindow();
		void Update();

		bool ShouldClose();
		
		void UpdateViewPort(uint32_t Width, uint32_t Height);
		struct sWindowSize GetWindowSize();

	private:
	#ifdef OPENGL 
		GLFWwindow* mWindow;
	#endif
		uint32_t mWidth = 800, mHeight = 800;

	public:
		static cWindow& get();
		~cWindow();

		cWindow(cWindow const&) = delete;
		void operator=(cWindow const&) = delete;

	private:
		static cWindow* sWindowInstance;

		cWindow();
		cWindow(cWindow&);
	};
}

