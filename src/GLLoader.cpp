#include "GLLoader.hpp"

GLDisplayModel GLLoader::loadDisplayModel(const std::vector<float>& positions)
{
    int vaoId = createVAO();
    auto v0 = storeStatic(0, positions);
    glEnableVertexAttribArray(0);
    unbindVAO();
    return GLDisplayModel(vaoId, v0);
}

GLuint GLLoader::createTexture(int width, int height)
{
    GLuint tId;
    glGenTextures(1, &tId);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return tId;
}

void GLLoader::updateTexture(GLuint texId, int width, int height, const void *data, bool sizeChanged)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);

    if (sizeChanged)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                        GL_RGB, GL_UNSIGNED_BYTE, data);
    }
}

void GLLoader::CleanUp()
{
    if (!vaos.empty())
        glDeleteVertexArrays(vaos.size(), vaos.data());

    if (!vbos.empty())
        glDeleteBuffers(vbos.size(), vbos.data());

    if (!textures.empty())
        glDeleteTextures(textures.size(), textures.data());
}

int GLLoader::createVAO()
{
    GLuint vaoID;
    glGenVertexArrays(1, &vaoID);
    vaos.push_back(vaoID);
    glBindVertexArray(vaoID);
    return vaoID;
}

GLuint GLLoader::storeStatic(int attributeNumber, const std::vector<float>& data)
{
    GLuint vboID;
    glGenBuffers(1, &vboID);
    vbos.push_back(vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(attributeNumber, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vboID;
}

void GLLoader::unbindVAO()
{
    glBindVertexArray(0);
}
