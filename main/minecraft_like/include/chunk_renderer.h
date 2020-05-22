#pragma once
#include "engine/system.h"
#include "gl/shader.h"
#include "gl/shape.h"
#include "graphics/graphics.h"
#include "sdl_engine/sdl_engine.h"

namespace neko
{
    class ChunkRenderer : public RenderCommandInterface
    {
    public:
        enum class RenderType
        {
            VaoProgram,
            EboProgram,
            NekoQuad,
            NekoCircle,
            Length
        };

        void Render() override;
        void Init();
        void Update(seconds dt);
        void Destroy();

    private:
        RenderType renderType_ = RenderType::VaoProgram;
        struct VaoProgram
        {
            float vertices[18] = {
                    0.5f, 0.5f, 0.0f,  // top right
                    0.5f, -0.5f, 0.0f,  // bottom right
                    -0.5f, 0.5f, 0.0f,   // top left
                    0.5f, -0.5f, 0.0f,  // bottom right
                    -0.5f, -0.5f, 0.0f,  // bottom left
                    -0.5f, 0.5f, 0.0f   // top left
            };
            float colors[18] = {
                    1.0f, 0.0f, 1.0f,// top right
                    0.0f, 1.0f, 0.0f, // bottom right
                    0.5f, 0.0f, 0.5f,// top left
                    0.0f, 1.0f, 0.0f, // bottom right
                    0.0f, 0.0f, 1.0f,// bottom left
                    0.5f, 0.0f, 0.5f,// top left
            };

            unsigned int VBO[2] = {}; //Vertex Buffer Object
            unsigned int VAO = 0; //Vertex Array Object
        };
        VaoProgram vaoProgam_;
        struct EboProgram
        {
            float vertices[12] = {
                    0.5f, 0.5f, 0.0f,  // top right
                    0.5f, -0.5f, 0.0f,  // bottom right
                    -0.5f, -0.5f, 0.0f,  // bottom left
                    -0.5f, 0.5f, 0.0f   // top left
            };
            float colors[12] = {
                    1.0f, 0.0f, 1.0f,
                    0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 1.0f,
                    0.5f, 0.0f, 0.5f
            };
            unsigned int indices[6] = {
                // note that we start from 0!
                0, 1, 3,   // first triangle
                1, 2, 3    // second triangle
            };

            unsigned int EBO = 0; // Element Buffer Object
            unsigned int VBO[2] = {}; //Vertex Buffer Object
            unsigned int VAO = 0; //Vertex Array Object
        };
        EboProgram eboProgram_;

        gl::RenderQuad quad_{ Vec3f::zero, Vec2f::one };
        gl::RenderCircle circle_{ Vec3f::zero, 0.5f };
        seconds timeSinceInit_ = seconds(0.0f);
        gl::Shader shader_;
        gl::Shader nekoShader_;
    };
}
