#include "PerspectiveCamera.h"

#include <GLFW/glfw3.h>
#include <ImGui/imgui.h>

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

		if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Space))
		{
			if (FirstMouse)
			{
				LastX = xPos;
				LastY = yPos;
				FirstMouse = false;
			}
			float MouseSpeed = 0.0f;
			glfwSetInputMode(GLFW_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			MouseSpeed = mMouseSpeed;
			mHorizontalAngle += MouseSpeed * float(LastX - xPos);
			mVerticalAngle += MouseSpeed * float(LastY - yPos);
		}
		else
			glfwSetInputMode(GLFW_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);	

		LastX = xPos;
		LastY = yPos;

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

		if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_W)) { mPostion += mDirection * DeltaTime * mSpeed; }

		if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_W)) mPostion += mDirection * DeltaTime * mSpeed;
		if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_S)) mPostion -= mDirection * DeltaTime * mSpeed;

		if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_D)) mPostion += Right * DeltaTime * mSpeed;
		if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_A)) mPostion -= Right * DeltaTime * mSpeed;

		if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_E)) mPostion += Upwards * DeltaTime * mSpeed;
		if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Q)) mPostion -= Upwards * DeltaTime * mSpeed;

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