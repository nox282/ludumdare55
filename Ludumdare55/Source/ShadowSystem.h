#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
	class AssetSystem;
}

struct ShadowComponent
{
	Mani::EntityId ownerId = Mani::INVALID_ID;
	float xOffset = 0.8f;
	float zOffset = 0.f;
};

class ShadowSystem : public Mani::SystemBase
{
public:
	virtual std::string_view getName() const override { return "ShadowSystem"; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry) override;

	Mani::EntityId spawnShadow(Mani::EntityRegistry& registry, const Mani::EntityId ownerId);

protected:
	virtual void onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer) override;

private:
	std::weak_ptr<Mani::AssetSystem> m_assetSystem;
};