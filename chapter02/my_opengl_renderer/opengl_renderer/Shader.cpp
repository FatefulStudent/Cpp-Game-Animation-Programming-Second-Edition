#include "Shader.h"
#include <fstream>
#include <vector>

#include "Logger.h"

bool Shader::loadShaders(std::string vertexShaderFileName, std::string fragmentShaderFileName)
{
    GLuint vertexShader = readShader(vertexShaderFileName, GL_VERTEX_SHADER);

    if (!vertexShader)
    {
        return false;
    }

    GLuint fragmentShader = readShader(fragmentShaderFileName, GL_FRAGMENT_SHADER);
    if (!fragmentShader)
    {
        return false;
    }

    mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, vertexShader);
    glAttachShader(mShaderProgram, fragmentShader);
    glLinkProgram(mShaderProgram);

    GLint isProgramLinked;
    glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &isProgramLinked);

    if (!isProgramLinked)
    {
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}

void Shader::use()
{
    glUseProgram(mShaderProgram);
}

void Shader::cleanup()
{
    glDeleteProgram(mShaderProgram);
}

GLuint Shader::readShader(std::string shaderFileName, GLuint shaderType)
{
    std::string shaderAsText;
    std::ifstream inFile(shaderFileName);

    if (inFile.is_open())
    {
        inFile.seekg(0, std::ios::end);
        shaderAsText.reserve(inFile.tellg());
        inFile.seekg(0, std::ios::beg);
        shaderAsText.assign(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>());
        inFile.close();
    }
    else
    {
        return 0;
    }

    if (inFile.bad() || inFile.fail())
    {
        inFile.close();
        return 0;
    }

    inFile.close();

    const char* shaderSource = shaderAsText.c_str();

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, (const GLchar**) &shaderSource, 0);
    glCompileShader(shader);

    if (!checkCompileStats(shaderFileName, shader)) {
        Logger::log(1, "%s error: compiling shader '%s' failed\n", __FUNCTION__, shaderFileName.c_str());
        return 0;
    }
    return shader;
}

bool Shader::checkCompileStats(std::string shaderFileName, GLuint shader) {
    GLint isShaderCompiled;
    int logMessageLength;
    std::vector<char> shaderLog;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &isShaderCompiled);
    if (!isShaderCompiled) {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logMessageLength);
        shaderLog = std::vector<char>(logMessageLength + 1);
        glGetShaderInfoLog(shader, logMessageLength, &logMessageLength, shaderLog.data());
        shaderLog.at(logMessageLength) = '\0';
        Logger::log(1, "%s error: shader compile of shader '%s' failed\n", __FUNCTION__, shaderFileName.c_str());
        Logger::log(1, "%s compile log:\n%s\n", __FUNCTION__, shaderLog.data());
        return false;
    }

    return true;
}
