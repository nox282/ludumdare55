#include "HealthSystem.h"

#include <RenderAPI/SpriteComponent.h>

#include <Attack/AttackSystem.h>
#include <Attack/HealthComponent.h>
#include <Minion/MinionSpawnSystem.h>
#include <FactionComponent.h>
#include <VFXSystem.h>

using namespace Mani;

void HealthSystem::onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer)
{
	m_vfxSystem = systemContainer.initializeDependency<VFXSystem>();
	m_attackSystem = systemContainer.initializeDependency<AttackSystem>();
	std::shared_ptr<AttackSystem> attackSystem = m_attackSystem.lock();

	onAttackEventHandle = attackSystem->onAttackEvent.subscribe(std::bind(&HealthSystem::onAttackEvent, this, std::placeholders::_1, std::placeholders::_2));
}

void HealthSystem::onDeinitialize(Mani::EntityRegistry& registry)
{
	if (m_attackSystem.expired())
	{
		return;
	}

	std::shared_ptr<AttackSystem> attackSystem = m_attackSystem.lock();
	attackSystem->onAttackEvent.unsubscribe(onAttackEventHandle);
}

void HealthSystem::tick(float deltaTime, Mani::EntityRegistry& registry)
{
	RegistryView<HurtComponent, SpriteComponent> hurtView(registry);
	for (const EntityId entityId : hurtView)
	{
		HurtComponent& hurtComponent = *registry.getComponent<HurtComponent>(entityId);
		SpriteComponent& spriteComponent = *registry.getComponent<SpriteComponent>(entityId);
		hurtComponent.elapsed += deltaTime;
	}
}

void HealthSystem::onAttackEvent(Mani::EntityRegistry& registry, const AttackEventData& eventData)
{
	if (HealthComponent* healthComponent = registry.getComponent<HealthComponent>(eventData.target))
	{
		healthComponent->health -= std::min(eventData.damage, healthComponent->health);
	}

	if (HurtComponent* hurtComponent = registry.getComponent<HurtComponent>(eventData.target))
	{
		hurtComponent->elapsed = 0.f;
	}
}