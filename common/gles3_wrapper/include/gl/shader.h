#pragma once
#include <string>
#include <graphics/shader.h>
#include "graphics/texture.h"
#include "gl/gles3_include.h"
#include "mathematics/vector.h"
#include "mathematics/matrix.h"

namespace neko::gl
{
class Shader : public neko::Shader
{
public:
    void LoadFromFile(const std::string_view vertexShaderPath, const std::string_view fragmentShaderPath) override;

    void Bind() const;

    void Destroy();

    GLuint GetProgram();

    void SetBool(const std::string& attributeName, bool value) const;

    void SetInt(const std::string& attributeName, int value) const;
	
    void SetUInt(const std::string& attributeName, uint32_t value) const;

    void SetFloat(const std::string& attributeName, float value) const;

    void SetVec2(const std::string& name, float x, float y) const;

    void SetVec2(const std::string& name, const Vec2f& value) const;

    void SetVec3(const std::string& name, float x, float y, float z) const;

    void SetVec3(const std::string& name, const Vec3f& value) const;

    void SetVec3(const std::string& name, const float value[3]) const;

    void SetVec4(const std::string& name, float x, float y, float z, float w);

    
    void SetVec4(const std::string& name, const Vec4f& value) const;
/*
    void SetMat2(const std::string& name, const glm::mat2& mat) const;

    void SetMat3(const std::string& name, const glm::mat3& mat) const;
*/
    void SetMat4(const std::string& name, const Mat4f& mat) const;

    void SetTexture(const std::string& name, const neko::Texture& texture, unsigned int slot = 0) const ;
    void SetTexture(const std::string& name, TextureId texture, unsigned int slot = 0) const;
	
    void SetCubemap(const std::string& name, const neko::Texture& texture, unsigned int slot = 0);
    void SetCubemap(const std::string& name, TextureId texture, unsigned int slot = 0);
private:
    GLuint shaderProgram_ = 0;
};
}