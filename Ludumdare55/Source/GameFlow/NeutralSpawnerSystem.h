#pragma once

#include <Core/CoreFwd.h>

struct SpawnSettings
{
	float spawnCadence = 10.f;
	float minSpawnCadence = 5.f;
	
	float spawnRadius = 150.f;
	float maxSpawnRadius = 250.f;
	// float spawnRadius = 75.f;
	// float maxSpawnRadius = 75.f;

	uint32_t spawnWaveCount = 2.f;
	uint32_t maxSpawnWaveCount = 25;

	uint32_t maxEntity = 50;
	uint32_t maxMaxEntity = 200;

	uint32_t initialEntityCount = 10;
	//uint32_t initialEntityCount = 50;

	float incrementCadence = 30.f;
};

struct SpawnSettingsIncrements
{
	float spawnCadence = -.5f;

	float spawnRadius = 20.f;

	uint32_t spawnWave = 3;

	uint32_t maxEntity = 10;
};

struct MinionTemplate;

class NeutralSpawnerSystem : public Mani::SystemBase
{
public:
	virtual std::string_view getName() const override { return "NeutralSpawnerSystem"; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry) override;

	void incrementSpawnSettings();
	void spawn(Mani::EntityRegistry& registry);

protected:
	virtual void onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer) override;
	virtual void onDeinitialize(Mani::EntityRegistry& registry) override;

private:
	std::weak_ptr<class MinionSpawnSystem> m_minionSpawnSystem;
	std::weak_ptr<class DeathSystem> m_deathSystem;
	
	Mani::EventHandle onBeforeMinionDestroyedHandle;

	float spawnCadenceElapsed = 0.f;
	float incrementCadenceElapsed = 0.f;
	uint32_t m_neutralMinionCount = 0;

	SpawnSettings m_spawnSettings;
	SpawnSettingsIncrements m_spawnSettingsInc;

	const MinionTemplate& getRandomMinionTemplate() const;

	void onBeforeMinionDestroyed(Mani::EntityRegistry& registry, const Mani::EntityId entityId);
};