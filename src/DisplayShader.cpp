#include "DisplayShader.hpp"

constexpr char DisplayShader::VERTEX_SHADER[];
constexpr char DisplayShader::FRAGMENT_SHADER[];

void DisplayShader::Init()
{
    ShaderProgram::Init({ ShaderInfo(VERTEX_SHADER, GL_VERTEX_SHADER),
                          ShaderInfo(FRAGMENT_SHADER, GL_FRAGMENT_SHADER)});
}

void DisplayShader::bindAttributes()
{
    bindAttribute(0, "position");
}

void DisplayShader::getAllUniformLocations()
{

}
