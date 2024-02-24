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
		mCamFrustum = CreateFrustum();
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

		mCamFrustum = CreateFrustum();

		if (ImGui::IsMouseDown(1))
		{
			if (FirstMouse)
			{
				LastX = xPos;
				LastY = yPos;
				FirstMouse = false;
			}
			float MouseSpeed = 0.0f;
			MouseSpeed = mMouseSpeed;
			mHorizontalAngle += MouseSpeed * float(LastX - xPos);
			mVerticalAngle += MouseSpeed * float(LastY - yPos);
		}

		LastX = xPos;
		LastY = yPos;

		mDirection = {
			cos(mVerticalAngle) * sin(mHorizontalAngle),
			sin(mVerticalAngle),
			cos(mVerticalAngle) * cos(mHorizontalAngle)
		};

		mRight = glm::vec3(
			sin(mHorizontalAngle - 3.14f / 2.0f),
			0,
			cos(mHorizontalAngle - 3.14f / 2.0f)
		);

		if (mVerticalAngle >= 1.0f) mVerticalAngle = 1.0f;
		if (mVerticalAngle <= -1.0f) mVerticalAngle = -1.0f;

		mUp = glm::cross(mRight, mDirection);
		glm::vec3 Upwards = glm::vec3(0.0f, 1.0f, 0.0f);
		if (isInputEnabled)
		{
			if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_W)) { mPostion += mDirection * DeltaTime * mSpeed; }

			if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_W)) mPostion += mDirection * DeltaTime * mSpeed;
			if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_S)) mPostion -= mDirection * DeltaTime * mSpeed;

			if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_D)) mPostion += mRight * DeltaTime * mSpeed;
			if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_A)) mPostion -= mRight * DeltaTime * mSpeed;

			if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_E)) mPostion += Upwards * DeltaTime * mSpeed;
			if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Q)) mPostion -= Upwards * DeltaTime * mSpeed;
		}
		
		float FoV = mFOV;

		if(DirLightPerspective)
		{
			mProjectionMatrix = glm::perspective(glm::radians(FoV), mAspectRatio, near, far);
			mViewMatrix = glm::lookAt(
				mPostion,
				mPostion + mDirection,
				mUp
			);
		}
		else
		{
			mProjectionMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
			mViewMatrix = glm::lookAt(GetDirectionalLightPos(), glm::vec3(1.0f), glm::vec3(0.0, 1.0, 0.0));
		}


		LastTime = CurrentTime;
	}

	Frustum cpCamera::CreateFrustum()
	{
		Frustum     frustum;
		const float halfVSide = far * tanf(glm::radians(mFOV * .5f));
		const float halfHSide = halfVSide * mAspectRatio;
		const glm::vec3 frontMultFar = far * mDirection;

		frustum.nearFace	= Plane(mPostion + near * mDirection, mDirection);
		frustum.farFace		= Plane(mPostion + frontMultFar, -mDirection );
		frustum.rightFace	= Plane(mPostion, glm::cross(frontMultFar - mRight * halfHSide, mUp));
		frustum.leftFace	= Plane(mPostion, glm::cross(mUp, frontMultFar + mRight * halfHSide));
		frustum.topFace		= Plane(mPostion, glm::cross(mRight, frontMultFar - mUp * halfVSide));
		frustum.bottomFace	= Plane(mPostion, glm::cross(frontMultFar + mUp * halfVSide, mRight));
		return frustum;
	}

	glm::mat4	cpCamera::Projection()						{ return mProjectionMatrix; }
	glm::mat4	cpCamera::View()							{ return mViewMatrix; }
	glm::vec3	cpCamera::Postion()							{ return mPostion; }
	glm::vec3	cpCamera::Front()							{ return mDirection; }
	float		cpCamera::FoV()								{ return mFOV;}
	void		cpCamera::SetAspectRatio(float ratio)		{ mAspectRatio = ratio; }
}