#pragma once

#include <memory>
#include "Window.h"
#include "Scene.h"
#include "UIManager.h"
#include "Physics/Physics.h"	

namespace FlawedEngine
{
	class cEngine
	{
	public:

		cEngine();
		~cEngine();

		void Run();
	
	private: 

		void OnEvent();

	private:
		cWindow& EngineWindow = cWindow::get();
		
		cUIManager UI;//make into singleton

		cPhysics Physics;//make into singleton

		std::vector<std::shared_ptr<cScene>> Scenes;

		void* PhysicsWorld = nullptr;
	};
}