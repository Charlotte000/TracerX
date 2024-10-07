/**
 * @file GLTFLoader.h
 */
#pragma once

#include "Scene.h"

#include <filesystem>

namespace TracerX
{

/**
 * @brief Loads a scene from a GLTF file.
 * @param path The path of the file to load the GLTF scene from. The file can be in GLTF or GLB format.
 * @return The loaded scene.
 * @throws std::runtime_error Thrown if the GLTF file fails to load.
 */
Scene loadGLTF(const std::filesystem::path& path);

}
