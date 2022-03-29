#include "TurretBehavior.h"
#include "Utils/ImGuiHelper.h"

#include "Gameplay/GameObject.h"
#include "Gameplay/SceneManagement/SceneManager.h"
#include "RenderComponent.h"
#include "Utils/ResourceManager/Resources.h"
#include "../Physics/Colliders/BoxCollider.h"
#include "../Physics/RigidBody.h"

using namespace Gameplay;

void TurretBehavior::RenderImGui()
{ }

nlohmann::json TurretBehavior::ToJson() const
{
	return {};
}

TurretBehavior::TurretBehavior()
{ }

TurretBehavior::~TurretBehavior() = default;

TurretBehavior::Sptr TurretBehavior::FromJson(const nlohmann::json & blob)
{
	TurretBehavior::Sptr result = std::make_shared<TurretBehavior>();
	return result;
}

void TurretBehavior::Initialize(float fireRate, float rateOffset, glm::vec3 origin, glm::vec3 direction, bool disabled)
{
	shotDelayTimer = fireRate;
	currentTimer = fireRate + rateOffset;
	shotOrigin = origin;
	shotDirection = direction;
	isDisabled = disabled;
	
}

void TurretBehavior::Update(float deltaTime)
{
	// Do Nothing if disabled
	if (isDisabled) return;

	// Count Down Timer
	if (currentTimer > 0.0f)
	{
		currentTimer -= deltaTime;
	}

	// When Timer Hits Zero
	else if (currentTimer <= 0.0f)
	{
		// Shoot a projectile
		//Shoot();
	}
}

void TurretBehavior::Shoot()
{
	// Spawn a projectile at origin
	GameObject::Sptr projectile = SceneManager::GetCurrentScene()->CreateGameObject("Projectile");
	{
		// Transform
		projectile->SetPosition(shotOrigin);
		projectile->SetScale(glm::vec3(2, 2, 2));

		// Renderer
		RenderComponent::Sptr renderer = projectile->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Turret Projectile"));

		// Collider
		Physics::RigidBody::Sptr physics = projectile->Add<Physics::RigidBody>(RigidBodyType::Dynamic);
		Physics::BoxCollider::Sptr collider = Physics::BoxCollider::Create();
		physics->AddCollider(collider);

		// Add Projectile Behavior and Collider
		physics->ApplyImpulse(shotDirection * 10.0f);

	}
}