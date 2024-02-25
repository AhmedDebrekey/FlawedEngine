#include "OBJModel.h"
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "stb_image.h"

namespace FlawedEngine
{
	
	cOBJModel::cOBJModel(const char* FilePath, std::string Name, void* PhysicsWorld, btAlignedObjectArray<btCollisionShape*>* CollisionShapes)
	{
		mCollisionShapesArray = CollisionShapes;
		mName = Name;
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



	void cOBJModel::AddAnimation(const char*)
	{
	}

	void cOBJModel::ChangeAnimation(const char*)
	{
	}

	void cOBJModel::SetAABB(glm::vec3& Scale)
	{
	}

	bool cOBJModel::isModelInFrustum()
	{
		return true;
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
		ScriptingManager.RunFunction(mScriptingId, "Update");
	}
}