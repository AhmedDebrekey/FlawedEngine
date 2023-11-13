#pragma once

#include <memory>
#include "Scene.h"
#include "Window.h"
#include "UIManager.h"
#include "Physics/Physics.h"
#include "Graphics/GraphicsAPI.h"
#include "Graphics/OpenGLAPI.h"

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
		cUIManager& UI = cUIManager::get();
		cPhysics Physics;//make into singleton

		std::vector<std::shared_ptr<cScene>> mScenes;
		std::shared_ptr<cScene> mActiveScene;
		std::shared_ptr<cGraphicsAPI> mGraphics_API;
		void* mPhysicsWorld = nullptr;
	};
}