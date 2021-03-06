////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                //
//   _)  |  |            _)                 This software may be modified and distributed         //
//    |  |  |  |  | (_-<  |   _ \    \      under the terms of the MIT license.                   //
//   _| _| _| \_,_| ___/ _| \___/ _| _|     See the LICENSE file for details.                     //
//                                                                                                //
//  Authors: Simon Schneegans (code@simonschneegans.de)                                           //
//                                                                                                //
////////////////////////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------------------- includes
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "TinyGLTF.hpp"

#include "../Utils/Logger.hpp"
#include "Device.hpp"
#include "Texture.hpp"

namespace Illusion {
namespace Graphics {
namespace TinyGLTF {

namespace {

vk::Filter convertFilter(int value) {
  switch (value) {
  case TINYGLTF_TEXTURE_FILTER_NEAREST:
  case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
  case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
    return vk::Filter::eNearest;
  case TINYGLTF_TEXTURE_FILTER_LINEAR:
  case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
  case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
    return vk::Filter::eNearest;
  }

  throw std::runtime_error{"Invalid filter mode " + std::to_string(value)};
}

vk::SamplerMipmapMode convertSamplerMipmapMode(int value) {
  switch (value) {
  case TINYGLTF_TEXTURE_FILTER_NEAREST:
  case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
  case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
    return vk::SamplerMipmapMode::eNearest;
  case TINYGLTF_TEXTURE_FILTER_LINEAR:
  case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
  case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
    return vk::SamplerMipmapMode::eLinear;
  }

  throw std::runtime_error{"Invalid sampler mipmap mode " + std::to_string(value)};
}

vk::SamplerAddressMode convertSamplerAddressMode(int value) {
  switch (value) {
  case TINYGLTF_TEXTURE_WRAP_RPEAT:
    return vk::SamplerAddressMode::eRepeat;
  case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
    return vk::SamplerAddressMode::eClampToEdge;
  case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
    return vk::SamplerAddressMode::eMirroredRepeat;
  }

  throw std::runtime_error{"Invalid sampler address mode " + std::to_string(value)};
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TexturePtr createTexture(
  DevicePtr const& device, tinygltf::Sampler const& sampler, tinygltf::Image const& image) {

  vk::SamplerCreateInfo info;
  info.magFilter               = convertFilter(sampler.magFilter);
  info.minFilter               = convertFilter(sampler.minFilter);
  info.addressModeU            = convertSamplerAddressMode(sampler.wrapS);
  info.addressModeV            = convertSamplerAddressMode(sampler.wrapT);
  info.addressModeW            = vk::SamplerAddressMode::eRepeat;
  info.anisotropyEnable        = true;
  info.maxAnisotropy           = 16;
  info.borderColor             = vk::BorderColor::eIntOpaqueBlack;
  info.unnormalizedCoordinates = false;
  info.compareEnable           = false;
  info.compareOp               = vk::CompareOp::eAlways;
  info.mipmapMode              = convertSamplerMipmapMode(sampler.minFilter);
  info.mipLodBias              = 0.0f;
  info.minLod                  = 0.0f;
  info.maxLod                  = 0;

  // if no image data has been loaded, try loading it on out own
  if (image.image.empty()) { return std::make_shared<Texture>(device, image.uri, info); }

  // if there is image data, create an appropriate texture object for it
  uint32_t channels = image.image.size() / image.width / image.height;

  return std::make_shared<Texture>(
    device,
    image.width,
    image.height,
    channels == 3 ? vk::Format::eR8G8B8Unorm : vk::Format::eR8G8B8A8Unorm,
    info,
    image.image.size(),
    (void*)image.image.data());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
}
}
}
