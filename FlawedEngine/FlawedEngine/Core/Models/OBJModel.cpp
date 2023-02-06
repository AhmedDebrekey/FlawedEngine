#include "OBJModel.h"
#include <glm/gtc/type_ptr.hpp>

#include <iostream>



namespace FlawedEngine
{
	
	cOBJModel::cOBJModel(const char* FilePath, std::string Name, void* PhysicsWorld, btAlignedObjectArray<btCollisionShape*>* CollisionShapes)
		:mCollisionShapesArray(CollisionShapes), mName(Name)
	{
		mPhysicsDynamicWorld = (btDiscreteDynamicsWorld*)PhysicsWorld;
		if (!LoadModel(FilePath))
		{
			std::cout << "Failed To Load 3D Model from: " << FilePath << std::endl;
		}

		Populate();
		
		Renderer.Init(mVertexBuffer, mTextureCoords, mIndices);
	}

	cOBJModel::~cOBJModel()
	{
		SetPhysics(Cube, mPhysicsDynamicWorld); //EXTREMELY BAD, but at least it makes sure that smth can get deleted and no unable to read memory, very probable that it will lead to memory leak ofc
		//To recreate error, remove the line above, add an object, give physics then remove physics then delete object.

		delete mRidigBody->getMotionState();
		mRidigBody->getCollisionShape();
		mPhysicsDynamicWorld->removeRigidBody(mRidigBody);
		mCollisionShapesArray->remove(mCollisionShape);
		delete mRidigBody;
	}

	bool cOBJModel::LoadModel(const char* FilePath)
	{
		bool Loaded = mLoader.LoadFile(FilePath);
		return Loaded;
	}

	void cOBJModel::Populate()
	{
		for (auto CurrentMesh : mLoader.LoadedMeshes)
		{
			for (uint32_t i = 0; i < CurrentMesh.Vertices.size(); i++)
			{
				//3 postional float, 3 Normal float, 2 UV Coords
				glm::vec3 Position = glm::vec3(CurrentMesh.Vertices[i].Position.X, CurrentMesh.Vertices[i].Position.Y, CurrentMesh.Vertices[i].Position.Z);
				glm::vec3 Normal = glm::vec3(CurrentMesh.Vertices[i].Normal.X, CurrentMesh.Vertices[i].Normal.Y, CurrentMesh.Vertices[i].Normal.Z);
				glm::vec2 TexCoords = glm::vec2(CurrentMesh.Vertices[i].TextureCoordinate.X, CurrentMesh.Vertices[i].TextureCoordinate.Y);
				mVertexBuffer.push_back({ Position, Normal, TexCoords });
			}

			for (uint32_t i = 0; i < CurrentMesh.Indices.size(); i += 3)
			{
				mIndices.push_back(CurrentMesh.Indices.at(i));
				mIndices.push_back(CurrentMesh.Indices.at(i + 1));
				mIndices.push_back(CurrentMesh.Indices.at(i + 2));
			}
		}
	}

	void cOBJModel::SetCollisionShape(eBasicObject Object)
	{
		switch (Object) //if it is not set code will crash ofc
		{
		case FlawedEngine::Cube:
		{
			mCollisionShape = new btBoxShape(btVector3(btScalar(1.), btScalar(1.), btScalar(1.)));
		}
		break;
		case FlawedEngine::Sphere:
		{
			mCollisionShape = new btSphereShape(1.0);
		}
		break;
		case FlawedEngine::Cone:
			break;
		case FlawedEngine::Torus:
			break;
		case FlawedEngine::Triangle:
			break;
		case FlawedEngine::PointLight:
			break;
		case FlawedEngine::SpotLight:
			break;
		default:
			break;
		}
	}

