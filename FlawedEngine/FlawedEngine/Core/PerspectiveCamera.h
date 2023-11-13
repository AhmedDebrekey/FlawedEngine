#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Core.h"

namespace FlawedEngine
{


	class cpCamera
	{
	public:
		cpCamera(void* Window);
		~cpCamera();

		void Compute();

		glm::mat4 Projection();
		glm::mat4 View();
		glm::vec3 Postion();
		glm::vec3 Front();
		float NearPlane() { return near_plane; }
		float FarPlane() { return far_plane; }
		float FoV();
		void UpdateProjection(glm::mat4 Proj);
		void DisableInput() { isInputEnabled = false; }
		void EnableInput() { isInputEnabled = true; }
		void ToggleShadowPerspective() { DirLightPerspective = !DirLightPerspective; } // UI to change the perspective
		Frustum CreateFrustum();
		Frustum mCamFrustum;

	private:
		void* mWindow;
		glm::mat4 mProjectionMatrix;
		glm::mat4 mViewMatrix;

		float near_plane = .5f;
		float far_plane = 100.f;

		glm::vec3 mPostion = glm::vec3(-5, 3, -5);
		glm::vec3 mDirection = glm::vec3(0);
		glm::vec3 mRight = glm::vec3(0);
		glm::vec3 mUp = glm::vec3(0);
		
		float mHorizontalAngle = 0.0f;
		float mVerticalAngle = 0.0f;

		float mFOV = 80.0f;
		float aspect = 4.0f / 3.0f;
		float near = .1f, far = 10000.f;

		float mSpeed = 10.f;
		float mMouseSpeed = 0.003f;
		bool FirstMouse = true;
		bool isInputEnabled = true;
		float LastX = 1600, LastY = 900;
		bool DirLightPerspective = true;
	};
}
