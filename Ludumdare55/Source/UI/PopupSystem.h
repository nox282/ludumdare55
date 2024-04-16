#pragma once

#include <Core/CoreFwd.h>
#include <filesystem>
#include <unordered_set>

namespace Mani
{
	class AssetSystem;
	class CameraSystem;
}

struct PopupComponent
{
	glm::vec3 startPosition = glm::vec3(0.f);
	glm::vec3 finishPosition = glm::vec3(0.f);
	float speed = 1.f;
	bool isAlive = false;
	float elapsed = 0.f;
	float ttl = 1.f;
};

class PopupSystem : public Mani::SystemBase
{
public:
	struct Request
	{
		std::filesystem::path spritePath = "";
		glm::vec3 startPosition = glm::vec3(0.f);
		glm::vec3 finishPosition = glm::vec3(0.f);
		glm::vec3 scale = glm::vec3(1.f);
		float speed = 1.f;
	};

	virtual std::string_view getName() const override { return "PopupSystem"; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry) override;

	void SpawnPopupAsync(const Request& request);
	void ClearAllPopupsAsync(Mani::EntityRegistry& registry);

protected:
	virtual void onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer) override;

private:
	std::weak_ptr<Mani::AssetSystem> m_assetSystem;
	std::weak_ptr<Mani::CameraSystem> m_cameraSystem;
	std::vector<Request> m_queue;
	std::unordered_set<Mani::EntityId> m_popups;
};