#include "Scene.h"

#include "Shader.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_inverse.hpp>
namespace FlawedEngine
{
	cScene::cScene(void* Window, void* Physics, btAlignedObjectArray<btCollisionShape*>* CollisionShapes)
		:mWindow(Window), Camera(mWindow), PhysicsWorld(Physics), mCollisionShapesArray(CollisionShapes)
	{
		Setup();
	}

	cScene::~cScene()
	{
	}

	void cScene::Setup()
	{
		ObjectMan.Init(PhysicsWorld, mCollisionShapesArray, &Camera.mCamFrustum);
		glfwSetWindowUserPointer((GLFWwindow*)mWindow, this);

		glGenFramebuffers(1, &mDepthMapFBO);

		glGenTextures(1, &mDepthMap);
		glBindTexture(GL_TEXTURE_2D, mDepthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glBindFramebuffer(GL_FRAMEBUFFER, mDepthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	bool cScene::isKeyDown(int key)
	{
		return glfwGetKey((GLFWwindow*)mWindow, key) == GLFW_PRESS;
	}

	void cScene::Render()
	{
		Camera.Compute();
		sTransform tCamera { Camera.Postion() , Camera.Front(), Camera.Projection(), Camera.View()};

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, mDepthMapFBO);

		glClear(GL_DEPTH_BUFFER_BIT);
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.f, far_plane = 100.f;

		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(GetDirectionalLightPos(), glm::vec3(1.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

		sTransform ShadowCam{ GetDirectionalLightPos(), glm::vec3(0.0f), lightProjection, lightView};

		glCullFace(GL_FRONT);
		ObjectMan.ShadowRender(ShadowCam, lightSpaceMatrix, mDepthMap);
		glCullFace(GL_BACK);

		glViewport(0, 0, mUIFrameBuffer->ViewportSize.x, mUIFrameBuffer->ViewportSize.y);
		glBindFramebuffer(GL_FRAMEBUFFER, mUIFrameBuffer->FBO);
		ObjectMan.RenderObjects(tCamera);

		double xpos, ypos;
		glfwGetCursorPos((GLFWwindow*)mWindow, &xpos, &ypos);
		CursorPos = { xpos, ypos };
			glfwSetMouseButtonCallback((GLFWwindow*)mWindow, [](GLFWwindow* window, int button, int action, int mods)
				{
					cScene* Scene = static_cast<cScene*>(glfwGetWindowUserPointer(window));

			ImGuiIO& io = ImGui::GetIO();
			if (action == GLFW_PRESS)
				io.MouseDown[button] = true;
			else if (action == GLFW_RELEASE)
				io.MouseDown[button] = false;

			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
			{
				// Mouse coordinates in window space
				float mouseX = Scene->CursorPos.x;
				float mouseY = Scene->CursorPos.y;

				// Projection and view matrices
				glm::mat4 projection = Scene->GetCamera()->Projection();
				glm::mat4 view = Scene->GetCamera()->View();

				// Camera position in world space
				glm::vec3 cameraPos = Scene->GetCamera()->Postion();

				float viewportX = Scene->ViewportPos.x; // The x coordinate of the top left corner of the viewport in pixels
				float viewportY = Scene->ViewportPos.y; // The y coordinate of the top left corner of the viewport in pixels
				// Viewport dimensions
				int viewportWidth = Scene->ViewportSize.x;
				int viewportHeight = Scene->ViewportSize.y;

				// Normalize the mouse position to the range [0, 1]

				float normalizedMouseX = (mouseX - viewportX) / viewportWidth;
				float normalizedMouseY = (mouseY - viewportY) / viewportHeight;

				// Convert the normalized mouse position to clip space
				glm::vec4 clipSpacePosition(
					2.0f * normalizedMouseX - 1.0f,
					1.0f - 2.0f * normalizedMouseY,
					0.1f,
					1.0f
				);

				// Convert the clip space position to eye space
				glm::mat4 inverseProjectionMatrix = glm::inverse(projection);
				glm::vec4 eyeSpacePosition = inverseProjectionMatrix * clipSpacePosition;

				// Convert the eye space position to world space
				glm::mat4 inverseViewMatrix = glm::inverse(view);
				glm::dvec4 worldSpacePosition = inverseViewMatrix * eyeSpacePosition;

				// Convert the world space position from homogeneous coordinates to Cartesian coordinates
				worldSpacePosition /= worldSpacePosition.w;

				// Convert the world space position from a dvec4 to a vec3
				glm::vec3 worldCoords(worldSpacePosition);

				glm::vec3 rayDir = worldCoords - cameraPos;
				rayDir = glm::normalize(rayDir);

				glm::vec3 RayTo = cameraPos + (50.f * rayDir);

				btDynamicsWorld* dynamicWorld = (btDynamicsWorld*)Scene->PhysicsWorld;

				btVector3 RayBegin(worldCoords.x, worldCoords.y, worldCoords.z);
				btVector3 RayEnd(RayTo.x, RayTo.y, RayTo.z);

				btCollisionWorld::ClosestRayResultCallback rayCallback(RayBegin, RayEnd);
				dynamicWorld->rayTest(RayBegin, RayEnd, rayCallback);
				if (Scene->ObjectMan.mMousePicking)
				{
					if (rayCallback.hasHit())
					{
						// The ray intersected an object
						const btCollisionObject* object = rayCallback.m_collisionObject;
						btVector3 intersectionPoint = rayCallback.m_hitPointWorld;
						btVector3 intersectionNormal = rayCallback.m_hitNormalWorld;

						const char* Name = static_cast<const char*>(object->getCollisionShape()->getUserPointer());
						*(Scene->mSelectedEntity) = Name;
					}
				}
			}
			//debug
			/*
			{
				Scene->ObjectMan.AddObject(Cube, "Ray Start");
				auto Entity = Scene->ObjectMan.GetObjectByName("Ray Start");
				sModel EntityModel = Entity->GetModel();
				EntityModel.Translation = worldCoords;
				EntityModel.Scale = glm::vec3(0.05);
				Entity->SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
				Entity->ModelTransform(EntityModel);
			}
			{
				Scene->ObjectMan.AddObject(Cube, "Ray end");
				auto Entity = Scene->ObjectMan.GetObjectByName("Ray end");
				sModel EntityModel = Entity->GetModel();
				EntityModel.Translation = RayTo;
				EntityModel.Scale = glm::vec3(0.05);
				Entity->SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
				Entity->ModelTransform(EntityModel);
			}
			*/
			});
	}
}