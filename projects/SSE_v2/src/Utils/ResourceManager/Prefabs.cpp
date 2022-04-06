#include "Prefabs.h"

#include "Application/Application.h"
#include "Audio/AudioManager.h"
#include "Gameplay/Scene.h"
#include "Gameplay/GameManager.h"

#include "Utils/ResourceManager/Resources.h"
#include "Utils/GlmDefines.h"

// Components
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/AudioSource.h"
#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Components/AnimatorComponent.h"
#include "Gameplay/Components/SpikeTrapBehavior.h"
#include "Gameplay/PlayerController.h"
#include "Gameplay/Components/InteractableComponent.h"
#include "Gameplay/Components/MovingPlatformBehavior.h"
#include "Gameplay/Components/Light.h"

// Physics
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Physics/Colliders/SphereCollider.h"
#include "Gameplay/Physics/TriggerVolume.h"

using namespace Gameplay;
using namespace Physics;

GameObject::Sptr Prefabs::Load(Scene::Sptr scene, std::string name, glm::vec3 position)
{	
	GameObject::Sptr result = nullptr;

	if (name == "Character Body")
	{
		result = scene->CreateGameObject("Character");
		{
			// Transform
			result->SetPosition(glm::vec3(0.0f, 0.0f, 1.0f));
			result->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			result->SetScale(glm::vec3(0.2));

			// Render Component
			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Character"));
			renderer->SetMaterial(Resources::GetMaterial("Character"));

			// Animation Component
			AnimatorComponent::Sptr animator = result->Add<AnimatorComponent>();
			animator->AddAnimation("Walk", Resources::GetAnimation("Character Walk"));
			animator->AddAnimation("Idle", Resources::GetAnimation("Character Idle"));

			animator->SetRenderer(*renderer);
			animator->SetAnimation("Idle");
			animator->SetLooping(true);
			animator->SetPause(false);
			animator->SetSpeed(4.0f);

			// Audio Source
			AudioSource::Sptr audio = result->Add<AudioSource>();
			{
				audio->Init({ false });
			}

			// Physics Collider
			RigidBody::Sptr physics = result->Add<RigidBody>(RigidBodyType::Dynamic);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);
			physics->SetCollisionGroup(Resources::Instance().PHYSICAL_GROUP);
			physics->SetCollisionMask(Resources::Instance().PHYSICAL_MASK);

			// Interaction Collider
			TriggerVolume::Sptr volume = result->Add<TriggerVolume>();
			BoxCollider::Sptr i_collider = BoxCollider::Create();
			i_collider->SetPosition(i_collider->GetPosition() + glm::vec3(0.0f, 0.0f, -2.5f));
			volume->AddCollider(i_collider);
			TriggerVolumeEnterBehaviour::Sptr trigger = result->Add<TriggerVolumeEnterBehaviour>();

			// Health Component
			result->Add<HealthComponent>(100);
		}

		return result;
	}

	if (name == "Shadow")
	{
		result = scene->CreateGameObject("Shadow");
		{
			// Set position in the SceneManager::GetCurrentScene()
			result->SetPosition(glm::vec3(0.0f, 0.0f, 1.0f));
			result->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			result->SetScale(glm::vec3(0.2));

			// Create and attach a renderer for the monkey
			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Character"));
			renderer->SetMaterial(Resources::GetMaterial("Shadow"));

			AnimatorComponent::Sptr animator = result->Add<AnimatorComponent>();
			animator->AddAnimation("Walk", Resources::GetAnimation("Character Walk"));
			animator->AddAnimation("Idle", Resources::GetAnimation("Character Idle"));

			animator->SetRenderer(*renderer);
			animator->SetAnimation("Idle");
			animator->SetLooping(true);
			animator->SetPause(false);
			animator->SetSpeed(4.0f);

			AudioSource::Sptr audio = result->Add<AudioSource>();
			{
				audio->Init({ false });
			}

			// Add a dynamic rigid body to this monkey
			RigidBody::Sptr physics = result->Add<RigidBody>(RigidBodyType::Dynamic);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);
			physics->SetCollisionGroup(Resources::Instance().SHADOW_GROUP);
			physics->SetCollisionMask(Resources::Instance().SHADOW_MASK);

			result->Add<HealthComponent>(100);
		}

		return result;
	}
	
	if (name == "Pressure Plate") {

		result = scene->CreateGameObject("Pressure Plate");
		{
			// Transform
			result->SetPosition(position);
			result->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			result->SetScale(glm::vec3(1.0f, 0.5f, 1.0f));

			// Renderer
			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Pressure Plate"));
			renderer->SetMaterial(Resources::GetMaterial("Pressure Plate"));

			// Trigger Volume
			TriggerVolume::Sptr volume = result->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
			collider->SetScale(glm::vec3(2.0f, 1.8f, 2.0f));
			volume->AddCollider(collider);
			volume->SetCollisionGroup(Resources::Instance().PHYSICAL_GROUP);
			volume->SetCollisionMask(Resources::Instance().PHYSICAL_MASK);

			AudioSource::Sptr audio = result->Add<AudioSource>();
			{
				audio->LoadEvent("Pressure Plate");
				audio->Init({ false });
			}

			// Trigger Event
			TriggerVolumeEnterBehaviour::Sptr trigger = result->Add<TriggerVolumeEnterBehaviour>();
			trigger->onTriggerEnterEvent.push_back([audio]
			{
				AudioEngine::Instance().GetEvent("Pressure Plate").SetParameter("Powered", 0);
				audio->Play();
			});

			trigger->onTriggerExitEvent.push_back([audio]
			{
				AudioEngine::Instance().GetEvent("Pressure Plate").SetParameter("Powered", 1);
				audio->Play();
			});
		}		

		return result;
	}

	if (name == "Spike Trap")
	{
		result = scene->CreateGameObject("Spike Trap");
		{
			result->SetPosition(position);
			result->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			result->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

			// Create and attach a renderer for the monkey
			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Spike Trap"));
			renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

			// Animator
			AnimatorComponent::Sptr animator = result->Add<AnimatorComponent>();
			animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
			animator->SetRenderer(*renderer);
			animator->SetLooping(false);


			// Trigger Volume
			TriggerVolume::Sptr volume = result->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
			collider->SetScale(glm::vec3(2.0f, 1.8f, 2.0f));
			volume->AddCollider(collider);
			volume->SetCollisionGroup(Resources::Instance().PHYSICAL_GROUP);
			volume->SetCollisionMask(Resources::Instance().PHYSICAL_MASK);

			// Trigger Event
			TriggerVolumeEnterBehaviour::Sptr trigger = result->Add<TriggerVolumeEnterBehaviour>();
			trigger->onTriggerEnterEvent.push_back([] 
			{

				PlayerController::Sptr pc = GameManager::GetInstance().GetPC();
				
				if (!pc->isShadow) {
					pc->GetCharacterBody()->Get<HealthComponent>()->DealDamage(10.0f);
				}
			});

			AudioSource::Sptr audio = result->Add<AudioSource>();
			{
				audio->LoadEvent("Spikes");
				audio->volume = 0.5f;
				audio->Init({ false });
			}

			// Spike Behavior
			SpikeTrapBehavior::Sptr spikeBehavior = result->Add<SpikeTrapBehavior>();
			spikeBehavior->SetAnimator(animator);
			spikeBehavior->SetTrigger(volume);
			spikeBehavior->Initialize(3, 0, false);
		}

		return result;
	}

	if (name == "Candle")
	{
		result = scene->CreateGameObject("Candle");
		{
			result->SetPosition(position);
			result->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			result->SetScale(glm::vec3(0.4));

			// Create and attach a renderer for the monkey
			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Candle"));
			renderer->SetMaterial(Resources::GetMaterial("Candle"));

			GameObject::Sptr c_Light = scene->CreateGameObject("Light");
			{
				c_Light->SetPosition(glm::vec3(0, 3.25, 0));

				Light::Sptr light = c_Light->Add<Light>();
				light->SetColor(glm::vec3(1, 0.5764, 0.18));
				light->SetIntensity(2);
				light->SetRadius(10);				

				result->AddChild(c_Light);
			}
		}

		return result;
	}

	if (name == "Torch")
	{
		result = scene->CreateGameObject("Torch");
		{
			result->SetPosition(position);
			result->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			result->SetScale(glm::vec3(0.4));

			// Create and attach a renderer for the monkey
			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Torch"));
			renderer->SetMaterial(Resources::GetMaterial("Torch"));

			GameObject::Sptr c_Light = scene->CreateGameObject("Light");
			{
				c_Light->SetPosition(glm::vec3(0, 20, 0));

				Light::Sptr light = c_Light->Add<Light>();
				light->SetColor(glm::vec3(1, 0.5674, 0.18));
				light->SetIntensity(10);
				light->SetRadius(15);

				// Trigger Volume
				TriggerVolume::Sptr volume = result->Add<TriggerVolume>();
				SphereCollider::Sptr collider = SphereCollider::Create();
				collider->SetPosition(collider->GetPosition());
				collider->SetRadius(7);
				volume->AddCollider(collider);
				volume->SetCollisionGroup(Resources::Instance().PHYSICAL_GROUP);
				volume->SetCollisionMask(Resources::Instance().PHYSICAL_MASK);

				// Trigger Event
				TriggerVolumeEnterBehaviour::Sptr trigger = result->Add<TriggerVolumeEnterBehaviour>();
				trigger->onTriggerStayEvent.push_back([]
				{
					PlayerController::Sptr pc = GameManager::GetInstance().GetPC();

					if (!pc->isShadow) {
						pc->GetCharacterShadow()->Get<HealthComponent>()->DealDamage(0.1f);
					}
				});

				result->AddChild(c_Light);
			}
		}

		return result;
	}

	if (name == "Moving Platform")
	{
		result = scene->CreateGameObject("Elevator");
		{
			result->SetPosition(position);
			result->SetScale(glm::vec3(2, 2, 0.5f));
			result->SetRotation(glm::vec3(0.f, 0.0f, 0.0f));

			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Cube"));
			renderer->SetMaterial(Resources::GetMaterial("Brown"));

			// Collider
			RigidBody::Sptr physics = result->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetScale(result->GetScale());
			physics->AddCollider(collider);
			physics->SetCollisionGroupMulti(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
			physics->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);

			// Moving Platform Behavior
			MovingPlatformBehavior::Sptr elevatorBehavior = result->Add<MovingPlatformBehavior>();
			elevatorBehavior->SetStartPosition(position);
			elevatorBehavior->SetEndPosition(position);
			elevatorBehavior->SetLooping(true);
			elevatorBehavior->SetCollider(collider);
		}

		return result;
	}

	if (name == "Lever")
	{
		result = scene->CreateGameObject("Lever");
		{
			// Transform
			result->SetPosition(position);
			result->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
			result->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

			// Renderer
			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Lever"));
			renderer->SetMaterial(Resources::GetMaterial("LeverTex"));

			// Collider
			RigidBody::Sptr physics = result->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);

			AudioSource::Sptr audio = result->Add<AudioSource>();
			{
				audio->LoadEvent("Lever");
				audio->volume = 0.75f;
				audio->Init({ false });
			}

			// Interaction Event
			InteractableComponent::Sptr interactable = result->Add<InteractableComponent>();
			interactable->onInteractionEvent.push_back([interactable, audio]
			{
				interactable->isToggled = !interactable->isToggled;
				audio->Play();
			});			
		}

		return result;
	}

	if (name == "Cage Door")
	{
		result = scene->CreateGameObject("Cage Door");
		{
			// Transform
			result->SetPosition(position);
			result->SetRotation(glm::vec3(90, 0, 90));
			result->SetScale(glm::vec3(0.25f, 0.125f, 0.065f));

			// Renderer
			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Small Cage"));
			renderer->SetMaterial(Resources::GetMaterial("Small Cage"));

			// Collider
			RigidBody::Sptr physics = result->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 2.0f, 0.0f));
			collider->SetScale(glm::vec3(1.0f, 4.0f, 3.5f));
			physics->AddCollider(collider);
			physics->SetCollisionGroupMulti(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
			physics->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);
		}
		
		return result;
	}

	if (name == "Key")
	{
		result = scene->CreateGameObject("Key");
		{
			result->SetPosition(position);
			result->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			result->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Key"));
			renderer->SetMaterial(Resources::GetMaterial("Key"));

			// Collider
			RigidBody::Sptr physics = result->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);

			// Audio
			AudioSource::Sptr audio = result->Add<AudioSource>();
			{
				audio->LoadEvent("Key");
				audio->volume = 1.0f;				
				audio->Init({ false });
			}

			// Interaction Event
			InteractableComponent::Sptr interactable = result->Add<InteractableComponent>();
			interactable->onInteractionEvent.push_back([result] {
				// If player already has the key, do nothing.
				if (GameManager::GetInstance().PlayerHasKey()) return;

				GameManager::GetInstance().GiveKey();
				result->Get<RenderComponent>()->IsEnabled = false;
				result->Get<AudioSource>()->Play();

				AudioEngine::Instance().GetEvent("Key").SetPosition(result->GetPosition());
				AudioEngine::Instance().GetEvent("Key").Play();
			});
		}

		return result;
	}

	if (name == "Key Door")
	{
		result = scene->CreateGameObject("Interact Door");
		{
			// Transform
			result->SetPosition(position);
			result->SetScale(glm::vec3(1.0f, 0.5f, 0.75f));

			// Render
			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("WallGrate"));
			renderer->SetMaterial(Resources::GetMaterial("WallGrate"));

			// Animator
			AnimatorComponent::Sptr animator = result->Add<AnimatorComponent>();
			animator->AddAnimation("Open", Resources::GetAnimation("Door"));
			animator->SetRenderer(*renderer);
			animator->SetLooping(false);
			animator->SetPause(true);
			animator->onAnimationCompleted = [animator] {
				animator->SetPause(true);
				animator->SetReverse(!animator->IsReversed());
			};
			animator->SetAnimation("Open");

			// Collider
			RigidBody::Sptr physics = result->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetScale(glm::vec3(1.5f, 3.8f, 4.3f));
			collider->SetPosition(collider->GetPosition() + glm::vec3(0, 0, 0));
			physics->AddCollider(collider);
			physics->SetCollisionGroupMulti(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
			physics->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);

			AudioSource::Sptr audio = result->Add<AudioSource>();
			{
				audio->LoadEvent("Door");
				audio->volume = 1.0f;				
				audio->Init({ false });
			}

			// Interaction Event
			InteractableComponent::Sptr interactable = result->Add<InteractableComponent>();
			interactable->onInteractionEvent.push_back([result, audio] {
				
				// If player doesn't have the key, do nothing.
				if (!GameManager::GetInstance().PlayerHasKey()) return;

				//AudioEngine::Instance().GetEvent("Door").SetPosition(interact_doorway->GetPosition());
				//AudioEngine::Instance().GetEvent("Door").Play();

				AnimatorComponent::Sptr anim = result->Get<AnimatorComponent>();
				RigidBody::Sptr rigid = result->Get<RigidBody>();

				if (anim->IsPaused() && anim->IsReversed()) {
					rigid->SetCollisionGroupMulti(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
					rigid->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);
					anim->SetPause(false);
					audio->Play();
				}
				else if (anim->IsPaused() && !anim->IsReversed()) {
					rigid->SetCollisionGroup(Resources::Instance().NO_GROUP);
					rigid->SetCollisionMask(Resources::Instance().NO_MASK);
					anim->SetPause(false);
					audio->Play();
				}
			});
		}

		return result;
	}
	
	if (name == "Shroom Trap Multi")
	{
		result = scene->CreateGameObject("Shroom Trap Multi");
		{
			result->SetPosition(position);
			result->SetRotation(glm::vec3(90, 0, 0));
			result->SetScale(glm::vec3(1.2));

			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("MS"));
			renderer->SetMaterial(Resources::GetMaterial("MS"));

			// Trigger Volume
			TriggerVolume::Sptr volume = result->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.2f, 0.0f));
			collider->SetScale(glm::vec3(2.0f, 1.55f, 2.0f));
			volume->AddCollider(collider);
			volume->SetCollisionGroup(Resources::Instance().PHYSICAL_GROUP);
			volume->SetCollisionMask(Resources::Instance().PHYSICAL_MASK);

			TriggerVolumeEnterBehaviour::Sptr trigger = result->Add<TriggerVolumeEnterBehaviour>();
			trigger->onTriggerEnterEvent.push_back([] 
			{
				PlayerController::Sptr pc = GameManager::GetInstance().GetPC();

				if (!pc->isShadow) {
					pc->movSpeed = 7.5f;  // Character Slow Speed
				}
			});
			trigger->onTriggerStayEvent.push_back([] 
			{
				PlayerController::Sptr pc = GameManager::GetInstance().GetPC();

				if (!pc->isShadow) {
					pc->GetCharacterBody()->Get<HealthComponent>()->DealDamage(0.1f);
				}
			});
			trigger->onTriggerExitEvent.push_back([] 
			{
				PlayerController::Sptr pc = GameManager::GetInstance().GetPC();

				if (!pc->isShadow) {
					pc->movSpeed = 15.0f; // Character Base Speed
				}
			});
		}

		return result;
	}

	if (name == "Shroom Trap Single")
	{
		result = scene->CreateGameObject("Shroom Trap (Single)");
		{
			result->SetPosition(position);
			result->SetRotation(glm::vec3(90, 0, 0));
			result->SetScale(glm::vec3(1.2));

			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("SS"));
			renderer->SetMaterial(Resources::GetMaterial("SS"));

			// Trigger Volume
			TriggerVolume::Sptr volume = result->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.2f, 0.0f));
			collider->SetScale(glm::vec3(2.0f, 1.55f, 2.0f));
			volume->AddCollider(collider);
			volume->SetCollisionGroup(Resources::Instance().PHYSICAL_GROUP);
			volume->SetCollisionMask(Resources::Instance().PHYSICAL_MASK);

			TriggerVolumeEnterBehaviour::Sptr trigger = result->Add<TriggerVolumeEnterBehaviour>();
			trigger->onTriggerEnterEvent.push_back([] 
			{
				PlayerController::Sptr pc = GameManager::GetInstance().GetPC();

				if (!pc->isShadow) {
					pc->movSpeed = 7.5f;  // Character Slow Speed
				}
			});
			trigger->onTriggerStayEvent.push_back([] 
			{
				PlayerController::Sptr pc = GameManager::GetInstance().GetPC();

				if (!pc->isShadow) {
					pc->GetCharacterBody()->Get<HealthComponent>()->DealDamage(0.1f);
				}
			});
			trigger->onTriggerExitEvent.push_back([] 
			{
				PlayerController::Sptr pc = GameManager::GetInstance().GetPC();

				if (!pc->isShadow) {
					pc->movSpeed = 15.0f; // Character Base Speed
				}
			});
		}

		return result;
	}

	if (name == "Crystal")
	{
		result = scene->CreateGameObject("Crystal");
		{
			result->SetPosition(position);
			result->SetRotation(glm::vec3(90, 0, -100));
			result->SetScale(glm::vec3(0.5));

			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Crystal"));
			renderer->SetMaterial(Resources::GetMaterial("Crystal"));

			GameObject::Sptr c_Light = scene->CreateGameObject("Light");
			{
				c_Light->SetPosition(glm::vec3(0, 7, 0));

				Light::Sptr light = c_Light->Add<Light>();
				light->SetColor(glm::vec3(0, 0.3215, 1.0));
				light->SetIntensity(10);
				light->SetRadius(1);

				// Trigger Volume
				TriggerVolume::Sptr volume = result->Add<TriggerVolume>();
				SphereCollider::Sptr collider = SphereCollider::Create();
				collider->SetPosition(collider->GetPosition());
				collider->SetRadius(3.5f);
				volume->AddCollider(collider);
				volume->SetCollisionGroup(Resources::Instance().PHYSICAL_GROUP);
				volume->SetCollisionMask(Resources::Instance().PHYSICAL_MASK);

				// Trigger Event
				TriggerVolumeEnterBehaviour::Sptr trigger = result->Add<TriggerVolumeEnterBehaviour>();
				trigger->onTriggerStayEvent.push_back([]
				{
					PlayerController::Sptr pc = GameManager::GetInstance().GetPC();

					if (pc->isShadow) {
						//pc->GetCharacterShadow()->Get<HealthComponent>()->DealDamage(0.1f);
					}
				});

				result->AddChild(c_Light);
			}
		}

		return result;
	}

	if (name == "Healing Well")
	{
		result = scene->CreateGameObject("Healing Well");
		{
			result->SetPosition(position);
			result->SetRotation(glm::vec3(90, 0, 0));
			result->SetScale(glm::vec3(0.5));

			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("HealingWell"));
			renderer->SetMaterial(Resources::GetMaterial("HealingWell"));

			// Trigger Volume
			TriggerVolume::Sptr volume = result->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
			collider->SetScale(glm::vec3(2.0f, 1.8f, 2.0f));
			volume->AddCollider(collider);
			volume->SetCollisionGroup(Resources::Instance().PHYSICAL_GROUP);
			volume->SetCollisionMask(Resources::Instance().PHYSICAL_MASK);

			TriggerVolumeEnterBehaviour::Sptr trigger = result->Add<TriggerVolumeEnterBehaviour>();
			trigger->onTriggerStayEvent.push_back([] 
			{
				PlayerController::Sptr pc = GameManager::GetInstance().GetPC();

				if (!pc->isShadow) {
					pc->GetCharacterBody()->Get<HealthComponent>()->Heal(0.1f);
				}
			});
		}

		return result;
	}

	if (name == "Cobweb")
	{
		result = scene->CreateGameObject("Cobweb");
		{
			result->SetPosition(position);
			result->SetRotation(glm::vec3(0, 0, 20));
			result->SetScale(glm::vec3(2));

			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("CobWeb"));
			renderer->SetMaterial(Resources::GetMaterial("CobWeb"));

			// Trigger Volume
			TriggerVolume::Sptr volume = result->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 0.5f, 0.0f));
			collider->SetScale(glm::vec3(2));
			volume->AddCollider(collider);
			volume->SetCollisionGroup(Resources::Instance().PHYSICAL_GROUP);
			volume->SetCollisionMask(Resources::Instance().PHYSICAL_MASK);

			// Trigger Event
			TriggerVolumeEnterBehaviour::Sptr trigger = result->Add<TriggerVolumeEnterBehaviour>();
			trigger->onTriggerEnterEvent.push_back([] 
			{			
				PlayerController::Sptr pc = GameManager::GetInstance().GetPC();

				if (!pc->isShadow) {
					pc->movSpeed = 4.0f;  // Character Slow Speed
				}
			});
			trigger->onTriggerExitEvent.push_back([] 
			{
				PlayerController::Sptr pc = GameManager::GetInstance().GetPC();

				if (!pc->isShadow) {
					pc->movSpeed = 15.0f;  // Character Base Speed
				}
			});
		}

		return result;
	}

	if (name == "Wall")
	{
		result = scene->CreateGameObject("Wall");
		{
			result->SetPosition(position);
			result->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
			result->SetScale(glm::vec3(1.0f, 1.0f, 1.67f));

			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Stone Wall"));
			renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

			RigidBody::Sptr physics = result->Add<RigidBody>();			
			physics->SetCollisionGroup(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
			physics->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);			
		}

		return result;
	}

	if (name == "Floor")
	{
		result = scene->CreateGameObject("Floor");
		{
			MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
			tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(50.0), glm::vec2(8.0f)));
			tiledMesh->GenerateMesh();

			result->SetPosition(position);
			result->SetScale(glm::vec3(1.0f));

			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(tiledMesh);
			renderer->SetMaterial(Resources::GetMaterial("StoneTex"));

			RigidBody::Sptr physics = result->Add<RigidBody>();
			//physics->AddCollider(BoxCollider::Create(glm::vec3(9.0f, 15.75f, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
			//divide by 2 for the values
			physics->SetCollisionGroupMulti(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
			physics->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);
		}

		return result;
	}

	if (name == "Platform")
	{
		result = scene->CreateGameObject("Platform");
		{
			result->SetPosition(position);
			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Cube"));
			renderer->SetMaterial(Resources::GetMaterial("Gray"));
		}

		return result;
	}

	if (name == "Shield")
	{
		result = scene->CreateGameObject("Shield");
		{
			result->SetPosition(position);
			result->SetRotation(glm::vec3(0));
			result->SetScale(glm::vec3(0.25f));

			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Shield"));
			renderer->SetMaterial(Resources::GetMaterial("ShieldTex"));
		}

		return result;
	}

	if (name == "Staff")
	{
		result = scene->CreateGameObject("Staff");
		{
			result->SetPosition(position);
			result->SetRotation(glm::vec3(0));
			result->SetScale(glm::vec3(1.2f));

			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Staff"));
			renderer->SetMaterial(Resources::GetMaterial("StaffTex"));
		}

		return result;
	}

	if (name == "Sword")
	{
		result = scene->CreateGameObject("Sword");
		{
			result->SetPosition(position);
			result->SetRotation(glm::vec3(0));
			result->SetScale(glm::vec3(0.1f));

			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Sword"));
			renderer->SetMaterial(Resources::GetMaterial("SwordTex"));
		}
		return result;
	}

	if (name == "Pillar")
	{
		result = scene->CreateGameObject("Pillar");
		{
			result->SetPosition(position);
			result->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			result->SetScale(glm::vec3(1));

			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Intact Pillar"));
			renderer->SetMaterial(Resources::GetMaterial("Intact Pillar"));
		}

		return result;
	}

	if (name == "Damaged Pillar")
	{
		result = scene->CreateGameObject("Damaged Pillar");
		{
			result->SetPosition(position);
			result->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			result->SetScale(glm::vec3(1));

			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Damaged Pillar"));
			renderer->SetMaterial(Resources::GetMaterial("Damaged Pillar"));
		}

		return result;
	}

	if (name == "Destroyed Pillar")
	{
		result = scene->CreateGameObject("Destroyed Pillar");
		{
			result->SetPosition(position);
			result->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			result->SetScale(glm::vec3(1));

			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Destroyed Pillar"));
			renderer->SetMaterial(Resources::GetMaterial("Destroyed Pillar"));
		}

		return result;
	}

	if (name == "Floor Grate")
	{
		result = scene->CreateGameObject("Floor Grate");
		{
			result->SetPosition(position);
			result->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			result->SetScale(glm::vec3(0.3f));

			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Floor Grate"));
			renderer->SetMaterial(Resources::GetMaterial("Floor Grate"));
		}

		return result;
	}

	if (name == "Rocks")
	{
		result = scene->CreateGameObject("Rock Pile");
		{
			result->SetPosition(position);
			result->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			result->SetScale(glm::vec3(1.25f));

			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Rock Pile"));
			renderer->SetMaterial(Resources::GetMaterial("Rock Pile"));
		}

		return result;
	}

	if (name == "Gravestone")
	{
		result = scene->CreateGameObject("Grave Stone 1");
		{
			result->SetPosition(position);
			result->SetRotation(glm::vec3(90.0f, 0.0f, -90.0f));
			result->SetScale(glm::vec3(0.3f));

			RenderComponent::Sptr renderer = result->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Grave Stone"));
			renderer->SetMaterial(Resources::GetMaterial("Grave Stone"));
		}

		return result;
	}

	return result;
}
