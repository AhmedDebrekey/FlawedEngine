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
		virtual void Render(sTransform&, std::unordered_map<std::string, sLight>&, uint32_t*, sGBufferObjects*) = 0;
		virtual void ShadowRender(sTransform&, glm::mat4&, uint32_t) = 0;
		virtual void Update(float deltaTime) = 0;
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
		void ReloadScript();
		void LSetColor(float x, float y, float z);
		void LMove(float x, float y, float z);
		void LSetPosition(float x, float y, float z);
		void LRotate(float x, float y, float z);
		void LScale(float x, float y, float z);
		void LApplyForce(float x, float y, float z);
		void LApplyRelativeForce(float x, float y, float z);
		void LSetPhysicsState(bool state);
		void LSetDynamic(bool state);
		cEntity* LSpawnObject(const char* name, uint8_t type);
		cEntity* LLoadObject(const char* path, const char* name);
		float LGetX();
		float LGetY();
		float LGetZ();
		float LRotGetX();
		float LRotGetY();
		float LRotGetZ();
		float LCamPitch();
		float LCamYaw();
		float LCamRoll();
		float LGetDeltaTime();
		std::string LGetName();
		void LChangeAnim(const char* Path);
		void LMoveCamera(float x, float y, float z);
		void LSetCameraPos(float x, float y, float z);
		void LRotateCamera(float pitch, float yaw, float roll);
		void LSetCameraRot(float pitch, float yaw, float roll);
		void LSetScript(const char* Path);
		void LRemoveObject();
		void LLog(const char*);
		int mScriptingId;
		lua_State* mLuaState = nullptr;
		cEntity* GetEntityByName(const char* name);
		void CallOnCollision(cEntity* entity);
		virtual void RemoveScripts() = 0;

		//Serlizalizations
		bool mIsCostume = false;
		bool mHasScripting = false;
		bool mHasAnimation = false;
		std::string mFilePath;
		std::string mScriptPath = "Empty";
		std::string mAnimationPath;

		//Physics
		btRigidBody* mRigidBody = nullptr;
		bool mDynamic = false;
		bool mPhysics = false;
		float mMass = 0.0f;
		float mFricton = 0.5f;
		float mRestitution = 0.0f;
		glm::vec3 mAngularForce;
		glm::vec3 mAABBOffset;
		bool mDead = false;
		eBasicObject Type = Cube;
		sAABB mAABB;
		bool mShowAABB = false;
		bool isPhysicsSet = false;
		btVector3 mInertia;
		btCompoundShape* mCollisionShape;
		btCollisionShape* mOriginalCollisionShape;
		btDiscreteDynamicsWorld* mPhysicsDynamicWorld;
		btAlignedObjectArray<btCollisionShape*>* mCollisionShapesArray;
		std::string mName;
		glm::vec3 GetAABB() { return mAABB.mExtents; }
		sModel GetModel() { return mTransformation; }
		glm::mat4* GetModelMatrix() { return &mModel; }
		glm::vec3* GetColor() { return &mMaterial.Color; }
		int GetActivationState() { return mRigidBody->getActivationState(); }
		void ApplyAABBOffest(glm::vec3& offset);
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
		float mDeltaTime = 0;

		cInput& Input = cInput::get();
		cScriptingManager& ScriptingManager = cScriptingManager::get();
	};


	inline cEntity::~cEntity()
	{
		mVertexBuffer.clear();
		mTextureCoords.clear();
		mIndices.clear();
		UnSetPhysics();
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
		{
			mRigidBody->activate(true);
		}
		btVector3 worldForce = btVector3(Force.x, Force.y, Force.z);
		mRigidBody->applyCentralForce(worldForce);
	}

	inline void cEntity::SetCollisionShape(eBasicObject Object)
	{
		btTransform localTrans;
		localTrans.setIdentity();
		
		switch (Object)
		{
		case FlawedEngine::Cube:
		{
			mOriginalCollisionShape = new btBoxShape(btVector3(btScalar(1.), btScalar(1.), btScalar(1.)));
		}
		break;
		case FlawedEngine::Sphere:
		{
			mOriginalCollisionShape = new btSphereShape(1.0);

		}
		break;
		case FlawedEngine::Custom:
		{
			mOriginalCollisionShape = new btBoxShape(btVector3(mAABB.mExtents.x, mAABB.mExtents.y, mAABB.mExtents.z));
		}
		break;
		case FlawedEngine::Cone:
		case FlawedEngine::Torus:
		case FlawedEngine::Triangle:
		case FlawedEngine::PointLight:
		case FlawedEngine::SpotLight:
		default:
			mOriginalCollisionShape = new btBoxShape(btVector3(mAABB.mExtents.x, mAABB.mExtents.y, mAABB.mExtents.z));
			break;
		}
		mCollisionShape = new btCompoundShape();
		mCollisionShape->addChildShape(localTrans, mOriginalCollisionShape);
	}

	inline void cEntity::setDynamic(bool IsDynamic)
	{
		if (!isPhysicsSet)
			return;
		if (!IsDynamic)
		{
			mRigidBody->setLinearVelocity(btVector3(0, 0, 0));
			mRigidBody->setAngularVelocity(btVector3(0, 0, 0));
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
			mCollisionShapesArray->remove(mCollisionShape);
			if (mCollisionShape->getNumChildShapes() > 0)
				mCollisionShape->removeChildShape(0);
			delete mRigidBody->getMotionState();
			delete mRigidBody->getCollisionShape();
			mPhysicsDynamicWorld->removeRigidBody(mRigidBody);
			delete mOriginalCollisionShape;
			delete mRigidBody;
			isPhysicsSet = false;
		}
	}

	inline void cEntity::ApplyAABBOffest(glm::vec3& offset)
	{
		if (!mRigidBody && !mCollisionShape)
			return;

		mAABBOffset = offset;

		btCollisionShape* child = mCollisionShape->getChildShape(0);
		btTransform localTransform;
		localTransform.setIdentity();
		localTransform.setOrigin(btVector3(offset.x, offset.y, offset.z));
		mCollisionShape->updateChildTransform(0, localTransform);
	}

	inline void cEntity::SetPhysics(eBasicObject Object, void* PhysicsWorld)
	{
		if (isPhysicsSet)
			return;

		glm::vec3 Trans = mTransformation.Translation;
		glm::vec3 Rotation = mTransformation.Rotation;
		glm::vec3 Scale = mTransformation.Scale;

		SetCollisionShape(Object);

		btTransform ObjectTransform;
		ObjectTransform.setIdentity();
		ObjectTransform.setOrigin(btVector3(Trans.x, Trans.y, Trans.z));

		glm::vec3 rotationEuler = glm::radians(Rotation);
		glm::quat rotationQuat = glm::quat(rotationEuler);
		ObjectTransform.setRotation(btQuaternion(rotationQuat.x, rotationQuat.y, rotationQuat.z, rotationQuat.w));

		btScalar mass(1.0);

		mInertia = btVector3(0, 0, 0);
		if (mass != 0.f) mCollisionShape->calculateLocalInertia(mass, mInertia);


		btDefaultMotionState* MotionState = new btDefaultMotionState(ObjectTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, MotionState, mCollisionShape, mInertia);
		mRigidBody = new btRigidBody(rbInfo);

		mCollisionShape->setLocalScaling(btVector3(btScalar(Scale.x), btScalar(Scale.y), btScalar(Scale.z)));
		mRigidBody->setActivationState(DISABLE_DEACTIVATION);
		//mRidigBody->setSleepingThresholds(0.2, 0.2);
		mRigidBody->setRestitution(mRestitution);
		mRigidBody->setFriction(mFricton);
		mRigidBody->setDamping(0.5, 0.5);
		mAngularForce = glm::vec3(1.0f);
		mRigidBody->setAngularFactor(btVector3(mAngularForce.x, mAngularForce.y, mAngularForce.z)); // Stop rotation around 0's

		mPhysicsDynamicWorld->addRigidBody(mRigidBody);
		mPhysicsDynamicWorld->updateSingleAabb(mRigidBody);

		mCollisionShapesArray->push_back(mCollisionShape);

		mCollisionShape->setUserPointer((void*)this);

		isPhysicsSet = true;
		mPhysics = true;
		mDynamic = false;
		
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
		
		funcptr = std::bind(&cEntity::LRotateCamera, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		ScriptingManager.RegisterFunction(mScriptingId, "RotateCamera", funcptr);
		
		funcptr = std::bind(&cEntity::LSetCameraRot, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		ScriptingManager.RegisterFunction(mScriptingId, "SetCameraRot", funcptr);

		funcptr = std::bind(&cEntity::LMoveCamera, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		ScriptingManager.RegisterFunction(mScriptingId, "MoveCamera", funcptr);
		
		funcptr = std::bind(&cEntity::LSetCameraPos, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		ScriptingManager.RegisterFunction(mScriptingId, "SetCameraPos", funcptr);

		std::function<void(const char*)> charInputfunc = std::bind(&cEntity::LChangeAnim, this, std::placeholders::_1);
		ScriptingManager.RegisterFunction(mScriptingId, "ChangeAnimation", charInputfunc);

		charInputfunc = std::bind(&cEntity::LSetScript, this, std::placeholders::_1);
		ScriptingManager.RegisterFunction(mScriptingId, "AddScript", charInputfunc);

		std::function<cEntity* (const char*)> entityfunc = std::bind(&cEntity::GetEntityByName, this, std::placeholders::_1);
		ScriptingManager.RegisterFunction(mScriptingId, "GetEntity", entityfunc);

		std::function<cEntity* (const char*, uint8_t)> spawnEntityfunc = std::bind(&cEntity::LSpawnObject, this, std::placeholders::_1, std::placeholders::_2);
		ScriptingManager.RegisterFunction(mScriptingId, "SpawnObject", spawnEntityfunc);

		std::function<cEntity* (const char*, const char*)> loadEntityfunc = std::bind(&cEntity::LLoadObject, this, std::placeholders::_1, std::placeholders::_2);
		ScriptingManager.RegisterFunction(mScriptingId, "LoadObject", loadEntityfunc);

		ScriptingManager.RegisterFunction(mScriptingId, "IsKeyDown", InputFunc);

		std::function<void(const char*)> logFunc = std::bind(&cEntity::LLog, this, std::placeholders::_1);
		ScriptingManager.RegisterFunction(mScriptingId, "Log", logFunc);

		std::function<float()> posFunc = std::bind(&cEntity::LGetX, this);
		ScriptingManager.RegisterFunctionInNamespace(mScriptingId, "Pos", "getX", posFunc);

		posFunc = std::bind(&cEntity::LGetY, this);
		ScriptingManager.RegisterFunctionInNamespace(mScriptingId, "Pos", "getY", posFunc);

		posFunc = std::bind(&cEntity::LGetZ, this);
		ScriptingManager.RegisterFunctionInNamespace(mScriptingId, "Pos", "getZ", posFunc);

		posFunc = std::bind(&cEntity::LRotGetX, this);
		ScriptingManager.RegisterFunctionInNamespace(mScriptingId, "Rot", "getX", posFunc);
		
		posFunc = std::bind(&cEntity::LRotGetY, this);
		ScriptingManager.RegisterFunctionInNamespace(mScriptingId, "Rot", "getY", posFunc);
		
		posFunc = std::bind(&cEntity::LRotGetZ, this);
		ScriptingManager.RegisterFunctionInNamespace(mScriptingId, "Rot", "getZ", posFunc);

		posFunc = std::bind(&cEntity::LCamPitch, this);
		ScriptingManager.RegisterFunction(mScriptingId, "CamPitch", posFunc);
		
		posFunc = std::bind(&cEntity::LCamYaw, this);
		ScriptingManager.RegisterFunction(mScriptingId, "CamYaw", posFunc);

		posFunc = std::bind(&cEntity::LGetDeltaTime, this);
		ScriptingManager.RegisterFunction(mScriptingId, "GetDeltaTime", posFunc);

		std::function<void(bool)> phxsState = std::bind(&cEntity::LSetPhysicsState, this, std::placeholders::_1);
		ScriptingManager.RegisterFunction(mScriptingId, "SetPhysics", phxsState);

		phxsState = std::bind(&cEntity::LSetDynamic, this, std::placeholders::_1);
		ScriptingManager.RegisterFunction(mScriptingId, "SetDynamic", phxsState);

		std::function<void()> removeFunc = std::bind(&cEntity::LRemoveObject, this);
		ScriptingManager.RegisterFunction(mScriptingId, "Remove", removeFunc);

		ScriptingManager.LoadFile(mScriptingId, Path);

		luabridge::getGlobalNamespace(mLuaState)
			.beginClass<cEntity>("cEntity")
			.addFunction("Move", &cEntity::LMove)
			.addFunction("SetPos", &cEntity::LSetPosition)
			.addFunction("Rotate", &cEntity::LRotate)
			.addFunction("Scale", &cEntity::LScale)
			.addFunction("ApplyForce", &cEntity::LApplyForce)
			.addFunction("ApplyRelativeForce", &cEntity::LApplyRelativeForce)
			.addFunction("SetPhysics", &cEntity::LSetPhysicsState)
			.addFunction("SetDynamic", &cEntity::LSetDynamic)
			.addFunction("ChangeColor", &cEntity::LSetColor)
			.addFunction("ChangeAnimation", &cEntity::LChangeAnim)
			.addFunction("AddScript", &cEntity::LSetScript)
			.addFunction("Remove", &cEntity::LRemoveObject)
			.addFunction("pgetX", &cEntity::LGetX)
			.addFunction("pgetY", &cEntity::LGetY)
			.addFunction("pgetZ", &cEntity::LGetZ)
			.addFunction("rgetX", &cEntity::LRotGetX)
			.addFunction("rgetY", &cEntity::LRotGetY)
			.addFunction("rgetZ", &cEntity::LRotGetZ)
			.endClass();

		mScriptPath = Path;
		mHasScripting = true;

		lua_pcall(mLuaState, 0, 0, 0);

		//ScriptingManager.RunFunction(mScriptingId, "Create");
	}

	inline void cEntity::ReloadScript()
	{
		if (mHasScripting)
		{
			ScriptingManager.LoadFile(mScriptingId, mScriptPath);
			lua_pcall(mLuaState, 0, 0, 0);
			//ScriptingManager.RunFunction(mScriptingId, "Create");
		}
	}

	inline cEntity* cEntity::GetEntityByName(const char* name)
	{
		if (name == nullptr || name[0] == '\0') {
			//EngineLog("Get Entity Failed", Error);
			return nullptr;
		}

		void* entity = GetEntity(name);
		if (!entity) {
			//EngineLog("Get Entity Failed", Error);
			return nullptr;
		}

		return static_cast<cEntity*>(entity);
	}

	inline void cEntity::CallOnCollision(cEntity* entity)
	{
		if (mHasScripting)
		{
			ScriptingManager.RunFunctionWithArgs(mScriptingId, "OnCollision", entity->mName);
		}
	}

	inline void cEntity::LSetColor(float x, float y, float z)
	{
		SetColor(glm::vec3(x, y, z));
	}

	inline cEntity* cEntity::LSpawnObject(const char* name, uint8_t type)
	{
		cEntity* ptr = (cEntity*)SpawnEntity(name, (eBasicObject)type);
		return ptr;
	}

	inline cEntity* cEntity::LLoadObject(const char* path, const char* name)
	{
		cEntity* ptr = (cEntity*)LoadEntity(path, name);
		return ptr;
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
		// Update the rotation
		mTransformation.Rotation = glm::vec3(x, y, z);

		// Ensure that the rotation values are within the range of 0 to 360 degrees
		mTransformation.Rotation.x = fmod(mTransformation.Rotation.x, 360.0f);
		mTransformation.Rotation.y = fmod(mTransformation.Rotation.y, 360.0f);
		mTransformation.Rotation.z = fmod(mTransformation.Rotation.z, 360.0f);

		ModelTransform(mTransformation);

		if (mPhysics)
		{
			btTransform Trans;
			mRigidBody->getMotionState()->getWorldTransform(Trans);

			// Create a quaternion for rotation
			btQuaternion quat = btQuaternion(glm::radians(mTransformation.Rotation.y), glm::radians(mTransformation.Rotation.x), glm::radians(mTransformation.Rotation.z));

			// Set the rotation of the transformation
			Trans.setRotation(quat);

			// Set the world transform
			mRigidBody->getMotionState()->setWorldTransform(Trans);
			mRigidBody->setWorldTransform(Trans);
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
		{
			ApplyForce(glm::vec3(x, y, z));
		}
	}

	inline void cEntity::LApplyRelativeForce(float x, float y, float z)
	{
		if (mPhysics)
			ApplyRelativeForce(glm::vec3(x, y, z));
	}

	inline void cEntity::LSetPhysicsState(bool state)
	{
		mPhysics = state;

		if (mPhysics)
		{
			SetPhysics(Type, GetPhxsWorld());
		}
		else
		{
			UnSetPhysics();
		}
	}

	inline void cEntity::LSetDynamic(bool state)
	{
		mDynamic = state;
		setDynamic(mDynamic);
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

	inline float cEntity::LRotGetX()
	{
		return mTransformation.Rotation.x;
	}	
	
	inline float cEntity::LRotGetY()
	{
		return mTransformation.Rotation.y;
	}
	
	inline float cEntity::LRotGetZ()
	{
		return mTransformation.Rotation.z;
	}

	inline float cEntity::LCamPitch()
	{
		return GetCamPitch();
	}

	inline float cEntity::LCamYaw()
	{
		return GetCamYaw();
	}

	inline float cEntity::LGetDeltaTime()
	{
		return mDeltaTime;
	}

	inline std::string cEntity::LGetName()
	{
		return mName;
	}

	inline void cEntity::LChangeAnim(const char* Path)
	{
		ChangeAnimation(Path);
	}

	inline void cEntity::LMoveCamera(float dx, float dy, float dz)
	{
		MoveCamera(dx, dy, dz);
	}
	
	inline void cEntity::LRotateCamera(float pitch, float yaw, float roll)
	{
		RotateCamera(pitch, yaw, roll);
	}

	inline void cEntity::LSetCameraRot(float pitch, float yaw, float roll)
	{
		SetCameraRotation(pitch, yaw, roll);
	}

	inline void cEntity::LSetCameraPos(float x, float y, float z)
	{
		SetCameraPos(x, y, z);
	}

	inline void cEntity::LSetScript(const char* Path)
	{
		SetupScripting(Path, GetInputFunc());
		ScriptingManager.RunFunction(mScriptingId, "Create");
		
	}
	
	inline void cEntity::LRemoveObject()
	{
		RemoveEntity(mName.c_str());
	}

	inline void cEntity::LLog(const char* msg)
	{
		EngineLog(msg, Script);
	}
}