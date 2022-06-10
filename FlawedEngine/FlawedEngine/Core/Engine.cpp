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
			cScene NewScene;
			Scenes.push_back(std::make_shared<cScene>(NewScene));
		}
	}

	void cEngine::Run()
	{
		while (!EngineWindow.ShouldClose())
		{
			EngineWindow.Update();
			for (auto Scene : Scenes)
			{
				Scene->Render();
			}
			//Physics update
			//Audio update
			//UI update
			//World update

			OnEvent();
		}
	}

	void cEngine::OnEvent()
	{
		//add implementation
	}
}