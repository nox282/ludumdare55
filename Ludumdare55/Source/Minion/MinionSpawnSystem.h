#pragma once

#include <Core/CoreFwd.h>
#include <filesystem>

namespace Mani
{
	class AssetSystem;
}

class MinionSpawnSystem : public Mani::SystemBase
{
public:
	virtual std::string_view getName() const override { return "MinionSpawnSystem"; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry);

protected:
	virtual void onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer) override;
	virtual void onDeinitialize(Mani::EntityRegistry& registry) override;

private:
	std::weak_ptr<Mani::AssetSystem> m_assetSystem;

	Mani::EntityId createMinion(Mani::EntityRegistry& registry, const std::filesystem::path& file, Mani::EntityId ownerId);
};