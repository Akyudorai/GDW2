#include "Level_Two.h"
#include "SceneManager.h"

// Gameplay
#include "Gameplay/MeshResource.h"
#include "Gameplay/GameManager.h"
#include "Gameplay/RoomData.h"

// Components
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/AnimatorComponent.h"
#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"

// Physics
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Physics/TriggerVolume.h"

// Utils
#include "Utils/ResourceManagement/Resources.h"
#include "Utils/GlmDefines.h"
#include <GLFW/glfw3.h>

using namespace Gameplay::Physics;

bool Level_Two::PlayOnLoad = true;

Scene::Sptr Level_Two::Load(GLFWwindow* window)
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

		GameManager::GetInstance().cameraManager.Initialize(camera);
		GameManager::GetInstance().cameraManager.SetCameraMode(CameraMode::FollowRoom);		
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

#pragma region Room_1

	RoomData room1_data;
	room1_data.RoomName = "Room 1";
	room1_data.cameraPos = glm::vec3(10, -10, 0);

	GameObject::Sptr r1_wall_1 = SceneManager::GetCurrentScene()->CreateGameObject("R1_Wall_1");
	{
		r1_wall_1->SetPosition(glm::vec3(10, 0, 0));
		r1_wall_1->SetRotation(glm::vec3(0.f, 0.0f, 0.0f));
		r1_wall_1->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

		RenderComponent::Sptr renderer = r1_wall_1->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall 2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall 2"));

		room1_data.ObjectsInRoom.push_back(r1_wall_1);
	}

	GameObject::Sptr r1_wall_2 = SceneManager::GetCurrentScene()->CreateGameObject("R1_Wall_2");
	{
		r1_wall_2->SetPosition(glm::vec3(10, 0, 0));
		r1_wall_2->SetRotation(glm::vec3(0.f, 0.0f, 0.0f));
		r1_wall_2->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

		RenderComponent::Sptr renderer = r1_wall_2->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall 2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall 2"));

		room1_data.ObjectsInRoom.push_back(r1_wall_2);
	}

	GameObject::Sptr r1_wall_3 = SceneManager::GetCurrentScene()->CreateGameObject("R1_Wall_3");
	{
		r1_wall_3->SetPosition(glm::vec3(10, 0, 0));
		r1_wall_3->SetRotation(glm::vec3(0.f, 0.0f, 0.0f));
		r1_wall_3->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

		RenderComponent::Sptr renderer = r1_wall_3->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall 2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall 2"));

		room1_data.ObjectsInRoom.push_back(r1_wall_3);
	}

	GameObject::Sptr r1_wall_4 = SceneManager::GetCurrentScene()->CreateGameObject("R1_Wall_4");
	{
		r1_wall_4->SetPosition(glm::vec3(10, 0, 0));
		r1_wall_4->SetRotation(glm::vec3(0.f, 0.0f, 0.0f));
		r1_wall_4->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

		RenderComponent::Sptr renderer = r1_wall_4->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall 2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall 2"));

		room1_data.ObjectsInRoom.push_back(r1_wall_4);
	}

	GameObject::Sptr r1_wall_5 = SceneManager::GetCurrentScene()->CreateGameObject("R1_Wall_5");
	{
		r1_wall_5->SetPosition(glm::vec3(10, 0, 0));
		r1_wall_5->SetRotation(glm::vec3(0.f, 0.0f, 0.0f));
		r1_wall_5->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

		RenderComponent::Sptr renderer = r1_wall_5->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall 2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall 2"));

		room1_data.ObjectsInRoom.push_back(r1_wall_5);
	}

	GameObject::Sptr r1_trigger_r2 = SceneManager::GetCurrentScene()->CreateGameObject("R1_Trigger_R2");
	{
		// Transform
		r1_trigger_r2->SetPosition(glm::vec3(10, 0, 0));
		r1_trigger_r2->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		r1_trigger_r2->SetScale(glm::vec3(1.0f, 0.5f, 1.0f));

		// Trigger Volume
		TriggerVolume::Sptr volume = r1_trigger_r2->Add<TriggerVolume>();
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
		collider->SetScale(glm::vec3(2.0f, 1.0f, 2.0f));
		volume->AddCollider(collider);
		volume->SetCollisionGroup(PHYSICAL_GROUP);
		volume->SetCollisionMask(PHYSICAL_MASK);

		// Trigger Event
		TriggerVolumeEnterBehaviour::Sptr trigger = r1_trigger_r2->Add<TriggerVolumeEnterBehaviour>();
		trigger->onTriggerEnterEvent = [] {
			SceneManager::roomLoader.LoadRoom("Room 2");
			LOG_WARN("LOADING ROOM 2");
		};

		room1_data.ObjectsInRoom.push_back(r1_trigger_r2);
	}

	SceneManager::roomLoader.Add("Room 1", room1_data);

