#pragma once

#include "Entity.h"
#include "PerspectiveCamera.h"
#include <unordered_map>

namespace FlawedEngine
{
	class cScene
	{
	public:

		enum eBasicObject
		{
			Cube = 0,
			Sphere,
			Triangle,
			PointLight,
			SpotLight
		}; 

		cScene(void* Window);
		~cScene();
		void Setup();
		void Render();
		void LoadModel(const char* FilePath, const char* Name);
		void LoadModel(eBasicObject Object, const char* Name);

		std::shared_ptr<cEntity> GetObjectByName(const char* Name);

	private:
		std::unordered_map<std::string, std::shared_ptr<cEntity>> WorldEntities;
		void* mWindow;
		cpCamera Camera;		
	};
}