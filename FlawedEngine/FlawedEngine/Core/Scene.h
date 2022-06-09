#pragma once

#include "Entity.h"

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

		cScene();
		~cScene();
		void Setup();
		void Render();
		void LoadModel(const char* FilePath);
		void LoadModel(eBasicObject Object);

	private:
		std::vector<cEntity*> WorldEntities; //TODO: Make this into a "non"-array type, better to have an unordered map
	};
}
