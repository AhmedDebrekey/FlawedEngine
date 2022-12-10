#include "PerspectiveCamera.h"

#include <GLFW/glfw3.h>

namespace FlawedEngine
{
	cpCamera::cpCamera(void* Window)
		:mWindow(Window)/*VOID PTR, convert to GLFWwindow later*/, mProjectionMatrix(glm::mat4(1.0f)), mViewMatrix(glm::mat4(1.0f))
	{
		int Width, Height;
		auto GLFW_Window = (GLFWwindow*)mWindow;
		glfwGetWindowSize(GLFW_Window, &Width, &Height);
		glfwSetCursorPos(GLFW_Window, double(Width/2), double(Height/2));
	}

	cpCamera::~cpCamera()
	{
	}

	void cpCamera::Compute()
	{
		auto GLFW_Window = (GLFWwindow*)mWindow;
		static double LastTime = glfwGetTime();
		double CurrentTime = glfwGetTime();
		float DeltaTime = float(CurrentTime - LastTime);

		double xPos, yPos;
		glfwGetCursorPos(GLFW_Window, &xPos, &yPos);

		GLFWmonitor* Monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* Mode = glfwGetVideoMode(Monitor);

		if (glfwGetKey(GLFW_Window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			float MouseSpeed = 0.0f;
			glfwSetCursorPos(GLFW_Window, double(Mode->width / 2), double(Mode->height / 2));
			glfwSetInputMode(GLFW_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			MouseSpeed = mMouseSpeed;
			mHorizontalAngle += MouseSpeed * float(Mode->width / 2 - xPos);
			mVerticalAngle += MouseSpeed * float(Mode->height / 2 - yPos);
		}
		if (glfwGetKey(GLFW_Window, GLFW_KEY_SPACE) == GLFW_RELEASE)
		{
			glfwSetInputMode(GLFW_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		mDirection = {
			cos(mVerticalAngle) * sin(mHorizontalAngle),
			sin(mVerticalAngle),
			cos(mVerticalAngle) * cos(mHorizontalAngle)
		};

		glm::vec3 Right = glm::vec3(
			sin(mHorizontalAngle - 3.14f / 2.0f),
			0,
			cos(mHorizontalAngle - 3.14f / 2.0f)
		);

		if (mVerticalAngle >= 1.0f) mVerticalAngle = 1.0f;
		if (mVerticalAngle <= -1.0f) mVerticalAngle = -1.0f;

		glm::vec3 Up = glm::cross(Right, mDirection);
		glm::vec3 Upwards = glm::vec3(0.0f, 1.0f, 0.0f);

		if (glfwGetKey(GLFW_Window, GLFW_KEY_W) == GLFW_PRESS) mPostion += mDirection * DeltaTime * mSpeed;
		if (glfwGetKey(GLFW_Window, GLFW_KEY_S) == GLFW_PRESS) mPostion -= mDirection * DeltaTime * mSpeed;

		if (glfwGetKey(GLFW_Window, GLFW_KEY_D) == GLFW_PRESS) mPostion += Right * DeltaTime * mSpeed;
		if (glfwGetKey(GLFW_Window, GLFW_KEY_A) == GLFW_PRESS) mPostion -= Right * DeltaTime * mSpeed;

		if (glfwGetKey(GLFW_Window, GLFW_KEY_E) == GLFW_PRESS) mPostion += Upwards * DeltaTime * mSpeed;
		if (glfwGetKey(GLFW_Window, GLFW_KEY_Q) == GLFW_PRESS) mPostion -= Upwards * DeltaTime * mSpeed;

		float FoV = mFOV;
		mProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 200.f);
		mViewMatrix = glm::lookAt(
			mPostion,
			mPostion + mDirection,
			Up
		);

		LastTime = CurrentTime;
	}
	glm::mat4 cpCamera::Projection() { return mProjectionMatrix; }
	glm::mat4 cpCamera::View() { return mViewMatrix; }
	glm::vec3 cpCamera::Postion() { return mPostion; }
	glm::vec3 cpCamera::Front() { return mDirection; }
	float cpCamera::FoV() {	return mFOV;	}
	void cpCamera::UpdateProjection(glm::mat4 Proj) { mProjectionMatrix = Proj; }
}