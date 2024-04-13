#pragma once

#include <RenderAPI/Sprite.h>

namespace Mani
{
	struct SpriteComponent
	{
		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		std::shared_ptr<Sprite> sprite;
	};
}