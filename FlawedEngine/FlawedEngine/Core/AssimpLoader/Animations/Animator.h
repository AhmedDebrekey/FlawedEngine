#pragma once

#include "Animation.h"


namespace FlawedEngine
{
    //Animator.....................................................................................................
    class Animator
    {
    public:
        Animator() {}
        
        ~Animator()
        {
        }

        void Setup(Animation* animation)
        {
            m_CurrentTime = 0.0f;
            m_CurrentAnimation = animation;


            m_FinalBoneMatrices.reserve(100);

            for (int i = 0; i < 100; i++)
                m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
        }

        void ChangeAnim(std::shared_ptr<Animation> animation)
        {
            
            m_BlendingAnimation = animation;
        }

        void UpdateAnimation(float dt)
        {
            m_DeltaTime = dt;
            if (m_CurrentAnimation)
            {
                m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
                m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
               
                if (m_BlendingAnimation)
                    UpdateBlendingFactor(m_DeltaTime);

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
            glm::mat4 BaseNodeTrans = node->transformation;

            std::shared_ptr<Bone> BaseBone = m_CurrentAnimation->FindBone(nodeName);


            if (BaseBone)
            {
                BaseBone->Update(m_CurrentTime);
                BaseNodeTrans = BaseBone->GetLocalTransform();
            }

            
            if (m_BlendingAnimation)
            {
                std::shared_ptr<Bone> LayeredBone = m_BlendingAnimation->FindBone(nodeName);
                glm::mat4 LayeredNodeTrans = node->transformation;

                if (LayeredBone)
                {
                    LayeredBone->Update(m_BlendingCurrentTime);
                    LayeredNodeTrans = LayeredBone->GetLocalTransform();
                }


                if (m_BlendingFactor == 1.0f)
                {
                    m_CurrentAnimation = m_BlendingAnimation.get();
                    m_CurrentTime = m_BlendingCurrentTime;

                    m_BlendingFactor = 0.0f;
                    m_ElapsedBlendingTime = 0.0f;
                    m_BlendingCurrentTime = 0.0f;

                    m_BlendingAnimation.reset();
                }
                if (m_BlendingFactor >= 0.5f && m_BlendingFactor <= .75f) // Debug purposes
                {
                    std::cout << "";
                }

                const glm::quat rot0 = glm::quat_cast(BaseNodeTrans);
                const glm::quat rot1 = glm::quat_cast(LayeredNodeTrans);
                const glm::quat finalRot = glm::slerp(rot0, rot1, m_BlendingFactor);

                const glm::vec3 scale0 = glm::vec3(glm::length(BaseNodeTrans[0]), glm::length(BaseNodeTrans[1]), glm::length(BaseNodeTrans[2]));
                const glm::vec3 scale1 = glm::vec3(glm::length(LayeredNodeTrans[0]), glm::length(LayeredNodeTrans[1]), glm::length(LayeredNodeTrans[2]));
                const glm::vec3 finalScale = glm::mix(scale0, scale1, m_BlendingFactor);

                glm::mat4 blendedMat = glm::mat4_cast(finalRot);
                blendedMat[0] *= finalScale.x;
                blendedMat[1] *= finalScale.y;
                blendedMat[2] *= finalScale.z;

                blendedMat[3] = (1.0f - m_BlendingFactor) * BaseNodeTrans[3] + LayeredNodeTrans[3] * m_BlendingFactor;

                BaseNodeTrans = blendedMat;
            }
            
            glm::mat4 globalTransformation = parentTransform * BaseNodeTrans;

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

        

        void UpdateBlendingFactor(float deltaTime)
        {
            m_ElapsedBlendingTime += deltaTime;

            m_BlendingFactor = m_ElapsedBlendingTime / m_BlendingDuration;

            m_BlendingFactor = glm::smoothstep(0.0f, 1.0f, m_BlendingFactor);

            m_BlendingFactor = glm::clamp(m_BlendingFactor, 0.0f, 1.0f);

            float a = 1.0f;
            float b = m_CurrentAnimation->GetDuration() / m_BlendingAnimation->GetDuration();
            const float animSpeedMultiplierUp = (1.0f - m_BlendingFactor) * a + b * m_BlendingFactor; // Lerp

            a = m_BlendingAnimation->GetDuration() / m_CurrentAnimation->GetDuration();
            b = 1.0f;
            const float animSpeedMultiplierDown = (1.0f - m_BlendingFactor) * a + b * m_BlendingFactor; // Lerp

            // Current time of each animation, "scaled" by the above speed multiplier variables
            m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * deltaTime * animSpeedMultiplierUp;
            m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());

            m_BlendingCurrentTime += m_BlendingAnimation->GetTicksPerSecond() * deltaTime * animSpeedMultiplierDown;
            m_BlendingCurrentTime = fmod(m_BlendingCurrentTime, m_BlendingAnimation->GetDuration());

        }

    private:
        std::vector<glm::mat4> m_FinalBoneMatrices;

        Animation* m_CurrentAnimation = nullptr;
        std::shared_ptr<Animation> m_BlendingAnimation = nullptr;

        bool m_Blending;

        float m_CurrentTime;
        float m_BlendingTime;

        float m_DeltaTime;

        float m_BlendingFactor;

        float m_BlendingDuration = 0.5f; 
        float m_ElapsedBlendingTime = 0.0f; 
        float m_BlendingCurrentTime = 0.0f;
    };
}