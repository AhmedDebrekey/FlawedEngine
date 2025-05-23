#pragma once

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/quaternion.hpp>

#include <string>
#include <vector>
#include <functional>

#include "Logger.h"

namespace FlawedEngine
{
	typedef unsigned int uint32_t;
#define MAX_BONE_INFLUENCE 4

	// Macro for simplified logging
#define EngineLog(message, level) Logger::Log(message, LogLevel::level)
#define EngineLogMatrix(matrixPtr, level)                                      \
    do {                                                                       \
        for (int row = 0; row < 4; ++row) {                                    \
            std::string line = "[ ";                                           \
            for (int col = 0; col < 4; ++col) {                                \
                int index = col * 4 + row; /* column-major access */           \
                line += std::to_string(matrixPtr[index]) + (col < 3 ? ", " : " "); \
            }                                                                  \
            line += "]";                                                       \
            EngineLog("mTmpMatrix Row " + std::to_string(row) + ": " + line, level); \
        }                                                                      \
    } while (0)
#define EngineLogVec3(vec, level)                                              \
    do {                                                                       \
        std::string line = "[ " +                                              \
            std::to_string((vec).x) + ", " +                                   \
            std::to_string((vec).y) + ", " +                                   \
            std::to_string((vec).z) + " ]";                                    \
        EngineLog("Vec3: " + line, level);                                     \
    } while (0)
#define EngineLogVec4(vec, level)                                              \
    do {                                                                       \
        std::string line = "[ " +                                              \
            std::to_string((vec).x) + ", " +                                   \
            std::to_string((vec).y) + ", " +                                   \
            std::to_string((vec).z) + ", " +                                   \
            std::to_string((vec).w) + " ]";                                    \
        EngineLog("Vec4: " + line, level);                                     \
    } while (0)

	extern bool mIsPlaying;

	struct sMaterial
	{
		glm::vec3 Color = glm::vec3(1.0f);
		glm::vec3 Diffuse = glm::vec3(0.5f);
		glm::vec3 Specular = glm::vec3(0.5f);
		float Shininess = 32.0f;
		float Roughness;
		float Reflectivity = 1.0f;
		uint32_t TextureID;
	};

	struct sLight
	{
		glm::vec3 position;

		float constant;
		float linear;
		float quadratic;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	struct sModel
	{
		glm::vec3 Translation = glm::vec3(0.0f);
		glm::vec3 Rotation = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);
	};

	struct sPhysicsProps
	{
		float Mass = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		sModel Transformation;
	};

	struct sTransform
	{
		glm::vec3 Position;
		glm::vec3 Front;
		glm::mat4 Projection = glm::mat4(1.0f);
		glm::mat4 View = glm::mat4(1.0f);
		glm::mat4 Model = glm::mat4(1.0f);
	};

	struct sVertex
	{
		glm::vec3 Postion;
		glm::vec3 Normal;
		glm::vec2 TexCoords;

		glm::vec3 Tangent;
		glm::vec3 BiTangent;

		int mBoneIDs[MAX_BONE_INFLUENCE];
		float mWeights[MAX_BONE_INFLUENCE];
	};

	struct sBoneInfo
	{
		/*id is index in finalBoneMatrices*/
		int id;

		/*offset matrix transforms vertex from model space to bone space*/
		glm::mat4 offset;

	};

	struct sTexture
	{
		uint32_t ID;
		std::string Type;
		std::string Path;
	};

	enum eBasicObject
	{
		Cube = 0,
		Sphere,
		Cone,
		Torus,
		Triangle,
		PointLight,
		SpotLight,
		Custom
	};

	struct Plane
	{
		glm::vec3 normal = { 0.f, 1.f, 0.f };	// unit vector
		float distance = 0.f;					// Distance with origin

		Plane() = default;

		Plane(const glm::vec3& p1, const glm::vec3& norm)
			: normal(glm::normalize(norm)),
			distance(glm::dot(normal, p1))
		{}

		float getSignedDistanceToPlane(const glm::vec3& point) const
		{
			return glm::dot(normal, point) - distance;
		}
	};

	struct Frustum
	{
		Plane topFace;
		Plane bottomFace;

		Plane rightFace;
		Plane leftFace;

