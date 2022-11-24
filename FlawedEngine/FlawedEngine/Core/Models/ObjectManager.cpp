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
		}
		break;
		case FlawedEngine::Sphere:
		{
			SceneObjects[Name] = std::make_shared<cOBJModel>("Core\\Models\\OBJ\\Sphere\\Sphere.obj", mPhysicsWorld, mCollisionShapesArray);
			sModel DefaultModel = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
			SceneObjects[Name]->ModelTransform(DefaultModel);
		}
		break;
		case FlawedEngine::Cone:
		{
			SceneObjects[Name] = std::make_shared<cOBJModel>("Core\\Models\\OBJ\\Cone\\Cone.obj", mPhysicsWorld, mCollisionShapesArray);
			sModel DefaultModel = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
			SceneObjects[Name]->ModelTransform(DefaultModel);

		}
		break;
		case FlawedEngine::Torus:
		{
			SceneObjects[Name] = std::make_shared<cOBJModel>("Core\\Models\\OBJ\\Torus\\Torus.obj", mPhysicsWorld, mCollisionShapesArray);
			sModel DefaultModel = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
			SceneObjects[Name]->ModelTransform(DefaultModel);
		}
		break;
		case FlawedEngine::Triangle:
		{
			SceneObjects[Name] = std::make_shared<cTriangle>();
			sModel DefaultModel = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
			SceneObjects[Name]->ModelTransform(DefaultModel);
		}
		break;
		case FlawedEngine::PointLight:
		{
			SceneObjects[Name] = std::make_shared<cPointLight>();
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
}