#include <stdio.h>
#include "Window.h"

namespace FlawedEngine
{
	cWindow* cWindow::sWindowInstance = nullptr;

	cWindow::cWindow()
		:mWindow(nullptr)
	{

	}

	cWindow::~cWindow()
	{
	#ifdef OPENGL
		glfwDestroyWindow(mWindow);
		glfwTerminate();
	#endif

	}

	cWindow& cWindow::get()
	{
		if (sWindowInstance == nullptr)
		{
			sWindowInstance = new cWindow();
		}
		return *sWindowInstance;
	}

	void cWindow::Init()
	{
	#ifdef OPENGL 
		glfwInit();
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#endif
	}

	int cWindow::CreateWindow()
	{
	#ifdef OPENGL
		mWindow = glfwCreateWindow(mWidth, mHeight, "FlawedEngine", NULL, NULL);

		if (!mWindow)
		{
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(mWindow);
		gladLoadGL();
		UpdateViewPort(mWidth, mHeight);
		glEnable(GL_DEPTH_TEST);
	#endif
	}

	void cWindow::UpdateViewPort(uint32_t Width, uint32_t Height)
	{
		mWidth = Width;
		mHeight = Height;

	#ifdef OPENGL
		glViewport(0, 0, mWidth, mHeight);
	#endif
	}

	sWindowSize cWindow::GetWindowSize()
	{
		return { mWidth, mHeight };
	}

	void cWindow::Update()
	{
	#ifdef OPENGL
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		int tempWidth, tempHeight;
		glfwGetWindowSize(mWindow, &tempWidth, &tempHeight);

		if (tempWidth != mWidth || tempHeight != mHeight)
		{
			UpdateViewPort(tempWidth, tempHeight);
		}
	#endif
	}

	void cWindow::PollEvents()
	{
		glfwPollEvents();
	}

	void cWindow::EndFrame()
	{
		glfwSwapBuffers(mWindow);
		glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
	}

	bool cWindow::ShouldClose()
	{
		bool bShouldClose;
	#ifdef OPENGL
		bShouldClose = /*glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS || */glfwWindowShouldClose(mWindow);
	#endif
		return bShouldClose;
	}
}