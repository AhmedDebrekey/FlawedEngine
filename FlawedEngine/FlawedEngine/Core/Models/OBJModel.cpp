#include "OBJModel.h"
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "stb_image.h"

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
		ShadowShader.Create("Core/Models/Shaders/ShadowVertex.glsl", "Core/Models/Shaders/ShadowFragment.glsl");
	}

	cOBJModel::~cOBJModel()
	{
		SetPhysics(Cube, mPhysicsDynamicWorld); //EXTREMELY BAD, but at least it makes sure that smth can get deleted and no unable to read memory, very probable that it will lead to memory leak ofc
		//To recreate error, remove the line above, add an object, give physics then remove physics then delete object.

		delete mRigidBody->getMotionState();
		mRigidBody->getCollisionShape();
		mPhysicsDynamicWorld->removeRigidBody(mRigidBody);
		mCollisionShapesArray->remove(mCollisionShape);
		delete mRigidBody;
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
			mRigidBody = new btRigidBody(rbInfo);

			btTransform trans = mRigidBody->getCenterOfMassTransform();
			btQuaternion transrot;
			trans.getBasis().getRotation(transrot);
			btQuaternion rotquat;
			rotquat = rotquat.getIdentity();
			rotquat.setEuler(glm::radians(Rotation.z), glm::radians(Rotation.y), glm::radians(Rotation.x));
			transrot = rotquat * transrot;
			trans.setRotation(transrot);
			mRigidBody->setCenterOfMassTransform(trans);

			mCollisionShape->setLocalScaling(btVector3(btScalar(Scale.x), btScalar(Scale.y), btScalar(Scale.z)));
			mRigidBody->setActivationState(DISABLE_DEACTIVATION);
			//mRidigBody->setSleepingThresholds(0.2, 0.2);
			mRigidBody->setRestitution(mRestitution);
			mRigidBody->setFriction(mFricton);

			mPhysicsDynamicWorld->addRigidBody(mRigidBody);
			mPhysicsDynamicWorld->updateSingleAabb(mRigidBody);


			mCollisionShapesArray->push_back(mCollisionShape);

			mCollisionShape->setUserPointer((void*)mName.data());

			isPhysicsSet = true;
		}
	}

	void cOBJModel::UnSetPhysics()
	{
		if (isPhysicsSet)
		{
			delete mRigidBody->getMotionState();
			delete mRigidBody->getCollisionShape();
			mPhysicsDynamicWorld->removeRigidBody(mRigidBody);
			mCollisionShapesArray->remove(mCollisionShape);
			delete mRigidBody;
			isPhysicsSet = false;
		}
	}

	void cOBJModel::SendInputToScripting(std::function<bool(int)> func)
	{
		using namespace luabridge;
		getGlobalNamespace(LuaState)
			.addFunction("IsKeyDown", func);
	}

	void cOBJModel::AddAnimation(const char*)
	{
	}

	void cOBJModel::SetAABB(glm::vec3& Scale)
	{
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
			mRigidBody->getMotionState()->getWorldTransform(Trans);
			btVector3 Origin = Trans.getOrigin();
			btVector3 FinalTranslation(mTransformation.Translation.x, mTransformation.Translation.y, mTransformation.Translation.z);
			Trans.setOrigin(FinalTranslation);
			mRigidBody->getMotionState()->setWorldTransform(Trans);
			if (!mDynamic)
			{
				mRigidBody->setWorldTransform(Trans);
			}
		}
	}

	void cOBJModel::LSetPosition(float x, float y, float z)
	{
		mTransformation.Translation = glm::vec3(x, y, z);
		ModelTransform(mTransformation);

		if (mPhysics)
		{
			btTransform Trans;
			mRigidBody->getMotionState()->getWorldTransform(Trans);
			btVector3 Origin = Trans.getOrigin();
			btVector3 FinalTranslation(mTransformation.Translation.x, mTransformation.Translation.y, mTransformation.Translation.z);
			Trans.setOrigin(FinalTranslation);
			mRigidBody->getMotionState()->setWorldTransform(Trans);
			if (!mDynamic)
			{
				mRigidBody->setWorldTransform(Trans);
			}
		}
	}

	void cOBJModel::LRotate(float x, float y, float z)
	{
		mTransformation.Rotation += glm::vec3(x, y, z);
		ModelTransform(mTransformation);
		if (mPhysics)
		{
			btTransform Trans;
			mRigidBody->getMotionState()->getWorldTransform(Trans);
			btQuaternion rot;
			rot.setEuler(mTransformation.Rotation.x, mTransformation.Rotation.y, mTransformation.Rotation.z);
			Trans.setRotation(rot);
			mRigidBody->getMotionState()->setWorldTransform(Trans);
		}
	}

	void cOBJModel::LScale(float x, float y, float z)
	{
		mTransformation.Scale = glm::vec3(x, y, z);
		ModelTransform(mTransformation);
		if (mPhysics)
		{
			btVector3 myscale = btVector3(mTransformation.Scale.x, mTransformation.Scale.y, mTransformation.Scale.z);
			mRigidBody->getCollisionShape()->setLocalScaling(myscale);
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

	float cOBJModel::LGetX()
	{
		if (mPhysics)
		{
			const btTransform trans = mRigidBody->getWorldTransform(); 
			const btVector3 pos = trans.getOrigin();
			
			return pos.getX();
		}
		return mTransformation.Translation.x;
	}

	float cOBJModel::LGetY()
	{
		if (mPhysics)
		{
			const btTransform trans = mRigidBody->getWorldTransform();
			const btVector3 pos = trans.getOrigin();

			return pos.getY();
		}
		return mTransformation.Translation.y;
	}

	float cOBJModel::LGetZ()
	{
		if (mPhysics)
		{
			const btTransform trans = mRigidBody->getWorldTransform();
			const btVector3 pos = trans.getOrigin();

			return pos.getZ();
		}
		return mTransformation.Translation.z;
	}


	void cOBJModel::SetupScripting(const char* Path)
	{
		ScriptingId = ScriptingManager.InitScripting();
		LuaState = ScriptingManager.GetLuaState(ScriptingId);

		ScriptingManager.RegisterFunction(ScriptingId, "Move",						std::bind(&cOBJModel::LMove,				this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ScriptingManager.RegisterFunction(ScriptingId, "SetPos",					std::bind(&cOBJModel::LSetPosition,			this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ScriptingManager.RegisterFunction(ScriptingId, "Rotate",					std::bind(&cOBJModel::LRotate,				this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ScriptingManager.RegisterFunction(ScriptingId, "Scale",						std::bind(&cOBJModel::LScale,				this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ScriptingManager.RegisterFunction(ScriptingId, "ApplyForce",				std::bind(&cOBJModel::LApplyForce,			this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ScriptingManager.RegisterFunction(ScriptingId, "ApplyRelativeForce",		std::bind(&cOBJModel::LApplyRelativeForce,	this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ScriptingManager.RegisterFunction(ScriptingId, "ChangeColor",				std::bind(&cOBJModel::LSetColor,			this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ScriptingManager.RegisterFunctionInNamespace(ScriptingId, "Pos", "getX",	std::bind(&cOBJModel::LGetX,				this));
		ScriptingManager.RegisterFunctionInNamespace(ScriptingId, "Pos", "getY",	std::bind(&cOBJModel::LGetY,				this));
		ScriptingManager.RegisterFunctionInNamespace(ScriptingId, "Pos", "getZ",	std::bind(&cOBJModel::LGetZ,				this));

		ScriptingManager.LoadFile(ScriptingId, Path);

		mScriptPath = Path;
		mHasScripting = true;

		lua_pcall(LuaState, 0, 0, 0);

		ScriptingManager.RunFunction(ScriptingId, "Create");
	}

	void cOBJModel::SendEntity(cEntity* Entity)
	{

	}

	float Matrix[16];
	void cOBJModel::Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions, uint32_t* SkyBox)
	{
		if (mRigidBody != nullptr && mRigidBody->getMotionState() && isPhysicsSet)
		{
			btTransform btTrans;
			mRigidBody->getMotionState()->getWorldTransform(btTrans);
			 
			btVector3 ObjectTransform = btTrans.getOrigin();
			btVector3 Scale = mRigidBody->getCollisionShape()->getLocalScaling();

			glm::vec4 Rotation;
			btQuaternion quat = mRigidBody->getCenterOfMassTransform().getRotation();
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

		if(mShouldRender)
			Renderer.Draw(Trans, mMaterial, LightPositions, SkyBox);
	}

	void cOBJModel::ShadowRender(sTransform& Trans, glm::mat4& LightSpaceMatrix, uint32_t DepthMap)
	{
		if (mRigidBody != nullptr && mRigidBody->getMotionState() && isPhysicsSet)
		{
			btTransform btTrans;
			mRigidBody->getMotionState()->getWorldTransform(btTrans);

			btVector3 ObjectTransform = btTrans.getOrigin();
			btVector3 Scale = mRigidBody->getCollisionShape()->getLocalScaling();

			glm::vec4 Rotation;
			btQuaternion quat = mRigidBody->getCenterOfMassTransform().getRotation();
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

		if (mShouldRender)
			Renderer.ShadowDraw(Trans, ShadowShader, LightSpaceMatrix, DepthMap);
	}

	void cOBJModel::Update()
	{
		ScriptingManager.RunFunction(ScriptingId, "Update");
	}

	void cOBJModel::setDynamic(bool isDynamic)
	{
		if (!isDynamic)
		{
			mRigidBody->setCollisionFlags(mRigidBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
		}
		else
		{
			mRigidBody->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
		}
	}
}