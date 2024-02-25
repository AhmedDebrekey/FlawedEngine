#pragma once

#include "Core.h"
#include "Input/Input.h"
#include <iostream>
#include <unordered_map>
#include <Bullet/btBulletDynamicsCommon.h>
#include <glm/gtc/type_ptr.hpp>
#include <functional>
#include "ScriptingManager.h"

namespace FlawedEngine
{
	class cEntity
	{
	public:
		virtual void Render(sTransform&, std::unordered_map<std::string, sLight>&, uint32_t*) = 0;
		virtual void ShadowRender(sTransform&, glm::mat4&, uint32_t) = 0;
		virtual void Update(/*Should be taking in the timestep*/) = 0;
		virtual void AddAnimation(const char*) = 0;
		virtual void ChangeAnimation(const char*) = 0;
		virtual void SetAABB(glm::vec3&) = 0;
		virtual bool isModelInFrustum() = 0;
		virtual ~cEntity() = 0;

		void ModelTransform(sModel& model);
		void SetColor(glm::vec3 Color);
		void SetMaterial(sMaterial& Mat);
		void SetPhysicsProps(sPhysicsProps& Props);
		void isInvisible();

		//LuaFunctions
		void SetupScripting(const char*, std::function<bool(int)>&);
		void LSetColor(float x, float y, float z);
		void LMove(float x, float y, float z);
		void LSetPosition(float x, float y, float z);
		void LRotate(float x, float y, float z);
		void LScale(float x, float y, float z);
		void LApplyForce(float x, float y, float z);
		void LApplyRelativeForce(float x, float y, float z);
		void LSpawnObject();
		float LGetX();
		float LGetY();
		float LGetZ();
		std::string LGetName();
		void LChangeAnim(const char* Path);
		int mScriptingId;
		lua_State* mLuaState = nullptr;

		//Serlizalizations
		bool mIsCostume = false;
		bool mHasScripting = false;
		bool mHasAnimation = false;
		std::string mFilePath;
		std::string mScriptPath;
		std::string mAnimationPath;

		//Physics
		btRigidBody* mRigidBody = nullptr;
		bool mDynamic = false;
		bool mPhysics = false;
		float mMass = 0.0f;
		float mFricton = 0.5;
		float mRestitution = 0.0f;
		bool mDead = false;
		eBasicObject Type = Cube;
		sAABB mAABB;
		bool isPhysicsSet = false;
		btVector3 mInertia;
		btCollisionShape* mCollisionShape;
		btDiscreteDynamicsWorld* mPhysicsDynamicWorld;
		btAlignedObjectArray<btCollisionShape*>* mCollisionShapesArray;
		std::string mName;
		glm::vec3 GetAABB() { return mAABB.mExtents; }
		sModel GetModel() { return mTransformation; }
		glm::mat4* GetModelMatrix() { return &mModel; }
		glm::vec3* GetColor() { return &mMaterial.Color; }
		int GetActivationState() { return mRigidBody->getActivationState(); }
		void SetPhysics(eBasicObject Object, void* PhysicsWorld);
		void SetCollisionShape(eBasicObject Object);
		void setDynamic(bool IsDynamic);
		void UnSetPhysics();
		void ApplyForce(glm::vec3 Force);
		void ApplyRelativeForce(glm::vec3 Force);

		//Rendering
		sMaterial mMaterial;
		glm::mat4 mModel;
		std::vector<sVertex> mVertexBuffer;
		std::vector<uint32_t> mIndices;
		std::vector<sTexture> mTextureCoords;
		bool mShouldRender = true;
		sModel mTransformation;

		cInput& Input = cInput::get();
		cScriptingManager& ScriptingManager = cScriptingManager::get();
	};


	inline cEntity::~cEntity()
	{
		mVertexBuffer.clear();
		mTextureCoords.clear();
		mIndices.clear();
	}