	void cOBJModel::SetPhysics(eBasicObject Object, void* PhysicsWorld)
	{
		if (!isPhysicsSet)
		{
			glm::vec3 Trans = mTransformation.Translation;
			glm::vec3 Rotation = mTransformation.Rotation;
			glm::vec3 Scale = mTransformation.Scale;

			SetCollisionShape(Object);

			btTransform ObjectTransform;
			ObjectTransform.setIdentity();
			ObjectTransform.setOrigin(btVector3(Trans.x, Trans.y, Trans.z));

			btScalar mass(1.0);

			btVector3 localInertia(0, 0, 0);
			if (mass != 0.f) mCollisionShape->calculateLocalInertia(mass, localInertia);

			btDefaultMotionState* MotionState = new btDefaultMotionState(ObjectTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, MotionState, mCollisionShape, localInertia);
			rbInfo.m_startWorldTransform;
			mRidigBody = new btRigidBody(rbInfo);

			btTransform trans = mRidigBody->getCenterOfMassTransform();
			btQuaternion transrot;
			trans.getBasis().getRotation(transrot);
			btQuaternion rotquat;
			rotquat = rotquat.getIdentity();
			rotquat.setEuler(glm::radians(Rotation.z), glm::radians(Rotation.y), glm::radians(Rotation.x));
			transrot = rotquat * transrot;
			trans.setRotation(transrot);
			mRidigBody->setCenterOfMassTransform(trans);

			mCollisionShape->setLocalScaling(btVector3(btScalar(Scale.x), btScalar(Scale.y), btScalar(Scale.z)));
			mRidigBody->setActivationState(DISABLE_DEACTIVATION);
			//mRidigBody->setSleepingThresholds(0.2, 0.2);
			mRidigBody->setRestitution(mRestitution);
			mRidigBody->setFriction(mFricton);

			mPhysicsDynamicWorld->addRigidBody(mRidigBody);
			mPhysicsDynamicWorld->updateSingleAabb(mRidigBody);


			mCollisionShapesArray->push_back(mCollisionShape);

			mCollisionShape->setUserPointer((void*)mName.data());

			isPhysicsSet = true;
		}
	}

	void cOBJModel::UnSetPhysics()
	{
		if (isPhysicsSet)
		{
			delete mRidigBody->getMotionState();
			delete mRidigBody->getCollisionShape();
			mPhysicsDynamicWorld->removeRigidBody(mRidigBody);
			mCollisionShapesArray->remove(mCollisionShape);
			delete mRidigBody;
			isPhysicsSet = false;
		}
	}

	void cOBJModel::SendInputToScripting(std::function<bool(int)> func)
	{
		using namespace luabridge;
		getGlobalNamespace(LuaState)
			.addFunction("IsKeyDown", func);
	}

	void cOBJModel::LSetColor(float x, float y, float z)
	{
		SetColor(glm::vec3(x, y, z));
	}

	void cOBJModel::LMove(float x, float y, float z)// L for Lua
	{
		mTransformation.Translation += glm::vec3(x, y, z);
		ModelTransform(mTransformation);
		if (mPhysics)
		{
			btTransform Trans;
			Trans.setOrigin(btVector3(mTransformation.Translation.x, mTransformation.Translation.y, mTransformation.Translation.z));
			mRidigBody->getMotionState()->setWorldTransform(Trans);
		}
	}


	void cOBJModel::LRotate(float x, float y, float z)
	{
		mTransformation.Rotation += glm::vec3(x, y, z);
		ModelTransform(mTransformation);
		if (mPhysics)
		{
			btTransform Trans;
			mRidigBody->getMotionState()->getWorldTransform(Trans);
			btQuaternion rot;
			rot.setEuler(mTransformation.Rotation.x, mTransformation.Rotation.y, mTransformation.Rotation.z);
			Trans.setRotation(rot);
			mRidigBody->getMotionState()->setWorldTransform(Trans);
		}
	}

