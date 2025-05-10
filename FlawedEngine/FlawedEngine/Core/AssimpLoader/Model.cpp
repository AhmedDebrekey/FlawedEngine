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
#include "../Models/ObjectManager.h"


namespace FlawedEngine
{
	cModel::cModel(const char* FilePath, std::string Name, void* PhysicsWorld, btAlignedObjectArray<btCollisionShape*>* CollisionShapes, Frustum* CamFrustum, void* Graphics_API)
		:mGfxAPI((cGraphicsAPI*)Graphics_API)
	{
		mCollisionShapesArray = CollisionShapes;
		mName = Name;
		mPhysicsDynamicWorld = (btDiscreteDynamicsWorld*)PhysicsWorld;
		mAABBOffset = glm::vec3(0.0f);
		loadModel(FilePath);
		mFilePath = FilePath;
		mIsCostume = true;
		mCamFrustum = CamFrustum;
	}
	void cModel::Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions, uint32_t* SkyBox, sGBufferObjects* GeometryObject)
	{
		SetModelTransform(Trans);

		if (mShouldRender)
		{
			for (uint32_t i = 0; i < mMeshes.size(); i++)
			{
				std::vector<glm::mat4>	emptyVector;
				if (mAnimator)
				{
					mMeshes[i].Draw(Trans, mMaterial, LightPositions, SkyBox, mGeometryShader, mLightShader, mAnimator->GetFinalBoneMatrices(), GeometryObject);
				}
				else
				{
					mMeshes[i].Draw(Trans, mMaterial, LightPositions, SkyBox, mGeometryShader, mLightShader, emptyVector, GeometryObject);
				}
			}
		}
	}

	void cModel::SetModelTransform(sTransform& Trans)
	{
		if (mRigidBody != nullptr && mRigidBody->getMotionState() && isPhysicsSet)
		{
			btTransform btTrans;
			mRigidBody->getMotionState()->getWorldTransform(btTrans);

			btVector3 pos = btTrans.getOrigin();
			btVector3 scl = mRigidBody->getCollisionShape()->getLocalScaling();
			btQuaternion quat = mRigidBody->getCenterOfMassTransform().getRotation();

			glm::quat glmQuat(quat.w(), quat.x(), quat.y(), quat.z());
			glm::vec3 eulerDegrees = glm::degrees(glm::eulerAngles(glmQuat));

			glm::mat4 Model = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x(), pos.y(), pos.z()));
			Model *= glm::mat4_cast(glmQuat);
			Model = glm::scale(Model, glm::vec3(scl.x(), scl.y(), scl.z()));

			Trans.Model = Model;
			mModel = Model;

			mTransformation.Translation = glm::vec3(pos.x(), pos.y(), pos.z());
			mTransformation.Rotation = glm::vec4(eulerDegrees, 0.0f); // W unused in this case
			mTransformation.Scale = glm::vec3(scl.x(), scl.y(), scl.z());

		}
		else
			Trans.Model = mModel;


		mShouldRender = isModelInFrustum();
	}

	void cModel::ShadowRender(sTransform& Trans, glm::mat4& LightSpaceMatrix, uint32_t DepthMap)
	{
		SetModelTransform(Trans);

		if (mShouldRender)
		{
			for (uint32_t i = 0; i < mMeshes.size(); i++)
			{
				mMeshes[i].ShadowDraw(Trans, mShadowShader, LightSpaceMatrix, DepthMap);
			}
		}
	}

	void cModel::Update(float deltaTime)
	{
		mShouldRender = isModelInFrustum();
		if (mIsPlaying)
		{
			ScriptingManager.RunFunction(mScriptingId, "Update");

			mDeltaTime = deltaTime;


			if (mAnimator && mShouldRender)
				mAnimator->UpdateAnimation(mDeltaTime);
		}
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

	void cModel::loadModel(std::string path)
	{
		//importer.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 0.1f);

		scene = importer.ReadFile(path, 
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals |
			aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_GlobalScale |
			aiProcess_ValidateDataStructure);


		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			EngineLog("ASSIMP: " + std::string(importer.GetErrorString()), Error);
			return;
		}

		CalculateAABB(scene);

		mDirectory = path.substr(0, path.find_last_of('\\'));

		processNode(scene->mRootNode, scene);


		mGeometryShader.Create("Core/Models/Shaders/Vertex.glsl", "Core/Models/Shaders/Fragment.glsl", mName.c_str());
		mLightShader.Create("Core/Models/Shaders/LightVertex.glsl", "Core/Models/Shaders/LightFragment.glsl", mName.c_str());
		mShadowShader.Create("Core/Models/Shaders/ShadowVertex.glsl", "Core/Models/Shaders/ShadowFragment.glsl", mName.c_str());
		mGeometryShader.Bind();
		mGeometryShader.Unbind();
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

	bool cModel::isModelInFrustum()
	{
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
}