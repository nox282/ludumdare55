#pragma once

#include <Core/CoreFwd.h>

class AggroSystem;

struct AIBehaviorComponent
{
	Mani::EntityId ownerId;
	float returnToOwnerDistance = 15.f;
	glm::vec3 desiredVelocity = glm::vec3(0.f);
	float wanderTimeoutInSeconds = 2.f;
	float wanderElapsedInSeconds = 0.f;
};

class AIBehaviorSystem : public Mani::SystemBase
{
	virtual std::string_view getName() const override { return "AIBehaviorSystem"; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry);

private:
	bool aggroAgentTick(float deltaTime, Mani::EntityRegistry& registry, Mani::EntityId entityId);
	void controlledAgentTick(float deltaTime, Mani::EntityRegistry& registry, Mani::EntityId entityId);
	void neutralAgentTick(float deltaTime, Mani::EntityRegistry& registry, Mani::EntityId entityId);
};