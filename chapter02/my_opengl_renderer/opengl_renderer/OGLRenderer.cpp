#include "OGLRenderer.h"

#include "Logger.h"

OGLRenderer::OGLRenderer()
{
}

bool OGLRenderer::init(unsigned width, unsigned height)
{
    /* initalize GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Logger::log(1, "%s error: failed to initialize GLAD\n", __FUNCTION__);
        return false;
    }

    if (!GLAD_GL_VERSION_4_6) {
        Logger::log(1, "%s error: failed to get at least OpenGL 4.6\n", __FUNCTION__);
        return false;
    }
    
    if (!mFramebuffer.init(width, height))
    {
        Logger::log(1, "%s error: could not init Framebuffer\n", __FUNCTION__);
        return false;
    }
    Logger::log(1, "%s: framebuffer succesfully initialized\n", __FUNCTION__);

    if (!mTex.loadTexture("textures/crate.png"))
    {
        Logger::log(1, "%s: texture loading failed\n", __FUNCTION__);
        return false;
    }
    Logger::log(1, "%s: texture successfully loaded\n", __FUNCTION__);

    mVertexBuffer.init();
    Logger::log(1, "%s: vertex buffer successfully created\n", __FUNCTION__);

    if (!mBasicShader.loadShaders("shader/basic.vert", "shader/basic.frag"))
    {
        Logger::log(1, "%s: shader loading failed\n", __FUNCTION__);
        return false;
    }
    
    Logger::log(1, "%s: shaders succesfully loaded\n", __FUNCTION__);

    return true;
}

void OGLRenderer::setSize(unsigned width, unsigned height)
{
    mFramebuffer.resize(width, height);
    glViewport(0, 0, width, height);
}

void OGLRenderer::cleanup()
{
    mBasicShader.cleanup();
    mTex.cleanup();
    mVertexBuffer.cleanup();
    mFramebuffer.cleanup();
}

void OGLRenderer::uploadData(OGLMesh vertexData)
{
    mTriangleCount = vertexData.vertices.size();
    mVertexBuffer.uploadData(vertexData);
}

void OGLRenderer::draw()
{
    mFramebuffer.bind();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);

    mBasicShader.use();
    mTex.bind();
    mVertexBuffer.bind();
    mVertexBuffer.draw(GL_TRIANGLES, 0, mTriangleCount);
    mVertexBuffer.unbind();
    mTex.unbind();
    mFramebuffer.unbind();

    mFramebuffer.drawToScreen();
}
