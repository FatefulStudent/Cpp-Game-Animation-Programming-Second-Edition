#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Texture.h"

#include "Logger.h"

bool Texture::loadTexture(std::string filename)
{
    mTextureName = filename;

    stbi_set_flip_vertically_on_load(true);
    unsigned char *textureData = stbi_load(filename.c_str(),
        &mTexWidth, &mTexHeight, &mNumberOfChannels, 0);

    if (!textureData)
    {
        Logger::log(1, "%s error: could not load file '%s'\n", __FUNCTION__,
            mTextureName.c_str());
        return false;
    }

    GLenum PictureFormat;

    unsigned int extensionStart = filename.rfind('.');
    if (extensionStart >= filename.size())
    {
        Logger::log(1, "%s error: could not load file '%s': file has no extension\n", __FUNCTION__,
            mTextureName.c_str());
        return false;
    }

    std::string extensionStr = filename.substr(extensionStart);
    if (extensionStr == ".png")
    {
        PictureFormat = GL_RGBA;
    }
    else if (extensionStr == ".jpg" || extensionStr == ".jpeg")
    {
        PictureFormat = GL_RGB;
    }
    else
    {
        Logger::log(1, "%s error: could not load file '%s': unsupported file format %s \n", __FUNCTION__,
            mTextureName.c_str(), extensionStr.c_str());
        return false;
    }

    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, PictureFormat, mTexWidth, mTexHeight, 0, PictureFormat, GL_UNSIGNED_BYTE, textureData);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    stbi_image_free(textureData);

    Logger::log(1, "%s: texture '%s' loaded (%dx%d, %d channels)\n", __FUNCTION__, mTextureName.c_str(), mTexWidth, mTexHeight, mNumberOfChannels);
  
    return true;
}

void Texture::bind()
{
    glBindTexture(GL_TEXTURE_2D, mTexture);
}

void Texture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::cleanup()
{
    glDeleteTextures(1, &mTexture);
}
