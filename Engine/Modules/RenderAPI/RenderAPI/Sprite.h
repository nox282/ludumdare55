#pragma once

#include <Core/Interfaces/IJsonAsset.h>
#include <filesystem>

namespace Mani
{
	struct Sprite : public IJsonAsset
	{
		std::string name;

		std::filesystem::path shaderPath;
		std::filesystem::path spritePath;

		virtual void parse(const std::string_view& content) override;
		virtual std::string toJson() override;
	};
}