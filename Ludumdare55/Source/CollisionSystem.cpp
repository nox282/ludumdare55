#include "CollisionSystem.h"

#include <Utils/MathUtils.h>

using namespace Mani;

void CollisionSystem::tick(float deltaTime, Mani::EntityRegistry& registry)
{
	RegistryView<Transform, SphereCollisionComponent> collisionView(registry);
	for (const EntityId entityId : collisionView)
	{
		Transform& transform = *registry.getComponent<Transform>(entityId);
		SphereCollisionComponent& sphereCollision = *registry.getComponent<SphereCollisionComponent>(entityId);
		
		transform.position += sphereCollision.centerOffset;

		for (const EntityId otherEntityId : collisionView)
		{
			if (entityId == otherEntityId)
			{
				continue;
			}

			Transform& otherTransform = *registry.getComponent<Transform>(otherEntityId);
			SphereCollisionComponent& otherSphereCollision = *registry.getComponent<SphereCollisionComponent>(otherEntityId);
			
			otherTransform.position += otherSphereCollision.centerOffset;

			const float lengthSquared = glm::distance2(transform.position, otherTransform.position);
			const float minDistance = sphereCollision.radius + otherSphereCollision.radius;
			const float minDistanceSquared = minDistance * minDistance;
			if (lengthSquared < minDistanceSquared)
			{
				if (otherSphereCollision.priotity > sphereCollision.priotity)
				{
					adjust(otherTransform, transform, minDistance);
				}
				else
				{
					adjust(transform, otherTransform, minDistance);
				}
			}

			otherTransform.position -= otherSphereCollision.centerOffset;
		}

		transform.position -= sphereCollision.centerOffset;
	}
}

void CollisionSystem::adjust(Mani::Transform& self, Mani::Transform& other, float minDistance)
{
	const glm::vec3 direction = MathUtils::normalize(other.position - self.position);
	other.position = self.position + direction * minDistance;
}
