#pragma once
#include "../Model.h"
namespace FlawedEngine 
{
    //Animation............................................................................................
    class Animation
    {
    public:
        Animation() {}

        ~Animation()
        {
            m_BoneInfoMap.clear();
            mBones.clear();
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

        std::shared_ptr<Bone> FindBone(const std::string& name)
        {
            auto Bone = mBones.find(name);

            if (Bone == mBones.end())
                return nullptr;

            return Bone->second;
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
                mBones[channel->mNodeName.C_Str()] = std::make_shared<Bone>(channel->mNodeName.data,
                    boneInfoMap[channel->mNodeName.data].id, channel);
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
        //std::vector<Bone> m_Bones;
        std::map<std::string, std::shared_ptr<Bone>> mBones;
        AssimpNodeData m_RootNode;
        std::map<std::string, sBoneInfo> m_BoneInfoMap;
    };
}
