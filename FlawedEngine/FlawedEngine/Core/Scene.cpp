#include "Scene.h"

#include "BasicModel/Triangle.h"
#include "BasicModel/Cube.h"
#include "BasicModel/PointLight.h"

#include "Models/OBJModel.h"



/*
* 
* OKAY PHYSICS WORKS, EXCEPT ALL TRANSFORMATIONS ARE F*ED UP :*)
* 
*/


namespace FlawedEngine
{
	cScene::cScene(void* Window)
		:mWindow(Window), Camera(mWindow)
	{
		//Should have more stuff in the future probably, IDK little secret, I am just winging it, I don't really know what I am doing.

		///-----initialization_start-----

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
		collisionConfiguration = new btDefaultCollisionConfiguration();

		///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		dispatcher = new btCollisionDispatcher(collisionConfiguration);

		///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
		overlappingPairCache = new btDbvtBroadphase();

		///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		solver = new btSequentialImpulseConstraintSolver;

		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration); 
		//TODO: should change to use std::share_ptr intead

		dynamicsWorld->setGravity(btVector3(0.0, -0.5, 0));
		///-----initialization_end-----

		Setup();
	}

	cScene::~cScene()
	{

	}

	void cScene::Setup()
	{
		LoadModel(Triangle,		"Triangle",		dynamicsWorld);
		LoadModel(PointLight,	"Light",		dynamicsWorld);
		LoadModel(PointLight,	"Example",		dynamicsWorld);
		LoadModel(PointLight,	"AnotherOne",	dynamicsWorld);
		LoadModel(Sphere,		"Sphere",		dynamicsWorld);
		LoadModel(Cube,			"Ground",		dynamicsWorld);
		LoadModel(Cone,			"Cone",			dynamicsWorld);
		LoadModel(Torus,		"Torus",		dynamicsWorld);
	}

	void cScene::Render()
	{
		Camera.Compute();
		Transform tCamera { Camera.Postion() , Camera.Front(),Camera.Projection(), Camera.View()};

		dynamicsWorld->stepSimulation(1.f / 60.f, 10);

		std::shared_ptr<cEntity> Triangle = GetObjectByName("Triangle");
		if (Triangle)
		{
			sModel TriangleModel = { glm::vec3(3.0f, 5.0f, 0.0f), glm::vec3(45.0f), glm::vec3(5.0f) };
			Triangle->ModelTransform(TriangleModel);
			Triangle->SetColor(glm::vec3( 0.3f, 0.6f, 0.1f ));
		}

		auto Sphere = GetObjectByName("Sphere");
		if (Sphere)
		{
			sModel SphereModel = { glm::vec3(-14.0f, 30.0f, -10.0f), glm::vec3(0.0f), glm::vec3(2.0f) };
			Sphere->ModelTransform(SphereModel);
			Sphere->SetColor(glm::vec3(0.5f, 0.2f, 0.5f));
			Sphere->SetPhysics();
			Sphere->setDynamic(true);
		}

		auto Torus = GetObjectByName("Torus");
		if (Torus)
		{
			sModel TorusModel = { glm::vec3(10.0f, 5.0f, -15.0f), glm::vec3(-45.0f), glm::vec3(5.0f)};
			Torus->ModelTransform(TorusModel);
			Torus->SetColor(glm::vec3(1.0f, 0.5f, 1.0f));
		}

		auto Cone = GetObjectByName("Cone");
		if (Cone)
		{
			glm::vec3 LightPos;
			LightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
			LightPos.y = sin(glfwGetTime() / 2.0f) * 2.0f;
			sModel ConeModel = { glm::vec3(-15.0f + LightPos.x , 20.0f, 4.0f + LightPos.y), glm::vec3(0.0f), glm::vec3(3.0f) };
			Cone->ModelTransform(ConeModel);
			sMaterial ConeMaterial = { glm::vec3(0.05f, 0.05f, 0.4f), glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(0.5f, 0.5f, 0.5f), 32.0f };
			Cone->SetMaterial(ConeMaterial);
			Cone->SetPhysics();
			Cone->setDynamic(true);
		}
			
		auto Light = GetObjectByName("Light");
		if (Light)
		{
			sModel LightModel = { glm::vec3(-18.0f, 6.0f, 2.0f), glm::vec3(0.0f), glm::vec3(0.5f) };
			Light->ModelTransform(LightModel);
			Light->SetColor(glm::vec3(0.5f, 0.3f, 0.9f));
			sLight LightProps = { LightModel.Translation, 1.0f, 0.09f, 0.032f, glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f)};
			AddLightIfNotFound("Light", LightProps);
		}

		auto ExampleLight = GetObjectByName("Example");
		if(ExampleLight) 
		{
			sModel LightModel = { glm::vec3(15.0f, 3.0f, 2.0f), glm::vec3(0.0f), glm::vec3(0.5f) };
			ExampleLight->ModelTransform(LightModel);
			ExampleLight->SetColor(glm::vec3(0.6f, 0.5f, 0.3f));
			sLight LightProps = { LightModel.Translation, 1.0f, 0.09f, 0.032f, glm::vec3(0.3f, 0.2f, 0.7f), glm::vec3(0.6f, 0.3f, 0.9f), glm::vec3(0.5f, 0.2f, 0.9f) };
			AddLightIfNotFound("Example", LightProps);
		}

		auto SomeLight = GetObjectByName("AnotherOne");
		if (SomeLight)
		{
			sModel LightModel = { glm::vec3(0.0f, 3.0f, 10.0f), glm::vec3(0.0f), glm::vec3(0.5f) };
			SomeLight->ModelTransform(LightModel);
			sLight LightProps = { LightModel.Translation, 1.0f, 0.09f, 0.032f, glm::vec3(0.5f, 0.1f, 0.8f), glm::vec3(0.8f), glm::vec3(1.0f) };
			AddLightIfNotFound("AnotherOne", LightProps);
		}

		auto Ground = GetObjectByName("Ground");
		if (Ground)
		{
			sModel GroundModel = { glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f), glm::vec3(30.0f, 0.1f, 30.0f) };
			Ground->ModelTransform(GroundModel);
			sMaterial GroundMat = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f), glm::vec3(1.0f), 16.0f };
			Ground->SetMaterial(GroundMat);
			Ground->SetPhysics();
			Ground->setDynamic(false);
		}

		//Render Models
		for (auto &Entities : WorldEntities)
		{
			Entities.second->Render(tCamera, PointLights); 
		}
	}

	void cScene::LoadModel(const char* FilePath, const char* Name, void* PhysicsWorld)
	{
		WorldEntities[Name] = std::make_shared<cOBJModel>(FilePath, dynamicsWorld);
	}

	void cScene::LoadModel(eBasicObject Object, const char* Name, void* PhysicsWorld)
	{
		switch (Object)
		{
		case FlawedEngine::cScene::Cube:
			{
				LoadModel("Core\\Models\\OBJ\\Cube\\Cube.obj", Name, dynamicsWorld); // should be a class but i cant bother
			}
			break;
		case FlawedEngine::cScene::Sphere:
			{
				LoadModel("Core\\Models\\OBJ\\Sphere\\Sphere.obj", Name, dynamicsWorld);
			}
			break;
		case FlawedEngine::cScene::Cone:
			{
				LoadModel("Core\\Models\\OBJ\\Cone\\Cone.obj", Name, dynamicsWorld);
			}
			break;
		case FlawedEngine::cScene::Torus:
			{
				LoadModel("Core\\Models\\OBJ\\Torus\\Torus.obj", Name, dynamicsWorld);
			}
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

	void cScene::AddLightIfNotFound(const char* Name, sLight& Props)
	{
		auto Light = PointLights.find(Name);

		if (Light == PointLights.end())
		{
			PointLights[Name] = Props;
		}
	}
}