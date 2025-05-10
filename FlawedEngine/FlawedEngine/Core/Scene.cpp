#include "Scene.h"

#include "Shader.h"

#include "Graphics/GraphicsAPI.h"
#include "UIManager.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_inverse.hpp>
namespace FlawedEngine
{
	cScene::cScene(void* Window, void* Physics, btAlignedObjectArray<btCollisionShape*>* CollisionShapes, void* Graphic_API)
		:mWindow(Window), PhysicsWorld(Physics), mCollisionShapesArray(CollisionShapes), mGfxAPI(Graphic_API)
	{
		Setup();
	}

	cScene::~cScene()
	{
	}

	void cScene::Setup()
	{
		ObjectMan.Init(PhysicsWorld, mCollisionShapesArray, &Camera.mCamFrustum, mGfxAPI);
		Camera.InitCamera(mWindow);
	}

	bool cScene::isKeyDown(int key)
	{
		return glfwGetKey((GLFWwindow*)mWindow, key) == GLFW_PRESS;
	}


	void cScene::Render()
	{
		Camera.Compute();

		sTransform tCamera { Camera.Postion() , Camera.Front(), Camera.Projection(), Camera.View()};
		cUIManager& ui = cUIManager::get();
		mGeometryObject = ui.GetGeometryBuffer();

		ObjectMan.RenderObjects(tCamera, &mGeometryObject);
	}
}