	inline void cEntity::ModelTransform(sModel& model)
	{
		mTransformation = model;
		glm::mat4 Model = glm::mat4(1.0f);
		Model = glm::translate(Model, mTransformation.Translation);

		glm::mat4 xRotation = glm::rotate(glm::mat4(1.0), glm::radians(mTransformation.Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 yRotation = glm::rotate(glm::mat4(1.0), glm::radians(mTransformation.Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 zRotation = glm::rotate(glm::mat4(1.0), glm::radians(mTransformation.Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		Model *= zRotation * yRotation * xRotation;

		Model = glm::scale(Model, mTransformation.Scale);

		mModel = Model;
	}

	inline void cEntity::SetColor(glm::vec3 Color)
	{
		mMaterial.Color = Color;
	}

	inline void cEntity::SetMaterial(sMaterial& Mat)
	{
		mMaterial = Mat;
	}

	inline void cEntity::SetPhysicsProps(sPhysicsProps& Props)
	{
		mMass = Props.Mass;
		mFricton = Props.Friction;
		mRestitution = Props.Restitution;
	}

	inline void cEntity::isInvisible() { mShouldRender = !mShouldRender; }

	inline void cEntity::ApplyRelativeForce(glm::vec3 Force)
	{
		if (!mRigidBody->isActive())
			mRigidBody->activate(true);
		btVector3 relativeForce = btVector3(Force.x, Force.y, Force.z);
		btTransform boxTrans;
		mRigidBody->getMotionState()->getWorldTransform(boxTrans);
		btVector3 RelativeTransform = relativeForce;
		btVector3 correctedForce = (boxTrans * relativeForce) - boxTrans.getOrigin();
		relativeForce = (boxTrans * RelativeTransform) - boxTrans.getOrigin();
		mRigidBody->applyCentralForce(relativeForce);
	}

	inline void cEntity::ApplyForce(glm::vec3 Force)
	{
		if (!mRigidBody->isActive())
			mRigidBody->activate(true);
		btVector3 worldForce = btVector3(Force.x, Force.y, Force.z);
		mRigidBody->applyCentralForce(worldForce);
	}

	inline void cEntity::SetCollisionShape(eBasicObject Object)
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
		case FlawedEngine::Custom:
		{
			mCollisionShape = new btBoxShape(btVector3(mAABB.mExtents.x, mAABB.mExtents.y, mAABB.mExtents.z));
		}
		break;
		case FlawedEngine::Cone:
		case FlawedEngine::Torus:
		case FlawedEngine::Triangle:
		case FlawedEngine::PointLight:
		case FlawedEngine::SpotLight:
		default:
			mCollisionShape = new btBoxShape(btVector3(mAABB.mExtents.x, mAABB.mExtents.y, mAABB.mExtents.z));
			break;
		}
	}

	inline void cEntity::setDynamic(bool IsDynamic)
	{
		if (!IsDynamic)
		{
			mRigidBody->setCollisionFlags(mRigidBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
		}
		else
		{
			mRigidBody->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
		}
	}

	inline void cEntity::UnSetPhysics()
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

	inline void cEntity::SetPhysics(eBasicObject Object, void* PhysicsWorld)
	{
		if (!isPhysicsSet)
		{
			glm::vec3 Trans = mTransformation.Translation;
			glm::vec3 Rotation = mTransformation.Rotation;
			glm::vec3 Scale = mTransformation.Scale;

			SetCollisionShape(Object);
			//mCollisionShape = CalculateMeshCollision(scene);

			btTransform ObjectTransform;
			ObjectTransform.setIdentity();
			ObjectTransform.setOrigin(btVector3(Trans.x, Trans.y, Trans.z));

			btScalar mass(1.0);

			mInertia = btVector3(0, 0, 0);
			if (mass != 0.f) mCollisionShape->calculateLocalInertia(mass, mInertia);

			btDefaultMotionState* MotionState = new btDefaultMotionState(ObjectTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, MotionState, mCollisionShape, mInertia);
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
			mPhysics = true;
			mDynamic = false;
		}
	}

	inline void cEntity::SetupScripting(const char* Path, std::function<bool(int)>& InputFunc)
	{
		mScriptingId = ScriptingManager.InitScripting();
		mLuaState = ScriptingManager.GetLuaState(mScriptingId);

		std::function<void(float, float, float)> funcptr = std::bind(&cEntity::LMove, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		ScriptingManager.RegisterFunction(mScriptingId, "Move", funcptr);

		funcptr = std::bind(&cEntity::LSetPosition, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		ScriptingManager.RegisterFunction(mScriptingId, "SetPos", funcptr);

		funcptr = std::bind(&cEntity::LRotate, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		ScriptingManager.RegisterFunction(mScriptingId, "Rotate", funcptr);

		funcptr = std::bind(&cEntity::LScale, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		ScriptingManager.RegisterFunction(mScriptingId, "Scale", funcptr);

		funcptr = std::bind(&cEntity::LApplyForce, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		ScriptingManager.RegisterFunction(mScriptingId, "ApplyForce", funcptr);

		funcptr = std::bind(&cEntity::LApplyRelativeForce, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		ScriptingManager.RegisterFunction(mScriptingId, "ApplyRelativeForce", funcptr);

		funcptr = std::bind(&cEntity::LSetColor, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		ScriptingManager.RegisterFunction(mScriptingId, "ChangeColor", funcptr);

		std::function<void(const char*)> ChangeAnimFunc = std::bind(&cEntity::LChangeAnim, this, std::placeholders::_1);
		ScriptingManager.RegisterFunction(mScriptingId, "ChangeAnimation", ChangeAnimFunc);

		ScriptingManager.RegisterFunction(mScriptingId, "IsKeyDown", InputFunc);

		std::function<float()> posFunc = std::bind(&cEntity::LGetX, this);
		ScriptingManager.RegisterFunctionInNamespace(mScriptingId, "Pos", "getX", posFunc);

		posFunc = std::bind(&cEntity::LGetY, this);
		ScriptingManager.RegisterFunctionInNamespace(mScriptingId, "Pos", "getY", posFunc);

		posFunc = std::bind(&cEntity::LGetZ, this);
		ScriptingManager.RegisterFunctionInNamespace(mScriptingId, "Pos", "getZ", posFunc);


		ScriptingManager.LoadFile(mScriptingId, Path);

		mScriptPath = Path;
		mHasScripting = true;

		lua_pcall(mLuaState, 0, 0, 0);

		ScriptingManager.RunFunction(mScriptingId, "Create");
	}

	inline void cEntity::LSetColor(float x, float y, float z)
	{
		SetColor(glm::vec3(x, y, z));
	}

	inline void cEntity::LMove(float x, float y, float z)// L for Lua
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

	inline void cEntity::LSetPosition(float x, float y, float z)
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

	inline void cEntity::LRotate(float x, float y, float z)
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

	inline void cEntity::LScale(float x, float y, float z)
	{
		mTransformation.Scale = glm::vec3(x, y, z);
		ModelTransform(mTransformation);
		if (mPhysics)
		{
			btVector3 myscale = btVector3(mTransformation.Scale.x, mTransformation.Scale.y, mTransformation.Scale.z);
			mRigidBody->getCollisionShape()->setLocalScaling(myscale);
		}
	}

	inline void cEntity::LApplyForce(float x, float y, float z)
	{
		if (mPhysics)
			ApplyForce(glm::vec3(x, y, z));
	}

	inline void cEntity::LApplyRelativeForce(float x, float y, float z)
	{
		if (mPhysics)
			ApplyRelativeForce(glm::vec3(x, y, z));
	}

	inline float cEntity::LGetX()
	{
		if (mPhysics)
		{
			const btTransform trans = mRigidBody->getWorldTransform();
			const btVector3 pos = trans.getOrigin();

			return pos.getX();
		}
		return mTransformation.Translation.x;
	}

	inline float cEntity::LGetY()
	{
		if (mPhysics)
		{
			const btTransform trans = mRigidBody->getWorldTransform();
			const btVector3 pos = trans.getOrigin();

			return pos.getY();
		}
		return mTransformation.Translation.y;
	}

	inline float cEntity::LGetZ()
	{
		if (mPhysics)
		{
			const btTransform trans = mRigidBody->getWorldTransform();
			const btVector3 pos = trans.getOrigin();

			return pos.getZ();
		}
		return mTransformation.Translation.z;
	}

	inline std::string cEntity::LGetName()
	{
		return mName;
	}

	inline void cEntity::LChangeAnim(const char* Path)
	{
		ChangeAnimation(Path);
	}


}