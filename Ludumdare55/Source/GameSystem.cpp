#include "GameSystem.h"

#include <Assets/AssetSystem.h>

#include <OpenGL/OpenGLSystem.h>
#include <Inputs/InputSystem.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Material.h>
#include <RenderAPI/MeshComponent.h>
#include <RenderAPI/Light/DirectionalLightComponent.h>

using namespace Mani;

void GameSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
	// initialize inputs
	std::shared_ptr<OpenGLSystem> openGLSystem = Application::get().getSystemContainer().getSystem<OpenGLSystem>().lock();
	std::shared_ptr<InputSystem> inputSystem = systemContainer.initializeDependency<InputSystem>().lock();
	inputSystem->assignInputGenerator(LOCAL_USERID, openGLSystem->getInputGenerator());

	std::shared_ptr<AssetSystem> assetSystem = systemContainer.initializeDependency<AssetSystem>().lock();

	{
		// light
		EntityId sunEntityId = registry.create();

		MeshComponent* meshComponent = registry.addComponent<MeshComponent>(sunEntityId);
		meshComponent->mesh = assetSystem->loadJsonAsset<Mesh>("Ludumdare55/Assets/Meshes/Sphere.mesh");
		meshComponent->material = assetSystem->loadJsonAsset<Material>("Ludumdare55/Assets/Materials/light.material");

		Transform* transform = registry.addComponent<Transform>(sunEntityId);
		transform->position = glm::vec3(500.f, 500.0f, 500.0f);
		transform->scale = glm::vec3(50.0f, 50.0f, 50.0f);

		DirectionalLightComponent* light = registry.addComponent<DirectionalLightComponent>(sunEntityId);
		light->ambient = glm::vec3(.1f, .1f, .01f);
		light->diffuse = glm::vec3(.9f, .9f, .8f);
		light->specular = glm::vec3(.9f, .9f, .8f);
		light->direction = glm::normalize(-transform->position);
	}

	{
		// light
		EntityId sunEntityId = registry.create();

		MeshComponent* meshComponent = registry.addComponent<MeshComponent>(sunEntityId);
		meshComponent->mesh = assetSystem->loadJsonAsset<Mesh>("Ludumdare55/Assets/Meshes/Sphere.mesh");
		meshComponent->material = assetSystem->loadJsonAsset<Material>("Ludumdare55/Assets/Materials/light.material");

		Transform* transform = registry.addComponent<Transform>(sunEntityId);
		transform->position = glm::vec3(-500.f, 500.0f, -500.0f);
		transform->scale = glm::vec3(50.0f, 50.0f, 50.0f);

		DirectionalLightComponent* light = registry.addComponent<DirectionalLightComponent>(sunEntityId);
		light->ambient = glm::vec3(.1f, .1f, .01f);
		light->diffuse = glm::vec3(.9f, .9f, .8f);
		light->specular = glm::vec3(.9f, .9f, .8f);
		light->direction = glm::normalize(-transform->position);
	}

	{
		// floor
		const EntityId floorEntityId = registry.create();

		Transform* transform = registry.addComponent<Transform>(floorEntityId);
		transform->scale = glm::vec3(500.f, .2f, 500.f);


		MeshComponent* meshComponent = registry.addComponent<MeshComponent>(floorEntityId);
		meshComponent->mesh = assetSystem->loadJsonAsset<Mesh>("Ludumdare55/Assets/Meshes/Cube.mesh");
		meshComponent->material = assetSystem->loadJsonAsset<Material>("Ludumdare55/Assets/Materials/floor.material");
	}
}

void GameSystem::onDeinitialize(EntityRegistry& registry)
{
}
