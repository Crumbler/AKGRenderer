#pragma once

#include <glad/glad.h>
#include <vector>

class GLRawModel
{
    public:
        GLRawModel();
        GLRawModel(GLuint vaoID, GLuint vertexCount);
        GLuint getVaoID() const;
        GLuint getVertexCount() const;
        void updateVBO(int attributeNumber, const std::vector<float>& data);

    protected:
        GLuint vaoID, vertexCount;
};
