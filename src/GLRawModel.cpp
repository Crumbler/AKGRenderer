#include "GLRawModel.hpp"

#include <iostream>

GLRawModel::GLRawModel(GLuint vaoID, GLuint vertexCount) : vaoID(vaoID), vertexCount(vertexCount)
{

}

GLRawModel::GLRawModel() { }

GLuint GLRawModel::getVaoID() const
{
    return vaoID;
}

GLuint GLRawModel::getVertexCount() const
{
    return vertexCount;
}

void GLRawModel::updateVBO(int attributeNumber, const std::vector<float>& data)
{
    glBindVertexArray(vaoID);
    GLuint vboID;
    glGetVertexAttribIuiv(attributeNumber, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
