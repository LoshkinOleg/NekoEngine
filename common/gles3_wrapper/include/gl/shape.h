#pragma once
#include <mathematics/matrix.h>
#include <graphics/shape.h>
#include <gl/texture.h>
#include <gl/shader.h>

namespace neko::gl
{
struct VertexArrayObject
{
    unsigned int vao = 0;
    unsigned int ebo = 0;
    unsigned int vbo[4] = {0};
	
    unsigned int instanceVbo = 0;
};

class RenderCircle final : public neko::RenderCircle, public VertexArrayObject
{
public:
    using neko::RenderCircle::RenderCircle;
    void Init() override;

    void Draw() const override;

    void Destroy() override;
};

class RenderQuad final : public neko::RenderQuad, public VertexArrayObject
{
public:
    using neko::RenderQuad::RenderQuad;
    void Init() override;

    void Draw() const override;

    void Destroy() override;

	void SetValues(const Vec2f& newSize, const Vec3f& newOffset);
	void SetSize(const Vec2f& newSize);
	void SetOffset(const Vec3f& newOffset);
};

class RenderCuboid final : public neko::RenderCuboid, public VertexArrayObject
{
public:
    using neko::RenderCuboid::RenderCuboid;
    void Init() override;
	void InitInstanced(const Vec3f& positions, int count);

    void Draw() const override;
    void DrawInstanced(unsigned count) const;

    void Destroy() override;
};

class RenderCubeMap final : public neko::RenderCubeMap, public VertexArrayObject
{
public:
    using neko::RenderCubeMap::RenderCubeMap;
    void Init() override;
    void SetTexture(const TextureId texture) { texture_ = texture; }

	void Draw() const override {};
    void Draw(const Mat4f& view, const Mat4f& projection) const;

    void Destroy() override;

protected:
	Shader shader_;
	TextureId texture_ = 0;
};

class RenderWireFrameCuboid final : public neko::RenderWireFrameCuboid, public VertexArrayObject
{
public:
    using neko::RenderWireFrameCuboid::RenderWireFrameCuboid;
    void Init() override;

    void Draw() const override;

    void Destroy() override;
};

class RenderLine3d final : public neko::RenderLine3d, public VertexArrayObject
{
public:
    using neko::RenderLine3d::RenderLine3d;
    void Init() override;

    void Draw() const override;

    void Destroy() override;
};
}