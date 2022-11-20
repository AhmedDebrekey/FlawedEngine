#pragma once

#include <memory>
#include "Window.h"
#include "Scene.h"
#include "UIManager.h"


namespace FlawedEngine
{
	class cEngine
	{
	public:

		cEngine();
		void Run();
	
	private: 

		void OnEvent();

	private:
		cWindow& EngineWindow = cWindow::get();
		std::vector<std::shared_ptr<cScene>> Scenes;
		cUIManager UI;
	};
}