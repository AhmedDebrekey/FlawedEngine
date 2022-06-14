#pragma once

#ifndef OPENGL
#define OPENGL //TODO: Add this to the renderer class to be deterimned there.
#endif
//there is going to be other API's in the future
#ifdef OPENGL 
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif

namespace FlawedEngine
{

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
		void PollEvents();
		void EndFrame();

		bool ShouldClose();
		
		void UpdateViewPort(uint32_t Width, uint32_t Height);
		struct sWindowSize GetWindowSize();

		GLFWwindow* GetWindow() { return mWindow; }

	private:
		GLFWwindow* mWindow;
		uint32_t mWidth = 1600, mHeight = 900;

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