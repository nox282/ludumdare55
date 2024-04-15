#include "AttackSystem.h"

#include <Utils/MathUtils.h>
#include <Minion/AggroSystem.h>
#include <FactionComponent.h>
#include <VFXSystem.h>
#include <PlayerSystem.h>
#include <SpatialGridSystem.h>

using namespace Mani;

void AttackSystem::onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer)
{
	m_vfxSystem = systemContainer.initializeDependency<VFXSystem>();
	m_spatialGridSystem = systemContainer.initializeDependency<SpatialGridSystem>();
}

void AttackSystem::onDeinitialize(Mani::EntityRegistry& registry)
{
}

void AttackSystem::tick(float deltaTime, Mani::EntityRegistry& registry)
{
	RegistryView<Transform, AggroComponent, AttackComponent> attackView(registry);
	for (const EntityId entityId : attackView)
	{
		AttackComponent& attackComponent = *registry.getComponent<AttackComponent>(entityId);
		if (attackComponent.attackCooldownElapsed <= attackComponent.attackCooldown)
		{
			attackComponent.attackCooldownElapsed += deltaTime;
			continue;
		}

		AggroComponent& aggroComponent = *registry.getComponent<AggroComponent>(entityId);
		if (aggroComponent.aggroTarget == INVALID_ID)
		{
			continue;
		}

		Transform& transform = *registry.getComponent<Transform>(entityId);
		Transform* targetTransform = registry.getComponent<Transform>(aggroComponent.aggroTarget);
		if (targetTransform == nullptr)
		{
			continue;
		}

		const float attackRangeSquared = attackComponent.attackRange * attackComponent.attackRange;
		if (glm::distance2(transform.position, targetTransform->position) > attackRangeSquared)
		{
			continue;
		}

		attackComponent.attackCooldownElapsed = 0.f;
		doAttack(registry, entityId, aggroComponent.aggroTarget, attackComponent.damage);
	}

	std::shared_ptr<SpatialGridSystem> spatialGrid = m_spatialGridSystem.lock();
	RegistryView<Transform, PlayerComponent, AttackComponent, SpatialGridClientComponent> playerAttackView(registry);
	for (const EntityId entityId : playerAttackView)
	{
		AttackComponent& attackComponent = *registry.getComponent<AttackComponent>(entityId);
		if (attackComponent.attackCooldownElapsed <= attackComponent.attackCooldown)
		{
			attackComponent.attackCooldownElapsed += deltaTime;
			continue;
		}

		const Transform& transform = *registry.getComponent<Transform>(entityId);
		const float attackRangeSquared = attackComponent.attackRange * attackComponent.attackRange;


		SpatialGridClientComponent& client = *registry.getComponent<SpatialGridClientComponent>(entityId);
		std::unordered_set<EntityId> nearbyEntities;
		spatialGrid->findNearby(transform.position, glm::vec2(attackComponent.attackRange), nearbyEntities);

		EntityId target = INVALID_ID;
		float minDistanceSquared = FLT_MAX;
		for (const EntityId otherEntityId : nearbyEntities)
		{
			if (otherEntityId == entityId)
			{
				continue;
			}

			if (const FactionComponent* factionComponent = registry.getComponent<FactionComponent>(otherEntityId))
			{
				if (factionComponent->factionOwnerId == entityId)
				{
					continue;
				}
			}

			if (const Transform* otherTransform = registry.getComponent<Transform>(otherEntityId))
			{
				const float distanceSquared = glm::distance2(transform.position, otherTransform->position);
				if (distanceSquared >= minDistanceSquared)
				{
					continue;
				}

				minDistanceSquared = distanceSquared;

				if (distanceSquared > attackRangeSquared)
				{
					continue;
				}

				target = otherEntityId;
			}
		}

		if (target == INVALID_ID)
		{
			continue;
		}	

		attackComponent.attackCooldownElapsed = 0.f;
		doAttack(registry, entityId, target, attackComponent.damage);
	}
}

void AttackSystem::doAttack(EntityRegistry& registry, EntityId entityId, EntityId targetEntityId, float damage)
{
	AttackEventData eventData;
	eventData.source = entityId;
	eventData.target = targetEntityId;
	eventData.damage = damage;

	onAttackEvent.broadcast(registry, eventData);

	Transform& transform = *registry.getComponent<Transform>(entityId);
	Transform& targetTransform = *registry.getComponent<Transform>(targetEntityId);

	if (m_vfxSystem.expired())
	{
		return;
	}

	if (entityId == PlayerSystem::PLAYER_ENTITY_ID)
	{
		std::shared_ptr<VFXSystem> vfxSystem = m_vfxSystem.lock();
		VFXSystem::Request vfxRequest;

		vfxRequest.spritePath = "Ludumdare55/Assets/Sprites/VFX_Slash01.sprite";

		const glm::vec3 direction = MathUtils::normalize(targetTransform.position - transform.position);
		vfxRequest.transform.position = transform.position;
		vfxRequest.transform.rotation = glm::quatLookAt(direction, glm::vec3(0.f, 1.f, 0.f));
		vfxRequest.transform.scale = glm::vec3(1.5f);

		vfxRequest.vfx.positionVelocity = (direction) * 50.f;
		vfxRequest.vfx.alphaSpeed = -.5f;
		vfxRequest.vfx.lifetime = .15f;

		vfxSystem->SpawnVFXAsync(vfxRequest);
	}
}