#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
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
	private:
		void* mWindow;
		glm::mat4 mProjectionMatrix;
		glm::mat4 mViewMatrix;

		glm::vec3 mPostion = glm::vec3(0, 0, -3);
		glm::vec3 mDirection = glm::vec3(0);
		
		float mHorizontalAngle = 1.0f;
		float mVerticalAngle = 0.0f;

		float mFOV = 80.0f;

		float mSpeed = 10.f;
		float mMouseSpeed = 0.003f;

	};
}