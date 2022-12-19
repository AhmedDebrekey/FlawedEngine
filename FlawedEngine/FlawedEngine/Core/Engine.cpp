#include "Engine.h"

namespace FlawedEngine
{
	cEngine::cEngine()
	{
		EngineWindow.Init();
		EngineWindow.CreateWindow();
		
		PhysicsWorld = Physics.Init();
		btAlignedObjectArray<btCollisionShape*>* CollisionShapes = Physics.GetCollisionShapesArray();

		{
			Scenes.push_back(std::make_shared<cScene>(EngineWindow.GetWindow(), PhysicsWorld, CollisionShapes));
		}

		//VERY BAD, should have CURRENT ACTIVE SCENE RETURNS THAT POINTER
		UI.Init(EngineWindow.GetWindow(), Scenes[0]->GetCamera());
		UI.SetObjectManager(Scenes[0]->GetObjectManager());
		Scenes[0]->SetSelectedEntity(UI.GetSelectedEntity());
	}

	void cEngine::Run()
	{
		while (!EngineWindow.ShouldClose())
		{
			EngineWindow.Update();
			
			UI.UpdateUI();
			Scenes[0]->UpdateViewport(UI.GetViewportSize(), UI.GetViewportPos());
			OnEvent();
			
			Physics.Update();

			for (auto Scene : Scenes)//TODO: Switch from a for loop to single based scene chosen from UI WAYYY later on. so don't create 2 scene hehe
			{
				Scene->Render();
			}

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