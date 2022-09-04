#include "GLRenderer.hpp"

#include <glad/glad.h>
#include <vector>

#include "ShaderProgram.hpp"

DisplayShader GLRenderer::shader;
GLLoader GLRenderer::loader;
GLDisplayModel GLRenderer::model;
GLuint GLRenderer::texId;

void GLRenderer::Init(int width, int height)
{
    shader.Init();

    const std::vector<float> pos
        {-1.0f, 1.0f,
         1.0f, 1.0f,
         -1.0f, -1.0f,
         1.0f, -1.0f};

    model = loader.loadDisplayModel(pos);

    texId = loader.createTexture(width, height);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void GLRenderer::Prepare()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLRenderer::Render()
{
    shader.Start();

    glBindVertexArray(model.getVaoID());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, model.getVertexCount());
    glBindVertexArray(0);

    shader.Stop();
}

void GLRenderer::UpdateDisplay(int width, int height, const void *data, bool sizeChanged)
{
    loader.updateTexture(texId, width, height, data, sizeChanged);
}


void GLRenderer::CleanUp()
{
    loader.CleanUp();
}
