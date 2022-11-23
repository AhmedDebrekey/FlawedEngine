#include "Scene.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>

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
		ObjectMan.Init(PhysicsWorld, mCollisionShapesArray);
		mSceneObjects = ObjectMan.GetObjectsPointer();
	}

	void cScene::Render()
	{
		Camera.Compute();
		sTransform tCamera { Camera.Postion() , Camera.Front(), Camera.Projection(), Camera.View()};

		ObjectMan.RenderObjects(tCamera);
	}
}