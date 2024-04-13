#pragma once

#include <Core/CoreFwd.h>

struct MovementComponent
{
	glm::vec3 velocity = glm::vec3(0.f);

	float maxSpeed = 8.f;

	float coherenceRatio = .2f;
	float alignementRatio = .1f;
	float seperationRatio = .8f;

	float coherenceRadius = 2.5f;
	float seperationRadius = 1.25f;
	float alignementRadius = 10.f;
};

struct AvoidanceSourceComponent
{
};

class MovementSystem : public Mani::SystemBase
{
public:
	virtual std::string_view getName() const override { return "MovementSystem"; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry);

private:

	static glm::vec3 doCoherence(Mani::EntityRegistry& registry, const Mani::EntityId entityId, const Mani::Transform& transform, const MovementComponent& movementComponent);
	static glm::vec3 doSeparation(Mani::EntityRegistry& registry, const Mani::EntityId entityId, const Mani::Transform& transform, const MovementComponent& movementComponent);
	static glm::vec3 doAlignement(Mani::EntityRegistry& registry, const Mani::EntityId entityId, const Mani::Transform& transform, const MovementComponent& movementComponent);
};