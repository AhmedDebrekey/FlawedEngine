#pragma once
#include "../Entity.h"
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include "../Shader.h"
#include "Animations/Bone.h"


namespace FlawedEngine
{
    class Animator;
    class Animation;

    struct sBoneInfo
    {
        /*id is index in finalBoneMatrices*/
        int id;

        /*offset matrix transforms vertex from model space to bone space*/
        glm::mat4 offset;

    };

    struct AssimpNodeData
    {
        glm::mat4 transformation;
        std::string name;
        int childrenCount;
        std::vector<AssimpNodeData> children;
    };

    struct AABB
    {
        glm::vec3 mCenter{ 0.f, 0.f, 0.f };
        glm::vec3 mExtents{ 0.f, 0.f, 0.f };

        AABB(const glm::vec3& min, const glm::vec3& max)
            : mCenter{ (max + min) * 0.5f }, mExtents{ max.x - mCenter.x, max.y - mCenter.y, max.z - mCenter.z }
        {}

        AABB(const glm::vec3& inCenter, float iI, float iJ, float iK)
            : mCenter{ inCenter }, mExtents{ iI, iJ, iK }
        {}

        //see https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
        bool isOnOrForwardPlane(const Plane& plane) const
        {
            // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
            const float r = mExtents.x * std::abs(plane.normal.x) + mExtents.y * std::abs(plane.normal.y) +
                mExtents.z * std::abs(plane.normal.z);

            return -r <= plane.getSignedDistanceToPlane(mCenter);
        }
    };

    class cModel : public cEntity
    {
    public:
        cModel(const char* FilePath, std::string Name, void* PhysicsWorld, btAlignedObjectArray<btCollisionShape*>* CollisionShapes, Frustum* CamFrustum);
            
        ~cModel()
        {
            for (auto Mesh : mMeshes)
            {
                Mesh.DeleteTextures();
            }

            mLoadedTextures.clear();
            mLoadedTextures.shrink_to_fit();
            mMeshes.clear();
            mMeshes.shrink_to_fit();

            delete mAnimation;
            delete mAnimator;
        }
        virtual void Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions, uint32_t* SkyBox) override;
        virtual void Update(/*Should be taking in the timestep*/) override;
        void SetCollisionShape(eBasicObject Object);
        virtual void SetPhysics(eBasicObject Object, void* PhysicsWorld) override;
        virtual void UnSetPhysics() override;
        virtual void setDynamic(bool IsDynamic) override;
        virtual void SetupScripting(const char*) override;
        virtual void SendEntity(cEntity* Entity) override;
        virtual void SendInputToScripting(std::function<bool(int)>) override;
        virtual void AddAnimation(const char*) override;

        void LSetColor(float x, float y, float z);

        void LMove(float x, float y, float z);

        void LSetPosition(float x, float y, float z);

        void LRotate(float x, float y, float z);

        void LScale(float x, float y, float z);

        void LApplyForce(float x, float y, float z);

        void LApplyRelativeForce(float x, float y, float z);

        float LGetX();
        float LGetY();
        float LGetZ();

        std::string LGetName();

        std::map<std::string, sBoneInfo>& GetBoneInfoMap() { return m_BoneInfoMap; }
        int& GetBoneCount() { return m_BoneCounter; }
    
    private:
        bool isModelInFrustum();
        Frustum* mCamFrustum = nullptr;
        //AABB mAABB;

    private:
        void loadModel(std::string path);
        void CalculateAABB(const aiScene* scene);
        virtual void SetAABB(glm::vec3& Scale) override;
        btCollisionShape* CalculateMeshCollision(const aiScene* scene);
        void processNode(aiNode* node, const aiScene* scene);
        cMesh processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<sTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

        // model data 
        std::vector<sTexture> mLoadedTextures;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        std::vector<cMesh>    mMeshes;
        std::string mDirectory;
        bool gammaCorrection;
        cShader Shader;
    private:
        std::string mName;
        bool isPhysicsSet = false;
        btVector3 mInertia;
        btCollisionShape* mCollisionShape;
        btDiscreteDynamicsWorld* mPhysicsDynamicWorld;
        btAlignedObjectArray<btCollisionShape*>* mCollisionShapesArray;
    private:
        int ScriptingId;
        lua_State* LuaState = nullptr;
    private:
        Assimp::Importer importer;
        const aiScene* scene;
        std::map<std::string, sBoneInfo> m_BoneInfoMap; 
        int m_BoneCounter = 0;
        void SetVertexBoneDataToDefault(sVertex& vertex);
        void SetVertexBoneData(sVertex& vertex, int boneID, float weight);
        void ExtractBoneWeightForVertices(std::vector<sVertex>& vertices, aiMesh* mesh, const aiScene* scene);
        Animation* mAnimation = nullptr;
        Animator* mAnimator = nullptr;
        float mDeltaTime = 0, mLastFrame = 0;
    };
}