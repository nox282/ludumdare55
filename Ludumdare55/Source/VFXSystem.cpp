#include "VFXSystem.h"

#include <Assets/AssetSystem.h>
#include <RenderAPI/SpriteComponent.h>

using namespace Mani;

void VFXSystem::onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer)
{
	m_assetSystem = systemContainer.initializeDependency<AssetSystem>();
}

void VFXSystem::onDeinitialize(Mani::EntityRegistry& registry)
{
}

void VFXSystem::tick(float deltaTime, Mani::EntityRegistry& registry)
{
	if (!m_vfxSapwnQueue.empty() && !m_assetSystem.expired())
	{
		std::shared_ptr<AssetSystem> assetSystem = m_assetSystem.lock();

		for (const Request& request : m_vfxSapwnQueue)
		{
			const EntityId vfxEntityId = registry.create();
		
			Transform* transform = registry.addComponent<Transform>(vfxEntityId);
			transform->position = request.transform.position;
			transform->rotation = request.transform.rotation;
			transform->scale = request.transform.scale;

			VFXComponent* vfxComponent = registry.addComponent<VFXComponent>(vfxEntityId);
			vfxComponent->positionVelocity = request.vfx.positionVelocity;
			vfxComponent->scaleVelocity = request.vfx.scaleVelocity;
			//vfxComponent->rotationAxis = request.vfx.rotationAxis;
			//vfxComponent->rotationDelta = request.vfx.rotationDelta;
			vfxComponent->alphaSpeed = request.vfx.alphaSpeed;
			vfxComponent->lifetime = request.vfx.lifetime;
			vfxComponent->elapsed = request.vfx.elapsed;

			SpriteComponent* spriteComponent = registry.addComponent<SpriteComponent>(vfxEntityId);
			spriteComponent->sprite = assetSystem->loadJsonAsset<Sprite>(request.spritePath);
		}

		m_vfxSapwnQueue.clear();
	}

	std::vector<EntityId> entitiesToDestroy;
	RegistryView<Transform, VFXComponent, SpriteComponent> vfxView(registry);
	for (const EntityId entityId : vfxView)
	{
		Transform& transform = *registry.getComponent<Transform>(entityId);
		VFXComponent& vfx = *registry.getComponent<VFXComponent>(entityId);

		vfx.elapsed += deltaTime;
		if (vfx.elapsed >= vfx.lifetime)
		{
			entitiesToDestroy.push_back(entityId);
			continue;
		}

		transform.position += vfx.positionVelocity * deltaTime;
		transform.scale += vfx.scaleVelocity * deltaTime;
		
		if (vfx.alphaSpeed > 0.f)
		{
			SpriteComponent& sprite = *registry.getComponent<SpriteComponent>(entityId);
			sprite.color = glm::vec4(
				sprite.color.x,
				sprite.color.y,
				sprite.color.z,
				std::clamp(sprite.color.w + vfx.alphaSpeed * deltaTime, 0.f, 1.0f)
			);
		}
	}

	for (const EntityId entityId : entitiesToDestroy)
	{
		registry.destroy(entityId);
	}
}

void VFXSystem::SpawnVFXAsync(const Request& request)
{
	m_vfxSapwnQueue.push_back(request);
}
