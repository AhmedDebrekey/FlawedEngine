#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

namespace FlawedEngine
{
	class cShader
	{
	public:
		cShader();
		~cShader();

		void Create(const char* vertexPath, const char* fragmentPath);

		uint32_t GetId() { return m_ShaderObject; }

		void Bind();
		void Unbind();

		void SetBool(const std::string& name, bool value);
		void SetInt(const std::string& name, int value);
		void SetFloat(const std::string& name, float value);
		void SetMat4f(const std::string& name, glm::mat4 matrix);
		void SetVec3(const std::string& name, glm::vec3 vector);
	private:
		uint32_t m_ShaderObject;
	};
}