	void cOBJModel::LScale(float x, float y, float z)
	{
		mTransformation.Scale += glm::vec3(x, y, z);
		ModelTransform(mTransformation);
		if (mPhysics)
		{
			btVector3 myscale = btVector3(mTransformation.Scale.x, mTransformation.Scale.y, mTransformation.Scale.z);
			mRidigBody->getCollisionShape()->setLocalScaling(myscale);
		}
	}

	void cOBJModel::LApplyForce(float x, float y, float z)
	{
		if (mPhysics)
			ApplyForce(glm::vec3(x, y, z));
	}

	void cOBJModel::LApplyRelativeForce(float x, float y, float z)
	{
		if (mPhysics)
			ApplyRelativeForce(glm::vec3(x, y, z));
	}

	void cOBJModel::LAddObject()
	{
		//Implementation 404	
	}

	void cOBJModel::SetupScripting()
	{

		ScriptingId = ScriptingManager.InitScripting();
		LuaState = ScriptingManager.GetLuaState(ScriptingId);
		ScriptingManager.RegisterFunction(ScriptingId, "Move", std::bind(&cOBJModel::LMove, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ScriptingManager.RegisterFunction(ScriptingId, "Rotate", std::bind(&cOBJModel::LRotate, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ScriptingManager.RegisterFunction(ScriptingId, "Scale", std::bind(&cOBJModel::LScale, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ScriptingManager.RegisterFunction(ScriptingId, "ApplyForce", std::bind(&cOBJModel::LApplyForce, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ScriptingManager.RegisterFunction(ScriptingId, "ApplyRelativeForce", std::bind(&cOBJModel::LApplyRelativeForce, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ScriptingManager.RegisterFunction(ScriptingId, "ChangeColor", std::bind(&cOBJModel::LSetColor, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

		luabridge::getGlobalNamespace(LuaState)
			.beginNamespace("Pos")
			.addVariable("x", &mTransformation.Translation.x)
			.addVariable("y", &mTransformation.Translation.y)
			.addVariable("z", &mTransformation.Translation.z)
			.endNamespace();

		ScriptingManager.RunFile(ScriptingId, "OnUserCreate.lua");
		lua_pcall(LuaState, 0, 0, 0);
	}

	void cOBJModel::SendEntity(cEntity* Entity)
	{

	}

	float Matrix[16];
	void cOBJModel::Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions)
	{
		if (mRidigBody != nullptr && mRidigBody->getMotionState() && isPhysicsSet)
		{
			btTransform btTrans;
			mRidigBody->getMotionState()->getWorldTransform(btTrans);
			 
			btVector3 ObjectTransform = btTrans.getOrigin();
			btVector3 Scale = mRidigBody->getCollisionShape()->getLocalScaling();

			glm::vec4 Rotation;
			btQuaternion quat = mRidigBody->getCenterOfMassTransform().getRotation();
			btVector3 v = quat.getAxis();
			Rotation.x = v.x();
			Rotation.y = v.y();
			Rotation.z = v.z();
			Rotation.w = quat.getAngle();

			glm::mat4 Model = glm::mat4(1.0f);
			Model = glm::translate(Model, glm::vec3(ObjectTransform.getX(), ObjectTransform.getY(), ObjectTransform.getZ()));
			Model = glm::rotate(Model, Rotation.w, glm::vec3(Rotation.x, Rotation.y, Rotation.z));
			Model = glm::scale(Model, glm::vec3(Scale.getX(), Scale.getY(), Scale.getZ()));
			Trans.Model = Model;
		}
		else
			Trans.Model = mModel;
		
		Renderer.Draw(Trans, mMaterial, LightPositions);
	}

	void cOBJModel::Update()
	{
		ScriptingManager.RunFile(ScriptingId, "OnUpdate.lua");
	}

	void cOBJModel::setDynamic(bool isDynamic)
	{
		if (!isDynamic)
		{
			mRidigBody->setCollisionFlags(mRidigBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
		}
		else
		{
			mRidigBody->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
		}
	}
}