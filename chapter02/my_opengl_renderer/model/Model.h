#pragma once

#include "OGLRenderData.h"

class Model
{
public:
    void init();

    OGLMesh getVertexData();

private:
    OGLMesh mVertexData{};
};
