#include "Engine.h"

namespace FlawedEngine
{
	cEngine::cEngine()
	{
		EngineWindow.Init();
		EngineWindow.CreateWindow();
		
		PhysicsWorld = Physics.Init();

		{
			Scenes.push_back(std::make_shared<cScene>(EngineWindow.GetWindow(), PhysicsWorld, Physics.GetCollisionShapesArray()));
			ActiveScene = Scenes.at(Scenes.size() - 1);
		}

		UI.Init(EngineWindow.GetWindow(), ActiveScene->GetCamera(), ActiveScene->GetObjectManager());
		ActiveScene->SetSelectedEntity(UI.GetSelectedEntity());
	}

	void cEngine::Run()
	{
		float DeltaTime = 0;
		float LastFrame = 0;
		while (!EngineWindow.ShouldClose())
		{
			float currentFrame = glfwGetTime();
			DeltaTime = currentFrame - LastFrame;
			LastFrame = currentFrame;

			EngineWindow.Update();
			
			UI.UpdateUI();
			ActiveScene->UpdateViewport(UI.GetViewportSize(), UI.GetViewportPos());
			OnEvent();
			
			Physics.Update(DeltaTime);

			ActiveScene->Render();

			UI.RenderUI();

			EngineWindow.EndFrame();
		}
	}

	void cEngine::OnEvent()
	{
		EngineWindow.PollEvents();
		//To be continued
	}

	cEngine::~cEngine()
	{

	}

}