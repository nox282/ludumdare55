#pragma once

#include <Core/CoreFwd.h>
#include <filesystem>

namespace Mani
{
	class AssetSystem;
}

struct VFXComponent
{
	glm::vec3 positionVelocity = glm::vec3(0.f);
	glm::vec3 scaleVelocity = glm::vec3(0.f);
	// todo if needed
	// glm::vec3 rotationVelocity = glm::vec3(0.f);
	// float rotationSpeed = 0.f;
	float alphaSpeed = 0.f;

	float lifetime = 0.f;
	float elapsed = 0.f;
};

class VFXSystem : public Mani::SystemBase
{
public:
	struct Request
	{
		std::filesystem::path spritePath;
		Mani::Transform transform;
		VFXComponent vfx;
	};

	virtual std::string_view getName() const override { return "VFXSystem"; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry) override;

	void SpawnVFXAsync(const Request& request);

protected:
	virtual void onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer) override;
	virtual void onDeinitialize(Mani::EntityRegistry& registry) override;

private:
	std::weak_ptr<Mani::AssetSystem> m_assetSystem;
	std::vector<Request> m_vfxSapwnQueue;
};