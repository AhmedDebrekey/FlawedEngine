#include "OBJModel.h"


namespace FlawedEngine
{
	cOBJModel::cOBJModel(const char* FilePath, void* PhysicsWorld)
	{
		dynamicsWorld = (btDiscreteDynamicsWorld*)PhysicsWorld;

		if (!LoadModel(FilePath))
		{
			std::cout << "Failed To Load 3D Model from: " << FilePath << std::endl;
		}

		Populate();
		
		Renderer.Init(mVertexBuffer, mTextureCoords, mIndices);
	}

	cOBJModel::~cOBJModel()
	{
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

	void cOBJModel::SetPhysics()
	{
		if (!isPhysicsSet)
		{
			glm::vec3 Trans = mTransformation.Translation;
			glm::vec3 Rotation = mTransformation.Rotation;
			glm::vec3 Scale = mTransformation.Scale;

			groundShape = new btBoxShape(btVector3(btScalar(1.), btScalar(1.), btScalar(1.)));
			btTransform groundTransform;
			groundTransform.setIdentity();
			groundTransform.setOrigin(btVector3(Trans.x, Trans.y, Trans.z));
			groundShape->setLocalScaling(btVector3(btScalar(Scale.x), btScalar(Scale.y), btScalar(Scale.z)));
			btScalar mass(1.0);

			//rigidbody is dynamic if and only if mass is non zero, otherwise static
			bool isDynamic = (mass != 0.f);

			btVector3 localInertia(0, 0, 0);
			if (isDynamic)
				groundShape->calculateLocalInertia(mass, localInertia);

			btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
			
			mRidigBody = new btRigidBody(rbInfo);
			mRidigBody->setRestitution(.1);

			//add the body to the dynamics world
			dynamicsWorld->addRigidBody(mRidigBody);
			dynamicsWorld->updateSingleAabb(mRidigBody);
			std::cout << "setting up physics..." << std::endl;
			isPhysicsSet = true;
		}
	}

	void cOBJModel::Render(Transform& Trans, std::unordered_map<std::string, sLight>& LightPositions)
	{
		btTransform btTrans;
		glm::mat4 Model = glm::mat4(1.0f);

		if (mRidigBody != nullptr && mRidigBody->getMotionState())
		{
			mRidigBody->getMotionState()->getWorldTransform(btTrans);
			btVector3 smth = btTrans.getOrigin();
			Model = glm::translate(Model, glm::vec3(smth.getX(), smth.getY(), smth.getZ()));
			btVector3 myscale = groundShape->getLocalScaling();
			Model = glm::scale(Model, glm::vec3(myscale.getX(), myscale.getY(), myscale.getZ()));
			Trans.Model = Model;
		}
		else
		{
			Trans.Model = mModel;
		}

		Renderer.Draw(Trans, mMaterial, LightPositions);
	}

	void cOBJModel::Update()
	{

	}

	void cOBJModel::setDynamic(bool isDynamic)
	{
		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
		{
			btScalar mass(1.0);
			groundShape->calculateLocalInertia(mass, localInertia);
		}
		else
		{
			mRidigBody->setCollisionFlags(mRidigBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
		}
	}
}