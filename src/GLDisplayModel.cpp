#include "GLDisplayModel.hpp"

GLDisplayModel::GLDisplayModel() { }

GLDisplayModel::GLDisplayModel(GLuint vaoID, GLuint vbo0)
    : GLRawModel(vaoID, 4), vbo0(vbo0) { }
