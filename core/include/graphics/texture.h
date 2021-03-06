#pragma once

#include <engine/resource.h>
#include <xxhash.hpp>
#include <utilities/service_locator.h>

namespace neko
{

using TextureId = std::uint32_t;
const TextureId INVALID_TEXTURE_ID = 0;
using TextureIndex = std::uint32_t;
const TextureIndex INVALID_TEXTURE_INDEX = std::numeric_limits<TextureIndex>::max();
using TexturePathHash = xxh::hash32_t;
struct Image
{
    unsigned char* data = nullptr;
    int width = -1, height = -1;
    int nbChannels = 0;
    void Destroy();
};

Image StbImageConvert(BufferFile imageFile, bool flipY=false, bool hdr = false);

/**
 * \brief Neko Texture contains an Image loaded async from disk, converted by stb_image
 */
class Texture
{
public:
    enum TextureFlags : unsigned
    {
        SMOOTH_TEXTURE = 1u << 0u,
        MIPMAPS_TEXTURE = 1u << 1u,
        CLAMP_WRAP = 1u << 2u,
        REPEAT_WRAP = 1u << 3u,
        MIRROR_REPEAT_WRAP = 1u << 4u,
        GAMMA_CORRECTION = 1u << 5u,
    	FLIP_Y = 1u << 6u,
        HDR = 1u << 7u,
    	DEFAULT = REPEAT_WRAP | SMOOTH_TEXTURE | MIPMAPS_TEXTURE,
        
    };
	virtual ~Texture() = default;
	Texture();
    Texture(Texture&& texture) noexcept :
        uploadToGpuJob_ (std::move(texture.uploadToGpuJob_)),
        convertImageJob_(std::move(texture.convertImageJob_)),
		diskLoadJob_ (std::move(texture.diskLoadJob_)),
		image_(texture.image_),
		textureId_ (texture.textureId_),
		flags_(texture.flags_)
    {
    	if(!texture.IsLoaded())
    	{
    		//If you are crashing here, it means you are moving the Texture before it is fully loaded!!!
            std::abort();
    	}
    }
    virtual void Destroy() = 0;
    void SetTextureFlags(TextureFlags textureFlags) { flags_ = textureFlags; }
    void SetPath(std::string_view path);
    /**
     * \brief This function schedules the resource load from disk and the image conversion
     */
    void LoadFromDisk();
	/**
	 * \brief This function is called if the file was loaded
	 */
    bool IsLoaded() const;

    void FreeImage();

    TextureId GetTextureId() const { return textureId_;};
protected:
	
    void Reset();
    virtual void CreateTexture() = 0;

    TextureFlags flags_ = DEFAULT;
    Job uploadToGpuJob_;
    Job convertImageJob_;
    ResourceJob diskLoadJob_;
    Image image_;
    TextureId textureId_ = INVALID_TEXTURE_ID;
};

class TextureManagerInterface
{
public:
    virtual TextureIndex LoadTexture(std::string_view path) = 0;
    virtual TextureId GetTextureId(TextureIndex index) = 0;
};

class NullTextureManager : public TextureManagerInterface
{
public:
    TextureIndex LoadTexture([[maybe_unused]] std::string_view path) override
    {
	    return INVALID_TEXTURE_INDEX;
    }
    TextureId GetTextureId([[maybe_unused]] TextureIndex index) override
    {
	    return INVALID_TEXTURE_ID;
    }
};
	
class TextureManager : public SystemInterface, public TextureManagerInterface
{
public:
    TextureManager();
	void Init() override {}
    void Destroy() override;
protected:
    TextureIndex currentIndex_ = 0;
    std::unordered_map<std::string, TextureIndex> textureIndexMap_;
};

using TextureManagerLocator = Locator<TextureManagerInterface, NullTextureManager>;

}