#pragma once

#include "Animation.h"


namespace FlawedEngine
{
    //Animator.....................................................................................................
    class Animator
    {
    public:
        Animator() {}

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
            const std::string& nodeName = node->name;
            glm::mat4 nodeTransform = node->transformation;

            std::shared_ptr<Bone> Bone = m_CurrentAnimation->FindBone(nodeName);

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
                //display_4x4("Final: ", m_FinalBoneMatrices[index]);
            }

            for (int i = 0; i < node->childrenCount; i++)
                CalculateBoneTransform(&node->children[i], globalTransformation);
        }

        std::vector<glm::mat4> GetFinalBoneMatrices()
        {
            return m_FinalBoneMatrices;
        }

        void display_4x4(std::string tag, glm::mat4& m4)
        {
            std::cout << tag << '\n';
            for (int col = 0; col < 4; ++col) {
                std::cout << "| ";
                for (int row = 0; row < 4; ++row) {
                    std::cout << m4[row][col] << '\t';
                }
                std::cout << '\n';
            }
            std::cout << '\n';
        }

    private:
        std::vector<glm::mat4> m_FinalBoneMatrices;
        Animation* m_CurrentAnimation;

        float m_CurrentTime;
        float m_DeltaTime;

    };
}