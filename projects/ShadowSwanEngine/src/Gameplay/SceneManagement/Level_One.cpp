#include "Level_One.h"
#include "SceneManager.h"

// Gameplay
#include "Gameplay/MeshResource.h"

// Components
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/AnimatorComponent.h"
#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Components/InteractableComponent.h"
#include "Gameplay/Components/Enemy.h"

// Physics
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Physics/TriggerVolume.h"

// Utils
#include "Utils/ResourceManagement/Resources.h"
#include "Utils/GlmDefines.h"
#include <GLFW/glfw3.h>
#include "Utils/UIHelper.h"
#include "Graphics/GuiBatcher.h"

using namespace Gameplay::Physics;

bool Level_One::PlayOnLoad = true;

Scene::Sptr Level_One::Load(GLFWwindow* window)
{

	// TO BE MOVED TO GAME SETTINGS
	int PHYSICAL_MASK = 0xFFFFFFFF;
	int PHYSICAL_GROUP = 0x01;
	int SHADOW_MASK = 0xFFFFFFFE;
	int SHADOW_GROUP = 0x02;
	int NO_MASK = 0xFFFFFFFD;
	int NO_GROUP = 0x03;

	/////////////////////////////////////////////////////////
	//					  LIGHTS
	/////////////////////////////////////////////////////////

	SceneManager::GetCurrentScene()->Lights.resize(8);
	SceneManager::GetCurrentScene()->Lights[0].Position = glm::vec3(0.0f, 12.0f, 13.0f);
	SceneManager::GetCurrentScene()->Lights[0].Color = glm::vec3(1.0f, 1.0f, 1.0f);
	SceneManager::GetCurrentScene()->Lights[0].Range = 100.0f;

	/////////////////////////////////////////////////////////
	//					  CONTROLLER
	/////////////////////////////////////////////////////////

	GameObject::Sptr camera = SceneManager::GetCurrentScene()->CreateGameObject("Main Camera");
	{
		camera->SetPosition(glm::vec3(0.0f, 0.0f, 25.0f));
		camera->SetRotation(glm::vec3(45.0f, 0.0f, 0.0f));
		//camera->LookAt(glm::vec3(0.0f));			

		Camera::Sptr cam = camera->Add<Camera>();
		cam->SetFovDegrees(60.0f);
		SceneManager::GetCurrentScene()->MainCamera = cam;
	}

	GameObject::Sptr body = SceneManager::GetCurrentScene()->CreateGameObject("Body");
	{
		// Set position in the SceneManager::GetCurrentScene()
		body->SetPosition(glm::vec3(-4.0f, 17.0f, 0.0f));
		body->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		body->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

		RenderComponent::Sptr renderer = body->Add<RenderComponent>();

		renderer->SetMesh(Resources::GetMesh("Character"));
		renderer->SetMaterial(Resources::GetMaterial("Character"));

		AnimatorComponent::Sptr animator = body->Add<AnimatorComponent>();
		std::vector<MeshResource::Sptr> walkFrames;
		animator->AddAnimation("Walk", Resources::GetAnimation("Character Walk"));
		animator->AddAnimation("Idle", Resources::GetAnimation("Character Idle"));

		animator->SetRenderer(*renderer);
		animator->SetAnimation("Idle");
		//animator->SetFrames(std::move(frames));
		animator->SetLooping(true);
		animator->SetPause(false);
		animator->SetSpeed(4.0f);

		//std::cout << std::to_string(frames.size()) << std::endl;

		RigidBody::Sptr physics = body->Add<RigidBody>(RigidBodyType::Dynamic);
		BoxCollider::Sptr collider = BoxCollider::Create();
		physics->AddCollider(collider);
		physics->SetCollisionGroup(PHYSICAL_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK);

		TriggerVolume::Sptr volume = body->Add<TriggerVolume>();
		BoxCollider::Sptr i_collider = BoxCollider::Create();
		i_collider->SetPosition(i_collider->GetPosition() + glm::vec3(0.0f, 0.0f, -2.5f));
		volume->AddCollider(i_collider);
		TriggerVolumeEnterBehaviour::Sptr trigger = body->Add<TriggerVolumeEnterBehaviour>();

		body->Add<HealthComponent>(100.0f);
	}

	GameObject::Sptr shadow = SceneManager::GetCurrentScene()->CreateGameObject("Shadow");
	{
		// Set position in the SceneManager::GetCurrentScene()
		shadow->SetPosition(glm::vec3(4.0f, 17.0f, 0.0f));
		shadow->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		shadow->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

		// Create and attach a renderer for the monkey
		RenderComponent::Sptr renderer = shadow->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Character"));
		renderer->SetMaterial(Resources::GetMaterial("Shadow"));

		AnimatorComponent::Sptr animator = shadow->Add<AnimatorComponent>();
		animator->AddAnimation("Walk", Resources::GetAnimation("Character Walk"));
		animator->AddAnimation("Idle", Resources::GetAnimation("Character Idle"));

		animator->SetRenderer(*renderer);
		animator->SetAnimation("Idle");
		//animator->SetFrames(std::move(frames));
		animator->SetLooping(true);
		animator->SetPause(false);
		animator->SetSpeed(4.0f);

		// Add a dynamic rigid body to this monkey
		RigidBody::Sptr physics = shadow->Add<RigidBody>(RigidBodyType::Dynamic);
		BoxCollider::Sptr collider = BoxCollider::Create();
		physics->AddCollider(collider);
		physics->SetCollisionGroup(SHADOW_GROUP);
		physics->SetCollisionMask(SHADOW_MASK);


		shadow->Add<HealthComponent>(100.0f);
	}

	SceneManager::GetCurrentScene()->PC.Initialize(*body, *shadow, *camera, SceneManager::GetCurrentScene()->Lights[0], *body->Get<TriggerVolume>());

	/////////////////////////////////////////////////////////
	//					ENVIRONMENT
	/////////////////////////////////////////////////////////

	// Set up all our sample objects
	GameObject::Sptr floor = SceneManager::GetCurrentScene()->CreateGameObject("Floor");
	{
		MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
		tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(100.0f), glm::vec2(8.0f)));
		tiledMesh->GenerateMesh();

		// Create and attach a RenderComponent to the object to draw our mesh
		RenderComponent::Sptr renderer = floor->Add<RenderComponent>();
		renderer->SetMesh(tiledMesh);
		renderer->SetMaterial(Resources::GetMaterial("Stone Slab"));

		RigidBody::Sptr physics = floor->Add<RigidBody>(/*static by default*/);
		physics->AddCollider(BoxCollider::Create(glm::vec3(100.0f, 100.0f, 1.5f)))->SetPosition({ 0,0,-1 });
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}


