#pragma once

#include "ShaderProgram.hpp"

class DisplayShader : public ShaderProgram
{
    public:
        void Init();

    protected:
        void bindAttributes();
        void getAllUniformLocations();

    private:
        static constexpr char VERTEX_SHADER[] = "shaders/vDisplayShader.txt",
                              FRAGMENT_SHADER[] = "shaders/fDisplayShader.txt";
};
