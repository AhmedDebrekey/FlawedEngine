#include "Engine.h"

namespace FlawedEngine
{
	cEngine::cEngine()
	{
		EngineWindow.Init();
		EngineWindow.CreateWindow();
		
		UI.Init(EngineWindow.GetWindow());
		PhysicsWorld = Physics.Init();

		{
			Scenes.push_back(std::make_shared<cScene>(EngineWindow.GetWindow(), PhysicsWorld));
		}
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
	}

	cEngine::~cEngine()
	{

	}

}