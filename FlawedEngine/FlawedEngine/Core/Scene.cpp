#include "Scene.h"

#include "BasicModel/Triangle.h"
#include "BasicModel/Cube.h"

#include "Models/OBJModel.h"

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
		LoadModel(Triangle, "Triangle");
		LoadModel(Cube, "Cube");
		LoadModel(Cube, "Debreky");
		LoadModel("Core\\Models\\OBJ\\DeformedBall\\Ball.obj", "Ball");
		LoadModel("Core\\Models\\OBJ\\Cone\\Cone.obj", "Cone");
	}

	void cScene::Render()
	{
		Camera.Compute();
		Transform tCamera { Camera.Projection(), Camera.View()};

		std::shared_ptr<cEntity> Triangle = GetObjectByName("Triangle");
		if (Triangle)
		{
			sModel TriangleModel = { glm::vec3(3.0f, 5.0f, 0.0f), glm::vec3(45.0f), glm::vec3(5.0f) };
			Triangle->ModelTransform(TriangleModel);
		}

		sModel CubeModel = { glm::vec3(-1.0f, 1.0f, 1.0f) };
		GetObjectByName("Cube")->ModelTransform(CubeModel); // Unsafe since it can be a nullptr

		sModel DebrekyModel = { glm::vec3(10.0f, 5.0f, 6.0f), glm::vec3(45.0f, 180.0f, 65.0f)};
		GetObjectByName("Debreky")->ModelTransform(DebrekyModel);

		sModel BallModel = { glm::vec3(15.0f, 2.0f, 4.0f), glm::vec3(0.0f), glm::vec3(2.0f) };
		GetObjectByName("Ball")->ModelTransform(BallModel);

		sModel ConeModel = { glm::vec3(-15.0f, 2.0f, 4.0f), glm::vec3(0.0f), glm::vec3(3.0f) };
		GetObjectByName("Cone")->ModelTransform(ConeModel);
		
		//Render Models
		for (auto Entities : WorldEntities)
		{
			Entities.second->Render(tCamera);
		}
	}

	void cScene::LoadModel(const char* FilePath, const char* Name)
	{
		WorldEntities[Name] = std::make_shared<cOBJModel>(FilePath);
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