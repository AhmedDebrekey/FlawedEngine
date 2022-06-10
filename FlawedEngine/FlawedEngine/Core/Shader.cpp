#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

namespace FlawedEngine
{
    cShader::cShader()
        :m_ShaderObject(NULL)
    {

    }

    cShader::~cShader()
    {
        glDeleteProgram(m_ShaderObject);
    }

    //Creating the shaders
    void cShader::Create(const char* vertexPath, const char* fragmentPath)
    {
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "[ERROR] {ShaderFile} Could not be read successfully" << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        uint32_t vertexShaderObject, fragmentShaderObject;
        int vertexSuccess, fragmentSuccess, programSuccess;
        char vertexInfoLog[512], fragmentInfoLog[512], programInfoLog[512];

        vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShaderObject, 1, &vShaderCode, NULL);
        glCompileShader(vertexShaderObject);

        glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &vertexSuccess);
        if (!vertexSuccess)
        {
            glGetShaderInfoLog(vertexShaderObject, 512, NULL, vertexInfoLog);
            std::cout << "[ERROR] {VertexShader}: " << vertexInfoLog << std::endl;
        }

        fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShaderObject, 1, &fShaderCode, NULL);
        glCompileShader(fragmentShaderObject);

        glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &fragmentSuccess);
        if (!fragmentSuccess)
        {
            glGetShaderInfoLog(fragmentShaderObject, 512, NULL, fragmentInfoLog);
            std::cout << "[ERROR] {FragmentShader}: " << fragmentInfoLog << std::endl;
        }

        m_ShaderObject = glCreateProgram();
        glAttachShader(m_ShaderObject, vertexShaderObject);
        glAttachShader(m_ShaderObject, fragmentShaderObject);
        glLinkProgram(m_ShaderObject);

        glGetProgramiv(m_ShaderObject, GL_LINK_STATUS, &programSuccess);
        if (!programSuccess)
        {
            glGetProgramInfoLog(m_ShaderObject, 512, NULL, programInfoLog);
            std::cout << "[ERROR] {ShaderProgram}: " << programInfoLog << std::endl;
        }

        glDeleteShader(vertexShaderObject);
        glDeleteShader(fragmentShaderObject);
    }

    //using shader
    void cShader::Bind()
    {
        glUseProgram(m_ShaderObject);
    }

    //unbinding it
    void cShader::Unbind()
    {
        glUseProgram(0);
    }


    //UNIFORMS  .... BOOL INT FLOAT MAT4F VEC3

    void cShader::SetBool(const std::string& name, bool value)
    {
        glUniform1i(glGetUniformLocation(m_ShaderObject, name.c_str()), (int)value);
    }

    void cShader::SetInt(const std::string& name, int value)
    {
        glUniform1i(glGetUniformLocation(m_ShaderObject, name.c_str()), value);
    }

    void cShader::SetFloat(const std::string& name, float value)
    {
        glUniform1f(glGetUniformLocation(m_ShaderObject, name.c_str()), value);
    }

    void cShader::SetMat4f(const std::string& name, glm::mat4 matrix)
    {
        glUniformMatrix4fv(glGetUniformLocation(m_ShaderObject, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
    }

    void cShader::SetVec3(const std::string& name, glm::vec3 vector)
    {
        glUniform3f(glGetUniformLocation(m_ShaderObject, name.c_str()), vector.x, vector.y, vector.z);
    }
}