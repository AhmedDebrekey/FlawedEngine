#include "Scene.h"

#include "BasicModel/Triangle.h"

namespace FlawedEngine
{
	cScene::cScene()
	{
		//Should have more stuff in the future probably, IDK little secret, I am just winging it, I don't really know what I am doing.
		Setup();
	}

	cScene::~cScene()
	{

	}

	void cScene::Setup()
	{
		LoadModel(Triangle);
	}

	void cScene::Render()
	{
		//Render Models
		for (auto Entity : WorldEntities)
		{
			Entity->Render();
			//Entity->GetInfo();
		}
	}

	void cScene::LoadModel(const char* FilePath)
	{
		//TODO: Implement a 3D Model Loader
	}

	void cScene::LoadModel(eBasicObject Object)
	{
		switch (Object)
		{
		case FlawedEngine::cScene::Cube:
			break;
		case FlawedEngine::cScene::Sphere:
			break;
		case FlawedEngine::cScene::Triangle:
			{
				WorldEntities.push_back(std::make_shared<cTriangle>());
			}
			break;
		case FlawedEngine::cScene::PointLight:
			break;
		case FlawedEngine::cScene::SpotLight:
			break;
		default:
			break;
		}
	}
}