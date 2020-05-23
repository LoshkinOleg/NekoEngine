//
// Created by efarhan on 07.05.20.
//

#include "graphics/graphics.h"
#include "graphics/texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "engine/engine.h"

#ifdef EASY_PROFILE_USE
#include "easy/profiler.h"
#endif

namespace neko
{


Image StbImageConvert(BufferFile imageFile)
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("Convert Image");
#endif
    Image image;
    image.data = stbi_load_from_memory((unsigned char*) (imageFile.dataBuffer),
                                       imageFile.dataLength, &image.width, &image.height, &image.nbChannels, 0);
    return image;
}
Texture::Texture() :
	uploadToGpuJob_([this]
	{
#ifdef EASY_PROFILE_USE
            EASY_BLOCK("Create GPU Texture");
#endif
		CreateTexture();
	}),
    convertImageJob_([this]
    {
	    image_ = StbImageConvert(diskLoadJob_.GetBufferFile());
	    diskLoadJob_.GetBufferFile().Destroy();
	    RendererLocator::get().AddPreRenderJob(&uploadToGpuJob_);
    })
{

}

void Texture::LoadFromDisk()
{
    if (textureId_ == INVALID_TEXTURE_ID)
    {
        BasicEngine::GetInstance()->ScheduleJob(&diskLoadJob_, JobThreadType::RESOURCE_THREAD);
        convertImageJob_.AddDependency(&diskLoadJob_);
        BasicEngine::GetInstance()->ScheduleJob(&convertImageJob_, JobThreadType::OTHER_THREAD);
    }
}

bool Texture::IsLoaded() const
{
    return uploadToGpuJob_.IsDone();
}


void Texture::SetPath(std::string_view path)
{
    diskLoadJob_.SetFilePath(path);
}

void Texture::FreeImage()
{
    image_.Destroy();
}

void Texture::Reset()
{
    diskLoadJob_.Reset();
    convertImageJob_.Reset();
    uploadToGpuJob_.Reset();
}

TextureManager::TextureManager()
{
}

void TextureManager::Destroy()
{
    textureIndexMap_.clear();
    currentIndex_ = 0;
}

void Image::Destroy()
{
    stbi_image_free(data);
    data = nullptr;
    height = -1;
    width = -1;
}
}
