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

    class cModel : public cEntity
    {
    public:
        cModel(const char* FilePath, std::string Name, void* PhysicsWorld, btAlignedObjectArray<btCollisionShape*>* CollisionShapes);
            
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

        void LRotate(float x, float y, float z);

        void LScale(float x, float y, float z);

        void LApplyForce(float x, float y, float z);

        void LApplyRelativeForce(float x, float y, float z);

        std::map<std::string, sBoneInfo>& GetBoneInfoMap() { return m_BoneInfoMap; }
        int& GetBoneCount() { return m_BoneCounter; }

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
        std::map<std::string, sBoneInfo> m_BoneInfoMap; 
        int m_BoneCounter = 0;
        void SetVertexBoneDataToDefault(sVertex& vertex);
        void SetVertexBoneData(sVertex& vertex, int boneID, float weight);
        void ExtractBoneWeightForVertices(std::vector<sVertex>& vertices, aiMesh* mesh, const aiScene* scene);
        Animation* mAnimation = nullptr;
        Animator* mAnimator = nullptr;
        float mDeltaTime = 0, mLastFrame = 0;
    };

    //Animation............................................................................................
    class Animation
    {
    public:
        Animation(){}

        ~Animation()
        {
        }

        void Setup(const std::string& animationPath, cModel* model, Assimp::Importer* importer)
        {
            const aiScene* scene = importer->ReadFile(animationPath, aiProcess_Triangulate);
            assert(scene && scene->mRootNode);
            auto animation = scene->mAnimations[0];
            m_Duration = animation->mDuration;
            m_TicksPerSecond = animation->mTicksPerSecond;
            aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
            globalTransformation = globalTransformation.Inverse();
            ReadHierarchyData(m_RootNode, scene->mRootNode);
            ReadMissingBones(animation, *model);
        }

        Bone* FindBone(const std::string& name)
        {
            auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
                [&](const Bone& Bone)
                {
                    return Bone.GetBoneName() == name;
                }
            );
            if (iter == m_Bones.end()) return nullptr;
            else return &(*iter);
        }


        inline float GetTicksPerSecond() { return m_TicksPerSecond; }
        inline float GetDuration() { return m_Duration; }
        inline const AssimpNodeData& GetRootNode() { return m_RootNode; }
        inline const std::map<std::string, sBoneInfo>& GetBoneIDMap()
        {
            return m_BoneInfoMap;
        }

    private:
        void ReadMissingBones(const aiAnimation* animation, cModel& model)
        {
            int size = animation->mNumChannels;

            std::map<std::string, sBoneInfo>& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
            int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

            //reading channels(bones engaged in an animation and their keyframes)
            for (int i = 0; i < size; i++)
            {
                auto channel = animation->mChannels[i];
                std::string boneName = channel->mNodeName.data;

                if (boneInfoMap.find(boneName) == boneInfoMap.end())
                {
                    boneInfoMap[boneName].id = boneCount;
                    boneCount++;
                }
                m_Bones.push_back(Bone(channel->mNodeName.data,
                    boneInfoMap[channel->mNodeName.data].id, channel));
                //mBones[i] = Bone(channel->mNodeName.data,
                   // boneInfoMap[channel->mNodeName.data].id, channel);
            }

            m_BoneInfoMap = boneInfoMap;
        }

        void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
        {
            assert(src);

            dest.name = src->mName.data;
            dest.transformation = ConvertMatrixToGLMFormat(src->mTransformation);
            dest.childrenCount = src->mNumChildren;

            for (int i = 0; i < src->mNumChildren; i++)
            {
                AssimpNodeData newData;
                ReadHierarchyData(newData, src->mChildren[i]);
                dest.children.push_back(newData);
            }
        }
        float m_Duration;
        int m_TicksPerSecond;
        std::vector<Bone> m_Bones;
        //std::map<std::string, Bone> mBones;
        AssimpNodeData m_RootNode;
        std::map<std::string, sBoneInfo> m_BoneInfoMap;
    };

    //Animator.....................................................................................................
    class Animator
    {
    public:
        Animator(){}

        void Setup(Animation* animation)
        {
            m_CurrentTime = 0.0;
            m_CurrentAnimation = animation;


            m_FinalBoneMatrices.reserve(100);

            for (int i = 0; i < 100; i++)
                m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
        }

        void UpdateAnimation(float dt)
        {
            m_DeltaTime = dt;
            if (m_CurrentAnimation)
            {
                m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
                m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
                CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
            }
        }

        void PlayAnimation(Animation* pAnimation)
        {
            m_CurrentAnimation = pAnimation;
            m_CurrentTime = 0.0f;
        }

        void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
        {
            std::string nodeName = node->name;
            glm::mat4 nodeTransform = node->transformation;

            Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

            if (Bone)
            {
                Bone->Update(m_CurrentTime);
                nodeTransform = Bone->GetLocalTransform();
            }

            glm::mat4 globalTransformation = parentTransform * nodeTransform;

            const auto& boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
            if (boneInfoMap.find(nodeName) != boneInfoMap.end())
            {
                int index = boneInfoMap.at(nodeName).id;
                m_FinalBoneMatrices[index] = globalTransformation * boneInfoMap.at(nodeName).offset;
            }

            for (int i = 0; i < node->childrenCount; i++)
                CalculateBoneTransform(&node->children[i], globalTransformation);
        }

        std::vector<glm::mat4> GetFinalBoneMatrices()
        {
            return m_FinalBoneMatrices;
        }

    private:
        std::vector<glm::mat4> m_FinalBoneMatrices;
        Animation* m_CurrentAnimation;
       
        float m_CurrentTime;
        float m_DeltaTime;

    };
}

