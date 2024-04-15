#include "NeutralSpawnerSystem.h"

#include <glm/gtc/random.hpp>

#include <Attack/DeathSystem.h>
#include <Minion/MinionSpawnSystem.h>
#include <PlayerSystem.h>
#include <FactionComponent.h>

using namespace Mani;

void NeutralSpawnerSystem::onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer)
{
	m_minionSpawnSystem = systemContainer.initializeDependency<MinionSpawnSystem>();
	m_deathSystem = systemContainer.initializeDependency<DeathSystem>();
	std::shared_ptr<DeathSystem> deathSystem = m_deathSystem.lock();
	onBeforeMinionDestroyedHandle = deathSystem->onBeforeMinionDestroyed.subscribe(std::bind_front(&NeutralSpawnerSystem::onBeforeMinionDestroyed, this));

	for (uint32_t i = 0; i < std::min(m_spawnSettings.initialEntityCount, m_spawnSettings.maxEntity - m_neutralMinionCount); i++)
	{
		spawn(registry);
	}
}

void NeutralSpawnerSystem::onDeinitialize(Mani::EntityRegistry& registry)
{
	if (!m_deathSystem.expired())
	{
		std::shared_ptr<DeathSystem> deathSystem = m_deathSystem.lock();
		deathSystem->onBeforeMinionDestroyed.unsubscribe(onBeforeMinionDestroyedHandle);
	}
}

void NeutralSpawnerSystem::tick(float deltaTime, Mani::EntityRegistry& registry)
{
	spawnCadenceElapsed += deltaTime;
	incrementCadenceElapsed += deltaTime;

	if (incrementCadenceElapsed >= m_spawnSettings.incrementCadence)
	{
		incrementCadenceElapsed = 0.f;
		incrementSpawnSettings();
	}

	if (spawnCadenceElapsed >= m_spawnSettings.spawnCadence)
	{
		spawnCadenceElapsed = 0.f;
		for (uint32_t i = 0; i < std::min(m_spawnSettings.spawnWaveCount, m_spawnSettings.maxEntity - m_neutralMinionCount); i++)
		{
			spawn(registry);
		}
	}
}

void NeutralSpawnerSystem::incrementSpawnSettings()
{
	m_spawnSettings.spawnCadence = std::max(m_spawnSettings.spawnCadence + m_spawnSettingsInc.spawnCadence, m_spawnSettings.minSpawnCadence);
	m_spawnSettings.spawnRadius = std::min(m_spawnSettings.spawnRadius + m_spawnSettingsInc.spawnRadius, m_spawnSettings.maxSpawnRadius);
	m_spawnSettings.maxEntity = std::min(m_spawnSettings.maxEntity + m_spawnSettingsInc.maxEntity, m_spawnSettings.maxMaxEntity);
	m_spawnSettings.spawnWaveCount = std::min(m_spawnSettings.spawnWaveCount + m_spawnSettingsInc.spawnWave, m_spawnSettings.maxSpawnWaveCount);

	MANI_LOG(Log, "Incremented spawn settings, new spawn settings:");
	MANI_LOG(Log, "m_spawnSettings.spawnCadence: {}", m_spawnSettings.spawnCadence);
	MANI_LOG(Log, "m_spawnSettings.spawnRadius: {}", m_spawnSettings.spawnRadius);
	MANI_LOG(Log, "m_spawnSettings.maxEntity: {}", m_spawnSettings.maxEntity);
	MANI_LOG(Log, "m_spawnSettings.spawnWaveCount: {}", m_spawnSettings.spawnWaveCount);
}

void NeutralSpawnerSystem::spawn(EntityRegistry& registry)
{
	if (m_minionSpawnSystem.expired() || m_neutralMinionCount >= m_spawnSettings.maxEntity)
	{
		return;
	}

	std::shared_ptr<MinionSpawnSystem> minionSpawnSystem = m_minionSpawnSystem.lock();

	glm::vec3 playerPosition = glm::vec3(0.f);
	if (const Transform* transform = registry.getComponent<Transform>(PlayerSystem::PLAYER_ENTITY_ID))
	{
		playerPosition = transform->position;
	}

	const glm::vec3 spawnPosition = playerPosition + glm::sphericalRand(m_spawnSettings.spawnRadius);
	const MinionTemplate& minionTemplate = getRandomMinionTemplate();
	minionSpawnSystem->spawnMinion(registry, minionTemplate, INVALID_ID, spawnPosition);
	m_neutralMinionCount++;
}

const MinionTemplate& NeutralSpawnerSystem::getRandomMinionTemplate() const
{
	const std::vector<std::string> minionTemplates = MinionTiers::get().at(0);
	const size_t randomIndex = rand() % minionTemplates.size();

	MANI_LOG(Log, "Spawned a {}", minionTemplates[randomIndex]);
	return MinionTemplates::get().at(minionTemplates[randomIndex]);
}

void NeutralSpawnerSystem::onBeforeMinionDestroyed(Mani::EntityRegistry& registry, const Mani::EntityId entityId)
{
	if (const FactionComponent* faction = registry.getComponent<FactionComponent>(entityId))
	{
		if (faction->factionOwnerId == INVALID_ID)
		{
			m_neutralMinionCount--;
		}
	}
}