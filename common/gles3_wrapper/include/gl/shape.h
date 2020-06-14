#pragma once

#include <mathematics/vector.h>
#include <graphics/shape.h>

namespace neko::gl
{
struct VertexArrayObject
{
    unsigned int VAO = 0;
    unsigned int EBO = 0;
    unsigned int VBO[4]{};
};

class RenderCircle : public neko::RenderCircle, public VertexArrayObject
{
public:
    using neko::RenderCircle::RenderCircle;
    void Init() override;

    void Draw() const override;

    void Destroy() override;
};

class RenderQuad : public neko::RenderQuad, public VertexArrayObject
{
public:
    using neko::RenderQuad::RenderQuad;
    void Init() override;

    void Draw() const override;

    void Destroy() override;
};

class RenderCuboid : public neko::RenderCuboid, public VertexArrayObject
{
public:
    using neko::RenderCuboid::RenderCuboid;
    void Init() override;

    void Draw() const override;

    void Destroy() override;
};

class RenderWireFrameCuboid : public neko::RenderWireFrameCuboid, public VertexArrayObject
{
public:
    using neko::RenderWireFrameCuboid::RenderWireFrameCuboid;
    void Init() override;

    void Draw() const override;

    void Destroy() override;
};

class RenderLine3d : public neko::RenderLine3d, public VertexArrayObject
{
public:
    using neko::RenderLine3d::RenderLine3d;
    void Init() override;

    void Draw() const override;

    void Destroy() override;
};
}