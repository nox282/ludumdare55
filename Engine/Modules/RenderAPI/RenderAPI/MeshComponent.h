#pragma once

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Material.h>
#include <memory>
#include <vector>

namespace Mani
{
	enum class ETextureMode : uint8_t
	{
		Stretch = 0,

	};

	struct MeshComponent
	{
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;
	};
}