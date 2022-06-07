#include "Scene.h"

namespace FlawedEngine
{
	cScene::cScene()
	{

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
		//Render 3D Models
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