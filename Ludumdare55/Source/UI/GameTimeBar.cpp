#include "GameTimeBar.h"

#include <Assets/AssetSystem.h>

#include <RenderAPI/SpriteComponent.h>

#include <GameFlow/GameTimeSystem.h>

#include <Camera/CameraSystem.h>

using namespace Mani;

void GameTimeBarSystem::onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer)
{
	m_cameraSystem = systemContainer.initializeDependency<CameraSystem>();
	m_gameTimeSystem = systemContainer.initializeDependency<GameTimeSystem>();
	
	std::shared_ptr<AssetSystem> assetSystem = systemContainer.initializeDependency<AssetSystem>().lock();
	std::shared_ptr<GameTimeSystem> gameTime = m_gameTimeSystem.lock();

	EntityId entityId;
	{
		// time bar
		entityId = registry.create();

		registry.addComponent<Transform>(entityId);
		
		GameTimeBarComponent* gameTimeBar = registry.addComponent<GameTimeBarComponent>(entityId);
		updateTimeBar(*gameTimeBar);

		SpriteComponent* spriteComponent = registry.addComponent<SpriteComponent>(entityId);
		spriteComponent->sprite = assetSystem->loadJsonAsset<Sprite>("Ludumdare55/Assets/Sprites/Timebar.sprite");
	}

	{
		// shadow bar
		EntityId entityId2 = registry.create();

		registry.addComponent<Transform>(entityId2);
		
		GameTimeBarShadowComponent* shadowComponent = registry.addComponent<GameTimeBarShadowComponent>(entityId2);
		shadowComponent->ownerId = entityId;

		SpriteComponent* spriteComponent = registry.addComponent<SpriteComponent>(entityId2);
		spriteComponent->sprite = assetSystem->loadJsonAsset<Sprite>("Ludumdare55/Assets/Sprites/Timebar.sprite");
		spriteComponent->color = glm::vec4(0.f, 0.f, 0.f, 1.0f);
	}
}

void GameTimeBarSystem::tick(float deltaTime, Mani::EntityRegistry& registry)
{
	if (m_cameraSystem.expired())
	{
		return;
	}

	std::shared_ptr<CameraSystem> cameraSystem = m_cameraSystem.lock();
	RegistryView<Transform, GameTimeBarComponent> timebarView(registry);
	for (const EntityId entityId : timebarView)
	{
		GameTimeBarComponent& gameTimeBar = *registry.getComponent<GameTimeBarComponent>(entityId);
		Transform& transform = *registry.getComponent<Transform>(entityId);

		updateTimeBar(gameTimeBar);
		transform.scale.x = -gameTimeBar.ratio * 15.f;
		transform.scale.z = 10.f;

		Transform* cameraTransform = cameraSystem->getCameraTransform(registry);

		transform.position.x = cameraTransform->position.x + 7.5f;
		transform.position.y = cameraTransform->position.y - 40.f;
		transform.position.z = cameraTransform->position.z + 16.35f;
	}

	RegistryView<Transform, GameTimeBarShadowComponent> sadowView(registry);
	for (const EntityId entityId : sadowView)
	{
		GameTimeBarShadowComponent& shadow = *registry.getComponent<GameTimeBarShadowComponent>(entityId);
		Transform& transform = *registry.getComponent<Transform>(entityId);

		const Transform* ownerTransform = registry.getComponent<Transform>(shadow.ownerId);
		if (ownerTransform == nullptr)
		{
			continue;
		}

		transform.scale.x = -15.f;
		transform.scale.z = ownerTransform->scale.z;
		transform.position = glm::vec3(
			ownerTransform->position.x,
			ownerTransform->position.y - 0.1f,
			ownerTransform->position.z - 0.1f
		);
	}
}

void GameTimeBarSystem::updateTimeBar(GameTimeBarComponent& timeBar) const
{
	if (m_gameTimeSystem.expired())
	{
		return;
	}

	std::shared_ptr<GameTimeSystem> gameTime = m_gameTimeSystem.lock();
	MANI_ASSERT(gameTime->getGameTime() > 0.f, "do not divide by zero");
	timeBar.ratio = (gameTime->getGameTime() - gameTime->getElapsed()) / gameTime->getGameTime();
}