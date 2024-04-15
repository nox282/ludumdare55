#include "ColorSystem.h"

#include <RenderAPI/SpriteComponent.h>

#include <FactionComponent.h>
#include <PlayerSystem.h>

#include <Attack/HealthSystem.h>

using namespace Mani;

void ColorSystem::tick(float deltaTime, Mani::EntityRegistry& registry)
{
	RegistryView<FactionComponent, HurtComponent, SpriteComponent> colorView(registry);
	for (const EntityId entityId : colorView)
	{
		SpriteComponent& spriteComponent = *registry.getComponent<SpriteComponent>(entityId);
		const HurtComponent& hurtComponent = *registry.getComponent<HurtComponent>(entityId);
		if (hurtComponent.elapsed < hurtComponent.duration)
		{
			spriteComponent.color = glm::vec4(1.f, 0.f, 0.f, 1.f);
			continue;
		}

		if (entityId == PlayerSystem::PLAYER_ENTITY_ID)
		{
			spriteComponent.color = glm::vec4(1.0f);
			continue;
		}

		const FactionComponent& factionComponent = *registry.getComponent<FactionComponent>(entityId);
		if (factionComponent.factionOwnerId == PlayerSystem::PLAYER_ENTITY_ID)
		{
			spriteComponent.color = glm::vec4(.75f, .75f, 1.f, .30);
		} 
		else
		{
			spriteComponent.color = glm::vec4(1.f, 1.f, 1.f, 1.f);
		}
	}
}
