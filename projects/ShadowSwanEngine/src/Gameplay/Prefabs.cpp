#include "Prefabs.h"
#include "SceneManagement/SceneManager.h"
#include "Utils/ResourceManagement/Resources.h"

// Utils
#include "Gameplay/GameSettings.h"

// Components
#include "Gameplay/MeshResource.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"

using namespace Physics;

namespace Gameplay
{
	GameObject::Sptr Prefabs::Instantiate(Prefab id)
	{
		switch (id) {
			default:
			case Prefab::Wall:
			{
				GameObject::Sptr wall = SceneManager::GetCurrentScene()->CreateGameObject("Wall");
				{
					wall->SetPosition(glm::vec3(0, 0, 0));
					wall->SetRotation(glm::vec3(0.f, 0.0f, 0.0f));
					wall->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

					RenderComponent::Sptr renderer = wall->Add<RenderComponent>();
					renderer->SetMesh(Resources::GetMesh("Wall 2"));
					renderer->SetMaterial(Resources::GetMaterial("Wall 2"));

					RigidBody::Sptr physics = wall->Add<RigidBody>(RigidBodyType::Static);
					BoxCollider::Sptr collider = BoxCollider::Create();
					collider->SetScale(glm::vec3(3.0f, 4.0f, 24.0f));
					collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
					physics->AddCollider(collider);
					physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
					physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
				}
				return wall;
			}
			
			case Prefab::Crate:
			{
				GameObject::Sptr crate = SceneManager::GetCurrentScene()->CreateGameObject("Crate");
				{
					crate->SetPosition(glm::vec3(0, 0, 0));
					crate->SetRotation(glm::vec3(0, 0, 0));
					crate->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

					RenderComponent::Sptr renderer = crate->Add<RenderComponent>();
					renderer->SetMesh(Resources::GetMesh("Cube"));
					renderer->SetMaterial(Resources::GetMaterial("Gray"));

					// Collider
					RigidBody::Sptr physics = crate->Add<RigidBody>(RigidBodyType::Dynamic);
					BoxCollider::Sptr collider = BoxCollider::Create();
					collider->SetScale(crate->GetScale());
					physics->AddCollider(collider);
					physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
					physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
				}
				return crate;
			}			
		}
	}
}