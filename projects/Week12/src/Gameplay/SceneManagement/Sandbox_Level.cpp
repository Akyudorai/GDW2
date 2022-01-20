#include "Sandbox_Level.h"

// Utilities
#include "Utils/MeshBuilder.h"
#include "Utils/MeshFactory.h"
#include "Utils/ObjLoader.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/ResourceManagement/ResourceManager.h"
#include "Utils/FileHelpers.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/StringUtils.h"
#include "Utils/GlmDefines.h"
#include "Utils/ResourceManagement/Resources.h"

// Gameplay
#include "Gameplay/Material.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Gameplay/PlayerController.h"
#include "Gameplay/MeshResource.h"

// Components
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/Components/Camera.h"
#include "Gameplay/Components/RotatingBehaviour.h"
#include "Gameplay/Components/JumpBehaviour.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/MaterialSwapBehaviour.h"
#include "Gameplay/Components/HealthComponent.h"
#include "Gameplay/Components/InteractableComponent.h"
#include "Gameplay/Components/AnimatorComponent.h"
#include "Gameplay/Components/Enemy.h"


// Components
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/Components/Camera.h"
#include "Gameplay/Components/RotatingBehaviour.h"
#include "Gameplay/Components/JumpBehaviour.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/MaterialSwapBehaviour.h"
#include "Gameplay/Components/HealthComponent.h"
#include "Gameplay/Components/InteractableComponent.h"
#include "Gameplay/Components/AnimatorComponent.h"
#include "Gameplay/Components/Enemy.h"

// Physics
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Physics/Colliders/PlaneCollider.h"
#include "Gameplay/Physics/Colliders/SphereCollider.h"
#include "Gameplay/Physics/Colliders/ConvexMeshCollider.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Graphics/DebugDraw.h"
#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Components/SimpleCameraControl.h"
#include "Gameplay/Physics/Colliders/CylinderCollider.h"

// GUI
#include "Gameplay/Components/GUI/RectTransform.h"
#include "Gameplay/Components/GUI/GuiPanel.h"
#include "Gameplay/Components/GUI/GuiText.h"
#include "Graphics/GuiBatcher.h"
#include "Utils/UIHelper.h"

#include <GLFW/glfw3.h>

#include "SceneManager.h"

using namespace Gameplay;
using namespace Gameplay::Physics;

bool Sandbox_Level::PlayOnLoad = true;

Scene::Sptr Sandbox_Level::Load(GLFWwindow* window)
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
		camera->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
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

	///////////////////////////////////////////////////////////
	////			BASIC ENEMY - Steering Behavior
	///////////////////////////////////////////////////////////

	//GameObject::Sptr nav1 = SceneManager::GetCurrentScene()->CreateGameObject("Nav 1");
	//{
	//	// Transform
	//	nav1->SetPosition(glm::vec3(-30.0f, -10.0f, 4));
	//	nav1->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
	//	nav1->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

	//	// Renderer
	//	//RenderComponent::Sptr renderer = nav1->Add<RenderComponent>();
	//	//renderer->SetMesh(pressurePlateMesh);
	//	//renderer->SetMaterial(stoneWallMaterial);
	//}

	//GameObject::Sptr nav2 = SceneManager::GetCurrentScene()->CreateGameObject("Nav 2");
	//{
	//	// Transform
	//	nav2->SetPosition(glm::vec3(-30.0f, -20.0f, 4));
	//	nav2->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
	//	nav2->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

	//	// Renderer
	//	//RenderComponent::Sptr renderer = nav2->Add<RenderComponent>();
	//	//renderer->SetMesh(pressurePlateMesh);
	//	//renderer->SetMaterial(stoneWallMaterial);
	//}

	//GameObject::Sptr nav3 = SceneManager::GetCurrentScene()->CreateGameObject("Nav 3");
	//{
	//	// Transform
	//	nav3->SetPosition(glm::vec3(-20.0f, -20.0f, 4));
	//	nav3->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
	//	nav3->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

	//	// Renderer
	//	//RenderComponent::Sptr renderer = nav3->Add<RenderComponent>();
	//	//renderer->SetMesh(pressurePlateMesh);
	//	//renderer->SetMaterial(stoneWallMaterial);
	//}

	//GameObject::Sptr nav4 = SceneManager::GetCurrentScene()->CreateGameObject("Nav 4");
	//{
	//	// Transform
	//	nav4->SetPosition(glm::vec3(-20.0f, -10.0f, 4));
	//	nav4->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
	//	nav4->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

	//	// Renderer
	//	//RenderComponent::Sptr renderer = nav4->Add<RenderComponent>();
	//	//renderer->SetMesh(pressurePlateMesh);
	//	//renderer->SetMaterial(stoneWallMaterial);
	//}

	//GameObject::Sptr enemy = SceneManager::GetCurrentScene()->CreateGameObject("Enemy");
	//{
	//	enemy->SetPosition(glm::vec3(-20.0f, -25.0f, 0.0f));
	//	enemy->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
	//	enemy->SetScale(glm::vec3(1.0f, 1.0f, 1.0));

	//	// Create and attach a renderer for the monkey
	//	RenderComponent::Sptr renderer = enemy->Add<RenderComponent>();
	//	renderer->SetMesh(Resources::GetMesh("Enemy"));
	//	renderer->SetMaterial(Resources::GetMaterial("Enemy"));

	//	// Add a dynamic rigid body to this monkey
	//	RigidBody::Sptr physics = enemy->Add<RigidBody>(RigidBodyType::Dynamic);
	//	BoxCollider::Sptr collider = BoxCollider::Create();
	//	physics->AddCollider(collider);
	//	physics->SetCollisionGroup(PHYSICAL_GROUP);
	//	physics->SetCollisionMask(PHYSICAL_MASK);

	//	// Enemy Navigation
	//	Enemy::Sptr enemyScript = enemy->Add<Enemy>();
	//	enemyScript->SetOwner(enemy);
	//	enemyScript->SetTarget(body);
	//	enemyScript->SetFleeTarget(shadow);
	//	/*std::vector<GameObject> navPts;
	//	navPts.push_back(*nav1);
	//	navPts.push_back(*nav2);
	//	navPts.push_back(*nav3);
	//	navPts.push_back(*nav4);
	//	enemyScript->SetNavPoints(navPts);*/
	//}

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
			wall1->SetPosition(glm::vec3(0, 30.0f, 0.0f));
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
			wall2->SetPosition(glm::vec3(-24.0f, 35.0f, 0.0f));
			wall2->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			wall2->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

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
			wall3->SetPosition(glm::vec3(-24.0f, 8.0f, 0.0f));
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
			wall4->SetPosition(glm::vec3(-28.0f, 2.0f, 0.0f));
			wall4->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			wall4->SetScale(glm::vec3(1.0f, 1.0f, 1.5f));

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
			wall5->SetPosition(glm::vec3(28.0f, 2.0f, 0.0f));
			wall5->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			wall5->SetScale(glm::vec3(1.0f, 1.0f, 1.5f));

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
			wall6->SetPosition(glm::vec3(24.0f, 8.0f, 0.0f));
			wall6->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			wall6->SetScale(glm::vec3(1.0f, 1.0f, 0.3f));

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
			wall7->SetPosition(glm::vec3(24.0f, 35.0f, 0.0f));
			wall7->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			wall7->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

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
			wall8->SetPosition(glm::vec3(-50, 26.5f, 0.0f));
			wall8->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			wall8->SetScale(glm::vec3(1.0f, 1.0f, 1.55f));

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
			wall9->SetPosition(glm::vec3(-37, 48.5f, 0.0f));
			wall9->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			wall9->SetScale(glm::vec3(1.0f, 1.0f, 0.75f));

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
			wall10->SetPosition(glm::vec3(50, -15.0f, 0.0f));
			wall10->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			wall10->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

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
			wall11->SetPosition(glm::vec3(37.0f, 48.5f, 0.0f));
			wall11->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			wall11->SetScale(glm::vec3(1.0f, 1.0f, 0.75f));

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
			wall12->SetPosition(glm::vec3(-50.0f, -15.0f, 0.0f));
			wall12->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			wall12->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

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
			wall13->SetPosition(glm::vec3(50.0f, 26.5f, 0.0f));
			wall13->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			wall13->SetScale(glm::vec3(1.0f, 1.0f, 1.55f));

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
			wall14->SetPosition(glm::vec3(0, -31.5f, 0.0f));
			wall14->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			wall14->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

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

