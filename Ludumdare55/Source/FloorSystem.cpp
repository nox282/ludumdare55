#include "FloorSystem.h"

#include <Camera/CameraSystem.h>
#include <OpenGL/Render/OpenGLResourceSystem.h>
#include <Assets/AssetSystem.h>

#include <RenderAPI/Sprite.h>

#include <OpenGL/Render/OpenGLVertexArray.h>
#include <OpenGL/Render/OpenGLBuffer.h>
#include <OpenGL/Render/OpenGLShader.h>
#include <OpenGL/Render/OpenGLSprite.h>
#include <OpenGL/Render/OpenGLTexture.h>

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

using namespace Mani;

void FloorSystem::onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer)
{
	m_cameraSystem = systemContainer.initializeDependency<Mani::CameraSystem>();
	m_openGLResourceSystem = systemContainer.initializeDependency<Mani::OpenGLResourceSystem>();
	m_assetSystem = systemContainer.initializeDependency<Mani::AssetSystem>();
	m_floorSprite = m_assetSystem.lock()->loadJsonAsset<Sprite>("Ludumdare55/Assets/Sprites/Floor.sprite");
	m_transform.position = glm::vec3(0.f, 0.f, 0.f);

	// hardcoded 2d quad
	std::vector<float> vertices =
	{
		//    vertex			// texture
		-500.0f, 0.0f, 500.0f,		0.0f, 20.f,
		500.0f, 0.0f, -500.0f,		20.f, 0.0f,
		-500.0f, 0.0f, -500.0f,		0.0f, 0.0f,

		-500.0f, 0.0f, 500.0f,		0.0f, 20.0f,
		500.0f, 0.0f, 500.0f,		20.f, 20.0f,
		500.0f, 0.0f, -500.0f,		20.f, 0.0f,
	};

	std::vector<unsigned int> indices = { 0, 1, 2, 3, 4, 5 };
	m_vbo = std::make_shared<OpenGLVertexBuffer>(&vertices[0], (int)(sizeof(float) * vertices.size()));
	m_vbo->layout =
	{
		{ ShaderDataType::Float3, false },
		{ ShaderDataType::Float2, false }
	};

	m_indexbuffer = std::make_shared<OpenGLIndexBuffer>(&indices[0], (int)(sizeof(uint32_t) * indices.size()));

	m_vao = std::make_shared<OpenGLVertexArray>();
	m_vao->addVertexBuffer(m_vbo);
	m_vao->setIndexBuffer(m_indexbuffer);
}

void FloorSystem::onDeinitialize(Mani::EntityRegistry& registry)
{
}

void FloorSystem::tick(float deltaTime, Mani::EntityRegistry& registry)
{
	if (m_openGLResourceSystem.expired() || m_cameraSystem.expired())
	{
		return;
	}

	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	{
		std::shared_ptr<CameraSystem> cameraSystem = m_cameraSystem.lock();
		const CameraComponent* cameraComponent = cameraSystem->getCameraComponent(registry);
		if (cameraComponent == nullptr)
		{
			return;
		}

		viewMatrix = cameraComponent->view;
		projectionMatrix = cameraComponent->projection;
	}

	std::shared_ptr<OpenGLResourceSystem> resourceSystem = m_openGLResourceSystem.lock();

	const std::shared_ptr<OpenGLSprite> sprite = resourceSystem->getSprite(m_floorSprite->name);
	if (sprite == nullptr)
	{
		MANI_LOG_WARNING(LogOpenGL, "Attempting to draw a sprite that is not loaded");
		return;
	}

	MANI_ASSERT(!sprite->shaderName.empty(), "A Shader is always expected in a sprite");

	std::shared_ptr<OpenGLShader> shader = resourceSystem->getShader(sprite->shaderName);
	if (shader == nullptr)
	{
		MANI_LOG_WARNING(LogOpenGL, "Attempting to draw a mesh with an uncompiled shader");
		return;
	}

	const std::shared_ptr<OpenGLTexture2D> texture = resourceSystem->getTexture(sprite->textureName);
	if (texture == nullptr)
	{
		MANI_LOG_WARNING(LogOpenGL, "Attempting to draw a texture that is not loaded");
		return;
	}

	shader->use();
	glm::mat4 modelMatrix = m_transform.calculateModelMatrix();

	shader->setFloatMatrix4("model", glm::value_ptr(modelMatrix));
	shader->setFloatMatrix4("view", glm::value_ptr(viewMatrix));
	shader->setFloatMatrix4("projection", glm::value_ptr(projectionMatrix));

	int textureIndex = 0;
	texture->bind(textureIndex);
	shader->setTextureSlot("sprite", textureIndex++);
	shader->setFloat4("color", 1.0f, 1.0f, 1.0f, 1.0f);

	m_vao->bind();
	if (const auto& indexBuffer = m_vao->getIndexBuffer())
	{
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexBuffer->getStrideCount()), GL_UNSIGNED_INT, nullptr);
	}
	else
	{
		MANI_ASSERT(false, "no index buffer provided with the vertices");
	}

	texture->unbind();
}
