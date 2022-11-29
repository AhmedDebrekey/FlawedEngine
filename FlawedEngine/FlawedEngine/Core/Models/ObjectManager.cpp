#include "ObjectManager.h"
#include "../BasicModel/Pointlight.h"	
#include "../BasicModel/Triangle.h"	


namespace FlawedEngine
{
	cObjectManager::cObjectManager()
	{
	}

	cObjectManager::~cObjectManager()
	{
	}

	void cObjectManager::Init(void* PhysicsWorld, btAlignedObjectArray<btCollisionShape*>* CollisionShapes)
	{
		mPhysicsWorld = PhysicsWorld;
		mCollisionShapesArray = CollisionShapes;
	}

	void cObjectManager::RenderObjects(sTransform& tCamera)
	{
		for (auto& Object : SceneObjects)
		{
			Object.second->Render(tCamera, PointLights);
		}
	}

	void cObjectManager::AddObject(eBasicObject ObjectType, const char* Name)
	{
		switch (ObjectType)
		{
		case FlawedEngine::Cube:
		{
			SceneObjects[Name] = std::make_shared<cOBJModel>("Core\\Models\\OBJ\\Cube\\Cube.obj", mPhysicsWorld, mCollisionShapesArray);
			sModel DefaultModel = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
			SceneObjects[Name]->ModelTransform(DefaultModel);
			sPhysicsProps DefaultPhysics = { 1.f, 1.0f, 0.5f };
			SceneObjects[Name]->SetPhysicsProps(DefaultPhysics);
			SceneObjects[Name]->Type = Cube;
		}
		break;
		case FlawedEngine::Sphere:
		{
			SceneObjects[Name] = std::make_shared<cOBJModel>("Core\\Models\\OBJ\\Sphere\\Sphere.obj", mPhysicsWorld, mCollisionShapesArray);
			sModel DefaultModel = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
			SceneObjects[Name]->ModelTransform(DefaultModel);
			sPhysicsProps DefaultPhysics = { 1.f, 1.0f, 0.5f };
			SceneObjects[Name]->SetPhysicsProps(DefaultPhysics);
			SceneObjects[Name]->Type = Sphere;
		}
		break;
		case FlawedEngine::Cone:
		{
			SceneObjects[Name] = std::make_shared<cOBJModel>("Core\\Models\\OBJ\\Cone\\Cone.obj", mPhysicsWorld, mCollisionShapesArray);
			sModel DefaultModel = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
			SceneObjects[Name]->ModelTransform(DefaultModel);
			sPhysicsProps DefaultPhysics = { 1.f, 1.0f, 0.5f };
			SceneObjects[Name]->SetPhysicsProps(DefaultPhysics);
			SceneObjects[Name]->Type = Cube;
		}
		break;
		case FlawedEngine::Torus:
		{
			SceneObjects[Name] = std::make_shared<cOBJModel>("Core\\Models\\OBJ\\Torus\\Torus.obj", mPhysicsWorld, mCollisionShapesArray);
			sModel DefaultModel = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
			SceneObjects[Name]->ModelTransform(DefaultModel);
			sPhysicsProps DefaultPhysics = { 1.f, 1.0f, 0.5f };
			SceneObjects[Name]->SetPhysicsProps(DefaultPhysics);
			SceneObjects[Name]->Type = Cube;
		}
		break;
		case FlawedEngine::Triangle:
		{
			SceneObjects[Name] = std::make_shared<cTriangle>();
			sModel DefaultModel = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
			SceneObjects[Name]->ModelTransform(DefaultModel);
			sPhysicsProps DefaultPhysics = { 1.f, 1.0f, 0.5f };
			SceneObjects[Name]->SetPhysicsProps(DefaultPhysics);
			SceneObjects[Name]->Type = Cube;
		}
		break;
		case FlawedEngine::PointLight:
		{
			SceneObjects[Name] = std::make_shared<cPointLight>();
			sModel DefaultModel = { glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.5f) };
			SceneObjects[Name]->ModelTransform(DefaultModel);
			glm::vec3 DefaultColor = glm::vec3(1.0f);
			SceneObjects[Name]->SetColor(DefaultColor);
			sLight DefualtLightProps = { DefaultModel.Translation, 1.0f, 0.09f, 0.032f, glm::vec3(1.0f), glm::vec3(0.6f, 0.3f, 0.9f), glm::vec3(0.5f, 0.2f, 0.9f) };
			AddLight(Name, DefualtLightProps);
			SceneObjects[Name]->Type = PointLight;
		}
		break;
		case FlawedEngine::SpotLight:
			break;
		default:
			break;
		}
	}

	void cObjectManager::RemoveObject(const char* Name)
	{
		if (SceneObjects[Name]->Type == PointLight)
			PointLights.erase(Name);
		SceneObjects.erase(Name);
	}

	void cObjectManager::ModifyObject(const char* Name, sModel& Model, sMaterial Material, bool setPhysics, sPhysicsProps PhysicsProps)
	{
		auto Object = GetObjectByName(Name);
		if (Object)
		{
			Object->ModelTransform(Model);

			Object->SetMaterial(Material);

			Object->SetPhysicsProps(PhysicsProps);

			Object->SetPhysics(Cube, mPhysicsWorld); //Get Type from Entity
			Object->setDynamic(setPhysics);
		}
	}

	glm::vec3 cObjectManager::GetLightColor(const char* Name)
	{
		auto Light = GetLightByName(Name);
		if(Light)
			return Light->ambient;
	}

	void cObjectManager::ChangeLightColor(const char* Name, glm::vec3 Color)
	{
		auto Light = GetLightByName(Name);
		if (Light)
			Light->ambient = Color;
	}

	void cObjectManager::ChangeLightPosition(const char* Name, glm::vec3 Position)
	{
		auto Light = GetLightByName(Name);
		if (Light)
			Light->position = Position;
	}

	void cObjectManager::ChangeName(const char* OldName, const char* NewName)
	{
		auto Object = SceneObjects.extract(OldName);
		Object.key() = NewName;
		SceneObjects.insert(std::move(Object));
	}

	void cObjectManager::AddLight(const char* Name, sLight& Props)
	{
		auto Light = PointLights.find(Name);

		if (Light == PointLights.end())
		{
			PointLights[Name] = Props;
		}
	}

	std::shared_ptr<cEntity> cObjectManager::GetObjectByName(const char* Name)
	{
		auto Object = SceneObjects.find(Name);

		if (Object == SceneObjects.end())
			return nullptr;

		if (Object->second->Dead)
			return nullptr;

		return Object->second;
	}
	sLight* cObjectManager::GetLightByName(const char* Name)
	{
		auto Light = PointLights.find(Name);

		if (Light == PointLights.end())
			return nullptr;

		return &Light->second;
	}
}