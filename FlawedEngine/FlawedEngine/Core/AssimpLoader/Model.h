#pragma once
#include "../Entity.h"
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include "../Shader.h"
#include "Animations/Bone.h"
#include "../Graphics/GraphicsAPI.h"


namespace FlawedEngine
{
    class Animator;
    class Animation;


    struct AssimpNodeData
    {
        glm::mat4 transformation;
        std::string name;
        int childrenCount;
        std::vector<AssimpNodeData> children;
    };

    class cModel : public cEntity
    {
    public:
        cModel(const char* FilePath, std::string Name, void* PhysicsWorld, btAlignedObjectArray<btCollisionShape*>* CollisionShapes, Frustum* CamFrustum, void* Graphics_API);
            
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

            mAnimationsMap.clear();

        }

        void SetModelTransform(sTransform& Trans);
        virtual void Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions, uint32_t* SkyBox, sGBufferObjects* GeometryObject) override;
        virtual void ShadowRender(sTransform& Trans, glm::mat4& LightSpaceMatrix, uint32_t DepthMap) override;
        virtual void Update(/*Should be taking in the timestep*/) override;

        virtual void AddAnimation(const char*) override;
        virtual void ChangeAnimation(const char*) override;
        virtual bool isModelInFrustum() override;

        

        std::map<std::string, sBoneInfo>& GetBoneInfoMap() { return m_BoneInfoMap; }
        int& GetBoneCount() { return m_BoneCounter; }
    
    private:
        cGraphicsAPI* mGfxAPI = nullptr;
    private:
        Frustum* mCamFrustum = nullptr; // Add Another one for the shadow camera

    private:
        void loadModel(std::string path);
        void CalculateAABB(const aiScene* scene);
        virtual void SetAABB(glm::vec3& Scale) override;
        btCollisionShape* CalculateMeshCollision(const aiScene* scene);

        void processNode(aiNode* node, const aiScene* scene);
        cMesh processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<sTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);


        void ExtractBoneWeightForVertices(std::vector<sVertex>& vertices, aiMesh* mesh, const aiScene* scene);
        void SetVertexBoneDataToDefault(sVertex& vertex);
        void SetVertexBoneData(sVertex& vertex, int boneID, float weight);


        // model data 
        std::vector<sTexture> mLoadedTextures;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        std::vector<cMesh>    mMeshes;
        std::map<std::string, sBoneInfo> m_BoneInfoMap; 
        std::string mDirectory;
        int m_BoneCounter = 0;
        
        bool gammaCorrection;
        cShader mGeometryShader;
        cShader mLightShader;
        cShader mShadowShader;

        Assimp::Importer importer;
        const aiScene* scene;
        std::unordered_map<std::string, std::shared_ptr<Animation>> mAnimationsMap;

        std::shared_ptr<Animator> mAnimator;
        std::shared_ptr<Animation> mCurrentAnimation;

        float mDeltaTime = 0, mLastFrame = 0;
    };
}