#pragma endregion

#pragma region Room_2

	RoomData room2_data;
	room2_data.RoomName = "Room 2";
	room2_data.cameraPos = glm::vec3(-10, -10, 0);

	GameObject::Sptr r2_wall_1 = SceneManager::GetCurrentScene()->CreateGameObject("R2_Wall_1");
	{
		r2_wall_1->SetPosition(glm::vec3(-10, 0, 0));
		r2_wall_1->SetRotation(glm::vec3(0.f, 0.0f, 0.0f));
		r2_wall_1->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

		RenderComponent::Sptr renderer = r2_wall_1->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall 2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall 2"));

		room2_data.ObjectsInRoom.push_back(r2_wall_1);
	}

	GameObject::Sptr r2_wall_2 = SceneManager::GetCurrentScene()->CreateGameObject("R2_Wall_2");
	{
		r2_wall_2->SetPosition(glm::vec3(-10, 0, 0));
		r2_wall_2->SetRotation(glm::vec3(0.f, 0.0f, 0.0f));
		r2_wall_2->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

		RenderComponent::Sptr renderer = r2_wall_2->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall 2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall 2"));

		room2_data.ObjectsInRoom.push_back(r2_wall_2);
	}

	GameObject::Sptr r2_wall_3 = SceneManager::GetCurrentScene()->CreateGameObject("R2_Wall_3");
	{
		r2_wall_3->SetPosition(glm::vec3(-10, 0, 0));
		r2_wall_3->SetRotation(glm::vec3(0.f, 0.0f, 0.0f));
		r2_wall_3->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

		RenderComponent::Sptr renderer = r2_wall_3->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall 2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall 2"));

		room2_data.ObjectsInRoom.push_back(r2_wall_3);
	}

	GameObject::Sptr r2_wall_4 = SceneManager::GetCurrentScene()->CreateGameObject("R2_Wall_4");
	{
		r2_wall_4->SetPosition(glm::vec3(-10, 0, 0));
		r2_wall_4->SetRotation(glm::vec3(0.f, 0.0f, 0.0f));
		r2_wall_4->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

		RenderComponent::Sptr renderer = r2_wall_4->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall 2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall 2"));

		room2_data.ObjectsInRoom.push_back(r2_wall_4);
	}

	GameObject::Sptr r2_wall_5 = SceneManager::GetCurrentScene()->CreateGameObject("R2_Wall_5");
	{
		r2_wall_5->SetPosition(glm::vec3(-10, 0, 0));
		r2_wall_5->SetRotation(glm::vec3(0.f, 0.0f, 0.0f));
		r2_wall_5->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

		RenderComponent::Sptr renderer = r2_wall_5->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall 2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall 2"));

		room2_data.ObjectsInRoom.push_back(r2_wall_5);
	}

	GameObject::Sptr r2_trigger_r1 = SceneManager::GetCurrentScene()->CreateGameObject("R2_Trigger_R1");
	{
		// Transform
		r2_trigger_r1->SetPosition(glm::vec3(-10, 0, 0));
		r2_trigger_r1->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		r2_trigger_r1->SetScale(glm::vec3(1.0f, 0.5f, 1.0f));

		// Trigger Volume
		TriggerVolume::Sptr volume = r2_trigger_r1->Add<TriggerVolume>();
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
		collider->SetScale(glm::vec3(2.0f, 1.0f, 2.0f));
		volume->AddCollider(collider);
		volume->SetCollisionGroup(PHYSICAL_GROUP);
		volume->SetCollisionMask(PHYSICAL_MASK);

		// Trigger Event
		TriggerVolumeEnterBehaviour::Sptr trigger = r2_trigger_r1->Add<TriggerVolumeEnterBehaviour>();
		trigger->onTriggerEnterEvent = [] {
			SceneManager::roomLoader.LoadRoom("Room 1");	
			LOG_WARN("LOADING ROOM 1");
		};

		room2_data.ObjectsInRoom.push_back(r2_trigger_r1);
	}

	SceneManager::roomLoader.Add("Room 2", room2_data);

#pragma endregion

	/////////////////////////////////////////////////////////

	// Call SceneManager::GetCurrentScene() awake to start up all of our components
	SceneManager::GetCurrentScene()->Window = window;
	SceneManager::GetCurrentScene()->Awake();

	GameManager::GetInstance().Reset();

	// Save the asset manifest for all the resources we just loaded
	//ResourceManager::SaveManifest("manifest.json");
	// Save the SceneManager::GetCurrentScene() to a JSON file
	//SceneManager::GetCurrentScene()->Save("Level_Two.json");

	return nullptr;
}