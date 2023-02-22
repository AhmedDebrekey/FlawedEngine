#pragma once
#include "../Entity.h"
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include "../Shader.h"
namespace FlawedEngine
{
    class cModel : public cEntity
    {
    public:
        cModel(const char* FilePath, std::string Name, void* PhysicsWorld, btAlignedObjectArray<btCollisionShape*>* CollisionShapes)
            :mCollisionShapesArray(CollisionShapes), mName(Name)
        {
            mPhysicsDynamicWorld = (btDiscreteDynamicsWorld*)PhysicsWorld;
            loadModel(FilePath);
            mFilePath = FilePath;
            isCostume = true;
        }
        ~cModel()
        {
            /*for (auto Texture : Textures)
            {
                glDeleteTextures(1, &Texture.ID);
            }*/

            for (auto Mesh : mMeshes)
            {
                Mesh.DeleteTextures();
            }

            mLoadedTextures.clear();
            mLoadedTextures.shrink_to_fit();
            mMeshes.clear();
            mMeshes.shrink_to_fit();
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

        void LSetColor(float x, float y, float z);

        void LMove(float x, float y, float z);

        void LRotate(float x, float y, float z);

        void LScale(float x, float y, float z);

        void LApplyForce(float x, float y, float z);

        void LApplyRelativeForce(float x, float y, float z);

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
    };
}

