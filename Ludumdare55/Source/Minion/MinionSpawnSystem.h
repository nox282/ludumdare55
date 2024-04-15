#pragma once

#include <Core/CoreFwd.h>
#include <Minion/MinionTemplate.h>
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

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry) override;

	Mani::EntityId spawnMinion(Mani::EntityRegistry& registry, const MinionTemplate& minionTemplate, Mani::EntityId ownerId, const glm::vec3& spawnPosition = glm::vec3(0.f));
protected:
	virtual void onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer) override;
	virtual void onDeinitialize(Mani::EntityRegistry& registry) override;

private:
	std::weak_ptr<Mani::AssetSystem> m_assetSystem;
	std::weak_ptr<class DeathSystem> m_deathSystem;

	uint8_t m_playerMinion = 0;

	Mani::EventHandle onMinionDestroyedHandle;

	void onBeforeMinionDestroyed(Mani::EntityRegistry& registry, const Mani::EntityId entityId);
};
