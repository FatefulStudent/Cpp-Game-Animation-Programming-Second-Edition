﻿#include "Framebuffer.h"

bool Framebuffer::init(unsigned width, unsigned height)
{
    if (width <= 0 || height <= 0)
    {
        return false;
    }
    
    mBufferWidth = width;
    mBufferHeight = height;

    glGenBuffers(1, &mBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mBuffer);

    glGenTextures(1, &mColorTex);
    glBindTexture(GL_TEXTURE_2D, mColorTex);
    glTexImage2D(GL_TEXTURE_2D, 0,
        GL_RGBA, width, height,0,
        GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // unbinding texture
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        mColorTex, 0);

    // Renderbuffers
    glGenRenderbuffers(1, &mDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
        width, height);

    // We bind the created renderbuffer as a depth attachment:
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER, mDepthBuffer);

    // unbinding
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return checkComplete();
}

bool Framebuffer::resize(unsigned newWidth, unsigned newHeight)
{
    if (newWidth <= 0 || newHeight <= 0)
    {
        return false;
    }

    if (newWidth == mBufferWidth && mBufferHeight == newHeight)
    {
        return true;
    }

    mBufferWidth = newWidth;
    mBufferHeight = newHeight;

    cleanup();
    
    return init(newWidth, newHeight);
}

void Framebuffer::bind()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mBuffer);
}

void Framebuffer::unbind()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Framebuffer::drawToScreen()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, mBufferWidth, mBufferHeight, 0, 0,
        mBufferWidth, mBufferHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void Framebuffer::cleanup()
{
    unbind();
    
    glDeleteTextures(1, &mColorTex);
    glDeleteRenderbuffers(1, &mDepthBuffer);
    glDeleteFramebuffers(1, &mBuffer);
}

bool Framebuffer::checkComplete()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mBuffer);
    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE)
    {
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}