#pragma endregion

#pragma region ENVIRONMENT LIGHTS

	GameObject::Sptr torch1 = SceneManager::GetCurrentScene()->CreateGameObject("Torch 1");
	{
		torch1->SetPosition(glm::vec3(0.0f, 15.0f, 0.0f));
		torch1->SetRotation(glm::vec3(90, 0, 90));
		torch1->SetScale(glm::vec3(1.0f, 0.3f, 1.0f));

		RenderComponent::Sptr renderer = torch1->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Standing Torch"));
		renderer->SetMaterial(Resources::GetMaterial("Standing Torch"));

		SceneManager::GetCurrentScene()->Lights[1].Position = torch1->GetPosition() + glm::vec3(0.0f, 0.0f, 10.5f);
		SceneManager::GetCurrentScene()->Lights[1].Color = glm::vec3(0.89f, 0.345f, 0.13f);
		SceneManager::GetCurrentScene()->Lights[1].Range = 100.0f;

		RigidBody::Sptr physics = torch1->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(1.0f, 8.0f, 1.0f));
		physics->AddCollider(collider);
	}

	GameObject::Sptr torch2 = SceneManager::GetCurrentScene()->CreateGameObject("Torch 2");
	{
		torch2->SetPosition(glm::vec3(-25.0f, -15.0f, 0.0f));
		torch2->SetRotation(glm::vec3(90, 0, 90));
		torch2->SetScale(glm::vec3(1.0f, 0.3f, 1.0f));

		RenderComponent::Sptr renderer = torch2->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Standing Torch"));
		renderer->SetMaterial(Resources::GetMaterial("Standing Torch"));

		SceneManager::GetCurrentScene()->Lights[2].Position = torch2->GetPosition() + glm::vec3(0.0f, 0.0f, 10.5f);
		SceneManager::GetCurrentScene()->Lights[2].Color = glm::vec3(0.89f, 0.345f, 0.13f);
		SceneManager::GetCurrentScene()->Lights[2].Range = 100.0f;

		RigidBody::Sptr physics = torch2->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(1.0f, 8.0f, 1.0f));
		physics->AddCollider(collider);

	}

	GameObject::Sptr torch3 = SceneManager::GetCurrentScene()->CreateGameObject("Torch 3");
	{
		torch3->SetPosition(glm::vec3(35.0f, -15.0f, 0.0f));
		torch3->SetRotation(glm::vec3(90, 0, 90));
		torch3->SetScale(glm::vec3(1.0f, 0.3f, 1.0f));

		RenderComponent::Sptr renderer = torch3->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Standing Torch"));
		renderer->SetMaterial(Resources::GetMaterial("Standing Torch"));

		SceneManager::GetCurrentScene()->Lights[3].Position = torch3->GetPosition() + glm::vec3(0.0f, 0.0f, 10.5f);
		SceneManager::GetCurrentScene()->Lights[3].Color = glm::vec3(0.89f, 0.345f, 0.13f);
		SceneManager::GetCurrentScene()->Lights[3].Range = 100.0f;


		RigidBody::Sptr physics = torch3->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(1.0f, 8.0f, 1.0f));
		physics->AddCollider(collider);

	}

	GameObject::Sptr torch4 = SceneManager::GetCurrentScene()->CreateGameObject("Torch 4");
	{
		torch4->SetPosition(glm::vec3(-46.5f, 15.0f, 0.0f));
		torch4->SetRotation(glm::vec3(90, 0, 90));
		torch4->SetScale(glm::vec3(1.0f, 0.3f, 1.0f));

		RenderComponent::Sptr renderer = torch4->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Standing Torch"));
		renderer->SetMaterial(Resources::GetMaterial("Standing Torch"));

		SceneManager::GetCurrentScene()->Lights[4].Position = torch4->GetPosition() + glm::vec3(0.0f, 0.0f, 10.5f);
		SceneManager::GetCurrentScene()->Lights[4].Color = glm::vec3(0.89f, 0.345f, 0.13f);
		SceneManager::GetCurrentScene()->Lights[4].Range = 100.0f;


		RigidBody::Sptr physics = torch4->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(1.0f, 8.0f, 1.0f));
		physics->AddCollider(collider);

	}

	GameObject::Sptr torch5 = SceneManager::GetCurrentScene()->CreateGameObject("Torch 5");
	{
		torch5->SetPosition(glm::vec3(-46.5f, 35.0f, 0.0f));
		torch5->SetRotation(glm::vec3(90, 0, 90));
		torch5->SetScale(glm::vec3(1.0f, 0.3f, 1.0f));

		RenderComponent::Sptr renderer = torch5->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Standing Torch"));
		renderer->SetMaterial(Resources::GetMaterial("Standing Torch"));

		SceneManager::GetCurrentScene()->Lights[5].Position = torch5->GetPosition() + glm::vec3(0.0f, 0.0f, 10.5f);
		SceneManager::GetCurrentScene()->Lights[5].Color = glm::vec3(0.89f, 0.345f, 0.13f);
		SceneManager::GetCurrentScene()->Lights[5].Range = 100.0f;

		RigidBody::Sptr physics = torch5->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(1.0f, 8.0f, 1.0f));
		physics->AddCollider(collider);
	}

	GameObject::Sptr torch6 = SceneManager::GetCurrentScene()->CreateGameObject("Torch 6");
	{
		torch6->SetPosition(glm::vec3(46.5f, 15.0f, 0.0f));
		torch6->SetRotation(glm::vec3(90, 0, 90));
		torch6->SetScale(glm::vec3(1.0f, 0.3f, 1.0f));

		RenderComponent::Sptr renderer = torch6->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Standing Torch"));
		renderer->SetMaterial(Resources::GetMaterial("Standing Torch"));

		SceneManager::GetCurrentScene()->Lights[6].Position = torch6->GetPosition() + glm::vec3(0.0f, 0.0f, 10.5f);
		SceneManager::GetCurrentScene()->Lights[6].Color = glm::vec3(0.89f, 0.345f, 0.13f);
		SceneManager::GetCurrentScene()->Lights[6].Range = 100.0f;


		RigidBody::Sptr physics = torch6->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(1.0f, 8.0f, 1.0f));
		physics->AddCollider(collider);

	}

	GameObject::Sptr torch7 = SceneManager::GetCurrentScene()->CreateGameObject("Torch 7");
	{
		torch7->SetPosition(glm::vec3(46.5f, 35.0f, 0.0f));
		torch7->SetRotation(glm::vec3(90, 0, 90));
		torch7->SetScale(glm::vec3(1.0f, 0.3f, 1.0f));

		RenderComponent::Sptr renderer = torch7->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Standing Torch"));
		renderer->SetMaterial(Resources::GetMaterial("Standing Torch"));

		SceneManager::GetCurrentScene()->Lights[7].Position = torch7->GetPosition() + glm::vec3(0.0f, 0.0f, 10.5f);
		SceneManager::GetCurrentScene()->Lights[7].Color = glm::vec3(0.89f, 0.345f, 0.13f);
		SceneManager::GetCurrentScene()->Lights[7].Range = 100.0f;


		RigidBody::Sptr physics = torch7->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(1.0f, 8.0f, 1.0f));
		physics->AddCollider(collider);
	}

	GameObject::Sptr intactPillar = SceneManager::GetCurrentScene()->CreateGameObject("Intact Pillar");
	{
		intactPillar->SetPosition(glm::vec3(0.0f, 10.0f, 0.0f));
		intactPillar->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		intactPillar->SetScale(glm::vec3(0.8f, 0.8f, 0.8f));

		RenderComponent::Sptr renderer = intactPillar->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Intact Pillar"));
		renderer->SetMaterial(Resources::GetMaterial("Turret Projectile"));

		RigidBody::Sptr physics = intactPillar->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		//collider->SetScale(glm::vec3(3.0, 4, 15.5));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr damagedPillar = SceneManager::GetCurrentScene()->CreateGameObject("Damaged Pillar");
	{
		damagedPillar->SetPosition(glm::vec3(5.0f, 10.0f, 0.0f));
		damagedPillar->SetRotation(glm::vec3(90.f, 0.0f, 180.0f));
		damagedPillar->SetScale(glm::vec3(0.8f, 0.8f, 0.8f));

		RenderComponent::Sptr renderer = damagedPillar->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Damaged Pillar"));
		renderer->SetMaterial(Resources::GetMaterial("Turret Projectile"));

		RigidBody::Sptr physics = damagedPillar->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		//collider->SetScale(glm::vec3(3.0, 4, 15.5));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr destroyedPillar = SceneManager::GetCurrentScene()->CreateGameObject("Destroyed Pillar");
	{
		destroyedPillar->SetPosition(glm::vec3(-5.0f, 10.0f, 0.0f));
		destroyedPillar->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		destroyedPillar->SetScale(glm::vec3(0.8f, 0.8f, 0.8f));

		RenderComponent::Sptr renderer = destroyedPillar->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Destroyed Pillar"));
		renderer->SetMaterial(Resources::GetMaterial("Turret Projectile"));

		RigidBody::Sptr physics = destroyedPillar->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		//collider->SetScale(glm::vec3(3.0, 4, 15.5));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

#pragma endregion

#pragma region OBSTACLES

	GameObject::Sptr sCage = SceneManager::GetCurrentScene()->CreateGameObject("S_Cage");
	{
		sCage->SetPosition(glm::vec3(0, 2, 0));
		sCage->SetRotation(glm::vec3(90, 0, 90));
		sCage->SetScale(glm::vec3(0.25f, 0.125f, 0.125f));

		RenderComponent::Sptr renderer = sCage->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Small Cage"));
		renderer->SetMaterial(Resources::GetMaterial("Small Cage"));

		RigidBody::Sptr physics = sCage->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(2.0f, 5.0f, 4.2f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0, 3.25f, 0));
		physics->AddCollider(collider);
	}

	GameObject::Sptr interact_doorway = SceneManager::GetCurrentScene()->CreateGameObject("Interact Door");
	{
		interact_doorway->SetPosition(glm::vec3(-24.0f, 16.0f, 2.0f));
		interact_doorway->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		interact_doorway->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		RenderComponent::Sptr renderer = interact_doorway->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Door"));
		renderer->SetMaterial(Resources::GetMaterial("Door"));

		AnimatorComponent::Sptr animator = interact_doorway->Add<AnimatorComponent>();
		std::vector<MeshResource::Sptr> frames;
		for (int i = 0; i < 4; ++i)
		{
			std::string file;
			file.append("models/door/door");
			file.append(std::to_string((i + 1)));
			file.append(".obj");
			frames.push_back(ResourceManager::CreateAsset<MeshResource>(file));
		}
		animator->AddAnimation("Open", frames);

		animator->SetRenderer(*renderer);
		animator->SetAnimation("Open");
		animator->SetLooping(false);
		animator->SetPause(true);
		animator->onAnimationCompleted = [animator] {
			animator->SetPause(true);
			animator->SetReverse(!animator->IsReversed());
		};

		RigidBody::Sptr physics = interact_doorway->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(1.5f, 3.8f, 4.3f));
		collider->SetPosition(collider->GetPosition() + glm::vec3(0, 0, 0));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr interact = SceneManager::GetCurrentScene()->CreateGameObject("Lever PlaceHolder");
	{
		interact->SetPosition(glm::vec3(-18.0f, 22.0f, 0.0f));
		interact->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		interact->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

		RenderComponent::Sptr renderer = interact->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Turret Projectile"));
		renderer->SetMaterial(Resources::GetMaterial("Turret Projectile"));

		RigidBody::Sptr physics = interact->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		physics->AddCollider(collider);


		// Add Interactable Component
		InteractableComponent::Sptr interactable = interact->Add<InteractableComponent>();
		interactable->onInteractionEvent = [interact_doorway, PHYSICAL_GROUP, PHYSICAL_MASK, SHADOW_GROUP, SHADOW_MASK, NO_GROUP, NO_MASK] {
			//bool isEnabled = interact_doorway->Get<RenderComponent>()->IsEnabled;
			//interact_doorway->Get<RenderComponent>()->IsEnabled = !isEnabled;
			//interact_doorway->Get<RigidBody>()->IsEnabled = !isEnabled;

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

	GameObject::Sptr pressure_door = SceneManager::GetCurrentScene()->CreateGameObject("Pressure Door");
	{
		pressure_door->SetPosition(glm::vec3(24.0f, 16.0f, 2.5f));
		pressure_door->SetRotation(glm::vec3(90.0f, 0.0f, 180.0f));
		pressure_door->SetScale(glm::vec3(1.0f, 0.5f, 0.65f));

		RenderComponent::Sptr renderer = pressure_door->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Door"));
		renderer->SetMaterial(Resources::GetMaterial("Door"));

		AnimatorComponent::Sptr animator = pressure_door->Add<AnimatorComponent>();
		std::vector<MeshResource::Sptr> frames;
		for (int i = 0; i < 4; ++i)
		{
			std::string file;
			file.append("models/door/door");
			file.append(std::to_string((i + 1)));
			file.append(".obj");
			frames.push_back(ResourceManager::CreateAsset<MeshResource>(file));
		}
		animator->AddAnimation("Open", frames);

		animator->SetRenderer(*renderer);
		animator->SetAnimation("Open");
		animator->SetLooping(false);
		animator->SetPause(true);

		RigidBody::Sptr physics = pressure_door->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 2.0f, 0.0f));
		collider->SetScale(glm::vec3(1.0f, 4.0f, 3.5f));
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr pressure_plate = SceneManager::GetCurrentScene()->CreateGameObject("Pressure Plate");
	{
		pressure_plate->SetPosition(glm::vec3(15.0f, 16.0f, 0.0f));
		pressure_plate->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		pressure_plate->SetScale(glm::vec3(1.25f, 0.5f, 1.25f));

		RenderComponent::Sptr renderer = pressure_plate->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Pressure Plate"));
		renderer->SetMaterial(Resources::GetMaterial("Pressure Plate"));

		AnimatorComponent::Sptr animator = pressure_plate->Add<AnimatorComponent>();
		std::vector<MeshResource::Sptr> frames;
		for (int i = 0; i < 2; ++i)
		{
			std::string file;
			file.append("models/pressurePlate/PressurePlate");
			file.append(std::to_string((i + 1)));
			file.append(".obj");
			frames.push_back(ResourceManager::CreateAsset<MeshResource>(file));
		}
		animator->AddAnimation("Activate", frames);

		animator->SetRenderer(*renderer);
		animator->SetAnimation("Activate");
		animator->SetLooping(false);
		animator->SetPause(true);
		animator->onAnimationCompleted = [animator] {
			animator->SetPause(true);
		};

		TriggerVolume::Sptr volume = pressure_plate->Add<TriggerVolume>();
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 2.5f, 0.0f));
		collider->SetScale(glm::vec3(2.0f, 1.0f, 2.0f));
		volume->AddCollider(collider);
		volume->SetCollisionGroup(PHYSICAL_GROUP);
		volume->SetCollisionMask(PHYSICAL_MASK);

		TriggerVolumeEnterBehaviour::Sptr trigger = pressure_plate->Add<TriggerVolumeEnterBehaviour>();
		trigger->onTriggerEnterEvent = [pressure_door, animator, NO_GROUP, NO_MASK] {
			AnimatorComponent::Sptr anim = pressure_door->Get<AnimatorComponent>();
			RigidBody::Sptr rigid = pressure_door->Get<RigidBody>();

			animator->SetReverse(false);
			animator->SetPause(false);

			// Open Door
			rigid->SetCollisionGroup(NO_GROUP);
			rigid->SetCollisionMask(NO_MASK);
			anim->SetReverse(false);
			anim->SetPause(false);
		};

		trigger->onTriggerExitEvent = [pressure_door, animator, PHYSICAL_GROUP, SHADOW_GROUP, PHYSICAL_MASK, SHADOW_MASK] {
			AnimatorComponent::Sptr anim = pressure_door->Get<AnimatorComponent>();
			RigidBody::Sptr rigid = pressure_door->Get<RigidBody>();

			animator->SetReverse(true);
			animator->SetPause(false);

			// Close Door
			rigid->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
			rigid->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
			anim->SetReverse(true);
			anim->SetPause(false);
		};

		// Cody :: To create a trigger event, start with square brackets '[]'.  If you want to pass in any references, put them inside the square brackets '[body]'
		// Then you can open it like a function with braces '{  }'.  That's because we just created a function with no name.  This works because we bind this created function
		// to the triggerEvent inside the TriggerVolume class.
	}

	GameObject::Sptr pressure_plate2 = SceneManager::GetCurrentScene()->CreateGameObject("Pressure Plate 2");
	{
		pressure_plate2->SetPosition(glm::vec3(35.0f, 16.0f, 0.0f));
		pressure_plate2->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		pressure_plate2->SetScale(glm::vec3(1.25f, 0.5f, 1.25f));

		RenderComponent::Sptr renderer = pressure_plate2->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Pressure Plate"));
		renderer->SetMaterial(Resources::GetMaterial("Pressure Plate"));

		AnimatorComponent::Sptr animator = pressure_plate2->Add<AnimatorComponent>();
		std::vector<MeshResource::Sptr> frames;
		for (int i = 0; i < 2; ++i)
		{
			std::string file;
			file.append("models/pressurePlate/PressurePlate");
			file.append(std::to_string((i + 1)));
			file.append(".obj");
			frames.push_back(ResourceManager::CreateAsset<MeshResource>(file));
		}
		animator->AddAnimation("Activate", frames);

		animator->SetRenderer(*renderer);
		animator->SetAnimation("Activate");
		animator->SetLooping(false);
		animator->SetPause(true);
		animator->onAnimationCompleted = [animator] {
			animator->SetPause(true);
		};
		TriggerVolume::Sptr volume = pressure_plate2->Add<TriggerVolume>();
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 2.5f, 0.0f));
		collider->SetScale(glm::vec3(2.0f, 1.0f, 2.0f));
		volume->AddCollider(collider);
		volume->SetCollisionGroup(PHYSICAL_GROUP);
		volume->SetCollisionMask(PHYSICAL_MASK);

		TriggerVolumeEnterBehaviour::Sptr trigger = pressure_plate2->Add<TriggerVolumeEnterBehaviour>();
		trigger->onTriggerEnterEvent = [pressure_door, animator, NO_GROUP, NO_MASK] {
			AnimatorComponent::Sptr anim = pressure_door->Get<AnimatorComponent>();
			RigidBody::Sptr rigid = pressure_door->Get<RigidBody>();

			animator->SetReverse(false);
			animator->SetPause(false);

			// Open Door
			rigid->SetCollisionGroup(NO_GROUP);
			rigid->SetCollisionMask(NO_MASK);
			anim->SetReverse(false);
			anim->SetPause(false);
		};

		trigger->onTriggerExitEvent = [pressure_door, animator, PHYSICAL_GROUP, SHADOW_GROUP, PHYSICAL_MASK, SHADOW_MASK] {
			AnimatorComponent::Sptr anim = pressure_door->Get<AnimatorComponent>();
			RigidBody::Sptr rigid = pressure_door->Get<RigidBody>();

			animator->SetReverse(true);
			animator->SetPause(false);

			// Close Door
			rigid->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
			rigid->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
			anim->SetReverse(true);
			anim->SetPause(false);
		};

		// Cody :: To create a trigger event, start with square brackets '[]'.  If you want to pass in any references, put them inside the square brackets '[body]'
		// Then you can open it like a function with braces '{  }'.  That's because we just created a function with no name.  This works because we bind this created function
		// to the triggerEvent inside the TriggerVolume class.
	}

	GameObject::Sptr spike_trap = SceneManager::GetCurrentScene()->CreateGameObject("Spike Trap");
	{
		spike_trap->SetPosition(glm::vec3(-6.0f, 25.0f, 0.0f));
		spike_trap->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		spike_trap->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		// Create and attach a renderer for the monkey
		RenderComponent::Sptr renderer = spike_trap->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Spike Trap"));
		renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

		AnimatorComponent::Sptr animator = spike_trap->Add<AnimatorComponent>();
		std::vector<MeshResource::Sptr> frames;
		for (int i = 0; i < 3; ++i)
		{
			std::string file;
			file.append("models/spikedTrap/SpikedTrap");
			file.append(std::to_string((i + 1)));
			file.append(".obj");
			frames.push_back(ResourceManager::CreateAsset<MeshResource>(file));
		}
		animator->AddAnimation("Activate", frames);

		animator->SetRenderer(*renderer);
		animator->SetAnimation("Activate");
		animator->SetLooping(false);
		animator->SetPause(true);
		animator->onAnimationCompleted = [animator] {

			if (animator->IsReversed()) {
				animator->SetPause(true);
			}

			animator->SetReverse(!animator->IsReversed());
		};

		TriggerVolume::Sptr volume = spike_trap->Add<TriggerVolume>();
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 2.5f, 0.0f));
		volume->AddCollider(collider);
		volume->SetCollisionGroup(PHYSICAL_GROUP);
		volume->SetCollisionMask(PHYSICAL_MASK);

		TriggerVolumeEnterBehaviour::Sptr trigger = spike_trap->Add<TriggerVolumeEnterBehaviour>();
		trigger->onTriggerEnterEvent = [body, animator] {
			if (!SceneManager::GetCurrentScene()->PC.isShadow) {
				body->Get<HealthComponent>()->DealDamage(10.0f);
			}

			animator->SetPause(false);
		};
	}

	GameObject::Sptr light_trap = SceneManager::GetCurrentScene()->CreateGameObject("Light Trap");
	{
		light_trap->SetPosition(glm::vec3(0.0f, 25.0f, 0.0f));
		light_trap->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		light_trap->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		// Create and attach a renderer for the monkey
		RenderComponent::Sptr renderer = light_trap->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Pressure Plate"));
		renderer->SetMaterial(Resources::GetMaterial("Pressure Plate"));

		// Add a dynamic rigid body to this monkey
		TriggerVolume::Sptr volume = light_trap->Add<TriggerVolume>();
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 2.5f, 0.0f));
		volume->AddCollider(collider);
		volume->SetCollisionGroup(SHADOW_GROUP);
		volume->SetCollisionMask(SHADOW_MASK);

		TriggerVolumeEnterBehaviour::Sptr trigger = light_trap->Add<TriggerVolumeEnterBehaviour>();

		trigger->onTriggerStayEvent = [shadow] {
			if (SceneManager::GetCurrentScene()->PC.isShadow) {
				shadow->Get<HealthComponent>()->DealDamage(0.1f);
			}
		};
	}

	GameObject::Sptr gas_trap = SceneManager::GetCurrentScene()->CreateGameObject("Gas Trap");
	{
		gas_trap->SetPosition(glm::vec3(6.0f, 25.0f, 0.0f));
		gas_trap->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		gas_trap->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		// Create and attach a renderer for the monkey
		RenderComponent::Sptr renderer = gas_trap->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Pressure Plate"));
		renderer->SetMaterial(Resources::GetMaterial("Pressure Plate"));

		AnimatorComponent::Sptr animator = gas_trap->Add<AnimatorComponent>();
		std::vector<MeshResource::Sptr> frames;
		for (int i = 0; i < 2; ++i)
		{
			std::string file;
			file.append("models/pressurePlate/PressurePlate");
			file.append(std::to_string((i + 1)));
			file.append(".obj");
			frames.push_back(ResourceManager::CreateAsset<MeshResource>(file));
		}
		animator->AddAnimation("Activate", frames);

		animator->SetRenderer(*renderer);
		animator->SetAnimation("Activate");
		animator->SetLooping(false);
		animator->SetPause(true);
		animator->onAnimationCompleted = [animator] {
			animator->SetPause(true);
		};

		TriggerVolume::Sptr volume = gas_trap->Add<TriggerVolume>();
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 2.5f, 0.0f));
		volume->AddCollider(collider);
		volume->SetCollisionGroup(PHYSICAL_GROUP);
		volume->SetCollisionMask(PHYSICAL_MASK);

		TriggerVolumeEnterBehaviour::Sptr trigger = gas_trap->Add<TriggerVolumeEnterBehaviour>();

		trigger->onTriggerEnterEvent = [animator] {
			animator->SetReverse(false);
			animator->SetPause(false);
		};

		trigger->onTriggerStayEvent = [body, animator] {
			if (!SceneManager::GetCurrentScene()->PC.isShadow) {
				body->Get<HealthComponent>()->DealDamage(0.1f);
			}
		};

		trigger->onTriggerExitEvent = [animator] {
			animator->SetReverse(true);
			animator->SetPause(false);
		};

	}

