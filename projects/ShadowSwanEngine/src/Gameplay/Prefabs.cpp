#include "Prefabs.h"
#include "SceneManagement/SceneManager.h"
#include "Gameplay/GameManager.h"
#include "Utils/ResourceManagement/Resources.h"

// Utils
#include "Gameplay/GameSettings.h"

// Components
#include "Gameplay/MeshResource.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Components/AnimatorComponent.h"
#include "Gameplay/Components/InteractableComponent.h"
#include "Gameplay/Components/TurretBehavior.h"
#include "Gameplay/Components/MovingPlatformBehavior.h"
#include "Gameplay/Components/SpikeTrapBehavior.h"
#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Physics/TriggerVolume.h"

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
					wall->SetScale(glm::vec3(2.0f, 2.0f, 2.0f));

					RenderComponent::Sptr renderer = wall->Add<RenderComponent>();
					renderer->SetMesh(Resources::GetMesh("Wall 2"));
					renderer->SetMaterial(Resources::GetMaterial("Wall 2"));

					RigidBody::Sptr physics = wall->Add<RigidBody>(RigidBodyType::Static);
					BoxCollider::Sptr collider = BoxCollider::Create();
					collider->SetScale(glm::vec3(7.5f, 2.0f, 6.0f));
					collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 0.0f, 6.0f));
					physics->AddCollider(collider);
					physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
					physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
				}
				return wall;
			}

			case Prefab::Ground:
			{
				GameObject::Sptr ground = SceneManager::GetCurrentScene()->CreateGameObject("Ground");
				{
					ground->SetPosition(glm::vec3(0.0f, -35.0f, 2.5f));
					ground->SetScale(glm::vec3(17.5f, 15.0f, 3.0f));

					RenderComponent::Sptr renderer = ground->Add<RenderComponent>();
					renderer->SetMesh(Resources::GetMesh("Cube"));
					renderer->SetMaterial(Resources::GetMaterial("Gray"));

					RigidBody::Sptr physics = ground->Add<RigidBody>(RigidBodyType::Static);
					BoxCollider::Sptr collider = BoxCollider::Create();
					collider->SetScale(ground->GetScale());
					collider->SetPosition(collider->GetPosition());
					physics->AddCollider(collider);
					physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
					physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
				}
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
					renderer->SetMaterial(Resources::GetMaterial("Brown"));

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

			case Prefab::PresurePlate:
			{
				// Need to specify trigger events.
				GameObject::Sptr pressure_plate = SceneManager::GetCurrentScene()->CreateGameObject("Pressure Plate");
				{
					// Transform
					pressure_plate->SetPosition(glm::vec3(0, 0, 0));
					pressure_plate->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
					pressure_plate->SetScale(glm::vec3(1.0f, 0.5f, 1.0f));

					// Renderer
					RenderComponent::Sptr renderer = pressure_plate->Add<RenderComponent>();
					renderer->SetMesh(Resources::GetMesh("Pressure Plate"));
					renderer->SetMaterial(Resources::GetMaterial("Pressure Plate"));

					// Trigger Volume
					TriggerVolume::Sptr volume = pressure_plate->Add<TriggerVolume>();
					BoxCollider::Sptr collider = BoxCollider::Create();
					collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
					collider->SetScale(glm::vec3(2.0f, 1.0f, 2.0f));
					volume->AddCollider(collider);
					volume->SetCollisionGroup(PHYSICAL_GROUP);
					volume->SetCollisionMask(PHYSICAL_MASK);

					// Trigger Event
					TriggerVolumeEnterBehaviour::Sptr trigger = pressure_plate->Add<TriggerVolumeEnterBehaviour>();
				}
			}

			case Prefab::Switch:
			{
				// Need to initialize an interactable event function
				GameObject::Sptr lever = SceneManager::GetCurrentScene()->CreateGameObject("Lever");
				{
					// Transform
					lever->SetPosition(glm::vec3(0, 0, 0));
					lever->SetRotation(glm::vec3(0, 0, 0));
					lever->SetScale(glm::vec3(0.5f, 0.5f, 1.0f));

					// Renderer
					RenderComponent::Sptr renderer = lever->Add<RenderComponent>();
					renderer->SetMesh(Resources::GetMesh("Turret Projectile"));
					renderer->SetMaterial(Resources::GetMaterial("Gray"));

					// Collider
					RigidBody::Sptr physics = lever->Add<RigidBody>(RigidBodyType::Static);
					BoxCollider::Sptr collider = BoxCollider::Create();
					physics->AddCollider(collider);

					// Interaction Event
					InteractableComponent::Sptr interactable = lever->Add<InteractableComponent>();					
				}
				return lever;
			}

			case Prefab::Elevator:
			{
				// Need to initialize start and end position, as well as whether or not looping is enabled.
				GameObject::Sptr elevator = SceneManager::GetCurrentScene()->CreateGameObject("Elevator");
				{
					elevator->SetPosition(glm::vec3(0, 0, 0));
					elevator->SetScale(glm::vec3(5, 7, 0.5f));
					elevator->SetRotation(glm::vec3(0.f, 0.0f, 0.0f));

					RenderComponent::Sptr renderer = elevator->Add<RenderComponent>();
					renderer->SetMesh(Resources::GetMesh("Cube"));
					renderer->SetMaterial(Resources::GetMaterial("Brown"));

					// Collider
					RigidBody::Sptr physics = elevator->Add<RigidBody>(RigidBodyType::Static);
					BoxCollider::Sptr collider = BoxCollider::Create();
					collider->SetScale(elevator->GetScale());
					physics->AddCollider(collider);
					physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
					physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);

					// Moving Platform Behavior
					MovingPlatformBehavior::Sptr elevatorBehavior = elevator->Add<MovingPlatformBehavior>();
					elevatorBehavior->SetCollider(collider);

				}
				return elevator;
			}

			case Prefab::Key:
			{
				GameObject::Sptr key = SceneManager::GetCurrentScene()->CreateGameObject("Key");
				{
					key->SetPosition(glm::vec3(0, 0, 0));
					key->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
					key->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

					RenderComponent::Sptr renderer = key->Add<RenderComponent>();
					renderer->SetMesh(Resources::GetMesh("Key"));
					renderer->SetMaterial(Resources::GetMaterial("Key"));
					
					// Collider
					RigidBody::Sptr physics = key->Add<RigidBody>(RigidBodyType::Static);
					BoxCollider::Sptr collider = BoxCollider::Create();
					physics->AddCollider(collider);

					// Interaction Event
					InteractableComponent::Sptr interactable = key->Add<InteractableComponent>();
					interactable->onInteractionEvent = [key]
					{
						// If player already has the key, do nothing.
						if (GameManager::GetInstance().PlayerHasKey()) return;

						GameManager::GetInstance().GiveKey();
						key->Get<RenderComponent>()->IsEnabled = false;
					};
				}			
			}

			case Prefab::KeyDoor:
			{
				GameObject::Sptr door = SceneManager::GetCurrentScene()->CreateGameObject("Door");
				{
					// Transform
					door->SetPosition(glm::vec3(0, 0, 0));
					door->SetRotation(glm::vec3(90.f, 0.0f, 0));
					door->SetScale(glm::vec3(1.0f, 0.5f, 0.75f));

					// Render
					RenderComponent::Sptr renderer = door->Add<RenderComponent>();
					renderer->SetMesh(Resources::GetMesh("Door"));
					renderer->SetMaterial(Resources::GetMaterial("Door"));

					// Animator
					AnimatorComponent::Sptr animator = door->Add<AnimatorComponent>();
					animator->AddAnimation("Open", Resources::GetAnimation("Door"));
					animator->SetRenderer(*renderer);
					animator->SetAnimation("Open");
					animator->SetLooping(false);
					animator->SetPause(true);
					animator->onAnimationCompleted = [animator] {
						animator->SetPause(true);
						animator->SetReverse(!animator->IsReversed());
					};

					// Collider
					RigidBody::Sptr physics = door->Add<RigidBody>(RigidBodyType::Static);
					BoxCollider::Sptr collider = BoxCollider::Create();
					collider->SetScale(glm::vec3(1.5f, 3.8f, 4.3f));
					collider->SetPosition(collider->GetPosition() + glm::vec3(0, 0, 0));
					physics->AddCollider(collider);
					physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
					physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);

					// Interaction Event
					InteractableComponent::Sptr interactable = door->Add<InteractableComponent>();
					interactable->onInteractionEvent = [door]
					{
						// If player doesn't have the key, do nothing.
						if (!GameManager::GetInstance().PlayerHasKey()) return;

						AnimatorComponent::Sptr anim = door->Get<AnimatorComponent>();
						RigidBody::Sptr rigid = door->Get<RigidBody>();

						if (anim->IsPaused() && anim->IsReversed()) {
							rigid->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
							rigid->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
							anim->SetPause(false);
						}
						else if (anim->IsPaused() && !anim->IsReversed()) {
							rigid->SetCollisionGroup(NO_GROUP);
							rigid->SetCollisionMask(NO_MASK);
							anim->SetPause(false);
						}
					};
				}
				return door;
			}

			case Prefab::Turret:
			{
				// Need to initialize turret.
				GameObject::Sptr turret = SceneManager::GetCurrentScene()->CreateGameObject("Turret");
				{
					turret->SetPosition(glm::vec3(0, 0, 0));
					turret->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
					turret->SetScale(glm::vec3(0.3f, 0.2f, 0.3f));

					RenderComponent::Sptr renderer = turret->Add<RenderComponent>();
					renderer->SetMesh(Resources::GetMesh("Turret"));
					renderer->SetMaterial(Resources::GetMaterial("Turret"));

					TurretBehavior::Sptr turretBehavior = turret->Add<TurretBehavior>();					
				}
				return turret;
			}		

			case Prefab::SpikeTrap:
			{
				// Need to specify a onTriggerEvent function
				GameObject::Sptr spikeTrap = SceneManager::GetCurrentScene()->CreateGameObject("Spike Trap");
				{
					spikeTrap->SetPosition(glm::vec3(45.75f, -6.0f, 5.5f));
					spikeTrap->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
					spikeTrap->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

					// Create and attach a renderer for the monkey
					RenderComponent::Sptr renderer = spikeTrap->Add<RenderComponent>();
					renderer->SetMesh(Resources::GetMesh("Spike Trap"));
					renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

					// Animator
					AnimatorComponent::Sptr animator = spikeTrap->Add<AnimatorComponent>();
					animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
					animator->SetRenderer(*renderer);
					animator->SetAnimation("Spikes");
					animator->SetLooping(false);

					// Trigger Volume
					TriggerVolume::Sptr volume = spikeTrap->Add<TriggerVolume>();
					BoxCollider::Sptr collider = BoxCollider::Create();
					collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
					collider->SetScale(glm::vec3(2.0f, 1.0f, 2.0f));
					volume->AddCollider(collider);
					volume->SetCollisionGroup(PHYSICAL_GROUP);
					volume->SetCollisionMask(PHYSICAL_MASK);

					// Trigger Event
					TriggerVolumeEnterBehaviour::Sptr trigger = spikeTrap->Add<TriggerVolumeEnterBehaviour>();					

					// Spike Behavior
					SpikeTrapBehavior::Sptr spikeBehavior = spikeTrap->Add<SpikeTrapBehavior>();
					spikeBehavior->SetAnimator(animator);
					spikeBehavior->SetTrigger(volume);
					spikeBehavior->Initialize(3, 0, false);

				}
				return spikeTrap;
			}
		}
	}
}