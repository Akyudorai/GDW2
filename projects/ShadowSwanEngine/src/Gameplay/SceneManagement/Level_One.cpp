#include "Level_One.h"
#include "SceneManager.h"

// Gameplay
#include "Gameplay/MeshResource.h"
#include "Gameplay/GameManager.h"

// Components
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/AnimatorComponent.h"
#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Components/InteractableComponent.h"
#include "Gameplay/Components/Enemy.h"
#include "Gameplay/Components/MovingPlatformBehavior.h"
#include "Gameplay/Components/SpikeTrapBehavior.h"
#include "Gameplay/Components/TurretBehavior.h"

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

using namespace Gameplay;
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

	SceneManager::GetCurrentScene()->Lights[1].Position = glm::vec3(0.0f, 0.0f, 50.0f);
	SceneManager::GetCurrentScene()->Lights[1].Color = glm::vec3(1.0f, 1.0f, 1.0f);
	SceneManager::GetCurrentScene()->Lights[1].Range = 2000.0f;

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
		body->SetPosition(glm::vec3(0.0f, -40.0f, 3.0f));
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

	//// Set up all our sample objects
	//GameObject::Sptr floor = SceneManager::GetCurrentScene()->CreateGameObject("Floor");
	//{
	//	MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
	//	tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(100.0f), glm::vec2(8.0f)));
	//	tiledMesh->GenerateMesh();

	//	// Create and attach a RenderComponent to the object to draw our mesh
	//	RenderComponent::Sptr renderer = floor->Add<RenderComponent>();
	//	renderer->SetMesh(tiledMesh);
	//	renderer->SetMaterial(Resources::GetMaterial("Stone Slab"));

	//	RigidBody::Sptr physics = floor->Add<RigidBody>(/*static by default*/);
	//	physics->AddCollider(BoxCollider::Create(glm::vec3(100.0f, 100.0f, 1.5f)))->SetPosition({ 0,0,-1 });
	//	physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
	//	physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	//}

