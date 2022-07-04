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
		LoadModel(Triangle, "Triangle"); //Maybe can get a shared pointer instead . . . It would be used to make transformations 
		LoadModel(Cube, "Cube");
		LoadModel(Cube, "Debreky");
	}

	void cScene::Render()
	{
		Camera.Compute();
		Transform tCamera { Camera.Projection(), Camera.View()};

		std::shared_ptr<cEntity> Triangle = GetObjectByName("Triangle");
		sModel TriangleModel = { glm::vec3(3.0f, 5.0f, 0.0f), glm::vec3(45.0f), glm::vec3(5.0f) };
		Triangle->ModelTransform(TriangleModel);

		sModel CubeModel = { glm::vec3(-1.0f, 1.0f, 1.0f) };
		GetObjectByName("Cube")->ModelTransform(CubeModel); // Unsafe since it can be a nullptr

		glm::vec3 DebrekyTranslation = glm::vec3(10.0f, 5.0f, 6.0f);
		sModel DebrekyModel = { glm::vec3(10.0f, 5.0f, 6.0f), glm::vec3(45.0f, 180.0f, 65.0f)};
		GetObjectByName("Debreky")->ModelTransform(DebrekyModel);
		
		//Render Models
		for (auto Entities : WorldEntities)
		{
			Entities.second->Render(tCamera);
		}
	}

	void cScene::LoadModel(const char* FilePath, const char* Name)
	{
		//TODO: Implement a 3D Model Loader
	}

	void cScene::LoadModel(eBasicObject Object, const char* Name)
	{
		switch (Object)
		{
		case FlawedEngine::cScene::Cube:
			{
				WorldEntities[Name] = std::make_shared<cCube>();
			}
			break;
		case FlawedEngine::cScene::Sphere:
			break;
		case FlawedEngine::cScene::Triangle:
			{
				WorldEntities[Name] = std::make_shared<cTriangle>();
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

	std::shared_ptr<cEntity> FlawedEngine::cScene::GetObjectByName(const char* Name)
	{
		auto Entity = WorldEntities.find(Name);

		if (Entity == WorldEntities.end())
			return nullptr;

		return Entity->second;;
	}
}