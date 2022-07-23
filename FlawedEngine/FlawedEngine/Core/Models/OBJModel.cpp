#include "OBJModel.h"


namespace FlawedEngine
{
	cOBJModel::cOBJModel(const char* FilePath, void* PhysicsWorld)
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
		switch (Object)
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

	void cOBJModel::SetPhysics(eBasicObject Object)
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
			mCollisionShape->setLocalScaling(btVector3(btScalar(Scale.x), btScalar(Scale.y), btScalar(Scale.z)));

			btScalar mass(1.0);

			btVector3 localInertia(0, 0, 0);
			if (mass != 0.f) mCollisionShape->calculateLocalInertia(mass, localInertia);

			btDefaultMotionState* MotionState = new btDefaultMotionState(ObjectTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, MotionState, mCollisionShape, localInertia);
			
			mRidigBody = new btRigidBody(rbInfo);
			mRidigBody->setActivationState(DISABLE_DEACTIVATION);
			mRidigBody->setRollingFriction(10);

			mPhysicsDynamicWorld->addRigidBody(mRidigBody);
			mPhysicsDynamicWorld->updateSingleAabb(mRidigBody);

			isPhysicsSet = true;
		}
	}

	void cOBJModel::Render(Transform& Trans, std::unordered_map<std::string, sLight>& LightPositions)
	{

		if (mRidigBody != nullptr && mRidigBody->getMotionState())
		{
			btTransform btTrans;
			mRidigBody->getMotionState()->getWorldTransform(btTrans);
			
			btVector3 ObjectTransform = btTrans.getOrigin();
			btVector3 myscale = mCollisionShape->getLocalScaling();

			glm::mat4 Model = glm::mat4(1.0f);
			Model = glm::translate(Model, glm::vec3(ObjectTransform.getX(), ObjectTransform.getY(), ObjectTransform.getZ()));
			Model = glm::scale(Model, glm::vec3(myscale.getX(), myscale.getY(), myscale.getZ()));

			Trans.Model = Model;
		}
		else
			Trans.Model = mModel;

		Renderer.Draw(Trans, mMaterial, LightPositions);
	}

	void cOBJModel::Update()
	{
		//Should Update Physics here
	}

	void cOBJModel::setDynamic(bool isDynamic)
	{
		if (!isDynamic)
		{
			mRidigBody->setCollisionFlags(mRidigBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
		}
	}
}