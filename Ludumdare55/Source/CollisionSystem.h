#pragma once

#include <Core/CoreFwd.h>

struct SphereCollisionComponent
{
	glm::vec3 centerOffset = glm::vec3(0.f);
	float radius = 1.f;
	uint8_t priotity = 0;
};

class CollisionSystem : public Mani::SystemBase
{
public:
	virtual std::string_view getName() const override { return "CollisionSystem"; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry);

private:
	static void adjust(Mani::Transform& self, Mani::Transform& other, float minDistance);
};