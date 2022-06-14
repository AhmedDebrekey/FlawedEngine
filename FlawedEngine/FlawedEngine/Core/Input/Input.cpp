#include "Input.h"
#include <GLFW/glfw3.h>

namespace FlawedEngine
{
	cInput* cInput::sInputInstance = nullptr;

	cInput::cInput()
		:mKeyPressed(0)
	{
	}

	cInput::~cInput()
	{

	}

	cInput& cInput::get()
	{
		if (sInputInstance == nullptr)
		{
			sInputInstance = new cInput();
		}
		return *sInputInstance;
	}


	void cInput::Poll(void* Window_)
	{
		auto Window = (GLFWwindow*)Window_;
		//this could be done in a better way
		if (glfwGetKey(Window, GLFW_KEY_I) == GLFW_PRESS) { mKeyPressed = GLFW_KEY_I; SetKeyPressed(GLFW_KEY_I); }

	}

	void cInput::SetKeyPressed(int Key) { mKeyPressed = Key; }

	int cInput::GetKeyPressed() { return mKeyPressed; }
}
