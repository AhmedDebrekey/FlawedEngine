#pragma once

#include "Core.h"
#include "Input/Input.h"

#include <iostream>

#include <unordered_map>

#include <Bullet/btBulletDynamicsCommon.h>


namespace FlawedEngine
{
	class cEntity
	{
	public:
		sModel mTransformation; //I can't find a better name

		virtual void Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions) = 0;
		virtual void Update(/*Should be taking in the timestep, Maybe make deltatime a singleton that is avaliable from the Engine*/) = 0;
		virtual void SetPhysics(eBasicObject Object, void* PhysicsWorld) = 0;
		virtual void setDynamic(bool IsDynamic) = 0;
		void ModelTransform(sModel& model);
		void SetColor(glm::vec3 Color);
		void SetMaterial(sMaterial& Mat);
		void SetPhysicsProps(sPhysicsProps& Props);
		void ApplyForce(glm::vec3 Force);
		virtual ~cEntity() = 0;

		sModel GetModel() { return mTransformation; }
		glm::vec3* GetColor() { return &mMaterial.Color; }

		sMaterial mMaterial;

		glm::mat4 mModel;

		std::vector<sVertex> mVertexBuffer;
		std::vector<uint32_t> mIndices;
		std::vector<sTexture> mTextureCoords;
		//std::vector<float> mIndexBuffer;

		btRigidBody* mRidigBody = nullptr;
		bool mDynamic;

		float mMass = 0.0f;
		float mFricton = 0.5;
		float mRestitution = 0.0f;

		bool Dead = false;

		cInput& Input = cInput::get();
	};

	inline cEntity::~cEntity()
	{
		mVertexBuffer.clear();
		mTextureCoords.clear();
		mIndices.clear();
	}

	inline void cEntity::ModelTransform(sModel& model)
	{//this is a big mess tbh, i am confusion
		mTransformation = model;
		glm::mat4 Model = glm::mat4(1.0f);
		Model = glm::translate(Model, mTransformation.Translation);
		Model = glm::rotate(Model, glm::radians(mTransformation.Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		Model = glm::rotate(Model, glm::radians(mTransformation.Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		Model = glm::rotate(Model, glm::radians(mTransformation.Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
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

	inline void cEntity::ApplyForce(glm::vec3 Force)
	{
		if (!mRidigBody->isActive())
			mRidigBody->activate(true);
		btVector3 relativeForce = btVector3(Force.x, Force.y, Force.z);
		btTransform boxTrans;
		mRidigBody->getMotionState()->getWorldTransform(boxTrans);
		btVector3 RelativeTransform = relativeForce;
		btVector3 correctedForce = (boxTrans * relativeForce) - boxTrans.getOrigin();
		relativeForce = (boxTrans * RelativeTransform) - boxTrans.getOrigin();
		mRidigBody->applyCentralForce(relativeForce);
	}
}