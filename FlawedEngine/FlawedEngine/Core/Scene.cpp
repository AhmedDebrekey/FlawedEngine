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
		glfwSetWindowUserPointer((GLFWwindow*)mWindow, this);
		ObjectMan.Init(PhysicsWorld, mCollisionShapesArray);
	}

	void cScene::Render()
	{
		Camera.Compute();
		sTransform tCamera { Camera.Postion() , Camera.Front(), Camera.Projection(), Camera.View()};

		auto AddObj = [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			cScene* scene = static_cast<cScene*>(glfwGetWindowUserPointer(window));
			if (key == GLFW_KEY_I && action == GLFW_PRESS)
			{
				scene->ObjectMan.AddObject(Cube, "Cube");
			}
			if (key == GLFW_KEY_O && action == GLFW_PRESS)
			{
				scene->ObjectMan.AddObject(Sphere, "Sphere");
			}
		};

		glfwSetKeyCallback((GLFWwindow*)mWindow, AddObj);

		ObjectMan.RenderObjects(tCamera);
	}



}