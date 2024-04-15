#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
	class CameraSystem;
	class AssetSystem;
	class OpenGLResourceSystem;

	struct Sprite;

	class OpenGLVertexBuffer;
	class OpenGLIndexBuffer;
	class OpenGLVertexArray;
}

class FloorSystem : public Mani::SystemBase
{
public:
	virtual std::string_view getName() const override { return "FloorSystem"; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry) override;

protected:
	virtual void onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer) override;
	virtual void onDeinitialize(Mani::EntityRegistry& registry) override;

private:
	std::weak_ptr<Mani::AssetSystem> m_assetSystem;
	std::weak_ptr<Mani::CameraSystem> m_cameraSystem;
	std::weak_ptr<Mani::OpenGLResourceSystem> m_openGLResourceSystem;

	std::shared_ptr<Mani::Sprite> m_floorSprite;
	Mani::Transform m_transform;

	std::shared_ptr<Mani::OpenGLVertexBuffer> m_vbo;
	std::shared_ptr<Mani::OpenGLIndexBuffer> m_indexbuffer;
	std::shared_ptr<Mani::OpenGLVertexArray> m_vao;
};