#pragma endregion

#pragma region DECORATIONS
	GameObject::Sptr darkPineTree1 = SceneManager::GetCurrentScene()->CreateGameObject("Dark Tree 1");
	{
		darkPineTree1->SetPosition(glm::vec3(-17.91f, 8.05f, 0.55f));
		darkPineTree1->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		darkPineTree1->SetScale(glm::vec3(0.15f, 0.15f, 0.15f));

		RenderComponent::Sptr renderer = darkPineTree1->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Dark Pine Tree"));
		renderer->SetMaterial(Resources::GetMaterial("Dark Pine Tree"));

		RigidBody::Sptr physics = darkPineTree1->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		physics->AddCollider(collider);
	}

	GameObject::Sptr lightPineTree1 = SceneManager::GetCurrentScene()->CreateGameObject("Light Tree 1");
	{
		lightPineTree1->SetPosition(glm::vec3(37.04f, 7.43f, 0.55f));
		lightPineTree1->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		lightPineTree1->SetScale(glm::vec3(0.15f, 0.15f, 0.15f));

		RenderComponent::Sptr renderer = lightPineTree1->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Light Pine Tree"));
		renderer->SetMaterial(Resources::GetMaterial("Light Pine Tree"));

		RigidBody::Sptr physics = lightPineTree1->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		physics->AddCollider(collider);
	}

	GameObject::Sptr symbol1 = SceneManager::GetCurrentScene()->CreateGameObject("Symbol 1");
	{
		symbol1->SetPosition(glm::vec3(0, 28.24f, 3.85f));
		symbol1->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		symbol1->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		RenderComponent::Sptr renderer = symbol1->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Relic 1"));
		renderer->SetMaterial(Resources::GetMaterial("Relic 1"));

		RigidBody::Sptr physics = symbol1->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		physics->AddCollider(collider);
	}

	GameObject::Sptr symbol2 = SceneManager::GetCurrentScene()->CreateGameObject("Symbol 2");
	{
		symbol2->SetPosition(glm::vec3(5.0f, 28.24f, 3.85f));
		symbol2->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		symbol2->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		RenderComponent::Sptr renderer = symbol2->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Relic 2"));
		renderer->SetMaterial(Resources::GetMaterial("Relic 2"));

		RigidBody::Sptr physics = symbol2->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		physics->AddCollider(collider);
	}

	GameObject::Sptr symbol3 = SceneManager::GetCurrentScene()->CreateGameObject("Symbol 3");
	{
		symbol3->SetPosition(glm::vec3(-5.0f, 28.24f, 3.85f));
		symbol3->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		symbol3->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		RenderComponent::Sptr renderer = symbol3->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Relic 3"));
		renderer->SetMaterial(Resources::GetMaterial("Relic 3"));

		RigidBody::Sptr physics = symbol3->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		physics->AddCollider(collider);
	}

	GameObject::Sptr symbol4 = SceneManager::GetCurrentScene()->CreateGameObject("Symbol 4");
	{
		symbol4->SetPosition(glm::vec3(10.0f, 28.24f, 3.85f));
		symbol4->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		symbol4->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		RenderComponent::Sptr renderer = symbol4->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Relic 4"));
		renderer->SetMaterial(Resources::GetMaterial("Relic 4"));

		RigidBody::Sptr physics = symbol4->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		physics->AddCollider(collider);
	}

	GameObject::Sptr symbol5 = SceneManager::GetCurrentScene()->CreateGameObject("Symbol 5");
	{
		symbol5->SetPosition(glm::vec3(-10.0f, 28.24f, 3.85f));
		symbol5->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		symbol5->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		RenderComponent::Sptr renderer = symbol5->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Relic 5"));
		renderer->SetMaterial(Resources::GetMaterial("Relic 5"));

		RigidBody::Sptr physics = symbol5->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		physics->AddCollider(collider);
	}

	GameObject::Sptr rock = SceneManager::GetCurrentScene()->CreateGameObject("Rock");
	{
		rock->SetPosition(glm::vec3(32.34f, 5.71f, 0.85f));
		rock->SetRotation(glm::vec3(-74.f, 0.0f, -90.0f));
		rock->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

		RenderComponent::Sptr renderer = rock->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Rock"));
		renderer->SetMaterial(Resources::GetMaterial("Rock"));

		RigidBody::Sptr physics = rock->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		physics->AddCollider(collider);
	}

	GameObject::Sptr turret = SceneManager::GetCurrentScene()->CreateGameObject("Turret");
	{
		turret->SetPosition(glm::vec3(-36.72f, 40.79f, 0.5f));
		turret->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		turret->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

		RenderComponent::Sptr renderer = turret->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Turret"));
		renderer->SetMaterial(Resources::GetMaterial("Turret"));

		//AnimatorComponent::Sptr animator = turret->Add<AnimatorComponent>();
		//animator->AddAnimation("Turret", TurretAnimation);

		//animator->SetRenderer(*renderer);
		//animator->SetAnimation("Turret");
		////animator->SetFrames(std::move(frames));
		//animator->SetLooping(true);
		//animator->SetPause(false);
		//animator->SetSpeed(5);

		RigidBody::Sptr physics = turret->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		physics->AddCollider(collider);
	}

	GameObject::Sptr boxCage = SceneManager::GetCurrentScene()->CreateGameObject("Box Cage");
	{
		boxCage->SetPosition(glm::vec3(31.01f, 38.56f, 0.14f));
		boxCage->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		boxCage->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

		RenderComponent::Sptr renderer = boxCage->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Box Cage"));
		renderer->SetMaterial(Resources::GetMaterial("Box Cage"));

		RigidBody::Sptr physics = boxCage->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		physics->AddCollider(collider);
	}

	GameObject::Sptr circleCage = SceneManager::GetCurrentScene()->CreateGameObject("Circle Cage");
	{
		circleCage->SetPosition(glm::vec3(42.0f, 42.0f, 0.14f));
		circleCage->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		circleCage->SetScale(glm::vec3(0.075f, 0.075f, 0.075f));

		RenderComponent::Sptr renderer = circleCage->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Circle Cage"));
		renderer->SetMaterial(Resources::GetMaterial("Circle Cage"));

		RigidBody::Sptr physics = circleCage->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		physics->AddCollider(collider);
	}

	GameObject::Sptr longCage = SceneManager::GetCurrentScene()->CreateGameObject("Long Cage");
	{
		longCage->SetPosition(glm::vec3(20, -11.75f, 0.14f));
		longCage->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		longCage->SetScale(glm::vec3(0.1f, 0.1f, 0.225f));

		RenderComponent::Sptr renderer = longCage->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Long Cage"));
		renderer->SetMaterial(Resources::GetMaterial("Long Cage"));

		RigidBody::Sptr physics = longCage->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		physics->AddCollider(collider);
	}

	GameObject::Sptr uCage = SceneManager::GetCurrentScene()->CreateGameObject("U Cage");
	{
		uCage->SetPosition(glm::vec3(-36.77f, 37.03f, 0.14f));
		uCage->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		uCage->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

		RenderComponent::Sptr renderer = uCage->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("U Cage"));
		renderer->SetMaterial(Resources::GetMaterial("U Cage"));

		RigidBody::Sptr physics = uCage->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		physics->AddCollider(collider);
	}

	GameObject::Sptr graveStone = SceneManager::GetCurrentScene()->CreateGameObject("Grave Stone");
	{
		graveStone->SetPosition(glm::vec3(-12.43f, 11.08f, 0.33f));
		graveStone->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		graveStone->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

		RenderComponent::Sptr renderer = graveStone->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Grave Stone"));
		renderer->SetMaterial(Resources::GetMaterial("Grave Stone"));

		RigidBody::Sptr physics = graveStone->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(glm::vec3(1.5f, 3.7f, 2.0f));
		physics->AddCollider(collider);
	}