#pragma region WALLS

	GameObject::Sptr wall1 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 1");
	{
		wall1->SetPosition(glm::vec3(0, 50.0f, 0.0f));
		wall1->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall1->SetScale(glm::vec3(1.0f, 1.0f, 1.5f));

		RenderComponent::Sptr renderer = wall1->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall1->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 24.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall2 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 2");
	{
		wall2->SetPosition(glm::vec3(-24.5f, 46.5f, 0.0f));
		wall2->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall2->SetScale(glm::vec3(1.0f, 1.0f, 0.3f));

		RenderComponent::Sptr renderer = wall2->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall2->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0, 15.5));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall3 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 3");
	{
		wall3->SetPosition(glm::vec3(-24.5f, 32.5f, 0.0f));
		wall3->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall3->SetScale(glm::vec3(1.0f, 1.0f, 0.3f));

		RenderComponent::Sptr renderer = wall3->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall3->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3, 4, 4.6));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall4 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 4");
	{
		wall4->SetPosition(glm::vec3(24.5f, 46.5f, 0.0f));
		wall4->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall4->SetScale(glm::vec3(1.0f, 1.0f, 0.3f));

		RenderComponent::Sptr renderer = wall4->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall4->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 24.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall5 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 5");
	{
		wall5->SetPosition(glm::vec3(24.5f, 46.5f, 0.0f));
		wall5->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall5->SetScale(glm::vec3(1.0f, 1.0f, 0.3f));

		RenderComponent::Sptr renderer = wall5->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall5->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 24.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall6 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 6");
	{
		wall6->SetPosition(glm::vec3(-13.5f, 29.0f, 0.0f));
		wall6->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall6->SetScale(glm::vec3(1.0f, 1.0f, 0.7f));

		RenderComponent::Sptr renderer = wall6->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall6->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3, 4, 4.6));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall7 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 7");
	{
		wall7->SetPosition(glm::vec3(13.5f, 29.0f, 0.0f));
		wall7->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall7->SetScale(glm::vec3(1.0f, 1.0f, 0.7f));

		RenderComponent::Sptr renderer = wall7->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall7->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0, 4, 15.5));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall8 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 8");
	{
		wall8->SetPosition(glm::vec3(-7.0f, 17.0f, 0.0f));
		wall8->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall8->SetScale(glm::vec3(1.0f, 1.0f, 0.7f));

		RenderComponent::Sptr renderer = wall8->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall8->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 24.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall9 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 9");
	{
		wall9->SetPosition(glm::vec3(-10.5f, 5.0f, 0.0f));
		wall9->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall9->SetScale(glm::vec3(1.0f, 1.0f, 0.5f));

		RenderComponent::Sptr renderer = wall9->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall9->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 15.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall10 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 10");
	{
		wall10->SetPosition(glm::vec3(6.5f, 5.0f, 0.0f));
		wall10->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall10->SetScale(glm::vec3(1.0f, 1.0f, 0.25f));

		RenderComponent::Sptr renderer = wall10->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall10->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0, 4, 15.5));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall11 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 11");
	{
		wall11->SetPosition(glm::vec3(9.0f, -3.5f, 0.0f));
		wall11->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall11->SetScale(glm::vec3(1.0f, 1.0f, 0.5f));

		RenderComponent::Sptr renderer = wall11->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall11->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0, 4, 15));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall12 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 12");
	{
		wall12->SetPosition(glm::vec3(-10.5f, -12.5f, 0.0f));
		wall12->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall12->SetScale(glm::vec3(1.0f, 1.0f, 0.5f));

		RenderComponent::Sptr renderer = wall12->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall12->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0, 4, 15.5));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall13 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 13");
	{
		wall13->SetPosition(glm::vec3(6.5f, -12.5f, 0.0f));
		wall13->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall13->SetScale(glm::vec3(1.0f, 1.0f, 0.25f));

		RenderComponent::Sptr renderer = wall13->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall13->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 24.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall14 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 14");
	{
		wall14->SetPosition(glm::vec3(-16.75f, 1.5f, 0.0f));
		wall14->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall14->SetScale(glm::vec3(1.0f, 1.0f, 0.15f));

		RenderComponent::Sptr renderer = wall14->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall14->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall15 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 15");
	{
		wall15->SetPosition(glm::vec3(-16.75f, -9.0f, 0.0f));
		wall15->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall15->SetScale(glm::vec3(1.0f, 1.0f, 0.15f));

		RenderComponent::Sptr renderer = wall15->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall15->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall16 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 16");
	{
		wall16->SetPosition(glm::vec3(-23.5f, 0.5f, 0.0f));
		wall16->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall16->SetScale(glm::vec3(1.0f, 1.0f, 0.35f));

		RenderComponent::Sptr renderer = wall16->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall16->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall17 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 17");
	{
		wall17->SetPosition(glm::vec3(-27.5f, -4.5f, 0.0f));
		wall17->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall17->SetScale(glm::vec3(1.0f, 1.0f, 0.2f));

		RenderComponent::Sptr renderer = wall17->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall17->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall18 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 18");
	{
		wall18->SetPosition(glm::vec3(-37.5f, 0.0f, 0.0f));
		wall18->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall18->SetScale(glm::vec3(1.0f, 1.0f, 0.5f));

		RenderComponent::Sptr renderer = wall18->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall18->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall19 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 19");
	{
		wall19->SetPosition(glm::vec3(-28.5f, -8.0f, 0.0f));
		wall19->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall19->SetScale(glm::vec3(1.0f, 1.0f, 0.7f));

		RenderComponent::Sptr renderer = wall19->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall19->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall20 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 20");
	{
		wall20->SetPosition(glm::vec3(0, -31.5f, 0.0f));
		wall20->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall20->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

		RenderComponent::Sptr renderer = wall20->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall20->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall21 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 21");
	{
		wall21->SetPosition(glm::vec3(0, -31.5f, 0.0f));
		wall21->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall21->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

		RenderComponent::Sptr renderer = wall21->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall21->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall22 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 22");
	{
		wall22->SetPosition(glm::vec3(0, -31.5f, 0.0f));
		wall22->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall22->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

		RenderComponent::Sptr renderer = wall22->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall22->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall23 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 23");
	{
		wall23->SetPosition(glm::vec3(0, -31.5f, 0.0f));
		wall23->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall23->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

		RenderComponent::Sptr renderer = wall23->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall23->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall24 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 24");
	{
		wall24->SetPosition(glm::vec3(0, -31.5f, 0.0f));
		wall24->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall24->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

		RenderComponent::Sptr renderer = wall24->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall24->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);

	}

	GameObject::Sptr wall25 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 25");
	{
		wall25->SetPosition(glm::vec3(0, -31.5f, 0.0f));
		wall25->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall25->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

		RenderComponent::Sptr renderer = wall25->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall25->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall26 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 26");
	{
		wall26->SetPosition(glm::vec3(0, -31.5f, 0.0f));
		wall26->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall26->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

		RenderComponent::Sptr renderer = wall26->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall26->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall27 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 27");
	{
		wall27->SetPosition(glm::vec3(0, -31.5f, 0.0f));
		wall27->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall27->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

		RenderComponent::Sptr renderer = wall27->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall27->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall28 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 28");
	{
		wall28->SetPosition(glm::vec3(0, -31.5f, 0.0f));
		wall28->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall28->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

		RenderComponent::Sptr renderer = wall28->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall28->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall29 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 29");
	{
		wall29->SetPosition(glm::vec3(0, -31.5f, 0.0f));
		wall29->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall29->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

		RenderComponent::Sptr renderer = wall29->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall29->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall30 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 30");
	{
		wall30->SetPosition(glm::vec3(0, -31.5f, 0.0f));
		wall30->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall30->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

		RenderComponent::Sptr renderer = wall30->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall30->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall31 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 31");
	{
		wall31->SetPosition(glm::vec3(0, -31.5f, 0.0f));
		wall31->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall31->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

		RenderComponent::Sptr renderer = wall31->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall31->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall32 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 32");
	{
		wall32->SetPosition(glm::vec3(0, -31.5f, 0.0f));
		wall32->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall32->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

		RenderComponent::Sptr renderer = wall32->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall32->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall33 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 33");
	{
		wall33->SetPosition(glm::vec3(0, -31.5f, 0.0f));
		wall33->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall33->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

		RenderComponent::Sptr renderer = wall33->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall33->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall34 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 34");
	{
		wall34->SetPosition(glm::vec3(0, -31.5f, 0.0f));
		wall34->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall34->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

		RenderComponent::Sptr renderer = wall34->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall34->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall35 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 35");
	{
		wall35->SetPosition(glm::vec3(0, -31.5f, 0.0f));
		wall35->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall35->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

		RenderComponent::Sptr renderer = wall35->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall35->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall36 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 36");
	{
		wall36->SetPosition(glm::vec3(0, -31.5f, 0.0f));
		wall36->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall36->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

		RenderComponent::Sptr renderer = wall36->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall36->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall37 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 37");
	{
		wall37->SetPosition(glm::vec3(0, -31.5f, 0.0f));
		wall37->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall37->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

		RenderComponent::Sptr renderer = wall37->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall37->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall38 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 38");
	{
		wall38->SetPosition(glm::vec3(0, -31.5f, 0.0f));
		wall38->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall38->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

		RenderComponent::Sptr renderer = wall38->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall38->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall39 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 39");
	{
		wall39->SetPosition(glm::vec3(0, -31.5f, 0.0f));
		wall39->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall39->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

		RenderComponent::Sptr renderer = wall39->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall39->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall40 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 40");
	{
		wall40->SetPosition(glm::vec3(0, -31.5f, 0.0f));
		wall40->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall40->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

		RenderComponent::Sptr renderer = wall40->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		RigidBody::Sptr physics = wall40->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

#pragma endregion



	/////////////////////////////////////////////////////////
	//				   USER INTERFACE
	/////////////////////////////////////////////////////////

	GameObject::Sptr gameCanvas = SceneManager::GetCurrentScene()->CreateGameObject("Game Canvas");
	{
		RectTransform::Sptr transform = gameCanvas->Add<RectTransform>();
		transform->SetMin({ 0, 0 });
		transform->SetMax({ 1000, 1000 });
		transform->SetPosition({ 500, 500 });

		GuiPanel::Sptr canPanel = gameCanvas->Add<GuiPanel>();
		canPanel->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));

		GameObject::Sptr healthText = UIHelper::CreateText("Body Health: ???", "Body Health Text");
		healthText->Get<RectTransform>()->SetPosition({ 120, 50 });
		//SceneManager::GetCurrentScene()->PC.SetBodyHealthUI(*healthText->Get<GuiText>());
		gameCanvas->AddChild(healthText);

		GameObject::Sptr shadowText = UIHelper::CreateText("Shadow Health: ???", "Shadow Health Text");
		shadowText->Get<RectTransform>()->SetPosition({ 120, 100 });
		//SceneManager::GetCurrentScene()->PC.SetShadowHealthUI(*shadowText->Get<GuiText>());
		gameCanvas->AddChild(shadowText);

		GameObject::Sptr timerText = UIHelper::CreateText("Time: 0:00", "Game Timer Text");
		timerText->Get<RectTransform>()->SetPosition({ 500, 100 });
		gameCanvas->AddChild(timerText);

		SceneManager::GameInterface.SetGameUserInterface(*gameCanvas);
		SceneManager::GameInterface.InitializeGameUserInterface(*healthText->Get<GuiText>(), *shadowText->Get<GuiText>(), *timerText->Get<GuiText>());	
	}

	GameObject::Sptr loseCanvas = SceneManager::GetCurrentScene()->CreateGameObject("UI Menu Canvas");
	{
		RectTransform::Sptr transform = loseCanvas->Add<RectTransform>();
		transform->SetMin({ 16, 16 });
		transform->SetMax({ 900, 900 });
		transform->SetPosition({ 400, 400 });

		GuiPanel::Sptr backgroundPanel = loseCanvas->Add<GuiPanel>();
		backgroundPanel->SetColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

		GameObject::Sptr menuTitle = UIHelper::CreateText("You Died!");
		menuTitle->Get<GuiText>()->SetTextScale(2);
		menuTitle->Get<RectTransform>()->SetPosition({ 800.0f, 700 });
		loseCanvas->AddChild(menuTitle);

		loseCanvas->IsActive = false;
		//SceneManager::GetCurrentScene()->PC.SetLoseCanvas(*loseCanvas);

		SceneManager::GameInterface.SetLosePanel(*loseCanvas);
	}

	GameObject::Sptr pauseMenu = SceneManager::GetCurrentScene()->CreateGameObject("UI Menu Canvas");
	{
		RectTransform::Sptr transform = pauseMenu->Add<RectTransform>();
		transform->SetMin({ 16, 16 });
		transform->SetMax({ 350, 500 });
		transform->SetPosition({ 400, 500 });

		GuiPanel::Sptr backgroundPanel = pauseMenu->Add<GuiPanel>();
		backgroundPanel->SetColor(glm::vec4(0.3f, 0.3f, 0.3f, 0.5f));


		GameObject::Sptr upperGraphic = UIHelper::CreateImage(Resources::GetTexture("Menu Gloss"), "Upper Graphic");
		upperGraphic->Get<RectTransform>()->SetPosition({ transform->GetPosition().x / 2.5f, 70 });
		upperGraphic->Get<RectTransform>()->SetSize({ 80, 30 });
		upperGraphic->Get<GuiPanel>()->SetBorderRadius(0);
		pauseMenu->AddChild(upperGraphic);

		GameObject::Sptr menuTitle = UIHelper::CreateText("Paused");
		menuTitle->Get<GuiText>()->SetTextScale(2);
		menuTitle->Get<RectTransform>()->SetPosition({ 280.0f, 175 });
		pauseMenu->AddChild(menuTitle);

		GameObject::Sptr button1 = UIHelper::CreateButton("Resume Game");
		button1->Get<RectTransform>()->SetPosition({ transform->GetPosition().x / 2.5f, 150 });
		pauseMenu->AddChild(button1);

		GameObject::Sptr button2 = UIHelper::CreateButton("Restart Level");
		button2->Get<RectTransform>()->SetPosition({ transform->GetPosition().x / 2.5f, 200 });
		pauseMenu->AddChild(button2);

		GameObject::Sptr button3 = UIHelper::CreateButton("Options");
		button3->Get<RectTransform>()->SetPosition({ transform->GetPosition().x / 2.5f, 250 });
		pauseMenu->AddChild(button3);

		GameObject::Sptr button4 = UIHelper::CreateButton("Quit to Menu");
		button4->Get<RectTransform>()->SetPosition({ transform->GetPosition().x / 2.5f, 300 });
		pauseMenu->AddChild(button4);

		GameObject::Sptr button5 = UIHelper::CreateButton("Quit Game");
		button5->Get<RectTransform>()->SetPosition({ transform->GetPosition().x / 2.5f, 350 });
		pauseMenu->AddChild(button5);

		GameObject::Sptr lowerGraphic = UIHelper::CreateImage(Resources::GetTexture("Menu Gloss Reverse"), "Lower Graphic");
		lowerGraphic->Get<RectTransform>()->SetPosition({ transform->GetPosition().x / 2.5f, 400 });
		lowerGraphic->Get<RectTransform>()->SetSize({ 40, 17.5f });
		lowerGraphic->Get<GuiPanel>()->SetBorderRadius(0);
		pauseMenu->AddChild(lowerGraphic);

		pauseMenu->IsActive = false;
		//SceneManager::GetCurrentScene()->PC.SetPauseMenu(*pauseMenu);
		SceneManager::GameInterface.SetPausePanel(*pauseMenu);
	}

	GuiBatcher::SetDefaultTexture(ResourceManager::CreateAsset<Texture2D>("textures/ui-sprite.png"));
	GuiBatcher::SetDefaultBorderRadius(8);

	/////////////////////////////////////////////////////////

	// Call SceneManager::GetCurrentScene() awake to start up all of our components
	SceneManager::GetCurrentScene()->Window = window;
	SceneManager::GetCurrentScene()->Awake();

	// Save the asset manifest for all the resources we just loaded
	ResourceManager::SaveManifest("manifest.json");
	// Save the SceneManager::GetCurrentScene() to a JSON file
	SceneManager::GetCurrentScene()->Save("Level_One.json");

	return nullptr;
}