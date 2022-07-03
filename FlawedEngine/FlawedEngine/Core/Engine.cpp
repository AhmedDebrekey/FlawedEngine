#include "Engine.h"

namespace FlawedEngine
{
	cEngine::cEngine()
	{
		EngineWindow.Init();
		EngineWindow.CreateWindow();
		
		/*
			Creating a new scene here
			In the future, this should be controlled by the use in the GUI/IMGUI
		*/
		{
			Scenes.push_back(std::make_shared<cScene>(EngineWindow.GetWindow()));
		}
	}

	void cEngine::Run()
	{
		while (!EngineWindow.ShouldClose())
		{
			EngineWindow.Update(); 
			OnEvent();
			for (auto Scene : Scenes)//TODO: Switch from a for loop to single based scene.
			{
				Scene->Render();
			}
			//Physics update
			//Audio update
			//UI update
			//World update

			EngineWindow.EndFrame();
		}
	}

	void cEngine::OnEvent()
	{
		EngineWindow.PollEvents();
	}
}