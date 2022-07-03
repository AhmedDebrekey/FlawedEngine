#include "Scene.h"

#include "BasicModel/Triangle.h"
#include "BasicModel/Cube.h"

namespace FlawedEngine
{
	cScene::cScene(void* Window)
		:mWindow(Window), Camera(mWindow)
	{
		//Should have more stuff in the future probably, IDK little secret, I am just winging it, I don't really know what I am doing.
		Setup();
	}

	cScene::~cScene()
	{

	}

	void cScene::Setup()
	{
		LoadModel(Triangle); //Maybe can get a shared pointer instead . . . It would be used to make transformations 
		LoadModel(Cube);
	}

	void cScene::Render()
	{
		Camera.Compute();
		Transform tCamera { Camera.Projection(), Camera.View()};
		
		//Render Models
		for (auto Entity : WorldEntities)
		{
			Entity->Render(tCamera);
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
			{
				WorldEntities.push_back(std::make_shared<cCube>());
			}
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