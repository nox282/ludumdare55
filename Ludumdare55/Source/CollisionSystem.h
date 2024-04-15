#pragma once

#include <Core/CoreFwd.h>
#include <filesystem>

namespace Mani
{
	struct Sprite;
	class AssetSystem;
	class OpenGLResourceSystem;
}

struct SphereCollisionComponent
{
	SphereCollisionComponent() {}
	SphereCollisionComponent(
		glm::vec3 inCenterOffset,
		float inRadius,
		uint8_t	inPriotity
	) : centerOffset(inCenterOffset),
		radius(inRadius),
		priotity(inPriotity)
	{}

	glm::vec3	centerOffset = glm::vec3(0.f);
	float		radius = 1.f;
	uint8_t		priotity = 0;
};

class CollisionSystem : public Mani::SystemBase
{
public:
	virtual std::string_view getName() const override { return "CollisionSystem"; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry) override;

protected:
	virtual void onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer) override;
	virtual void onDeinitialize(Mani::EntityRegistry& registry) override;

private:
	std::weak_ptr<class SpatialGridSystem> m_spatialGridSystem;
	std::weak_ptr<Mani::OpenGLResourceSystem> m_openGLResourceSystem;

	static void adjust(Mani::Transform& self, Mani::Transform& other, float minDistance);
};