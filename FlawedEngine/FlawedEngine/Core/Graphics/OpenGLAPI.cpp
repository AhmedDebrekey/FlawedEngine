#include "OpenGLAPI.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace FlawedEngine
{
    cOpenGLAPI::cOpenGLAPI()
    {
    }
    cOpenGLAPI::~cOpenGLAPI()
    {
    }
    void cOpenGLAPI::Initialize()
    {
    }
    void cOpenGLAPI::Shutdown()
    {
    }

    unsigned int cOpenGLAPI::CreateFramebuffer()
    {
        return 0;
    }

    void cOpenGLAPI::DeleteFramebuffer(unsigned int framebuffer)
    {
    }

    void cOpenGLAPI::BindFramebuffer(unsigned int framebuffer)
    {
    }

    unsigned int cOpenGLAPI::CreateBuffer(eBufferType type, const void* data, size_t size)
    {
        GLuint bufferId;
        GLenum target = GetBufferType(type);

        glGenBuffers(1, &bufferId);
        glBindBuffer(target, bufferId);
        glBufferData(target, size, data, GL_STATIC_DRAW);

        return bufferId;
    }

    void cOpenGLAPI::DeleteBuffer(unsigned int buffer)
    {
        glDeleteBuffers(1, &buffer);
    }

    void cOpenGLAPI::BindBuffer(eBufferType type, unsigned int buffer)
    {
        GLenum target = GetBufferType(type);
        glBindBuffer(target, buffer);
    }

    void cOpenGLAPI::BindBufferBase(eBufferType type, unsigned int index, unsigned int buffer)
    {
        GLenum target = GetBufferType(type);
        glBindBufferBase(target, index, buffer);
    }

    void cOpenGLAPI::BindBufferSubData(eBufferType type, int offset, size_t size, void* data)
    {
        GLenum target = GetBufferType(type);
        glBufferSubData(target, offset, size, data);
    }

    unsigned int cOpenGLAPI::CreateVertexArray()
    {
        GLuint vertexArrayID;
        glGenVertexArrays(1, &vertexArrayID);
        return vertexArrayID;
    }

    void cOpenGLAPI::BindVertexArray(unsigned int VAO)
    {
        glBindVertexArray(VAO);
    }

    void cOpenGLAPI::Present()
    {
    }

    unsigned int cOpenGLAPI::GetBufferType(eBufferType type)
    {
        GLenum target;
        switch (type) {
        case eBufferType::Vertex:
            target = GL_ARRAY_BUFFER;
            break;
        case eBufferType::Index:
            target = GL_ELEMENT_ARRAY_BUFFER;
            break;
        case eBufferType::Uniform:
            target = GL_UNIFORM_BUFFER;
            break;
        default:
            assert(false && "Unsupported buffer type");
        }
        return target;
    }

    unsigned int cOpenGLAPI::CreateTexture(int width, int height, bool isDepth, unsigned char* data, int nrComponents)
    {
        GLuint textureID;
        GLenum target = isDepth ? GL_TEXTURE_DEPTH : GL_TEXTURE_2D; //Not Sure If Necessary
        
        glGenTextures(1, &textureID);
    
        glBindTexture(target, textureID);
        if (isDepth)
        {
            glTexImage2D(target, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
            glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, borderColor);
        }
        else
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;
            glTexImage2D(target, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(target);
            glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        glBindTexture(target, 0);
        return textureID;
    }

    void cOpenGLAPI::DeleteTexture(unsigned int texture)
    {
        glDeleteTextures(1, &texture);
    }

    void cOpenGLAPI::BindTexture(unsigned int texture, eTextureType type)
    {
        GLenum textureType = GetTextureType(type);
        glBindTexture(textureType, texture);
    }

    void cOpenGLAPI::ActiveTexture(unsigned int unit)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
    }

    unsigned int cOpenGLAPI::GetTextureType(eTextureType type)
    {
        GLenum target;
        switch (type) {
        case eTextureType::Texture2D:
            target = GL_TEXTURE_2D;
            break;
        case eTextureType::CubeMap:
            target = GL_TEXTURE_CUBE_MAP;
            break;
        default:
            assert(false && "Unsupported buffer type");
        }
        return target;
    }

    void cOpenGLAPI::SetViewport(int x, int y, int width, int height)
    {
        glViewport(x, y, width, height);
    }

    void cOpenGLAPI::ClearColorBuffer(const sColor& color)
    {
        glClearColor(color.Red, color.Green, color.Blue, color.Alpha);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void cOpenGLAPI::ClearDepthBuffer()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void cOpenGLAPI::DrawElements(size_t count)
    {
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
    }

    void cOpenGLAPI::SetDepthTest(bool enabled)
    {
    }
    void cOpenGLAPI::SetBlendMode(eBlendMode mode)
    {
    }
    void cOpenGLAPI::EnableFaceCulling(bool enabled)
    {
    }
}