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
		virtual void Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions, uint32_t* SkyBox) = 0;
		virtual void Update(/*Should be taking in the timestep*/) = 0;
		virtual void SetPhysics(eBasicObject Object, void* PhysicsWorld) = 0;
		virtual void UnSetPhysics() = 0;
		virtual void setDynamic(bool IsDynamic) = 0;
		virtual void SetupScripting(const char*) = 0;
		virtual void SendEntity(cEntity* Entity) = 0;
		virtual void SendInputToScripting(std::function<bool(int)>) = 0;
		virtual void AddAnimation(const char*) = 0;
		void ModelTransform(sModel& model);
		void SetColor(glm::vec3 Color);
		void SetMaterial(sMaterial& Mat);
		void SetPhysicsProps(sPhysicsProps& Props);
		void ApplyForce(glm::vec3 Force);
		void ApplyRelativeForce(glm::vec3 Force);
		void isInvisible() { mShouldRender = !mShouldRender; }
		virtual void SetAABB(glm::vec3& Scale) = 0;
		virtual ~cEntity() = 0;

		glm::vec3 GetAABB() { return glm::vec3(mhalfExtents.x(), mhalfExtents.y(), mhalfExtents.z()); }
		sModel GetModel() { return mTransformation; }
		glm::mat4* GetModelMatrix() { return &mModel; }
		glm::vec3* GetColor() { return &mMaterial.Color; }
		int GetActivationState() { return mRigidBody->getActivationState(); }
		
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
		btVector3 mhalfExtents;
		eBasicObject Type = Cube;

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
}