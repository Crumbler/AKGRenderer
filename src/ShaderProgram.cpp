#include "ShaderProgram.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

void ShaderProgram::Init(std::initializer_list<ShaderInfo> shaderInfos)
{
    shaderIds = std::vector<int>(shaderInfos.size());

    int i = 0;
    for (ShaderInfo info : shaderInfos)
    {
        shaderIds[i] = loadShader(info.file, info.type);
        ++i;
    }

    programID = glCreateProgram();
    for (int id : shaderIds)
        glAttachShader(programID, id);
    bindAttributes();
    glLinkProgram(programID);
    glValidateProgram(programID);
    getAllUniformLocations();
}

GLint ShaderProgram::getUniformLocation(const char* uniformName)
{
    return glGetUniformLocation(programID, uniformName);
}

void ShaderProgram::Start()
{
    glUseProgram(programID);
}

void ShaderProgram::Stop()
{
    glUseProgram(0);
}

GLuint ShaderProgram::getProgramID()
{
    return programID;
}

void ShaderProgram::CleanUp()
{
    Stop();

    for (int sId : shaderIds)
    {
        glDetachShader(programID, sId);
        glDeleteShader(sId);
    }

    glDeleteProgram(programID);
}

void ShaderProgram::bindAttributes()
{
    printf("bind attributes base\n");
}

void ShaderProgram::getAllUniformLocations()
{
    printf("getAllUniformLocations base\n");
}

void ShaderProgram::bindAttribute(GLuint attribute, const char* variableName)
{
    glBindAttribLocation(programID, attribute, variableName);
}

GLuint ShaderProgram::loadShader(const char* file, GLenum type)
{
    // Create the shader
	GLuint shaderID = glCreateShader(type);

	// Read the Shader code from the file
	std::string shaderCode;
	std::ifstream shaderStream(file, std::ios::in);
	if (shaderStream.is_open())
    {
		std::stringstream sstr;
		sstr << shaderStream.rdbuf();
		shaderCode = sstr.str();
		shaderStream.close();
	}
	else
	{
		printf("Can't open %s\n", file);
		getchar();
		return 0;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Shader
	printf("Compiling shader : %s\n", file);
	char const *sourcePointer = shaderCode.c_str();
	glShaderSource(shaderID, 1, &sourcePointer, NULL);
	glCompileShader(shaderID);

	// Check Shader
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0){
		std::vector<char> shaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(shaderID, InfoLogLength, NULL, &shaderErrorMessage[0]);
		printf("%s\n", &shaderErrorMessage[0]);
	}

    return shaderID;
}
