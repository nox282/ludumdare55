#include "CollisionSystem.h"

#include <Assets/AssetSystem.h>

#include <OpenGL/Render/OpenGLResourceSystem.h>
#include <OpenGL/Render/OpenGLTexture.h>

#include <RenderAPI/Sprite.h>

#include <SpatialGridSystem.h>
#include <Utils/MathUtils.h>

using namespace Mani;

void CollisionSystem::onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer)
{
	m_spatialGridSystem = systemContainer.initializeDependency<SpatialGridSystem>();
	m_openGLResourceSystem = systemContainer.initializeDependency<OpenGLResourceSystem>();
}

void CollisionSystem::onDeinitialize(Mani::EntityRegistry& registry)
{
}

void CollisionSystem::tick(float deltaTime, Mani::EntityRegistry& registry)
{
	if (m_spatialGridSystem.expired())
	{
		return;
	}

	std::shared_ptr<SpatialGridSystem> spatialGridSystem = m_spatialGridSystem.lock();
	RegistryView<Transform, SphereCollisionComponent> collisionView(registry);
	for (const EntityId entityId : collisionView)
	{
		Transform& transform = *registry.getComponent<Transform>(entityId);
		SphereCollisionComponent& sphereCollision = *registry.getComponent<SphereCollisionComponent>(entityId);
		
		transform.position += sphereCollision.centerOffset;

		std::unordered_set<EntityId> nearbyEntities;
		spatialGridSystem->findNearby(transform.position, glm::vec2(sphereCollision.radius), nearbyEntities);
		for (const EntityId otherEntityId : nearbyEntities)
		{
			if (entityId == otherEntityId)
			{
				continue;
			}

			Transform* otherTransform = registry.getComponent<Transform>(otherEntityId);
			SphereCollisionComponent* otherSphereCollision = registry.getComponent<SphereCollisionComponent>(otherEntityId);
			if (otherTransform == nullptr || otherSphereCollision == nullptr)
			{
				continue;
			}
			
			otherTransform->position += otherSphereCollision->centerOffset;

			const float lengthSquared = glm::distance2(transform.position, otherTransform->position);
			const float minDistance = sphereCollision.radius + otherSphereCollision->radius;
			const float minDistanceSquared = minDistance * minDistance;
			if (lengthSquared < minDistanceSquared)
			{
				if (otherSphereCollision->priotity > sphereCollision.priotity)
				{
					adjust(*otherTransform, transform, minDistance);
				}
				else
				{
					adjust(transform, *otherTransform, minDistance);
				}
			}

			otherTransform->position -= otherSphereCollision->centerOffset;
		}

		transform.position -= sphereCollision.centerOffset;
	}
}

void CollisionSystem::adjust(Mani::Transform& self, Mani::Transform& other, float minDistance)
{
	const glm::vec3 direction = MathUtils::normalize(other.position - self.position);
	other.position = self.position + direction * minDistance;
}
