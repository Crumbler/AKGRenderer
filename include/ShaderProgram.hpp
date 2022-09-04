#pragma once

#include <glad/glad.h>
#include <vector>
#include "ShaderInfo.hpp"
#include <initializer_list>

class ShaderProgram
{
    public:
        void Start();
        void Stop();
        void CleanUp();
        GLuint getProgramID();

    protected:
        void Init(std::initializer_list<ShaderInfo> shaderInfos);
        virtual void bindAttributes();
        virtual void getAllUniformLocations();
        void bindAttribute(GLuint attribute, const char* variableName);

        GLint getUniformLocation(const char* uniformName);
        GLuint programID;

    private:
        std::vector<int> shaderIds;

    static GLuint loadShader(const char* file, GLenum type);
};
