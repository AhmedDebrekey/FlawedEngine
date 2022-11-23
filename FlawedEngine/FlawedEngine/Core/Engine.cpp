#include "Engine.h"

namespace FlawedEngine
{
	cEngine::cEngine()
	{
		EngineWindow.Init();
		EngineWindow.CreateWindow();
		
		PhysicsWorld = Physics.Init();
		UI.Init(EngineWindow.GetWindow());
		btAlignedObjectArray<btCollisionShape*>* CollisionShapes = Physics.GetCollisionShapesArray();

		{
			Scenes.push_back(std::make_shared<cScene>(EngineWindow.GetWindow(), PhysicsWorld, CollisionShapes));
		}

		UI.SetSceneObjectsPointer(Scenes[0]->GetObjectsPointer()); //VERY BAD, should have CURRENT ACTIVE SCENE RETURNS THAT POINTER
		UI.SetObjectManager(Scenes[0]->GetObjectManager());
	}

	void cEngine::Run()
	{
		while (!EngineWindow.ShouldClose())
		{
			EngineWindow.Update();
			
			UI.UpdateUI();
			
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