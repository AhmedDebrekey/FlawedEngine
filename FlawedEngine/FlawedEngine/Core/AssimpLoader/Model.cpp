#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../Models/stb_image.h"

#include <Bullet/BulletCollision/CollisionShapes/btShapeHull.h>.
#include <Bullet/btBulletCollisionCommon.h>

#include <GLFW/glfw3.h>
#include "Animations/Animation.h"
#include "Animations/Animator.h"

namespace FlawedEngine
{
	cModel::cModel(const char* FilePath, std::string Name, void* PhysicsWorld, btAlignedObjectArray<btCollisionShape*>* CollisionShapes, Frustum* CamFrustum, void* Graphics_API)
		:mCollisionShapesArray(CollisionShapes), mName(Name), mGraphics_API((cGraphicsAPI*)Graphics_API)
	{
		mPhysicsDynamicWorld = (btDiscreteDynamicsWorld*)PhysicsWorld;
		loadModel(FilePath);
		mFilePath = FilePath;
		mIsCostume = true;
		mCamFrustum = CamFrustum;
	}
	void cModel::Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions, uint32_t* SkyBox)
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


		mShouldRender = isModelInFrustum();

		if (mShouldRender)
		{
			for (uint32_t i = 0; i < mMeshes.size(); i++)
			{
				std::vector<glm::mat4>	emptyVector;
				if (mAnimator)
				{
					mMeshes[i].Draw(Trans, mMaterial, LightPositions, SkyBox, Shader, mAnimator->GetFinalBoneMatrices());
				}
				else
				{
					mMeshes[i].Draw(Trans, mMaterial, LightPositions, SkyBox, Shader, emptyVector);
				}
			}
		}
	}

	void cModel::ShadowRender(sTransform& Trans, glm::mat4& LightSpaceMatrix, uint32_t DepthMap)
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

		mShouldRender = isModelInFrustum();

		if (mShouldRender)
		{
			for (uint32_t i = 0; i < mMeshes.size(); i++)
			{
				mMeshes[i].ShadowDraw(Trans, mShadowShader, LightSpaceMatrix, DepthMap);
			}
		}
	}

	void cModel::Update()
	{
		ScriptingManager.RunFunction(ScriptingId, "Update");
		float currentFrame = glfwGetTime();
		mDeltaTime = currentFrame - mLastFrame;
		mLastFrame = currentFrame;

		mShouldRender = isModelInFrustum();

		if (mAnimator && mShouldRender)
			mAnimator->UpdateAnimation(mDeltaTime);

	}

	void cModel::SetCollisionShape(eBasicObject Object)
	{
		switch (Object) //if it is not set code will crash ofc
		{
		case FlawedEngine::Cube:
		{
			mCollisionShape = new btBoxShape(btVector3(mAABB.mExtents.x, mAABB.mExtents.y, mAABB.mExtents.z));
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

	void cModel::SetPhysics(eBasicObject Object, void* PhysicsWorld)
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

	void cModel::UnSetPhysics()
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

	void cModel::setDynamic(bool IsDynamic)
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

	void cModel::SendInputToScripting(std::function<bool(int)> func)
	{
		std::cout << "[C++] SendInputToScripting was called." << std::endl;
	}

	void cModel::AddAnimation(const char* Path)
	{
		if (mCurrentAnimation == nullptr)
		{
			mAnimationsMap[Path] = std::make_shared<Animation>();
			mAnimationsMap[Path]->Setup(Path, this, &importer);

			mCurrentAnimation = mAnimationsMap[Path];

			mAnimator = std::make_shared<Animator>();
			mAnimator->Setup(mAnimationsMap[Path].get());

			mHasAnimation = true;
			mAnimationPath = Path;
		}
	}

	void cModel::ChangeAnimation(const char* Path)
	{
		if (mAnimator != nullptr)
		{
			if (mAnimationsMap.find(Path) != mAnimationsMap.end())
			{
				if (mCurrentAnimation == mAnimationsMap[Path])
					return;


				mAnimator->ChangeAnim(mAnimationsMap[Path]);
				mCurrentAnimation = mAnimationsMap[Path];
				return;
			}

			mAnimationsMap[Path] = std::make_shared<Animation>();
			mAnimationsMap[Path]->Setup(Path, this, &importer);

			mCurrentAnimation = mAnimationsMap[Path];

			mAnimator->ChangeAnim(mAnimationsMap[Path]);
			return;
		}
	}


	void cModel::LSetColor(float x, float y, float z)
	{
		SetColor(glm::vec3(x, y, z));
	}

	void cModel::LMove(float x, float y, float z)// L for Lua
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

	void cModel::LSetPosition(float x, float y, float z)
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

	void cModel::LRotate(float x, float y, float z)
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

	void cModel::LScale(float x, float y, float z)
	{
		mTransformation.Scale = glm::vec3(x, y, z);
		ModelTransform(mTransformation);
		if (mPhysics)
		{
			btVector3 myscale = btVector3(mTransformation.Scale.x, mTransformation.Scale.y, mTransformation.Scale.z);
			mRigidBody->getCollisionShape()->setLocalScaling(myscale);
		}
	}

	void cModel::LApplyForce(float x, float y, float z)
	{
		if (mPhysics)
			ApplyForce(glm::vec3(x, y, z));
	}

	void cModel::LApplyRelativeForce(float x, float y, float z)
	{
		if (mPhysics)
			ApplyRelativeForce(glm::vec3(x, y, z));
	}

	float cModel::LGetX()
	{
		if (mPhysics)
		{
			const btTransform trans = mRigidBody->getWorldTransform();
			const btVector3 pos = trans.getOrigin();

			return pos.getX();
		}
		return mTransformation.Translation.x;
	}

	float cModel::LGetY()
	{
		if (mPhysics)
		{
			const btTransform trans = mRigidBody->getWorldTransform();
			const btVector3 pos = trans.getOrigin();

			return pos.getY();
		}
		return mTransformation.Translation.y;
	}

	float cModel::LGetZ()
	{
		if (mPhysics)
		{
			const btTransform trans = mRigidBody->getWorldTransform();
			const btVector3 pos = trans.getOrigin();

			return pos.getZ();
		}
		return mTransformation.Translation.z;
	}

	std::string cModel::LGetName()
	{
		return mName;
	}

	void cModel::LChangeAnim(const char* Path)
	{
		ChangeAnimation(Path);
	}

	void cModel::SetupScripting(const char* Path, std::function<bool(int)>& InputFunc)
	{
		ScriptingId = ScriptingManager.InitScripting();
		LuaState = ScriptingManager.GetLuaState(ScriptingId);

		ScriptingManager.RegisterFunction(ScriptingId, "Move",						std::bind(&cModel::LMove,				this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ScriptingManager.RegisterFunction(ScriptingId, "SetPos",					std::bind(&cModel::LSetPosition,		this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ScriptingManager.RegisterFunction(ScriptingId, "Rotate",					std::bind(&cModel::LRotate,				this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ScriptingManager.RegisterFunction(ScriptingId, "Scale",						std::bind(&cModel::LScale,				this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ScriptingManager.RegisterFunction(ScriptingId, "ApplyForce",				std::bind(&cModel::LApplyForce,			this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ScriptingManager.RegisterFunction(ScriptingId, "ApplyRelativeForce",		std::bind(&cModel::LApplyRelativeForce, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ScriptingManager.RegisterFunction(ScriptingId, "ChangeColor",				std::bind(&cModel::LSetColor,			this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ScriptingManager.RegisterFunctionInNamespace(ScriptingId, "Pos", "getX",	std::bind(&cModel::LGetX,				this));
		ScriptingManager.RegisterFunctionInNamespace(ScriptingId, "Pos", "getY",	std::bind(&cModel::LGetY,				this));
		ScriptingManager.RegisterFunctionInNamespace(ScriptingId, "Pos", "getZ",	std::bind(&cModel::LGetZ,				this));

		
		std::function<std::string()> Func = std::bind(&cModel::LGetName, this);
		luabridge::getGlobalNamespace(LuaState).addFunction("GetName", Func);

		luabridge::getGlobalNamespace(LuaState).addFunction("IsKeyDown", InputFunc);

		std::function<void(const char*)> ChangeAnimFunc = std::bind(&cModel::LChangeAnim, this, std::placeholders::_1);
		luabridge::getGlobalNamespace(LuaState).addFunction("ChangeAnimation", ChangeAnimFunc);

		ScriptingManager.LoadFile(ScriptingId, Path);

		mScriptPath = Path;
		mHasScripting = true;

		lua_pcall(LuaState, 0, 0, 0);

		ScriptingManager.RunFunction(ScriptingId, "Create");
	}

	void cModel::SendEntity(cEntity* Entity)
	{
	}

	void cModel::loadModel(std::string path)
	{
		scene = importer.ReadFile(path, 
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals |
			aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_GlobalScale | aiProcess_ValidateDataStructure);


		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}

		CalculateAABB(scene);

		mDirectory = path.substr(0, path.find_last_of('\\'));

		processNode(scene->mRootNode, scene);

		Shader.Create("Core/Models/Shaders/Vertex.glsl", "Core/Models/Shaders/Fragment.glsl");
		mShadowShader.Create("Core/Models/Shaders/ShadowVertex.glsl", "Core/Models/Shaders/ShadowFragment.glsl");
		Shader.Bind();
		Shader.Unbind();
	}

	void cModel::CalculateAABB(const aiScene* scene)
	{
		// Calculate overall bounding box dimensions
		glm::vec3 overallMin(FLT_MAX, FLT_MAX, FLT_MAX), overallMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[i];
			btVector3 min(FLT_MAX, FLT_MAX, FLT_MAX), max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
			for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
				aiVector3D v = mesh->mVertices[j];
				if (v.x < min.x()) min.setX(v.x);
				if (v.y < min.y()) min.setY(v.y);
				if (v.z < min.z()) min.setZ(v.z);
				if (v.x > max.x()) max.setX(v.x);
				if (v.y > max.y()) max.setY(v.y);
				if (v.z > max.z()) max.setZ(v.z);
			}
			if (min.x() < overallMin.x) overallMin.x = (min.x());
			if (min.y() < overallMin.y) overallMin.y = (min.y());
			if (min.z() < overallMin.z) overallMin.z = (min.z());
			if (max.x() > overallMax.x) overallMax.x = (max.x());
			if (max.y() > overallMax.y) overallMax.y = (max.y());
			if (max.z() > overallMax.z) overallMax.z = (max.z());
		}

		mAABB = sAABB(overallMin, overallMax);
	}

	// Function to test if an AABB is inside the frustum
	bool cModel::isModelInFrustum()
	{
		//Get global scale thanks to our transform
		const glm::vec3 globalCenter{ mModel * glm::vec4(mAABB.mCenter, 1.f) };

		// Scaled orientation
		const glm::vec3 right = mModel[0] * mAABB.mExtents.x;
		const glm::vec3 up = mModel[1] * mAABB.mExtents.y;
		const glm::vec3 forward = (-mModel[2]) * mAABB.mExtents.z;

		const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
			std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
			std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

		const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

		const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

		const sAABB globalAABB(globalCenter, newIi, newIj, newIk);


		return (globalAABB.isOnOrForwardPlane(mCamFrustum->leftFace) &&
				globalAABB.isOnOrForwardPlane(mCamFrustum->rightFace) &&
				globalAABB.isOnOrForwardPlane(mCamFrustum->topFace) &&
				globalAABB.isOnOrForwardPlane(mCamFrustum->bottomFace) &&
				globalAABB.isOnOrForwardPlane(mCamFrustum->nearFace) &&
				globalAABB.isOnOrForwardPlane(mCamFrustum->farFace));
	}

	void cModel::SetAABB(glm::vec3& Scale)
	{
		//To be implemented
	}


	btCollisionShape* cModel::CalculateMeshCollision(const aiScene* scene)
	{
		// Create a Bullet Physics mesh object
		btTriangleMesh* triangleMesh = new btTriangleMesh();

		// Convert the mesh data to Bullet Physics format
		for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		{
			const aiMesh* mesh = scene->mMeshes[i];
			for (unsigned int j = 0; j < mesh->mNumFaces; j++)
			{
				const aiFace& face = mesh->mFaces[j];
				btVector3 vertex1(mesh->mVertices[face.mIndices[0]].x, mesh->mVertices[face.mIndices[0]].y, mesh->mVertices[face.mIndices[0]].z);
				btVector3 vertex2(mesh->mVertices[face.mIndices[1]].x, mesh->mVertices[face.mIndices[1]].y, mesh->mVertices[face.mIndices[1]].z);
				btVector3 vertex3(mesh->mVertices[face.mIndices[2]].x, mesh->mVertices[face.mIndices[2]].y, mesh->mVertices[face.mIndices[2]].z);
				triangleMesh->addTriangle(vertex1, vertex2, vertex3);
			}
		}

		// Create a collision shape from the mesh object
		btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(triangleMesh, true);
		return shape;
	}

	void cModel::processNode(aiNode* node, const aiScene* scene)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* Mesh = scene->mMeshes[node->mMeshes[i]];
			mMeshes.push_back(processMesh(Mesh, scene));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	cMesh cModel::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<sVertex> Vertecies;
		std::vector<uint32_t> Indecides;
		std::vector<sTexture> Textures;

		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			sVertex Vertex;
			SetVertexBoneDataToDefault(Vertex);

			glm::vec3 Vector;
			//Positions
			Vector.x = mesh->mVertices[i].x;
			Vector.y = mesh->mVertices[i].y;
			Vector.z = mesh->mVertices[i].z;
			Vertex.Postion = Vector;
			//Normals
			if (mesh->HasNormals())
			{
				Vector.x = mesh->mNormals[i].x;
				Vector.y = mesh->mNormals[i].y;
				Vector.z = mesh->mNormals[i].z;
				Vertex.Normal = Vector;
			}
			//Tex Coords
			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).

				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				Vertex.TexCoords = vec;
				//Tangents
				if (mesh->HasTangentsAndBitangents())
				{
					Vector.x = mesh->mTangents[i].x;
					Vector.y = mesh->mTangents[i].y;
					Vector.z = mesh->mTangents[i].z;
					Vertex.Tangent = Vector;
					//Bitangents
					Vector.x = mesh->mBitangents[i].x;
					Vector.y = mesh->mBitangents[i].y;
					Vector.z = mesh->mBitangents[i].z;
					Vertex.BiTangent = Vector;
				}
			}
			else
					Vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			
			Vertecies.push_back(Vertex);

		}

		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; j++)
				Indecides.push_back(face.mIndices[j]);
		}
		
		aiMaterial* Material = scene->mMaterials[mesh->mMaterialIndex];

		/*std::cout << scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str() << std::endl;
		std::cout << "Diffuse :" << scene->mMaterials[mesh->mMaterialIndex]->GetTextureCount(aiTextureType_DIFFUSE) << std::endl;
		std::cout << "Specular:" << scene->mMaterials[mesh->mMaterialIndex]->GetTextureCount(aiTextureType_SPECULAR) << std::endl;
		std::cout << "Normal  :" << scene->mMaterials[mesh->mMaterialIndex]->GetTextureCount(aiTextureType_NORMALS) << std::endl << std::endl;*/

		std::vector<sTexture> diffuseMaps = loadMaterialTextures(Material, aiTextureType_DIFFUSE, "texture_diffuse");
		Textures.insert(Textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<sTexture> specularMaps = loadMaterialTextures(Material, aiTextureType_SPECULAR, "texture_specular");
		Textures.insert(Textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<sTexture> normalMaps = loadMaterialTextures(Material, aiTextureType_NORMALS, "texture_normal");
		Textures.insert(Textures.end(), normalMaps.begin(), normalMaps.end());

		std::vector<sTexture> heightMaps = loadMaterialTextures(Material, aiTextureType_AMBIENT, "texture_height");
		Textures.insert(Textures.end(), heightMaps.begin(), heightMaps.end());

		ExtractBoneWeightForVertices(Vertecies, mesh, scene);

		return cMesh(Vertecies, Indecides, Textures, mGraphics_API);
	}

	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma, cGraphicsAPI* Graphics_API)
	{
		std::string filename = std::string(path);
		filename = directory + '\\' + filename;

		int width = 0, height = 0, nrComponents = 0;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		unsigned int textureID;
		if (data)
		{
			textureID = Graphics_API->CreateTexture(width, height, false, data, nrComponents);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}
	
	std::vector<sTexture> cModel::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
	{
		std::vector<sTexture> Textures;
		for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			
			bool skip = false;
			for (uint32_t j = 0; j < mLoadedTextures.size(); j++)
			{
				if (std::strcmp(mLoadedTextures[j].Path.data(), str.C_Str()) == 0)
				{
					Textures.push_back(mLoadedTextures[j]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{
				sTexture Texture;
 				Texture.ID = TextureFromFile(str.C_Str(), mDirectory, false, mGraphics_API);
				Texture.Type = typeName;
				Texture.Path = str.C_Str();
				Textures.push_back(Texture);
				mLoadedTextures.push_back(Texture);
			}
		}
		return Textures;
	}

	void cModel::SetVertexBoneDataToDefault(sVertex& vertex)
	{
		for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
		{
			vertex.mBoneIDs[i] = -1;
			vertex.mWeights[i] = 0.0f;
		}
	}

	void display_v3(const std::string& tag, const glm::vec3& v3)
	{
		std::cout
			<< tag
			<< "\n| "
			<< v3.x << '\t'
			<< v3.y << '\t'
			<< v3.z << '\t'
			<< "\n"
			;
	}

	void cModel::SetVertexBoneData(sVertex& vertex, int boneID, float weight)
	{
		for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
		{
			if (vertex.mBoneIDs[i] < 0)
			{
				if (weight == 0)
					break;

				vertex.mWeights[i] = weight;
				vertex.mBoneIDs[i] = boneID;
				break;
			}
		}
	}


	void cModel::ExtractBoneWeightForVertices(std::vector<sVertex>& vertices, aiMesh* mesh, const aiScene* scene)
	{
		auto& boneInfoMap = m_BoneInfoMap;
		int& boneCount = m_BoneCounter;

		for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
		{
			int boneID = -1;
			std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				sBoneInfo newBoneInfo;
				newBoneInfo.id = boneCount;
				newBoneInfo.offset = ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
				boneInfoMap[boneName] = newBoneInfo;
				boneID = boneCount;
				boneCount++;
			}
			else
			{
				boneID = boneInfoMap[boneName].id;
			}
			assert(boneID != -1);
			auto weights = mesh->mBones[boneIndex]->mWeights;
			int numWeights = mesh->mBones[boneIndex]->mNumWeights;

			for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
			{
				int vertexId = weights[weightIndex].mVertexId;
				float weight = weights[weightIndex].mWeight;
				assert(vertexId <= vertices.size());
				SetVertexBoneData(vertices[vertexId], boneID, weight);
			}
		}
	}
}