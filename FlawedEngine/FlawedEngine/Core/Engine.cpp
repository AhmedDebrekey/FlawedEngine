#include "Engine.h"

namespace FlawedEngine
{
	cEngine::cEngine()
	{
		EngineWindow.Init();
		EngineWindow.CreateWindow();
		
		UI.Init(EngineWindow.GetWindow());

		{
			Scenes.push_back(std::make_shared<cScene>(EngineWindow.GetWindow()));
		}
	}

	void cEngine::Run()
	{
		while (!EngineWindow.ShouldClose())
		{
			EngineWindow.Update();
			
			UI.UpdateUI();
			
			OnEvent();
			
			for (auto Scene : Scenes)//TODO: Switch from a for loop to single based scene.
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
}