#pragma endregion


	/////////////////////////////////////////////////////////
	//				   USER INTERFACE - GAME
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

	/////////////////////////////////////////////////////////
	//				   USER INTERFACE - Main Menu
	/////////////////////////////////////////////////////////

	/*GameObject::Sptr mainMenu = SceneManager::GetCurrentScene()->CreateGameObject("UI Menu Canvas");
	{
		RectTransform::Sptr transform = mainMenu->Add<RectTransform>();
		transform->SetMin({ 16, 16 });
		transform->SetMax({ 900, 900 });
		transform->SetPosition({ 400, 400 });

		GuiPanel::Sptr backgroundPanel = mainMenu->Add<GuiPanel>();
		backgroundPanel->SetColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

		GameObject::Sptr menuTitle = UIHelper::CreateText("Main Menu");
		menuTitle->Get<GuiText>()->SetTextScale(2);
		menuTitle->Get<RectTransform>()->SetPosition({ 800.0f, 700 });
		mainMenu->AddChild(menuTitle);

		GameObject::Sptr button1 = UIHelper::CreateButton("Press 'P' to Start Game");
		button1->Get<RectTransform>()->SetPosition({ 425, 400 });
		mainMenu->AddChild(button1);

		mainMenu->IsActive = true;
		SceneManager::GetCurrentScene()->PC.SetMainMenuCanvas(*mainMenu);
	}*/

	/////////////////////////////////////////////////////////
	//				   USER INTERFACE - Lose Screen
	/////////////////////////////////////////////////////////

	GameObject::Sptr losePanel = SceneManager::GetCurrentScene()->CreateGameObject("Lose Panel UI");
	{
		RectTransform::Sptr transform = losePanel->Add<RectTransform>();
		transform->SetMin({ 16, 16 });
		transform->SetMax({ 900, 900 });
		transform->SetPosition({ 400, 400 });

		GuiPanel::Sptr backgroundPanel = losePanel->Add<GuiPanel>();
		backgroundPanel->SetColor(glm::vec4(0.1f, 0.1f, 0.1f, 0.75f));

		GameObject::Sptr menuTitle = UIHelper::CreateText("You Died!");
		menuTitle->Get<GuiText>()->SetTextScale(2);
		menuTitle->Get<RectTransform>()->SetPosition({ 800.0f, 700 });
		losePanel->AddChild(menuTitle);

		losePanel->IsActive = false;
		SceneManager::GameInterface.SetLosePanel(*losePanel);
		//SceneManager::GetCurrentScene()->PC.SetLoseCanvas(*loseCanvas);
	}

	/////////////////////////////////////////////////////////
	//				   Pause Menu
	/////////////////////////////////////////////////////////

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

	// Call SceneManager::GetCurrentScene() awake to start up all of our components
	SceneManager::GetCurrentScene()->Window = window;
	SceneManager::GetCurrentScene()->Awake();

	// Save the asset manifest for all the resources we just loaded
	ResourceManager::SaveManifest("manifest.json");
	// Save the SceneManager::GetCurrentScene() to a JSON file
	SceneManager::GetCurrentScene()->Save("Sandbox_Level.json");

	return nullptr;
}