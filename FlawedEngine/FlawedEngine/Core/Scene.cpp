#include "Scene.h"

#include "Shader.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_inverse.hpp>
namespace FlawedEngine
{
	cScene::cScene(void* Window, void* Physics, btAlignedObjectArray<btCollisionShape*>* CollisionShapes, void* Graphic_API)
		:mWindow(Window), Camera(mWindow), PhysicsWorld(Physics), mCollisionShapesArray(CollisionShapes), mGfxAPI(Graphic_API)
	{
		Setup();
	}

	cScene::~cScene()
	{
	}

	void cScene::Setup()
	{
		ObjectMan.Init(PhysicsWorld, mCollisionShapesArray, &Camera.mCamFrustum, mGfxAPI);
		ObjectMan.LoadSave("KnightAnim");
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

		/*glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
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
		glBindFramebuffer(GL_FRAMEBUFFER, mUIFrameBuffer->FBO);*/

		ObjectMan.RenderObjects(tCamera);
	}
}