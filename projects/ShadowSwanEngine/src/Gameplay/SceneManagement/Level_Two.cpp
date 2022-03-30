#include "Level_Two.h"
#include "SceneManager.h"

// Gameplay
#include "Gameplay/MeshResource.h"
#include "Gameplay/GameManager.h"

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
		//animator->AddAnimation("Walk", Resources::GetAnimation("Character Walk"), 1.0f);
		//animator->AddAnimation("Idle", Resources::GetAnimation("Character Idle"), 1.0f);

		animator->SetRenderer(*renderer);
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
		//animator->AddAnimation("Walk", Resources::GetAnimation("Character Walk"), 1.0f);
		//animator->AddAnimation("Idle", Resources::GetAnimation("Character Idle"), 1.0f);

		animator->SetRenderer(*renderer);
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



	GameObject::Sptr wall1 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 1");
	{
		wall1->SetPosition(glm::vec3(-25.640f, 19.120f, 0));
		wall1->SetRotation(glm::vec3(0, 0.0f, 0));
		wall1->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

		RenderComponent::Sptr renderer = wall1->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall2"));
	}

	GameObject::Sptr wall2 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 2");
	{
		wall2->SetPosition(glm::vec3(-23.920f, 11.800f, 0.0f));
		wall2->SetRotation(glm::vec3(0, 0.0f, 0));
		wall2->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

		RenderComponent::Sptr renderer = wall2->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall2"));
	}

	GameObject::Sptr wall3 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 3");
	{
		wall3->SetPosition(glm::vec3(-18.250f, 19.120f, 0.0f));
		wall3->SetRotation(glm::vec3(0, 0.0f, 0));
		wall3->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

		RenderComponent::Sptr renderer = wall3->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall2"));
	}

	GameObject::Sptr wall4 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 4");
	{
		wall4->SetPosition(glm::vec3(-10.880f, 19.120f, 0.0f));
		wall4->SetRotation(glm::vec3(0, 0.0f, 0));
		wall4->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

		RenderComponent::Sptr renderer = wall4->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall2"));
	}

	GameObject::Sptr wall5 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 5");
	{
		wall5->SetPosition(glm::vec3(0, -20.0f, 4.0f));
		wall5->SetRotation(glm::vec3(0, 0.0f, 0));
		wall5->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

		RenderComponent::Sptr renderer = wall5->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall2"));
	}

	GameObject::Sptr wall6 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 6");
	{
		wall6->SetPosition(glm::vec3(0, -20.0f, 4.0f));
		wall6->SetRotation(glm::vec3(0, 0.0f, 0));
		wall6->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

		RenderComponent::Sptr renderer = wall6->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall2"));
	}


	GameObject::Sptr wall7 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 7");
	{
		wall7->SetPosition(glm::vec3(0, -20.0f, 4.0f));
		wall7->SetRotation(glm::vec3(0, 0.0f, 0));
		wall7->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

		RenderComponent::Sptr renderer = wall7->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall2"));
	}


	GameObject::Sptr wall8 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 8");
	{
		wall8->SetPosition(glm::vec3(0, -20.0f, 4.0f));
		wall8->SetRotation(glm::vec3(0, 0.0f, 0));
		wall8->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

		RenderComponent::Sptr renderer = wall8->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall2"));
	}


	GameObject::Sptr wall9 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 9");
	{
		wall9->SetPosition(glm::vec3(0, -20.0f, 4.0f));
		wall9->SetRotation(glm::vec3(0, 0.0f, 0));
		wall9->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

		RenderComponent::Sptr renderer = wall9->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall2"));
	}


	GameObject::Sptr wall10 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 10");
	{
		wall10->SetPosition(glm::vec3(0, -20.0f, 4.0f));
		wall10->SetRotation(glm::vec3(0, 0.0f, 0));
		wall10->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

		RenderComponent::Sptr renderer = wall10->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall2"));
	}

	GameObject::Sptr wall11 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 11");
	{
		wall11->SetPosition(glm::vec3(0, -20.0f, 4.0f));
		wall11->SetRotation(glm::vec3(0, 0.0f, 0));
		wall11->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

		RenderComponent::Sptr renderer = wall11->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall2"));
	}

	GameObject::Sptr wall16 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 16");
	{
		wall16->SetPosition(glm::vec3(0, -20.0f, 4.0f));
		wall16->SetRotation(glm::vec3(0, 0.0f, 0));
		wall16->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

		RenderComponent::Sptr renderer = wall16->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall2"));
	}

	GameObject::Sptr wall12 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 12");
	{
		wall12->SetPosition(glm::vec3(0, -20.0f, 4.0f));
		wall12->SetRotation(glm::vec3(0, 0.0f, 0));
		wall12->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

		RenderComponent::Sptr renderer = wall12->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall2"));
	}

	GameObject::Sptr wall13 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 13");
	{
		wall13->SetPosition(glm::vec3(0, -20.0f, 4.0f));
		wall13->SetRotation(glm::vec3(0, 0.0f, 0));
		wall13->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

		RenderComponent::Sptr renderer = wall13->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall2"));
	}

	GameObject::Sptr wall14 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 14");
	{
		wall14->SetPosition(glm::vec3(0, -20.0f, 4.0f));
		wall14->SetRotation(glm::vec3(0, 0.0f, 0));
		wall14->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

		RenderComponent::Sptr renderer = wall14->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall2"));
	}

	GameObject::Sptr wall15 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 15");
	{
		wall15->SetPosition(glm::vec3(0, -20.0f, 4.0f));
		wall15->SetRotation(glm::vec3(0, 0.0f, 0));
		wall15->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

		RenderComponent::Sptr renderer = wall15->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Wall2"));
		renderer->SetMaterial(Resources::GetMaterial("Wall2"));
	}
	




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