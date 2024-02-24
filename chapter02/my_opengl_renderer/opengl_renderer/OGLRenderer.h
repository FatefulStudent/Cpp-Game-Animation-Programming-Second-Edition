#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Framebuffer.h"
#include "OGLRenderData.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexBuffer.h"


class OGLRenderer
{
public:
    OGLRenderer();

    bool init(unsigned int width, unsigned int height);
    void setSize(unsigned int width, unsigned int height);
    void cleanup();
    void uploadData(OGLMesh vertexData);
    void draw();

private:
    Shader mBasicShader{};
    Framebuffer mFramebuffer{};
    VertexBuffer mVertexBuffer{};
    Texture mTex{};
    unsigned long mTriangleCount = 0;
};
