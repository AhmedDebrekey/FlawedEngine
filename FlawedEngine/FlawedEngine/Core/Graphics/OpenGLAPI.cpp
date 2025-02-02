#include "OpenGLAPI.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../Models/stb_image.h"


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

    unsigned int cOpenGLAPI::CreateFramebuffer(unsigned int TextureBuffer)
    {
        GLuint frameBufferId;

        glGenFramebuffers(1, &frameBufferId);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            assert("FrameBuffer Is Not Complete");

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureBuffer, 0);
        return frameBufferId;
    }

    void cOpenGLAPI::DeleteFramebuffer(unsigned int framebuffer)
    {
        glDeleteFramebuffers(1, &framebuffer);
    }

    void cOpenGLAPI::BindFramebuffer(unsigned int framebuffer)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    }

    void cOpenGLAPI::AttachRenderBufferToFrameBuffer(unsigned int renderbuffer)
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
    }

    // This function bind and unbinds the renderbuffer, Do not forget to bind it again if needed
    unsigned int cOpenGLAPI::CreateRenderBuffer(int width, int height)
    {
        GLuint renderBufferId;
        glGenRenderbuffers(1, &renderBufferId);
        BindRenderBuffer(renderBufferId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        BindRenderBuffer(0);
        return renderBufferId;
    }

    void cOpenGLAPI::BindRenderBuffer(unsigned int renderbuffer)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    }

    void cOpenGLAPI::DeleteRenderBuffer(unsigned int renderBuffer)
    {
        glDeleteRenderbuffers(1, &renderBuffer);
    }

    unsigned int cOpenGLAPI::CreateBuffer(eBufferType type, const void* data, size_t size, eDrawType draw)
    {
        GLuint bufferId;
        GLenum target = GetBufferType(type);
        GLenum usage = GetDrawType(draw);

        glGenBuffers(1, &bufferId);
        glBindBuffer(target, bufferId);
        glBufferData(target, size, data, usage);

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

    void cOpenGLAPI::SetDepthFunc(eGLFuncType func)
    {
        GLenum functype = GetFuncType(func);
        glDepthFunc(functype);
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

    void cOpenGLAPI::VertexAttribProps(unsigned int index, size_t size, eVertexType type, bool normalized, size_t stride, const void* data)
    {
        glEnableVertexAttribArray(index);
        if (type == eVertexType::Int)
        {
            glVertexAttribIPointer(index, size, GL_INT, stride, (void*)data);
            return;
        }

        GLenum vertextype = GetVertexType(type);
        glVertexAttribPointer(index, size, vertextype, normalized, stride, (void*)data);
    }

    sGBufferObjects cOpenGLAPI::CreateGeometryBuffer(unsigned int width, unsigned int height)
    {
        sGBufferObjects GBO;
        glGenFramebuffers(1, &GBO.GBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, GBO.GBuffer);

        // - position color buffer
        glGenTextures(1, &GBO.Position);
        glBindTexture(GL_TEXTURE_2D, GBO.Position);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GBO.Position, 0);

        // - normal color buffer
        glGenTextures(1, &GBO.Normal);
        glBindTexture(GL_TEXTURE_2D, GBO.Normal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, GBO.Normal, 0);

        // - color + specular color buffer
        glGenTextures(1, &GBO.AlbedoSpec);
        glBindTexture(GL_TEXTURE_2D, GBO.AlbedoSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, GBO.AlbedoSpec, 0);

        // ------------------- Depth Texture (GL_DEPTH_COMPONENT32F) -------------------
        glGenTextures(1, &GBO.Depth);
        glBindTexture(GL_TEXTURE_2D, GBO.Depth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, GBO.Depth, 0);

        // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
        GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);

        // create and attach depth buffer (renderbuffer)
        /*glGenRenderbuffers(1, &GBO.RenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, GBO.RenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, GBO.RenderBuffer);*/
        // finally check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            assert("FrameBuffer Is Not Complete");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return GBO;
    }

    void cOpenGLAPI::DeleteGeometryBuffer(sGBufferObjects& gBuffer)
    {
        // Delete the G-buffer's framebuffer
        if (gBuffer.GBuffer) {
            glDeleteFramebuffers(1, &gBuffer.GBuffer);
            gBuffer.GBuffer = 0;
        }

        // Delete the position texture
        if (gBuffer.Position) {
            glDeleteTextures(1, &gBuffer.Position);
            gBuffer.Position = 0;
        }

        // Delete the normal texture
        if (gBuffer.Normal) {
            glDeleteTextures(1, &gBuffer.Normal);
            gBuffer.Normal = 0;
        }

        // Delete the albedo/specular texture
        if (gBuffer.AlbedoSpec) {
            glDeleteTextures(1, &gBuffer.AlbedoSpec);
            gBuffer.AlbedoSpec = 0;
        }

        if (gBuffer.Depth) {
            glDeleteTextures(1, &gBuffer.Depth);
            gBuffer.Depth = 0;
        }

        // Delete the depth renderbuffer
        if (gBuffer.RenderBuffer) {
            glDeleteRenderbuffers(1, &gBuffer.RenderBuffer);
            gBuffer.RenderBuffer = 0;
        }
    }

    unsigned int cOpenGLAPI::CreateTexture(int width, int height, int nrComponents, unsigned char* data, sTextureProps props)
    {
        GLuint textureID;
        GLenum target = GL_TEXTURE_2D; //Not Sure If Necessary
        
        glGenTextures(1, &textureID);
    
        glBindTexture(target, textureID);

        GLenum format = 0;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glTexImage2D(target, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(target);

        GLint Wrap_s = GetTextureProp(props.Wrap_s);
        GLint Wrap_t = GetTextureProp(props.Wrap_t);
        GLint Min_Filter = GetTextureProp(props.Min_Filter);
        GLint Mag_Filter = GetTextureProp(props.Mag_Filter);
        if (Wrap_s != 0)
            glTexParameteri(target, GL_TEXTURE_WRAP_S, Wrap_s);
        if (Wrap_t != 0)
            glTexParameteri(target, GL_TEXTURE_WRAP_T, Wrap_t);
        if (Min_Filter != 0)
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, Min_Filter);
        if (Mag_Filter != 0)
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, Mag_Filter);

        glBindTexture(target, 0);
        return textureID;
    }

    unsigned int cOpenGLAPI::CreateDepthTexture(int width, int height)
    {
        GLuint depthTextureID;
        GLenum target = GL_TEXTURE_2D;

        glGenTextures(1, &depthTextureID);
        glBindTexture(target, depthTextureID);

        glTexImage2D(target, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, borderColor);
        return depthTextureID;
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

    unsigned int cOpenGLAPI::GetTextureProp(eTextureProperties prop)
    {
        GLint param;
        switch (prop) {
        case eTextureProperties::Linear:
            param = GL_LINEAR;
            break;
        case eTextureProperties::Repeat:
            param = GL_REPEAT;
            break;
        case eTextureProperties::MIPMAP_Linear:
            param = GL_LINEAR_MIPMAP_LINEAR;
            break;
        case eTextureProperties::None:
            param = 0;
            break;
        default:
            assert(false && "Unsupported buffer type");
        }
        return param;
    }

    unsigned int cOpenGLAPI::GetDrawType(eDrawType type)
    {
        GLenum usage;
        switch (type) {
        case eDrawType::Static:
            usage = GL_STATIC_DRAW;
            break;
        case eDrawType::Dynmaic:
            usage = GL_DYNAMIC_DRAW;
            break;
        default:
            assert(false && "Unsupported draw type");
        }
        return usage;
    }

    unsigned int cOpenGLAPI::GetVertexType(eVertexType type)
    {
        GLenum usage;
        switch (type) {
        case eVertexType::Float:
            usage = GL_FLOAT;
            break;
        case eVertexType::Int:
            usage = GL_INT;
            break;
        default:
            assert(false && "Unsupported vertex type");
        }
        return usage;
    }

    unsigned int cOpenGLAPI::GetFuncType(eGLFuncType func)
    {
        GLenum functype;
        switch (func) {
        case eGLFuncType::LessEqual:
            functype = GL_LEQUAL;
            break;
        case eGLFuncType::Less:
            functype = GL_LESS;
            break;
        default:
            assert(false && "Unsupported vertex type");
        }
        return functype;
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

    unsigned int cOpenGLAPI::CreateCubeMapTexture(const std::vector<std::string>& faces)
    {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
                );
                stbi_image_free(data);
            }
            else
            {
                printf("Cubemap tex failed to load at path:");
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }

    void cOpenGLAPI::DrawElements(size_t count)
    {
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
    }

    void cOpenGLAPI::DrawArrays(size_t count)
    {
        glDrawArrays(GL_TRIANGLES, 0, count);
    }

    void cOpenGLAPI::SetDepthTest(bool enabled)
    {
        glEnable(GL_DEPTH_TEST);
    }

    void cOpenGLAPI::SetBlendMode(eBlendMode mode)
    {
    }
    void cOpenGLAPI::EnableFaceCulling(bool enabled)
    {
    }
}