		Plane farFace;
		Plane nearFace;
	};


	struct sAABB
	{
		glm::vec3 mCenter{ 0.f, 0.f, 0.f };
		glm::vec3 mExtents{ 0.f, 0.f, 0.f };

		sAABB()
		{}

		sAABB(const glm::vec3& min, const glm::vec3& max)
			: mCenter{ (max + min) * 0.5f }, mExtents{ max.x - mCenter.x, max.y - mCenter.y, max.z - mCenter.z }
		{}

		sAABB(const glm::vec3& inCenter, float iI, float iJ, float iK)
			: mCenter{ inCenter }, mExtents{ iI, iJ, iK }
		{}

		//https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
		bool isOnOrForwardPlane(const Plane& plane) const
		{
			// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
			const float r = mExtents.x * std::abs(plane.normal.x) + mExtents.y * std::abs(plane.normal.y) +
				mExtents.z * std::abs(plane.normal.z);

			return -r <= plane.getSignedDistanceToPlane(mCenter);
		}
	};

	struct sFrameBuffer
	{
		sFrameBuffer()
		{}
		sFrameBuffer(uint32_t fbo, glm::vec2 Size, glm::vec2 Pos, glm::vec2 PrevSize)
			:FBO(fbo), ViewportSize(Size), ViewportPos(Pos), PrevViewportSize(PrevSize)
		{}

		uint32_t FBO;
		glm::vec2 ViewportSize;
		glm::vec2 ViewportPos;
		glm::vec2 PrevViewportSize;
	};

	struct sColor
	{
		float Red;
		float Green;
		float Blue;
		float Alpha;
	};

	enum eBufferType
	{
		Vertex,
		Index,
		Uniform,
		Frame,
		Depth,
		Stencil
	};

	enum eDrawType
	{
		Static,
		Dynmaic
	};

	enum eVertexType
	{
		Float,
		Int,
		UInt
	};

	enum eBlendMode
	{
		Opaque,
		Additive,
		Subtractive,
		Multiply,
		AlphaBlend
	};

	enum eTextureType
	{
		Texture2D,
		CubeMap
	};

	enum eTextureProperties
	{
		Linear,
		Repeat,
		MIPMAP_Linear,
		None
	};

	struct sTextureProps
	{
		sTextureProps()
		{}
		sTextureProps
		(
			eTextureProperties _Wrap_s,
			eTextureProperties _Wrap_t,
			eTextureProperties _Min_Filter,
			eTextureProperties _Mag_Filter
		)
			:Wrap_s(_Wrap_s),
			Wrap_t(_Wrap_t),
			Min_Filter(_Min_Filter),
			Mag_Filter(_Mag_Filter)
		{}
		eTextureProperties Wrap_s = None;
		eTextureProperties Wrap_t = None;
		eTextureProperties Min_Filter = None;
		eTextureProperties Mag_Filter = None;
	};

	enum eGLFuncType
	{
		Always,
		Never,
		Less,
		Equal,
		LessEqual,
		Greater,
		NotEqual,
		GreateOrEqual
	};

	struct sGBufferObjects
	{
		uint32_t GBuffer;
		uint32_t Position;
		uint32_t Normal;
		uint32_t AlbedoSpec;
		uint32_t Depth;
		uint32_t RenderBuffer;
	};


	//ObjectManager
	void* GetEntity(const char* name);
	void* SpawnEntity(const char* name, eBasicObject type);
	void* LoadEntity(const char* path, const char* name);
	void RemoveEntity(const char* name);
	std::function<bool(int)>& GetInputFunc();

	//Perspective Camera
	void MoveCamera(float dx, float dy, float dz);
	void SetCameraPos(float x, float y, float z);
	void RotateCamera(float pitch, float yaw, float roll);
	void SetCameraRotation(float pitch, float yaw, float roll);
	float GetCamYaw();
	float GetCamPitch();
	void StartPlayMode();
	void StopPlayMode();

	//Physics
	void* GetPhxsWorld();

	void SetDirectionalLightPos(const glm::vec3& Position);
	glm::vec3& GetDirectionalLightPos();
	using DragDropCallback = std::function<void()>;
	using PanelRefreshCallback = std::function<void()>;
}