#pragma region Floors
	GameObject::Sptr block = SceneManager::GetCurrentScene()->CreateGameObject("Block 1");
	{
		block->SetPosition(glm::vec3(0.0f, -35.0f, 2.5f));
		block->SetScale(glm::vec3(17.5f, 15.0f, 3.0f));

		RenderComponent::Sptr renderer = block->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block2 = SceneManager::GetCurrentScene()->CreateGameObject("Block 2");
	{
		block2->SetPosition(glm::vec3(32.5f, -42.5f, 0.0f));
		block2->SetScale(glm::vec3(15.0f, 5.0f, 1.0f));

		RenderComponent::Sptr renderer = block2->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block2->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block2->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block3 = SceneManager::GetCurrentScene()->CreateGameObject("Block 3");
	{
		block3->SetPosition(glm::vec3(26.5f, -40.0f, 3.5f));
		block3->SetScale(glm::vec3(4.0f, 2.5f, 2.5f));

		RenderComponent::Sptr renderer = block3->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block3->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block3->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block4 = SceneManager::GetCurrentScene()->CreateGameObject("Block 4");
	{
		block4->SetPosition(glm::vec3(40.0f, -40.0f, 3.5f));
		block4->SetScale(glm::vec3(4.0f, 2.5f, 2.5f));

		RenderComponent::Sptr renderer = block4->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block4->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block4->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block5 = SceneManager::GetCurrentScene()->CreateGameObject("Block 5");
	{
		block5->SetPosition(glm::vec3(58.0f, -37.0f, 2.5f));
		block5->SetScale(glm::vec3(10.0f, 12.5f, 3.0f));

		RenderComponent::Sptr renderer = block5->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block5->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block5->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block6 = SceneManager::GetCurrentScene()->CreateGameObject("Block 6");
	{
		block6->SetPosition(glm::vec3(55.5f, -21.5f, 2.5f));
		block6->SetScale(glm::vec3(2.5f, 3.0f, 3.0f));

		RenderComponent::Sptr renderer = block6->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block6->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block6->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block8 = SceneManager::GetCurrentScene()->CreateGameObject("Block 8");
	{
		block8->SetPosition(glm::vec3(-12.5f, -8.5f, 3.5f));
		block8->SetScale(glm::vec3(25.0f, 4.0f, 2.0f));

		RenderComponent::Sptr renderer = block8->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block8->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block8->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block9 = SceneManager::GetCurrentScene()->CreateGameObject("Block 9");
	{
		block9->SetPosition(glm::vec3(-47.5f, -7.5f, 3.5f));
		block9->SetScale(glm::vec3(10.0f, 17.5f, 2.0f));

		RenderComponent::Sptr renderer = block9->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block9->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block9->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block10 = SceneManager::GetCurrentScene()->CreateGameObject("Block 10");
	{
		block10->SetPosition(glm::vec3(-24.5f, -32.0f, 7.5f));
		block10->SetScale(glm::vec3(7.0f, 8.0f, 8.0f));

		RenderComponent::Sptr renderer = block10->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block10->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block10->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block11 = SceneManager::GetCurrentScene()->CreateGameObject("Block 11");
	{
		block11->SetPosition(glm::vec3(-24.5f, -45.0f, 2.0f));
		block11->SetScale(glm::vec3(7.0f, 5.0f, 3.0f));

		RenderComponent::Sptr renderer = block11->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block11->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block11->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block12 = SceneManager::GetCurrentScene()->CreateGameObject("Block 12");
	{
		block12->SetPosition(glm::vec3(-40.75f, -32.5f, 7.5f));
		block12->SetScale(glm::vec3(9.25f, 2.5f, 8.0f));

		RenderComponent::Sptr renderer = block12->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block12->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block12->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block13 = SceneManager::GetCurrentScene()->CreateGameObject("Block 13");
	{
		block13->SetPosition(glm::vec3(-47.5f, -27.5f, 7.5f));
		block13->SetScale(glm::vec3(2.5f, 2.5f, 8.0f));

		RenderComponent::Sptr renderer = block13->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block13->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block13->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block14 = SceneManager::GetCurrentScene()->CreateGameObject("Block 14");
	{
		block14->SetPosition(glm::vec3(-40.5f, 6.0f, 5.5f));
		block14->SetScale(glm::vec3(2.0f, 2.0f, 2.0f));

		RenderComponent::Sptr renderer = block14->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block14->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block14->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block15 = SceneManager::GetCurrentScene()->CreateGameObject("Block 15");
	{
		block15->SetPosition(glm::vec3(-47.0f, 6.0f, 6.0f));
		block15->SetScale(glm::vec3(4.25f, 2.0f, 3.5f));

		RenderComponent::Sptr renderer = block15->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block15->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block15->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block16 = SceneManager::GetCurrentScene()->CreateGameObject("Block 16");
	{
		block16->SetPosition(glm::vec3(-54.5f, -2.0f, 6.0f));
		block16->SetScale(glm::vec3(2.0f, 4.25f, 4.5f));

		RenderComponent::Sptr renderer = block16->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block16->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block16->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block17 = SceneManager::GetCurrentScene()->CreateGameObject("Block 17");
	{
		block17->SetPosition(glm::vec3(-54.5f, -14.0f, 6.0f));
		block17->SetScale(glm::vec3(2.0f, 2.0f, 4.5f));

		RenderComponent::Sptr renderer = block17->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block17->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block17->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block18 = SceneManager::GetCurrentScene()->CreateGameObject("Block 18");
	{
		block18->SetPosition(glm::vec3(-46.0f, -15.0f, 12.25f));
		block18->SetRotation(glm::vec3(0.0f, -11.0f, 0.0f));
		block18->SetScale(glm::vec3(7.0f, 0.5f, 0.2f));

		RenderComponent::Sptr renderer = block18->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block18->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block18->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block19 = SceneManager::GetCurrentScene()->CreateGameObject("Block 19");
	{
		block19->SetPosition(glm::vec3(-40.5f, -21.5f, 9.5f));
		block19->SetScale(glm::vec3(2.0f, 2.0f, 4.5f));

		RenderComponent::Sptr renderer = block19->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block19->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block19->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block30 = SceneManager::GetCurrentScene()->CreateGameObject("Block 30");
	{
		block30->SetPosition(glm::vec3(-54.0f, -21.5f, 9.5f));
		block30->SetScale(glm::vec3(2.0f, 2.0f, 4.5f));

		RenderComponent::Sptr renderer = block30->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block30->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block19->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block31 = SceneManager::GetCurrentScene()->CreateGameObject("Block 31");
	{
		block31->SetPosition(glm::vec3(-54.0f, 7.0f, 9.5f));
		block31->SetScale(glm::vec3(2.0f, 2.0f, 4.5f));

		RenderComponent::Sptr renderer = block31->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block31->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block19->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}
	

	GameObject::Sptr block20 = SceneManager::GetCurrentScene()->CreateGameObject("Block 20");
	{
		block20->SetPosition(glm::vec3(-39.0f, -15.0f, 12.5f));
		block20->SetScale(glm::vec3(1.25f, 2.5f, 1.0f));

		RenderComponent::Sptr renderer = block20->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block20->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block20->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block21 = SceneManager::GetCurrentScene()->CreateGameObject("Block 21");
	{
		block21->SetPosition(glm::vec3(-47.5f, -24.0f, 14.5f));
		block21->SetScale(glm::vec3(2.5f, 1.0f, 1.0f));

		RenderComponent::Sptr renderer = block21->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block21->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block21->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block22 = SceneManager::GetCurrentScene()->CreateGameObject("Block 22");
	{
		block22->SetPosition(glm::vec3(-25.0f, -6.5f, 14.5f));
		block22->SetScale(glm::vec3(4.0f, 17.5f, 1.0f));

		RenderComponent::Sptr renderer = block22->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block22->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block22->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block23 = SceneManager::GetCurrentScene()->CreateGameObject("Block 23");
	{
		block23->SetPosition(glm::vec3(-25.0f, 21.0f, 14.5f));
		block23->SetScale(glm::vec3(12.0f, 10.0f, 1.0f));

		RenderComponent::Sptr renderer = block23->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block23->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block23->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block24 = SceneManager::GetCurrentScene()->CreateGameObject("Block 24");
	{
		block24->SetPosition(glm::vec3(-56.0f, 21.0f, 12.5f));
		block24->SetScale(glm::vec3(12.0f, 7.0f, 1.0f));

		RenderComponent::Sptr renderer = block24->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block24->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block24->GetScale()); 
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block25 = SceneManager::GetCurrentScene()->CreateGameObject("Block 25");
	{
		block25->SetPosition(glm::vec3(-41.5f, 21.0f, 14.0f));
		block25->SetScale(glm::vec3(2.5f, 7.0f, 1.5f));

		RenderComponent::Sptr renderer = block25->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block25->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block25->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block26 = SceneManager::GetCurrentScene()->CreateGameObject("Block 26");
	{
		block26->SetPosition(glm::vec3(-70.5f, 21.0f, 14.0f));
		block26->SetScale(glm::vec3(2.5f, 7.0f, 1.5f));

		RenderComponent::Sptr renderer = block26->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block26->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block26->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block27 = SceneManager::GetCurrentScene()->CreateGameObject("Block 27");
	{
		block27->SetPosition(glm::vec3(-38.0f, 21.0f, 14.5f));
		block27->SetScale(glm::vec3(1.0f, 2.5f, 1.0f));

		RenderComponent::Sptr renderer = block27->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block27->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block27->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block28 = SceneManager::GetCurrentScene()->CreateGameObject("Block 28");
	{
		block28->SetPosition(glm::vec3(37.0f, 21.0f, 8.5f));
		block28->SetScale(glm::vec3(12.0f, 10.0f, 1.0f));

		RenderComponent::Sptr renderer = block28->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block28->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block28->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block29 = SceneManager::GetCurrentScene()->CreateGameObject("Block 29");
	{
		block29->SetPosition(glm::vec3(5.75f, 20.5f, 11.5f));
		block29->SetRotation(glm::vec3(0.0f, 9.0f, 0.0f));
		block29->SetScale(glm::vec3(19.5f, 5.0f, 1.0f));

		RenderComponent::Sptr renderer = block29->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = block29->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block29->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

#pragma endregion
	 
#pragma region WALLS

	GameObject::Sptr wall1 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 1");
	{
		wall1->SetPosition(glm::vec3(0, -20.0f, 4.0f));
		wall1->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall1->SetScale(glm::vec3(1.0f, 1.0f, 1.25f));

		RenderComponent::Sptr renderer = wall1->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall2 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 2");
	{
		wall2->SetPosition(glm::vec3(18.0f, -28.5f, 4.0f));
		wall2->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall2->SetScale(glm::vec3(1.0f, 1.0f, 0.6f));

		RenderComponent::Sptr renderer = wall2->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall3 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 3");
	{
		wall3->SetPosition(glm::vec3(9.5f, -31.5f, 4.0f));
		wall3->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall3->SetScale(glm::vec3(1.0f, 1.0f, 0.5f));

		RenderComponent::Sptr renderer = wall3->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall4 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 4");
	{
		wall4->SetPosition(glm::vec3(-10.5f, -31.5f, 4.0f));
		wall4->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall4->SetScale(glm::vec3(1.0f, 1.0f, 0.5f));

		RenderComponent::Sptr renderer = wall4->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall5 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 5");
	{
		wall5->SetPosition(glm::vec3(-16.5f, -45.75f, 16.0f));
		wall5->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall5->SetScale(glm::vec3(1.0f, 0.6f, 0.3f));

		RenderComponent::Sptr renderer = wall5->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall6 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 6");
	{
		wall6->SetPosition(glm::vec3(-16.5f, -49.375f, 4.0f));
		wall6->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall6->SetScale(glm::vec3(1.0f, 1.0f, 0.125f));

		RenderComponent::Sptr renderer = wall6->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall7 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 7");
	{
		wall7->SetPosition(glm::vec3(-9.95f, -50.0f, 4.0f));
		wall7->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall7->SetScale(glm::vec3(1.0f, 1.0f, 0.35f));

		RenderComponent::Sptr renderer = wall7->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall8 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 8");
	{
		wall8->SetPosition(glm::vec3(10.0f, -50.0f, 4.0f));
		wall8->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall8->SetScale(glm::vec3(1.0f, 1.0f, 0.5f));

		RenderComponent::Sptr renderer = wall8->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall9 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 9");
	{
		wall9->SetPosition(glm::vec3(36.25f, -19.75f, 4.0f));
		wall9->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall9->SetScale(glm::vec3(1.0f, 1.0f, 1.1f));

		RenderComponent::Sptr renderer = wall9->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall10 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 10");
	{
		wall10->SetPosition(glm::vec3(18.0f, -49.75f, 4.0f));
		wall10->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall10->SetScale(glm::vec3(1.0f, 1.0f, 0.125f));

		RenderComponent::Sptr renderer = wall10->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall11 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 11");
	{
		wall11->SetPosition(glm::vec3(43.5f, -49.0f, 4.0f));
		wall11->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall11->SetScale(glm::vec3(1.0f, 1.0f, 1.6f));

		RenderComponent::Sptr renderer = wall11->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall12 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 12");
	{
		wall12->SetPosition(glm::vec3(69.0f, -37.0f, 4.0f));
		wall12->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall12->SetScale(glm::vec3(1.0f, 1.0f, 0.85f));

		RenderComponent::Sptr renderer = wall12->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall13 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 13");
	{
		wall13->SetPosition(glm::vec3(64.0f, -23.0f, 4.0f));
		wall13->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall13->SetScale(glm::vec3(1.0f, 1.0f, 0.4f));

		RenderComponent::Sptr renderer = wall13->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall14 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 14");
	{
		wall14->SetPosition(glm::vec3(47.5f, -30.0f, 4.0f));
		wall14->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall14->SetScale(glm::vec3(1.0f, 1.0f, 0.35f));

		RenderComponent::Sptr renderer = wall14->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall15 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 15");
	{
		wall15->SetPosition(glm::vec3(49.5f, -23.0f, 4.0f));
		wall15->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall15->SetScale(glm::vec3(1.0f, 1.0f, 0.225f));

		RenderComponent::Sptr renderer = wall15->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall16 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 16");
	{
		wall16->SetPosition(glm::vec3(37.5f, 3.0f, 4));
		wall16->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall16->SetScale(glm::vec3(1.0f, 1.0f, 1.65f));

		RenderComponent::Sptr renderer = wall16->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall17 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 17");
	{
		wall17->SetPosition(glm::vec3(33.0f, -36.25f, 4.0f));
		wall17->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall17->SetScale(glm::vec3(1.0f, 1.0f, 1.02f));

		RenderComponent::Sptr renderer = wall17->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall18 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 18");
	{
		wall18->SetPosition(glm::vec3(64.0f, -7.0f, 4.0f));
		wall18->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall18->SetScale(glm::vec3(1.0f, 1.0f, 0.75f));

		RenderComponent::Sptr renderer = wall18->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall19 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 19");
	{
		wall19->SetPosition(glm::vec3(61.5f, -20.25f, 4.0f));
		wall19->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall19->SetScale(glm::vec3(1.0f, 1.0f, 0.25f));

		RenderComponent::Sptr renderer = wall19->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall20 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 20");
	{
		wall20->SetPosition(glm::vec3(11.0f, 0.0f, 4.0f));
		wall20->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall20->SetScale(glm::vec3(1.0f, 1.0f, 0.3f));

		RenderComponent::Sptr renderer = wall20->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall21 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 21");
	{
		wall21->SetPosition(glm::vec3(11, -15.5f, 4.0f));
		wall21->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall21->SetScale(glm::vec3(1.0f, 1.0f, 0.25f));

		RenderComponent::Sptr renderer = wall21->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall22 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 22");
	{
		wall22->SetPosition(glm::vec3(-13.75f, -3.5f, 4.0f));
		wall22->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall22->SetScale(glm::vec3(1.0f, 1.0f, 1.55f));

		RenderComponent::Sptr renderer = wall22->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall23 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 23");
	{
		wall23->SetPosition(glm::vec3(-13.75f, -13.00f, 4.0f));
		wall23->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall23->SetScale(glm::vec3(1.0f, 1.0f, 1.55f));

		RenderComponent::Sptr renderer = wall23->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall24 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 24");
	{
		wall24->SetPosition(glm::vec3(-46.5f, 10.5f, 9.75f));
		wall24->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall24->SetScale(glm::vec3(1.0f, 1.35f, 0.75f));

		RenderComponent::Sptr renderer = wall24->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall25 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 25");
	{
		wall25->SetPosition(glm::vec3(-36.25f, 2.75f, 9.75f));
		wall25->SetRotation(glm::vec3(90.f, 0.0f, 00.0f));
		wall25->SetScale(glm::vec3(1.0f, 1.35f, 0.5f));

		RenderComponent::Sptr renderer = wall25->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall26 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 26");
	{
		wall26->SetPosition(glm::vec3(-46.5f, -24.5f, 7.0f));
		wall26->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall26->SetScale(glm::vec3(1.0f, 1.0f, 0.75f));

		RenderComponent::Sptr renderer = wall26->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall27 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 27");
	{
		wall27->SetPosition(glm::vec3(-40, -24.5f, 16.75f));
		wall27->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall27->SetScale(glm::vec3(1.0f, 0.5f, 0.325f));

		RenderComponent::Sptr renderer = wall27->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall28 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 28");
	{
		wall28->SetPosition(glm::vec3(-54, -24.5f, 16.75f));
		wall28->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall28->SetScale(glm::vec3(1.0f, 0.5f, 0.275f));

		RenderComponent::Sptr renderer = wall28->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall29 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 29");
	{
		wall29->SetPosition(glm::vec3(-58, -6.76f, 9.75f));
		wall29->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall29->SetScale(glm::vec3(1.0f, 1.35f, 1.25f));

		RenderComponent::Sptr renderer = wall29->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall30 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 30");
	{
		wall30->SetPosition(glm::vec3(-36.5f, -18.5f, 9.75f));
		wall30->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall30->SetScale(glm::vec3(1.0f, 1.35f, 0.4f));

		RenderComponent::Sptr renderer = wall30->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall31 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 31");
	{
		wall31->SetPosition(glm::vec3(-51.25f, -31.5f, 12.5f));
		wall31->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall31->SetScale(glm::vec3(1.0f, 1.0f, 0.4f));

		RenderComponent::Sptr renderer = wall31->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall32 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 32");
	{
		wall32->SetPosition(glm::vec3(-41, -36.5f, 12.5f));
		wall32->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall32->SetScale(glm::vec3(1.0f, 1.0f, 0.65f));

		RenderComponent::Sptr renderer = wall32->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall33 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 33");
	{
		wall33->SetPosition(glm::vec3(-43.5f, -27.75f, 12.5f));
		wall33->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall33->SetScale(glm::vec3(1.0f, 1.0f, 0.15f));

		RenderComponent::Sptr renderer = wall33->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall34 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 34");
	{
		wall34->SetPosition(glm::vec3(-36.75f, -28.75f, 12.5f));
		wall34->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall34->SetScale(glm::vec3(1.0f, 1.0f, 0.35f));

		RenderComponent::Sptr renderer = wall34->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall35 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 35");
	{
		wall35->SetPosition(glm::vec3(-32.54, -44.84f, 9.75f));
		wall35->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall35->SetScale(glm::vec3(1.0f, 1.35f, 0.5f));

		RenderComponent::Sptr renderer = wall35->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall36 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 36");
	{
		wall36->SetPosition(glm::vec3(-24, -51, 9.75f));
		wall36->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall36->SetScale(glm::vec3(1.0f, 1.35f, 0.5f));

		RenderComponent::Sptr renderer = wall36->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall37 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 37");
	{
		wall37->SetPosition(glm::vec3(-16.46, -31.33f, 9.75f));
		wall37->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall37->SetScale(glm::vec3(1.0f, 1.35f, 0.65f));

		RenderComponent::Sptr renderer = wall37->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall38 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 38");
	{
		wall38->SetPosition(glm::vec3(-32.75f, -26, 12.5f));
		wall38->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall38->SetScale(glm::vec3(1.0f, 1.0f, 0.15f));

		RenderComponent::Sptr renderer = wall38->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall39 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 39");
	{
		wall39->SetPosition(glm::vec3(-31.5f, -23, 12.5f));
		wall39->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall39->SetScale(glm::vec3(1.0f, 1.0f, 0.175f));

		RenderComponent::Sptr renderer = wall39->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall40 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 40");
	{
		wall40->SetPosition(glm::vec3(-19.5f, -23, 12.5f));
		wall40->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall40->SetScale(glm::vec3(1.0f, 1.0f, 0.125f));

		RenderComponent::Sptr renderer = wall40->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall41 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 41");
	{
		wall41->SetPosition(glm::vec3(-30.25f,-5.25f, 12.5f));
		wall41->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall41->SetScale(glm::vec3(1.0f, 1.0f, 1.1f));

		RenderComponent::Sptr renderer = wall41->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall42 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 42");
	{
		wall42->SetPosition(glm::vec3(-34.5f, 10.25f, 12.5f));
		wall42->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall42->SetScale(glm::vec3(1.0f, 1.0f, 0.2f));

		RenderComponent::Sptr renderer = wall42->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall43 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 43");
	{
		wall43->SetPosition(glm::vec3(-38.0f, 14.0f, 12.5f));
		wall43->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall43->SetScale(glm::vec3(1.0f, 1.0f, 0.3f));

		RenderComponent::Sptr renderer = wall43->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall44 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 44");
	{
		wall44->SetPosition(glm::vec3(-38, 27.25f, 12.5f));
		wall44->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall44->SetScale(glm::vec3(1.0f, 1.0f, 0.275f));

		RenderComponent::Sptr renderer = wall44->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall45 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 45");
	{
		wall45->SetPosition(glm::vec3(-25.0f, 32.0f, 12.5f));
		wall45->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall45->SetScale(glm::vec3(1.0f, 1.0f, 0.95f));

		RenderComponent::Sptr renderer = wall45->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall46 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 46");
	{
		wall46->SetPosition(glm::vec3(-11.75f, 28.0f, 12.5f));
		wall46->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall46->SetScale(glm::vec3(1.0f, 1.0f, 0.125f));

		RenderComponent::Sptr renderer = wall46->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall47 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 47");
	{
		wall47->SetPosition(glm::vec3(-11.75f, 13.5f, 12.5f));
		wall47->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall47->SetScale(glm::vec3(1.0f, 1.0f, 0.15f));

		RenderComponent::Sptr renderer = wall47->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall48 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 48");
	{
		wall48->SetPosition(glm::vec3(-15.7f, 10.28f, 12.5f));
		wall48->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall48->SetScale(glm::vec3(1.0f, 1.0f, 0.35f));

		RenderComponent::Sptr renderer = wall48->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall49 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 49");
	{
		wall49->SetPosition(glm::vec3(-20.25f, -5.25f, 12.5f));
		wall49->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall49->SetScale(glm::vec3(1.0f, 1.0f, 1.1f));

		RenderComponent::Sptr renderer = wall49->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall50 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 50");
	{
		wall50->SetPosition(glm::vec3(-19.5f, -23.0f, 12.5f));
		wall50->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall50->SetScale(glm::vec3(1.0f, 1.0f, 0.125f));

		RenderComponent::Sptr renderer = wall50->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall51 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 51");
	{
		wall51->SetPosition(glm::vec3(-56.25f, 12.3f, 12.5f));
		wall51->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall51->SetScale(glm::vec3(1.0f, 1.0f, 1.1f));

		RenderComponent::Sptr renderer = wall51->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall52 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 52");
	{
		wall52->SetPosition(glm::vec3(-74.0f, 20.75f, 12.5f));
		wall52->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall52->SetScale(glm::vec3(1.0f, 1.0f, 0.625f));

		RenderComponent::Sptr renderer = wall52->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall53 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 53");
	{
		wall53->SetPosition(glm::vec3(8.25f, 26.75f, 12.5f));
		wall53->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall53->SetScale(glm::vec3(1.0f, 1.0f, 1.25f));

		RenderComponent::Sptr renderer = wall53->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

	}

	GameObject::Sptr wall54 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 54");
	{
		wall54->SetPosition(glm::vec3(8.25f, 14.35f, 12.5f));
		wall54->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall54->SetScale(glm::vec3(1.0f, 1.0f, 1.25f));

		RenderComponent::Sptr renderer = wall54->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall55 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 55");
	{
		wall55->SetPosition(glm::vec3(26.5f, 14.0f, 12.5f));
		wall55->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall55->SetScale(glm::vec3(1.0f, 1.0f, 0.2f));

		RenderComponent::Sptr renderer = wall55->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall56 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 56");
	{
		wall56->SetPosition(glm::vec3(26.5f, 27.5f, 12.5f));
		wall56->SetRotation(glm::vec3(90.f, 0.0f, 00.0f));
		wall56->SetScale(glm::vec3(1.0f, 1.0f, 0.225f));

		RenderComponent::Sptr renderer = wall56->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall57 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 57");
	{
		wall57->SetPosition(glm::vec3(39.25f, 12.0f, 12.5f));
		wall57->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall57->SetScale(glm::vec3(1.0f, 1.0f, 0.785f));

		RenderComponent::Sptr renderer = wall57->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall58 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 58");
	{
		wall58->SetPosition(glm::vec3(38.0f, 32.0f, 12.5f));
		wall58->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall58->SetScale(glm::vec3(1.0f, 1.0f, 0.85f));

		RenderComponent::Sptr renderer = wall58->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall60 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 60");
	{
		wall60->SetPosition(glm::vec3(50.5f, 21.5f, 12.5f));
		wall60->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall60->SetScale(glm::vec3(1.0f, 1.0f, 0.8f));

		RenderComponent::Sptr renderer = wall60->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall61 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 61");
	{
		wall61->SetPosition(glm::vec3(-56.25f, 29.5f, 12.5f));
		wall61->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall61->SetScale(glm::vec3(1.0f, 1.0f, 1.1f));

		RenderComponent::Sptr renderer = wall61->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall59 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 59");
	{
		wall59->SetPosition(glm::vec3(33.0f, -36.25f, 4.5f));
		wall59->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall59->SetScale(glm::vec3(1.0f, 1.0f, 1.02f));

		RenderComponent::Sptr renderer = wall59->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}
	

#pragma endregion

#pragma region Interactables

	// Push Tutorial Room
	{
		// Intro To Pushing Mechanic
		GameObject::Sptr cageDoor_1 = SceneManager::GetCurrentScene()->CreateGameObject("Snake Room Door");
		{
			// Transform
			cageDoor_1->SetPosition(glm::vec3(55.5f, -23.0f, 4.5f));
			cageDoor_1->SetRotation(glm::vec3(90, 0, 90));
			cageDoor_1->SetScale(glm::vec3(0.25f, 0.125f, 0.065f));

			// Renderer
			RenderComponent::Sptr renderer = cageDoor_1->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Small Cage"));
			renderer->SetMaterial(Resources::GetMaterial("Small Cage"));

			// Collider
			RigidBody::Sptr physics = cageDoor_1->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 2.0f, 0.0f));
			collider->SetScale(glm::vec3(1.0f, 4.0f, 3.5f));
			physics->AddCollider(collider);
			physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
			physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
		}

		// Intro To Pushing Mechanic 
		GameObject::Sptr pressure_plate_1 = SceneManager::GetCurrentScene()->CreateGameObject("Snake Room Plate");
		{
			// Transform
			pressure_plate_1->SetPosition(glm::vec3(62.5f, -29.0f, 6.0f));
			pressure_plate_1->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			pressure_plate_1->SetScale(glm::vec3(1.0f, 0.5f, 1.0f));

			// Renderer
			RenderComponent::Sptr renderer = pressure_plate_1->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Pressure Plate"));
			renderer->SetMaterial(Resources::GetMaterial("Pressure Plate"));

			// Trigger Volume
			TriggerVolume::Sptr volume = pressure_plate_1->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
			collider->SetScale(glm::vec3(2.0f, 1.0f, 2.0f));
			volume->AddCollider(collider);
			volume->SetCollisionGroup(PHYSICAL_GROUP);
			volume->SetCollisionMask(PHYSICAL_MASK);

			// Trigger Event
			TriggerVolumeEnterBehaviour::Sptr trigger = pressure_plate_1->Add<TriggerVolumeEnterBehaviour>();
			trigger->onTriggerEnterEvent = [cageDoor_1, NO_GROUP, NO_MASK]
			{					
				// Open The Door
				cageDoor_1->Get<RenderComponent>()->IsEnabled = false;
				RigidBody::Sptr rigid = cageDoor_1->Get<RigidBody>();
				rigid->SetCollisionGroup(NO_GROUP);
				rigid->SetCollisionMask(NO_MASK);			
			};

			trigger->onTriggerExitEvent = [cageDoor_1, PHYSICAL_GROUP, SHADOW_GROUP, PHYSICAL_MASK, SHADOW_MASK]
			{
				// Close The Door
				cageDoor_1->Get<RenderComponent>()->IsEnabled = true;
				RigidBody::Sptr rigid = cageDoor_1->Get<RigidBody>();				
				rigid->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				rigid->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);				
			};
		}

		GameObject::Sptr crate_1 = SceneManager::GetCurrentScene()->CreateGameObject("Crate 1");
		{
			crate_1->SetPosition(glm::vec3(62.5f, -42.0f, 7.5f));
			crate_1->SetRotation(glm::vec3(0.f, 0.0f, 0.0f));
			crate_1->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

			RenderComponent::Sptr renderer = crate_1->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Cube"));
			renderer->SetMaterial(Resources::GetMaterial("Gray"));

			// Collider
			RigidBody::Sptr physics = crate_1->Add<RigidBody>(RigidBodyType::Dynamic);
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetScale(crate_1->GetScale());
			physics->AddCollider(collider);
			physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
			physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
		}
	}

	// Trap Room
	{
		// Start
		{
			GameObject::Sptr trap_ground_upper_1 = SceneManager::GetCurrentScene()->CreateGameObject("Trap Room Upper Ground 1");
			{
				trap_ground_upper_1->SetPosition(glm::vec3(57.5f, -8.5f, 3.5f));
				trap_ground_upper_1->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
				trap_ground_upper_1->SetScale(glm::vec3(5, 10, 2));

				RenderComponent::Sptr renderer = trap_ground_upper_1->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Cube"));
				renderer->SetMaterial(Resources::GetMaterial("Brown"));

				// Collider
				RigidBody::Sptr physics = trap_ground_upper_1->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(trap_ground_upper_1->GetScale());
				collider->SetPosition(collider->GetPosition());
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
			}

			GameObject::Sptr trap_ground_lower = SceneManager::GetCurrentScene()->CreateGameObject("Trap Room Lower Ground");
			{
				trap_ground_lower->SetPosition(glm::vec3(37.5f, -8.5f, 0.0f));
				trap_ground_lower->SetRotation(glm::vec3(0.f, 0.0f, 0.0f));
				trap_ground_lower->SetScale(glm::vec3(25.0f, 10.0f, 2.0f));

				RenderComponent::Sptr renderer = trap_ground_lower->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Cube"));
				renderer->SetMaterial(Resources::GetMaterial("Brown"));

				// Collider
				RigidBody::Sptr physics = trap_ground_lower->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(trap_ground_lower->GetScale());
				collider->SetPosition(collider->GetPosition());
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
			}

			GameObject::Sptr trap_ramp = SceneManager::GetCurrentScene()->CreateGameObject("Trap Room Ramp");
			{
				trap_ramp->SetPosition(glm::vec3(51.0f, 0.5f, 3.0f));
				trap_ramp->SetRotation(glm::vec3(0.0f, -38.0f, 0.0f));
				trap_ramp->SetScale(glm::vec3(2.75f, 1, 1));

				RenderComponent::Sptr renderer = trap_ramp->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Cube"));
				renderer->SetMaterial(Resources::GetMaterial("Brown"));

				// Collider
				RigidBody::Sptr physics = trap_ramp->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(trap_ramp->GetScale());
				collider->SetPosition(collider->GetPosition());
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
			}
		}
		
		// Platform 1
		{
			GameObject::Sptr trap_ground_upper_2 = SceneManager::GetCurrentScene()->CreateGameObject("Trap Room Upper Ground 2");
			{
				trap_ground_upper_2->SetPosition(glm::vec3(45.75f, -10.5f, 3.5f));
				trap_ground_upper_2->SetRotation(glm::vec3(0.f, 0.0f, 0.0f));
				trap_ground_upper_2->SetScale(glm::vec3(2.5f, 7, 2));

				RenderComponent::Sptr renderer = trap_ground_upper_2->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Cube"));
				renderer->SetMaterial(Resources::GetMaterial("Brown"));

				// Collider
				RigidBody::Sptr physics = trap_ground_upper_2->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(trap_ground_upper_2->GetScale());
				collider->SetPosition(collider->GetPosition());
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
			}

			GameObject::Sptr spike_trap_1 = SceneManager::GetCurrentScene()->CreateGameObject("Spike Trap");
			{
				spike_trap_1->SetPosition(glm::vec3(45.75f, -6.0f, 5.5f));
				spike_trap_1->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
				spike_trap_1->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				RenderComponent::Sptr renderer = spike_trap_1->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Spike Trap"));
				renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

				// Animator
				AnimatorComponent::Sptr animator = spike_trap_1->Add<AnimatorComponent>();
				animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
				animator->SetRenderer(*renderer);
				animator->SetAnimation("Spikes");
				animator->SetLooping(false);

				// Trigger Volume
				TriggerVolume::Sptr volume = spike_trap_1->Add<TriggerVolume>();
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
				collider->SetScale(glm::vec3(2.0f, 1.0f, 2.0f));
				volume->AddCollider(collider);
				volume->SetCollisionGroup(PHYSICAL_GROUP);
				volume->SetCollisionMask(PHYSICAL_MASK);

				// Trigger Event
				TriggerVolumeEnterBehaviour::Sptr trigger = spike_trap_1->Add<TriggerVolumeEnterBehaviour>();
				trigger->onTriggerEnterEvent = [body] {
					if (!SceneManager::GetCurrentScene()->PC.isShadow) {
						body->Get<HealthComponent>()->DealDamage(10.0f);
					}
				};

				// Spike Behavior
				SpikeTrapBehavior::Sptr spikeBehavior = spike_trap_1->Add<SpikeTrapBehavior>();
				spikeBehavior->SetAnimator(animator);
				spikeBehavior->SetTrigger(volume);
				spikeBehavior->Initialize(3, 0, false);

			}

			GameObject::Sptr spike_trap_2 = SceneManager::GetCurrentScene()->CreateGameObject("Spike Trap");
			{
				spike_trap_2->SetPosition(glm::vec3(45.75f, -10.5f, 5.5f));
				spike_trap_2->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				spike_trap_2->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				// Create and attach a renderer for the monkey
				RenderComponent::Sptr renderer = spike_trap_2->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Spike Trap"));
				renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

				// Animator
				AnimatorComponent::Sptr animator = spike_trap_2->Add<AnimatorComponent>();
				animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
				animator->SetRenderer(*renderer);
				animator->SetAnimation("Spikes");
				animator->SetLooping(false);

				// Trigger Volume
				TriggerVolume::Sptr volume = spike_trap_2->Add<TriggerVolume>();
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
				collider->SetScale(glm::vec3(2.0f, 1.0f, 2.0f));
				volume->AddCollider(collider);
				volume->SetCollisionGroup(PHYSICAL_GROUP);
				volume->SetCollisionMask(PHYSICAL_MASK);

				// Trigger Event
				TriggerVolumeEnterBehaviour::Sptr trigger = spike_trap_2->Add<TriggerVolumeEnterBehaviour>();
				trigger->onTriggerEnterEvent = [body] {
					if (!SceneManager::GetCurrentScene()->PC.isShadow) {
						body->Get<HealthComponent>()->DealDamage(10.0f);
					}
				};

				// Spike Behavior
				SpikeTrapBehavior::Sptr spikeBehavior = spike_trap_2->Add<SpikeTrapBehavior>();
				spikeBehavior->SetAnimator(animator);
				spikeBehavior->SetTrigger(volume);
				spikeBehavior->Initialize(3, 0, false);
			}

			GameObject::Sptr spike_trap_3 = SceneManager::GetCurrentScene()->CreateGameObject("Spike Trap");
			{
				spike_trap_3->SetPosition(glm::vec3(45.75f, -15.0f, 5.5f));
				spike_trap_3->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
				spike_trap_3->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				// Create and attach a renderer for the monkey
				RenderComponent::Sptr renderer = spike_trap_3->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Spike Trap"));
				renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

				// Animator
				AnimatorComponent::Sptr animator = spike_trap_3->Add<AnimatorComponent>();
				animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
				animator->SetRenderer(*renderer);
				animator->SetAnimation("Spikes");
				animator->SetLooping(false);

				// Trigger Volume
				TriggerVolume::Sptr volume = spike_trap_3->Add<TriggerVolume>();
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
				collider->SetScale(glm::vec3(2.0f, 1.0f, 2.0f));
				volume->AddCollider(collider);
				volume->SetCollisionGroup(PHYSICAL_GROUP);
				volume->SetCollisionMask(PHYSICAL_MASK);

				// Trigger Event
				TriggerVolumeEnterBehaviour::Sptr trigger = spike_trap_3->Add<TriggerVolumeEnterBehaviour>();
				trigger->onTriggerEnterEvent = [body] {
					if (!SceneManager::GetCurrentScene()->PC.isShadow) {
						body->Get<HealthComponent>()->DealDamage(10.0f);
					}
				};

				// Spike Behavior
				SpikeTrapBehavior::Sptr spikeBehavior = spike_trap_3->Add<SpikeTrapBehavior>();
				spikeBehavior->SetAnimator(animator);
				spikeBehavior->SetTrigger(volume);
				spikeBehavior->Initialize(3, 0, false);
			}
		}
		
		// Platform 2
		{
			GameObject::Sptr trap_ground_upper_3 = SceneManager::GetCurrentScene()->CreateGameObject("Trap Room Upper Ground 3");
			{
				trap_ground_upper_3->SetPosition(glm::vec3(35.5f, -10.5f, 3.5f));
				trap_ground_upper_3->SetRotation(glm::vec3(0.f, 0.0f, 0.0f));
				trap_ground_upper_3->SetScale(glm::vec3(2.5f, 7, 2));

				RenderComponent::Sptr renderer = trap_ground_upper_3->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Cube"));
				renderer->SetMaterial(Resources::GetMaterial("Brown"));

				// Collider
				RigidBody::Sptr physics = trap_ground_upper_3->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(trap_ground_upper_3->GetScale());
				collider->SetPosition(collider->GetPosition());
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
			}

			GameObject::Sptr spike_trap_4 = SceneManager::GetCurrentScene()->CreateGameObject("Spike Trap");
			{
				spike_trap_4->SetPosition(glm::vec3(35.5f, -6.0f, 5.5f));
				spike_trap_4->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
				spike_trap_4->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				// Create and attach a renderer for the monkey
				RenderComponent::Sptr renderer = spike_trap_4->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Spike Trap"));
				renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

				// Animator
				AnimatorComponent::Sptr animator = spike_trap_4->Add<AnimatorComponent>();
				animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
				animator->SetRenderer(*renderer);
				animator->SetAnimation("Spikes");
				animator->SetLooping(false);

				// Trigger Volume
				TriggerVolume::Sptr volume = spike_trap_4->Add<TriggerVolume>();
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
				collider->SetScale(glm::vec3(2.0f, 1.0f, 2.0f));
				volume->AddCollider(collider);
				volume->SetCollisionGroup(PHYSICAL_GROUP);
				volume->SetCollisionMask(PHYSICAL_MASK);

				// Trigger Event
				TriggerVolumeEnterBehaviour::Sptr trigger = spike_trap_4->Add<TriggerVolumeEnterBehaviour>();
				trigger->onTriggerEnterEvent = [body] {
					if (!SceneManager::GetCurrentScene()->PC.isShadow) {
						body->Get<HealthComponent>()->DealDamage(10.0f);
					}
				};

				// Spike Behavior
				SpikeTrapBehavior::Sptr spikeBehavior = spike_trap_4->Add<SpikeTrapBehavior>();
				spikeBehavior->SetAnimator(animator);
				spikeBehavior->SetTrigger(volume);
				spikeBehavior->Initialize(3, 3, false);
			}

			GameObject::Sptr spike_trap_5 = SceneManager::GetCurrentScene()->CreateGameObject("Spike Trap");
			{
				spike_trap_5->SetPosition(glm::vec3(35.5f, -10.5f, 5.5f));
				spike_trap_5->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
				spike_trap_5->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				// Create and attach a renderer for the monkey
				RenderComponent::Sptr renderer = spike_trap_5->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Spike Trap"));
				renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

				// Animator
				AnimatorComponent::Sptr animator = spike_trap_5->Add<AnimatorComponent>();
				animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
				animator->SetRenderer(*renderer);
				animator->SetAnimation("Spikes");
				animator->SetLooping(false);

				// Trigger Volume
				TriggerVolume::Sptr volume = spike_trap_5->Add<TriggerVolume>();
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
				collider->SetScale(glm::vec3(2.0f, 1.0f, 2.0f));
				volume->AddCollider(collider);
				volume->SetCollisionGroup(PHYSICAL_GROUP);
				volume->SetCollisionMask(PHYSICAL_MASK);

				// Trigger Event
				TriggerVolumeEnterBehaviour::Sptr trigger = spike_trap_5->Add<TriggerVolumeEnterBehaviour>();
				trigger->onTriggerEnterEvent = [body] {
					if (!SceneManager::GetCurrentScene()->PC.isShadow) {
						body->Get<HealthComponent>()->DealDamage(10.0f);
					}
				};

				// Spike Behavior
				SpikeTrapBehavior::Sptr spikeBehavior = spike_trap_5->Add<SpikeTrapBehavior>();
				spikeBehavior->SetAnimator(animator);
				spikeBehavior->SetTrigger(volume);
				spikeBehavior->Initialize(3, 3, false);
			}

			GameObject::Sptr spike_trap_6 = SceneManager::GetCurrentScene()->CreateGameObject("Spike Trap");
			{
				spike_trap_6->SetPosition(glm::vec3(35.5f, -15.0f, 5.5f));
				spike_trap_6->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				spike_trap_6->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				// Create and attach a renderer for the monkey
				RenderComponent::Sptr renderer = spike_trap_6->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Spike Trap"));
				renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

				// Animator
				AnimatorComponent::Sptr animator = spike_trap_6->Add<AnimatorComponent>();
				animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
				animator->SetRenderer(*renderer);
				animator->SetAnimation("Spikes");
				animator->SetLooping(false);

				// Trigger Volume
				TriggerVolume::Sptr volume = spike_trap_6->Add<TriggerVolume>();
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
				collider->SetScale(glm::vec3(2.0f, 1.0f, 2.0f));
				volume->AddCollider(collider);
				volume->SetCollisionGroup(PHYSICAL_GROUP);
				volume->SetCollisionMask(PHYSICAL_MASK);

				// Trigger Event
				TriggerVolumeEnterBehaviour::Sptr trigger = spike_trap_6->Add<TriggerVolumeEnterBehaviour>();
				trigger->onTriggerEnterEvent = [body] {
					if (!SceneManager::GetCurrentScene()->PC.isShadow) {
						body->Get<HealthComponent>()->DealDamage(10.0f);
					}
				};

				// Spike Behavior
				SpikeTrapBehavior::Sptr spikeBehavior = spike_trap_6->Add<SpikeTrapBehavior>();
				spikeBehavior->SetAnimator(animator);
				spikeBehavior->SetTrigger(volume);
				spikeBehavior->Initialize(3, 3, false);
			}
		}

		// Platform 3
		{
			GameObject::Sptr trap_ground_upper_4 = SceneManager::GetCurrentScene()->CreateGameObject("Trap Room Upper Ground 4");
			{
				trap_ground_upper_4->SetPosition(glm::vec3(26.25f, -10.5f, 3.5f));
				trap_ground_upper_4->SetRotation(glm::vec3(0, 0, 0));
				trap_ground_upper_4->SetScale(glm::vec3(2.5f, 7, 2));

				RenderComponent::Sptr renderer = trap_ground_upper_4->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Cube"));
				renderer->SetMaterial(Resources::GetMaterial("Brown"));

				// Collider
				RigidBody::Sptr physics = trap_ground_upper_4->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(trap_ground_upper_4->GetScale());
				collider->SetPosition(collider->GetPosition());
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
			}

			GameObject::Sptr spike_trap_7 = SceneManager::GetCurrentScene()->CreateGameObject("Spike Trap");
			{
				spike_trap_7->SetPosition(glm::vec3(26.25f, -6.0f, 5.5f));
				spike_trap_7->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
				spike_trap_7->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				// Create and attach a renderer for the monkey
				RenderComponent::Sptr renderer = spike_trap_7->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Spike Trap"));
				renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

				// Animator
				AnimatorComponent::Sptr animator = spike_trap_7->Add<AnimatorComponent>();
				animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
				animator->SetRenderer(*renderer);
				animator->SetAnimation("Spikes");
				animator->SetLooping(false);

				// Trigger Volume
				TriggerVolume::Sptr volume = spike_trap_7->Add<TriggerVolume>();
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
				collider->SetScale(glm::vec3(2.0f, 1.0f, 2.0f));
				volume->AddCollider(collider);
				volume->SetCollisionGroup(PHYSICAL_GROUP);
				volume->SetCollisionMask(PHYSICAL_MASK);

				// Trigger Event
				TriggerVolumeEnterBehaviour::Sptr trigger = spike_trap_7->Add<TriggerVolumeEnterBehaviour>();
				trigger->onTriggerEnterEvent = [body] {
					if (!SceneManager::GetCurrentScene()->PC.isShadow) {
						body->Get<HealthComponent>()->DealDamage(10.0f);
					}
				};

				// Spike Behavior
				SpikeTrapBehavior::Sptr spikeBehavior = spike_trap_7->Add<SpikeTrapBehavior>();
				spikeBehavior->SetAnimator(animator);
				spikeBehavior->SetTrigger(volume);
				spikeBehavior->Initialize(3, 0, false);
			}

			GameObject::Sptr spike_trap_8 = SceneManager::GetCurrentScene()->CreateGameObject("Spike Trap");
			{
				spike_trap_8->SetPosition(glm::vec3(26.25f, -10.5f, 5.5f));
				spike_trap_8->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
				spike_trap_8->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				// Create and attach a renderer for the monkey
				RenderComponent::Sptr renderer = spike_trap_8->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Spike Trap"));
				renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

				// Animator
				AnimatorComponent::Sptr animator = spike_trap_8->Add<AnimatorComponent>();
				animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
				animator->SetRenderer(*renderer);
				animator->SetAnimation("Spikes");
				animator->SetLooping(false);

				// Trigger Volume
				TriggerVolume::Sptr volume = spike_trap_8->Add<TriggerVolume>();
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
				collider->SetScale(glm::vec3(2.0f, 1.0f, 2.0f));
				volume->AddCollider(collider);
				volume->SetCollisionGroup(PHYSICAL_GROUP);
				volume->SetCollisionMask(PHYSICAL_MASK);

				// Trigger Event
				TriggerVolumeEnterBehaviour::Sptr trigger = spike_trap_8->Add<TriggerVolumeEnterBehaviour>();
				trigger->onTriggerEnterEvent = [body] {
					if (!SceneManager::GetCurrentScene()->PC.isShadow) {
						body->Get<HealthComponent>()->DealDamage(10.0f);
					}
				};

				// Spike Behavior
				SpikeTrapBehavior::Sptr spikeBehavior = spike_trap_8->Add<SpikeTrapBehavior>();
				spikeBehavior->SetAnimator(animator);
				spikeBehavior->SetTrigger(volume);
				spikeBehavior->Initialize(3, 0, false);
			}

			GameObject::Sptr spike_trap_9 = SceneManager::GetCurrentScene()->CreateGameObject("Spike Trap");
			{
				spike_trap_9->SetPosition(glm::vec3(26.25f, -15.0f, 5.5f));
				spike_trap_9->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
				spike_trap_9->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

				// Create and attach a renderer for the monkey
				RenderComponent::Sptr renderer = spike_trap_9->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Spike Trap"));
				renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

				// Animator
				AnimatorComponent::Sptr animator = spike_trap_9->Add<AnimatorComponent>();
				animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
				animator->SetRenderer(*renderer);
				animator->SetAnimation("Spikes");
				animator->SetLooping(false);

				// Trigger Volume
				TriggerVolume::Sptr volume = spike_trap_9->Add<TriggerVolume>();
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
				collider->SetScale(glm::vec3(2.0f, 1.0f, 2.0f));
				volume->AddCollider(collider);
				volume->SetCollisionGroup(PHYSICAL_GROUP);
				volume->SetCollisionMask(PHYSICAL_MASK);

				// Trigger Event
				TriggerVolumeEnterBehaviour::Sptr trigger = spike_trap_9->Add<TriggerVolumeEnterBehaviour>();
				trigger->onTriggerEnterEvent = [body] {
					if (!SceneManager::GetCurrentScene()->PC.isShadow) {
						body->Get<HealthComponent>()->DealDamage(10.0f);
					}
				};

				// Spike Behavior
				SpikeTrapBehavior::Sptr spikeBehavior = spike_trap_9->Add<SpikeTrapBehavior>();
				spikeBehavior->SetAnimator(animator);
				spikeBehavior->SetTrigger(volume);
				spikeBehavior->Initialize(3, 0, false);
			}


		}

		// End
		{
			GameObject::Sptr trap_ground_upper_5 = SceneManager::GetCurrentScene()->CreateGameObject("Trap Room Upper Ground 5");
			{
				trap_ground_upper_5->SetPosition(glm::vec3(15.5f, -8.5f, 3.5f));
				trap_ground_upper_5->SetRotation(glm::vec3(0, 0, 0));
				trap_ground_upper_5->SetScale(glm::vec3(3, 10, 2));

				RenderComponent::Sptr renderer = trap_ground_upper_5->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Cube"));
				renderer->SetMaterial(Resources::GetMaterial("Brown"));

				// Collider
				RigidBody::Sptr physics = trap_ground_upper_5->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(trap_ground_upper_5->GetScale());
				collider->SetPosition(collider->GetPosition());
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
			}
		}		
	}

	// Jump Room
	{

	}

	// Upper Access
	{
		// Upper Access
		GameObject::Sptr cageDoor_2 = SceneManager::GetCurrentScene()->CreateGameObject("Upper Access Door");
		{
			// Transform
			cageDoor_2->SetPosition(glm::vec3(-25.0f, -23.0f, 15.0f));
			cageDoor_2->SetRotation(glm::vec3(90, 0, 90));
			cageDoor_2->SetScale(glm::vec3(0.25f, 0.085f, 0.1f));

			// Renderer
			RenderComponent::Sptr renderer = cageDoor_2->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Small Cage"));
			renderer->SetMaterial(Resources::GetMaterial("Small Cage"));

			// Collider
			RigidBody::Sptr physics = cageDoor_2->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 2.0f, 0.0f));
			collider->SetScale(glm::vec3(1.0f, 4.0f, 3.5f));
			physics->AddCollider(collider);
			physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
			physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
		}

		// Upper Access
		GameObject::Sptr pressure_plate_2 = SceneManager::GetCurrentScene()->CreateGameObject("Upper Access Plate");
		{
			// Transform
			pressure_plate_2->SetPosition(glm::vec3(-10.5f, -26.5f, 6.0f));
			pressure_plate_2->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			pressure_plate_2->SetScale(glm::vec3(1.0f, 0.5f, 1.0f));

			// Renderer
			RenderComponent::Sptr renderer = pressure_plate_2->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Pressure Plate"));
			renderer->SetMaterial(Resources::GetMaterial("Pressure Plate"));

			// Trigger Volume
			TriggerVolume::Sptr volume = pressure_plate_2->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
			collider->SetScale(glm::vec3(2.0f, 1.0f, 2.0f));
			volume->AddCollider(collider);
			volume->SetCollisionGroup(PHYSICAL_GROUP);
			volume->SetCollisionMask(PHYSICAL_MASK);

			// Trigger Event
			TriggerVolumeEnterBehaviour::Sptr trigger = pressure_plate_2->Add<TriggerVolumeEnterBehaviour>();
			trigger->onTriggerEnterEvent = [cageDoor_2, NO_GROUP, NO_MASK]
			{
				// Open The Door
				cageDoor_2->Get<RenderComponent>()->IsEnabled = false;
				RigidBody::Sptr rigid = cageDoor_2->Get<RigidBody>();
				rigid->SetCollisionGroup(NO_GROUP);
				rigid->SetCollisionMask(NO_MASK);			
			};

			trigger->onTriggerExitEvent = [cageDoor_2, PHYSICAL_GROUP, SHADOW_GROUP, PHYSICAL_MASK, SHADOW_MASK]
			{
				// Close The Door
				cageDoor_2->Get<RenderComponent>()->IsEnabled = true;
				RigidBody::Sptr rigid = cageDoor_2->Get<RigidBody>();
				rigid->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				rigid->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
			};
		}

		GameObject::Sptr crate_2 = SceneManager::GetCurrentScene()->CreateGameObject("Crate 2");
		{
			crate_2->SetPosition(glm::vec3(10.5f, -26.5f, 7.5f));
			crate_2->SetRotation(glm::vec3(0.f, 0.0f, 0.0f));
			crate_2->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

			RenderComponent::Sptr renderer = crate_2->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Cube"));
			renderer->SetMaterial(Resources::GetMaterial("Gray"));

			// Collider
			RigidBody::Sptr physics = crate_2->Add<RigidBody>(RigidBodyType::Dynamic);
			BoxCollider::Sptr collider = BoxCollider::Create();			
			collider->SetScale(crate_2->GetScale());
			physics->AddCollider(collider);
			physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
			physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);

		}
	}

	// Elevator Access
	{
		// Elevator Access
		GameObject::Sptr cageDoor_3 = SceneManager::GetCurrentScene()->CreateGameObject("Elevator Access Door 1");
		{
			// Transform
			cageDoor_3->SetPosition(glm::vec3(-0.5f, -31.5f, 4.5f));
			cageDoor_3->SetRotation(glm::vec3(90, 0, 90));
			cageDoor_3->SetScale(glm::vec3(0.25f, 0.125f, 0.075f));

			// Renderer
			RenderComponent::Sptr renderer = cageDoor_3->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Small Cage"));
			renderer->SetMaterial(Resources::GetMaterial("Small Cage"));

			// Collider
			RigidBody::Sptr physics = cageDoor_3->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 2.0f, 0.0f));
			collider->SetScale(glm::vec3(1.0f, 4.0f, 3.5f));
			physics->AddCollider(collider);
			physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
			physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
		}

		// Elevator Access
		GameObject::Sptr cageDoor_4 = SceneManager::GetCurrentScene()->CreateGameObject("Elevator Access Door 2");
		{
			// Transform
			cageDoor_4->SetPosition(glm::vec3(-16.0f, -44.5f, 5.0f));
			cageDoor_4->SetRotation(glm::vec3(90, 0, 0));
			cageDoor_4->SetScale(glm::vec3(0.25f, 0.125f, 0.08f));

			// Renderer
			RenderComponent::Sptr renderer = cageDoor_4->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Small Cage"));
			renderer->SetMaterial(Resources::GetMaterial("Small Cage"));

			// Collider
			RigidBody::Sptr physics = cageDoor_4->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 2.0f, 0.0f));
			collider->SetScale(glm::vec3(1.0f, 4.0f, 3.5f));
			physics->AddCollider(collider);
			physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
			physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
		}

		// Elevator Access
		GameObject::Sptr cageDoor_5 = SceneManager::GetCurrentScene()->CreateGameObject("Elevator Access Door 3");
		{
			// Transform
			cageDoor_5->SetPosition(glm::vec3(-24.25f, -39.0f, 15.0f));
			cageDoor_5->SetRotation(glm::vec3(90, 0, 90));
			cageDoor_5->SetScale(glm::vec3(0.25f, 0.125f, 0.165f));

			// Renderer
			RenderComponent::Sptr renderer = cageDoor_5->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Small Cage"));
			renderer->SetMaterial(Resources::GetMaterial("Small Cage"));

			// Collider
			RigidBody::Sptr physics = cageDoor_5->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
			collider->SetScale(glm::vec3(1.0f, 4.0f, 7.0f));
			physics->AddCollider(collider);
			physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
			physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
		}

		// Elevator Access
		GameObject::Sptr lever_1 = SceneManager::GetCurrentScene()->CreateGameObject("Elevator Access Lever");
		{
			// Transform
			lever_1->SetPosition(glm::vec3(-19.5f, -31.0f, 15.0f));
			lever_1->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
			lever_1->SetScale(glm::vec3(0.5f, 0.5f, 1.0f));

			// Renderer
			RenderComponent::Sptr renderer = lever_1->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Turret Projectile"));
			renderer->SetMaterial(Resources::GetMaterial("Gray"));
			
			// Collider
			RigidBody::Sptr physics = lever_1->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 0.0f, 1.5f));			
			physics->AddCollider(collider);
			physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
			physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);			

			// Interaction Event
			InteractableComponent::Sptr interactable = lever_1->Add<InteractableComponent>();
			interactable->onInteractionEvent = [interactable, cageDoor_3, cageDoor_4, cageDoor_5, PHYSICAL_GROUP, PHYSICAL_MASK, SHADOW_GROUP, SHADOW_MASK, NO_GROUP, NO_MASK]
			{	
				interactable->isToggled = !interactable->isToggled;

				RigidBody::Sptr rigid_1 = cageDoor_3->Get<RigidBody>();
				RigidBody::Sptr rigid_2 = cageDoor_4->Get<RigidBody>();
				RigidBody::Sptr rigid_3 = cageDoor_5->Get<RigidBody>();

				if (interactable->isToggled) {
					rigid_1->SetCollisionGroup(NO_GROUP);
					rigid_1->SetCollisionMask(NO_MASK);
					rigid_2->SetCollisionGroup(NO_GROUP);
					rigid_2->SetCollisionMask(NO_MASK);
					rigid_3->SetCollisionGroup(NO_GROUP);
					rigid_3->SetCollisionMask(NO_MASK);

					cageDoor_3->Get<RenderComponent>()->IsEnabled = false;
					cageDoor_4->Get<RenderComponent>()->IsEnabled = false;
					cageDoor_5->Get<RenderComponent>()->IsEnabled = false;					
				}

				else {
					rigid_1->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
					rigid_1->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
					rigid_2->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
					rigid_2->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
					rigid_3->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
					rigid_3->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
					
					cageDoor_3->Get<RenderComponent>()->IsEnabled = true;
					cageDoor_4->Get<RenderComponent>()->IsEnabled = true;
					cageDoor_5->Get<RenderComponent>()->IsEnabled = true;
				}
			};
		}

		GameObject::Sptr elevator = SceneManager::GetCurrentScene()->CreateGameObject("Elevator");
		{
			elevator->SetPosition(glm::vec3(-26.75f, -44.75f, 15.0f));
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
			elevatorBehavior->SetStartPosition(glm::vec3(-26.75f, -46.75f, 15.0f));
			elevatorBehavior->SetEndPosition(glm::vec3(-26.75f, -46.75f, 5.5f));
			elevatorBehavior->SetLooping(true);
			elevatorBehavior->SetCollider(collider);

		}		
	}

	// Turret Room
	{
		GameObject::Sptr turret_1 = SceneManager::GetCurrentScene()->CreateGameObject("Turret 1");
		{
			turret_1->SetPosition(glm::vec3(-68.25f, 15.5f, 13.0f));
			turret_1->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			turret_1->SetScale(glm::vec3(0.3f, 0.2f, 0.3f));

			RenderComponent::Sptr renderer = turret_1->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Turret"));
			renderer->SetMaterial(Resources::GetMaterial("Turret"));

			TurretBehavior::Sptr turretBehavior = turret_1->Add<TurretBehavior>();
			turretBehavior->Initialize(3, 0, glm::vec3(-64.0f, 15.5f, 15.0f), glm::vec3(1.0f, 0.0f, 0.0f), false);
		}

		GameObject::Sptr turret_2 = SceneManager::GetCurrentScene()->CreateGameObject("Turret 2");
		{
			turret_2->SetPosition(glm::vec3(-68.25f, 19.0f, 13.0f));
			turret_2->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			turret_2->SetScale(glm::vec3(0.3f, 0.2f, 0.3f));

			RenderComponent::Sptr renderer = turret_2->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Turret"));
			renderer->SetMaterial(Resources::GetMaterial("Turret"));
		}

		GameObject::Sptr turret_3 = SceneManager::GetCurrentScene()->CreateGameObject("Turret 3");
		{
			turret_3->SetPosition(glm::vec3(-68.25f, 22.5f, 13.0f));
			turret_3->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			turret_3->SetScale(glm::vec3(0.3f, 0.2f, 0.3f));

			RenderComponent::Sptr renderer = turret_3->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Turret"));
			renderer->SetMaterial(Resources::GetMaterial("Turret"));
		}

		GameObject::Sptr turret_4 = SceneManager::GetCurrentScene()->CreateGameObject("Turret 4");
		{
			turret_4->SetPosition(glm::vec3(-68.25f, 26.0f, 13.0f));
			turret_4->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			turret_4->SetScale(glm::vec3(0.3f, 0.2f, 0.3f));

			RenderComponent::Sptr renderer = turret_4->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Turret"));
			renderer->SetMaterial(Resources::GetMaterial("Turret"));
		}

		// Turret Room Activator
		GameObject::Sptr lever_2 = SceneManager::GetCurrentScene()->CreateGameObject("Turret Activation Lever");
		{
			// Transform
			lever_2->SetPosition(glm::vec3(-70.0f, 15.0f, 15.0f));
			lever_2->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
			lever_2->SetScale(glm::vec3(0.5f, 0.5f, 1.0f));

			// Renderer
			RenderComponent::Sptr renderer = lever_2->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Turret Projectile"));
			renderer->SetMaterial(Resources::GetMaterial("Gray"));

			// Collider
			RigidBody::Sptr physics = lever_2->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);

			// Interaction Event
			InteractableComponent::Sptr interactable = lever_2->Add<InteractableComponent>();
			interactable->onInteractionEvent = [interactable, turret_1, turret_2, turret_3, turret_4]
			{
				interactable->isToggled = !interactable->isToggled;

				if (interactable->isToggled) {
					// Shut off Turrets
				}

				else {
					// Turn on Turrets
				}
			};
		}

		GameObject::Sptr key = SceneManager::GetCurrentScene()->CreateGameObject("Key");
		{
			key->SetPosition(glm::vec3(-71.75f, 20.5f, 17.5f));
			key->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			key->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

			RenderComponent::Sptr renderer = key->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Key"));
			renderer->SetMaterial(Resources::GetMaterial("Key"));
		}

		GameObject::Sptr pedestal = SceneManager::GetCurrentScene()->CreateGameObject("Key Pedestal");
		{
			pedestal->SetPosition(glm::vec3(-72.0f, 21.0f, 16.5f));
			pedestal->SetRotation(glm::vec3(0.f, 0.0f, 0.0f));
			pedestal->SetScale(glm::vec3(1, 1, 1));

			RenderComponent::Sptr renderer = pedestal->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Cube"));
			renderer->SetMaterial(Resources::GetMaterial("Gray"));

			// Collider
			RigidBody::Sptr physics = pedestal->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);

			// Interaction Event
			InteractableComponent::Sptr interactable = pedestal->Add<InteractableComponent>();
			interactable->onInteractionEvent = [key]
			{
				// If player already has the key, do nothing.
				if (GameManager::GetInstance().PlayerHasKey()) return;

				GameManager::GetInstance().GiveKey();
				key->Get<RenderComponent>()->IsEnabled = false;				
			};
		}
	}
	
	// Door Room
	{
		GameObject::Sptr interact_doorway = SceneManager::GetCurrentScene()->CreateGameObject("Interact Door");
		{
			// Transform
			interact_doorway->SetPosition(glm::vec3(-12.75f, 21.0f, 18.5f));
			interact_doorway->SetRotation(glm::vec3(90.f, 0.0f, 180.0f));
			interact_doorway->SetScale(glm::vec3(1.0f, 0.5f, 0.75f));

			// Render
			RenderComponent::Sptr renderer = interact_doorway->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Door"));
			renderer->SetMaterial(Resources::GetMaterial("Door"));

			// Animator
			AnimatorComponent::Sptr animator = interact_doorway->Add<AnimatorComponent>();
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
			RigidBody::Sptr physics = interact_doorway->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetScale(glm::vec3(1.5f, 3.8f, 4.3f));
			collider->SetPosition(collider->GetPosition() + glm::vec3(0, 0, 0));
			physics->AddCollider(collider);
			physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
			physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);

			// Interaction Event
			InteractableComponent::Sptr interactable = interact_doorway->Add<InteractableComponent>();
			interactable->onInteractionEvent = [interact_doorway, PHYSICAL_GROUP, PHYSICAL_MASK, SHADOW_GROUP, SHADOW_MASK, NO_GROUP, NO_MASK] 
			{
				// If player doesn't have the key, do nothing.
				if (!GameManager::GetInstance().PlayerHasKey()) return;
				
				AnimatorComponent::Sptr anim = interact_doorway->Get<AnimatorComponent>();
				RigidBody::Sptr rigid = interact_doorway->Get<RigidBody>();

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

		GameObject::Sptr statue_1 = SceneManager::GetCurrentScene()->CreateGameObject("Statue 1");
		{
			statue_1->SetPosition(glm::vec3(-17.5f, 28.0f, 18.0f));
			statue_1->SetRotation(glm::vec3(0.f, 0.0f, -30.0f));
			statue_1->SetScale(glm::vec3(2,2,3));

			RenderComponent::Sptr renderer = statue_1->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Cube"));
			renderer->SetMaterial(Resources::GetMaterial("Gray"));
		}

		GameObject::Sptr statue_2 = SceneManager::GetCurrentScene()->CreateGameObject("Statue 2");
		{
			statue_2->SetPosition(glm::vec3(-31.5f, 28.0f, 18.0f));
			statue_2->SetRotation(glm::vec3(0.f, 0.0f, 30.0f));
			statue_2->SetScale(glm::vec3(2,2,3));

			RenderComponent::Sptr renderer = statue_2->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Cube"));
			renderer->SetMaterial(Resources::GetMaterial("Gray"));
		}

		GameObject::Sptr interactable_note = SceneManager::GetCurrentScene()->CreateGameObject("Interactable Note");
		{
			interactable_note->SetPosition(glm::vec3(38.75f, 29.5f, 13.0f));
			interactable_note->SetRotation(glm::vec3(0, 0, 0));
			interactable_note->SetScale(glm::vec3(5,1,4));

			RenderComponent::Sptr renderer = interactable_note->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Cube"));
			renderer->SetMaterial(Resources::GetMaterial("Gray"));
		}
	}

	// Final Room
	{
		GameObject::Sptr exit_door = SceneManager::GetCurrentScene()->CreateGameObject("Exit Door");
		{
			exit_door->SetPosition(glm::vec3(38.75f, 25.0f, 11.5f));
			exit_door->SetRotation(glm::vec3(0.f, 0.0f, 0.0f));
			exit_door->SetScale(glm::vec3(6.0f, 0.3f, 0.1f));			

			// Trigger Volume
			TriggerVolume::Sptr volume = exit_door->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
			collider->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
			volume->AddCollider(collider);
			volume->SetCollisionGroup(PHYSICAL_GROUP);
			volume->SetCollisionMask(PHYSICAL_MASK);

			// Trigger Event
			TriggerVolumeEnterBehaviour::Sptr trigger = exit_door->Add<TriggerVolumeEnterBehaviour>();
			trigger->onTriggerEnterEvent = []
			{
				// Load next Scene
				//SceneManager::LoadScene(SceneManager::Scenes::LevelTwo, true);				
			};

			trigger->onTriggerExitEvent = []
			{

			};
		}
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

	GameManager::GetInstance().Reset();

	// Save the asset manifest for all the resources we just loaded
	//ResourceManager::SaveManifest("manifest.json");
	// Save the SceneManager::GetCurrentScene() to a JSON file
	//SceneManager::GetCurrentScene()->Save("Level_One.json");

	return nullptr;
}