#include "Scene.h"

#include "BasicModel/Triangle.h"
#include "BasicModel/Cube.h"
#include "BasicModel/PointLight.h"

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
		LoadModel("Core\\Models\\OBJ\\Cube\\Cube.obj", "Cube");
		LoadModel("Core\\Models\\OBJ\\Cone\\Cone.obj", "Cone");
		LoadModel(PointLight, "Light");
		LoadModel("Core\\Models\\OBJ\\Cube\\Cube.obj", "Ground");
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
			Triangle->SetColor(glm::vec3( 0.3f, 0.6f, 0.1f ));
		}

		sModel CubeModel = { glm::vec3(-1.0f, 1.0f, 1.0f) };
		GetObjectByName("Cube")->ModelTransform(CubeModel); // Unsafe since it can be a nullptr

		auto Cone = GetObjectByName("Cone");
		if (Cone)
		{
			sModel ConeModel = { glm::vec3(-15.0f, 2.0f, 4.0f), glm::vec3(0.0f), glm::vec3(3.0f) };
			Cone->ModelTransform(ConeModel);
			Cone->SetColor(glm::vec3(0.05f, 0.05f, 0.4f));
		}
			
		auto Light = GetObjectByName("Light");
		if (Light)
		{
			sModel LightModel = { glm::vec3(-18.0f, 6.0f, 2.0f), glm::vec3(0.0f), glm::vec3(0.5f) };
			Light->ModelTransform(LightModel);
			Light->SetColor(glm::vec3(0.5f, 0.3f, 0.9f));
			AddLightIfNotFound("Light", LightModel.Translation);
		}

		auto Ground = GetObjectByName("Ground");
		if (Ground)
		{
			sModel GroundModel = { glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f), glm::vec3(30.0f, 0.1f, 30.0f) };
			Ground->ModelTransform(GroundModel);
			Ground->SetColor(glm::vec3(0.3f, 0.3f, 0.3f));
		}

		//Render Models
		for (auto &Entities : WorldEntities)
		{
			Entities.second->Render(tCamera, LightPositions); 
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
			{
				WorldEntities[Name] = std::make_shared<cPointLight>();
			}
			break;
		case FlawedEngine::cScene::SpotLight:
			break;
		default:
			break;
		}
	}

	std::shared_ptr<cEntity> cScene::GetObjectByName(const char* Name)
	{
		auto Entity = WorldEntities.find(Name);

		if (Entity == WorldEntities.end())
			return nullptr;

		return Entity->second;
	}

	void cScene::AddLightIfNotFound(const char* Name, glm::vec3& Pos)
	{
		auto Light = LightPositions.find(Name);

		if (Light == LightPositions.end())
		{
			LightPositions[Name] = Pos;
		}
	}
}