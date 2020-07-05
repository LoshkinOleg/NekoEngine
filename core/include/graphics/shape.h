#pragma once

#include <algorithm>

#include <mathematics/vector.h>
#include <mathematics/circle.h>
#include <graphics/texture.h>

namespace neko
{

class RenderableObject
{
public:
    virtual ~RenderableObject() = default;
    virtual void Init() = 0;
    virtual void Draw() const = 0;
    virtual void Destroy() = 0;
};

class RenderShape : RenderableObject
{
public:
    RenderShape() = delete;
    explicit RenderShape(Vec3f offset) : offset_(offset){};
protected:
    Vec3f offset_;
};

class RenderCircle : public RenderShape
{
public:
    RenderCircle() = delete;
    explicit RenderCircle(Vec3f offset, float radius) : RenderShape(offset), radius_(radius){}
protected:
    float radius_ = 0.0f;
    static const size_t resolution = 50;
};

class RenderQuad : public RenderShape
{
public:
    RenderQuad() = delete;
    explicit RenderQuad(Vec3f offset, Vec2f size) : RenderShape(offset), size_(size){}
protected:
    Vec2f size_;
};

class RenderCuboid : public RenderShape
{
public:
    RenderCuboid()=delete;
    explicit RenderCuboid(const Vec3f& offset, const Vec3f& size) : RenderShape(offset), size_(size){}

    [[nodiscard]] Sphere3D GenerateBoundingSphere() const
    {
        Sphere3D s;
        s.center = offset_;
        s.radius = std::max(std::max(size_.x, size_.y), size_.z);
        return s;
    }

protected:
    Vec3f size_;
};

class RenderCuboidUnique : public RenderShape
{
public:
    RenderCuboidUnique()=delete;
    explicit RenderCuboidUnique(const Vec3f& offset, const Vec3f& size) : RenderShape(offset), size_(size){}

    void SetTextures(const std::array<TextureId, 3>& textures) { cubeTex_ = textures; }
	
    [[nodiscard]] Sphere3D GenerateBoundingSphere() const
    {
        Sphere3D s;
        s.center = offset_;
        s.radius = std::max(std::max(size_.x, size_.y), size_.z);
        return s;
    }

protected:
    Vec3f size_;
	std::array<TextureId, 3> cubeTex_{0};
};

class RenderWireFrameCuboid : public RenderShape
{
public:
    RenderWireFrameCuboid()=delete;
    explicit RenderWireFrameCuboid(const Vec3f& offset, const Vec3f& size) : RenderShape(offset), size_(size){}

    [[nodiscard]] Sphere3D GenerateBoundingSphere() const
    {
        Sphere3D s;
        s.center = offset_;
        s.radius = std::max(std::max(size_.x, size_.y), size_.z);
        return s;
    }

protected:
    Vec3f size_;
};

class RenderLine3d : public RenderShape
{
public:
    RenderLine3d() = delete;
    explicit RenderLine3d(Vec3f startPos, Vec3f endPos) : RenderShape(startPos), relativeEndPos_(endPos-startPos) {}

protected:
    Vec3f relativeEndPos_;
};

class RenderSphere : public RenderShape
{
public:
    RenderSphere() = delete;
    explicit RenderSphere(Vec3f offset, float radius) : RenderShape(offset), radius_(radius){}
protected:
    float radius_ = 0.0f;
};
}