//
// Created by efarhan on 26.01.20.
//

#include "chunk_renderer.h"

#include "glad/glad.h"

namespace neko
{
    void ChunkRenderer::Init()
    {
        const auto& config = BasicEngine::GetInstance()->config;

        shader_.LoadFromFile(config.dataRootPath + "data/shaders/01_hello_triangle/hello_triangle.vert",
            config.dataRootPath + "data/shaders/01_hello_triangle/hello_triangle.frag");

        //Initialize the VAO program
        glGenBuffers(2, &vaoProgam_.VBO[0]);
        glGenVertexArrays(1, &vaoProgam_.VAO);
        // 1. bind Vertex Array Object
        glBindVertexArray(vaoProgam_.VAO);
        // 2. copy our vertices array in a buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, vaoProgam_.VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vaoProgam_.vertices), vaoProgam_.vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // 2. copy our colors array in a buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, vaoProgam_.VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vaoProgam_.colors), vaoProgam_.colors, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);


        //Initialize the EBO program
        glGenBuffers(2, &eboProgram_.VBO[0]);
        glGenBuffers(1, &eboProgram_.EBO);
        glGenVertexArrays(1, &eboProgram_.VAO);
        // 1. bind Vertex Array Object
        glBindVertexArray(eboProgram_.VAO);
        // 2. copy our vertices array in a buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, eboProgram_.VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(eboProgram_.vertices), eboProgram_.vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // 2. copy our colors array in a buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, eboProgram_.VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(eboProgram_.colors), eboProgram_.colors, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboProgram_.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(eboProgram_.indices), eboProgram_.indices, GL_STATIC_DRAW);

        nekoShader_.LoadFromFile(
            config.dataRootPath + "data/shaders/01_hello_triangle/hello_neko_quad.vert",
            config.dataRootPath + "data/shaders/01_hello_triangle/hello_triangle.frag"
        );
        quad_.Init();
        circle_.Init();

        glEnable(GL_DEPTH_TEST);
    }

    void ChunkRenderer::Update(seconds dt)
    {
        timeSinceInit_ += dt;
    }

    void ChunkRenderer::Render()
    {
        if (shader_.GetProgram() == 0)
            return;
        switch (renderType_)
        {

        case RenderType::VaoProgram:
        {
            shader_.Bind();
            const float colorValue = (std::cos(timeSinceInit_.count()) + 1.0f) / 2.0f;
            shader_.SetFloat("colorCoeff", colorValue);

            glBindVertexArray(vaoProgam_.VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            break;
        }
        case RenderType::EboProgram:
        {
            shader_.Bind();
            const float colorValue = (std::cos(timeSinceInit_.count()) + 1.0f) / 2.0f;
            shader_.SetFloat("colorCoeff", colorValue);

            glBindVertexArray(eboProgram_.VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            break;
        }
        case RenderType::NekoQuad:
        {
            nekoShader_.Bind();
            const float colorValue = (std::cos(timeSinceInit_.count()) + 1.0f) / 2.0f;
            nekoShader_.SetFloat("colorCoeff", colorValue);
            nekoShader_.SetVec4("aColor", Vec4f(1.0f, 0.5f, 0.0f, 1.0f));
            quad_.Draw();

            break;
        }
        case RenderType::NekoCircle:
        {
            nekoShader_.Bind();
            const float colorValue = (std::cos(timeSinceInit_.count()) + 1.0f) / 2.0f;
            nekoShader_.SetFloat("colorCoeff", colorValue);
            nekoShader_.SetVec4("aColor", Vec4f(1.0f, 0.5f, 0.0f, 1.0f));
            circle_.Draw();
        }
        default:
            break;
        }


    }

    void ChunkRenderer::Destroy()
    {

        glDeleteVertexArrays(1, &eboProgram_.VAO);
        glDeleteBuffers(2, &eboProgram_.VBO[0]);
        glDeleteBuffers(2, &eboProgram_.EBO);

        glDeleteVertexArrays(1, &vaoProgam_.VAO);
        glDeleteBuffers(2, &vaoProgam_.VBO[0]);

        quad_.Destroy();
        circle_.Destroy();

        shader_.Destroy();
        nekoShader_.Destroy();


    }
}