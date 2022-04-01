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
#include "Audio/AudioSource.h"

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

#include "Audio/AudioManager.h"

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

	SceneManager::GetCurrentScene()->Lights[1].Position = glm::vec3(0.0f, 0.0f, 40.0f);
	SceneManager::GetCurrentScene()->Lights[1].Color = glm::vec3(0.5f, 0.7f, 1.0f);
	SceneManager::GetCurrentScene()->Lights[1].Range = 3000.0f;


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
		body->SetPosition(glm::vec3(0.0f, -40.0f, 5.4f));
		body->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		body->SetScale(glm::vec3(0.15));

		RenderComponent::Sptr renderer = body->Add<RenderComponent>();

		renderer->SetMesh(Resources::GetMesh("Character"));
		renderer->SetMaterial(Resources::GetMaterial("Character"));

		AnimatorComponent::Sptr animator = body->Add<AnimatorComponent>();
		animator->AddAnimation("Walk", Resources::GetAnimation("Character Walk"));
		animator->AddAnimation("Idle", Resources::GetAnimation("Character Idle"));
		animator->AddAnimation("Jump", Resources::GetAnimation("Character Jump"));

		animator->SetRenderer(*renderer);
		animator->SetAnimation("Idle");
		animator->SetLooping(true);
		animator->SetPause(false);
		animator->SetSpeed(4.0f);

		AudioSource::Sptr audio = body->Add<AudioSource>();
		{
			audio->playOnAwake = false;
			/*audio->m_Resource = Resources::GetSound("Walk");
			audio->m_Settings = AudioSettings{
				false, false, false
			};*/
			audio->Init();
		}

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

		body->Add<HealthComponent>(100);
	}

	GameObject::Sptr shadow = SceneManager::GetCurrentScene()->CreateGameObject("Shadow");
	{
		// Set position in the SceneManager::GetCurrentScene()
		shadow->SetPosition(glm::vec3(0.0f, -40.0f, 5.4f));
		shadow->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		shadow->SetScale(glm::vec3(0.15));

		RenderComponent::Sptr renderer = shadow->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Character"));
		renderer->SetMaterial(Resources::GetMaterial("Shadow"));

		AnimatorComponent::Sptr animator = shadow->Add<AnimatorComponent>();
		animator->AddAnimation("Walk", Resources::GetAnimation("Character Walk"));
		animator->AddAnimation("Idle", Resources::GetAnimation("Character Idle"));
		animator->AddAnimation("Jump", Resources::GetAnimation("Character Jump"));

		animator->SetRenderer(*renderer);
		animator->SetAnimation("Idle");
		animator->SetLooping(true);
		animator->SetPause(false);
		animator->SetSpeed(4.0f);

		AudioSource::Sptr audio = shadow->Add<AudioSource>();
		{
			audio->playOnAwake = false;
			/*audio->m_Resource = Resources::GetSound("Walk");
			audio->m_Settings = AudioSettings{
				false, false, false
			};*/
			audio->Init();
		}
		RigidBody::Sptr physics = shadow->Add<RigidBody>(RigidBodyType::Dynamic);
		BoxCollider::Sptr collider = BoxCollider::Create();
		physics->AddCollider(collider);
		physics->SetCollisionGroup(SHADOW_GROUP);
		physics->SetCollisionMask(SHADOW_MASK);

		shadow->Add<HealthComponent>(100);
	}

	SceneManager::GetCurrentScene()->PC.Initialize(*body, *shadow, *camera, SceneManager::GetCurrentScene()->Lights[0], *body->Get<TriggerVolume>());

	/////////////////////////////////////////////////////////
	//					ENVIRONMENT
	/////////////////////////////////////////////////////////

#pragma region Floors
	GameObject::Sptr floor = SceneManager::GetCurrentScene()->CreateGameObject("Floor0");
	{
		MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
		tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(50.0), glm::vec2(8.0f)));
		tiledMesh->GenerateMesh();

		floor->SetPosition(glm::vec3(0.3f, -35.0f, 5.4f));
		floor->SetScale(glm::vec3(0.67f, 0.65f, 1.0f));

		RenderComponent::Sptr renderer = floor->Add<RenderComponent>();
		renderer->SetMesh(tiledMesh);
		renderer->SetMaterial(Resources::GetMaterial("StoneTex")); 

		RigidBody::Sptr physics = floor->Add<RigidBody>();
		physics->AddCollider(BoxCollider::Create(glm::vec3(16.75f, 16.25f, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3});  
		//divide by 2 for the values
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr floor1 = SceneManager::GetCurrentScene()->CreateGameObject("Floor1");
	{
		MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
		tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(50.0), glm::vec2(8.0f)));
		tiledMesh->GenerateMesh();

		floor1->SetPosition(glm::vec3(31.0f, -33.0f, 1.0f));
		floor1->SetScale(glm::vec3(0.75f, 0.65f, 1.0f));

		RenderComponent::Sptr renderer = floor1->Add<RenderComponent>();
		renderer->SetMesh(tiledMesh);
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = floor1->Add<RigidBody>(); 
		physics->AddCollider(BoxCollider::Create(glm::vec3(18.75f, 16.25f, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
		//divide by 2 for the values
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr floor2 = SceneManager::GetCurrentScene()->CreateGameObject("Floor2");
	{
		MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
		tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(50.0), glm::vec2(8.0f)));
		tiledMesh->GenerateMesh();

		floor2->SetPosition(glm::vec3(59.0f, -33.5f, 5.5f));
		floor2->SetScale(glm::vec3(0.5f, 0.6f, 1.0f));

		RenderComponent::Sptr renderer = floor2->Add<RenderComponent>();
		renderer->SetMesh(tiledMesh);
		renderer->SetMaterial(Resources::GetMaterial("StoneTex"));

		RigidBody::Sptr physics = floor2->Add<RigidBody>(); 
		physics->AddCollider(BoxCollider::Create(glm::vec3(12.5f, 15.0f, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
		//divide by 2 for the values
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr floor3 = SceneManager::GetCurrentScene()->CreateGameObject("Floor3");
	{
		MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
		tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(50.0), glm::vec2(8.0f)));
		tiledMesh->GenerateMesh();

		floor3->SetPosition(glm::vec3(-43.0f, -7.0f, 5.0f));
		floor3->SetScale(glm::vec3(0.6f, 0.72f, 1.0f));

		RenderComponent::Sptr renderer = floor3->Add<RenderComponent>();
		renderer->SetMesh(tiledMesh);
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = floor3->Add<RigidBody>(); 
		physics->AddCollider(BoxCollider::Create(glm::vec3(15.0f, 18, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
		//divide by 2 for the values
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr floor4 = SceneManager::GetCurrentScene()->CreateGameObject("Floor4");
	{
		MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
		tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(50.0), glm::vec2(8.0f)));
		tiledMesh->GenerateMesh();

		floor4->SetPosition(glm::vec3(-56.0f, 27.0f, 13.0f));
		floor4->SetScale(glm::vec3(0.85f, 0.6f, 1.0f));

		RenderComponent::Sptr renderer = floor4->Add<RenderComponent>();
		renderer->SetMesh(tiledMesh);
		renderer->SetMaterial(Resources::GetMaterial("StoneTex"));

		RigidBody::Sptr physics = floor4->Add<RigidBody>();
		physics->AddCollider(BoxCollider::Create(glm::vec3(21.25f, 15, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
		//divide by 2 for the values
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr floor5 = SceneManager::GetCurrentScene()->CreateGameObject("Floor5");
	{
		MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
		tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(50.0), glm::vec2(8.0f)));
		tiledMesh->GenerateMesh();

		floor5->SetPosition(glm::vec3(-13.0f, -2.5f, 5.0f));
		floor5->SetScale(glm::vec3(0.6f, 0.72f, 1.0f));

		RenderComponent::Sptr renderer = floor5->Add<RenderComponent>();
		renderer->SetMesh(tiledMesh);
		renderer->SetMaterial(Resources::GetMaterial("StoneTex"));

		RigidBody::Sptr physics = floor5->Add<RigidBody>();
		physics->AddCollider(BoxCollider::Create(glm::vec3(15.0f, 18, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
		//divide by 2 for the values
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr floor6 = SceneManager::GetCurrentScene()->CreateGameObject("Floor6");
	{
		MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
		tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(50.0), glm::vec2(8.0f)));
		tiledMesh->GenerateMesh();

		floor6->SetPosition(glm::vec3(17.0f, -2.5f, 5.0f));
		floor6->SetScale(glm::vec3(0.6f, 0.72f, 1.0f));

		RenderComponent::Sptr renderer = floor6->Add<RenderComponent>(); 
		renderer->SetMesh(tiledMesh);
		renderer->SetMaterial(Resources::GetMaterial("StoneTex"));

		RigidBody::Sptr physics = floor6->Add<RigidBody>();
		physics->AddCollider(BoxCollider::Create(glm::vec3(15.0f, 18, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
		//divide by 2 for the values
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr floor7 = SceneManager::GetCurrentScene()->CreateGameObject("Floor7");
	{
		MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
		tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(50.0), glm::vec2(8.0f)));
		tiledMesh->GenerateMesh();

		floor7->SetPosition(glm::vec3(47.0f, -2.5f, 5.0f));
		floor7->SetScale(glm::vec3(0.6f, 0.72f, 1.0f));

		RenderComponent::Sptr renderer = floor7->Add<RenderComponent>();
		renderer->SetMesh(tiledMesh);
		renderer->SetMaterial(Resources::GetMaterial("StoneTex"));

		RigidBody::Sptr physics = floor7->Add<RigidBody>();
		physics->AddCollider(BoxCollider::Create(glm::vec3(15.0f, 18, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
		//divide by 2 for the values
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr floor8 = SceneManager::GetCurrentScene()->CreateGameObject("Floor8");
	{
		MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
		tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(50.0), glm::vec2(8.0f)));
		tiledMesh->GenerateMesh();

		floor8->SetPosition(glm::vec3(-13.5f, 27.0f, 13.0f)); 
		floor8->SetScale(glm::vec3(0.85f, 0.6f, 1.0f)); 

		RenderComponent::Sptr renderer = floor8->Add<RenderComponent>();
		renderer->SetMesh(tiledMesh);
		renderer->SetMaterial(Resources::GetMaterial("StoneTex"));

		RigidBody::Sptr physics = floor8->Add<RigidBody>(); 
		physics->AddCollider(BoxCollider::Create(glm::vec3(21.25f, 15, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
		//divide by 2 for the values
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr floor9 = SceneManager::GetCurrentScene()->CreateGameObject("Floor9");
	{
		MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
		tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(50.0), glm::vec2(8.0f)));
		tiledMesh->GenerateMesh();

		floor9->SetPosition(glm::vec3(29.0f, 27.0f, 13.0f));
		floor9->SetScale(glm::vec3(0.85f, 0.6f, 1.0f));

		RenderComponent::Sptr renderer = floor9->Add<RenderComponent>();
		renderer->SetMesh(tiledMesh);
		renderer->SetMaterial(Resources::GetMaterial("StoneTex"));

		RigidBody::Sptr physics = floor9->Add<RigidBody>();
		physics->AddCollider(BoxCollider::Create(glm::vec3(21.25f, 15, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
		//divide by 2 for the values
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr floor10 = SceneManager::GetCurrentScene()->CreateGameObject("Floor10");
	{
		MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
		tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(50.0), glm::vec2(8.0f)));
		tiledMesh->GenerateMesh();

		floor10->SetPosition(glm::vec3(-33.0f, -29.8f, 15.0f)); 
		floor10->SetScale(glm::vec3(0.67f, 0.4f, 1.0f)); 

		RenderComponent::Sptr renderer = floor10->Add<RenderComponent>();
		renderer->SetMesh(tiledMesh);
		renderer->SetMaterial(Resources::GetMaterial("StoneTex"));

		RigidBody::Sptr physics = floor10->Add<RigidBody>(); 
		physics->AddCollider(BoxCollider::Create(glm::vec3(16.75f, 10.0f, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
		//divide by 2 for the values
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr floor11 = SceneManager::GetCurrentScene()->CreateGameObject("Floor11");
	{
		MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
		tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(50.0), glm::vec2(8.0f)));
		tiledMesh->GenerateMesh();

		floor11->SetPosition(glm::vec3(-24.77f, -4.0f, 15.0f));
		floor11->SetScale(glm::vec3(0.36f, 0.63f, 1.0f));

		RenderComponent::Sptr renderer = floor11->Add<RenderComponent>();
		renderer->SetMesh(tiledMesh);
		renderer->SetMaterial(Resources::GetMaterial("Brown"));

		RigidBody::Sptr physics = floor11->Add<RigidBody>();
		physics->AddCollider(BoxCollider::Create(glm::vec3(9.0f, 15.75f, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
		//divide by 2 for the values
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	//Right Jumping Puzzle Room
	GameObject::Sptr block1 = SceneManager::GetCurrentScene()->CreateGameObject("Block 1");
	{
		block1->SetPosition(glm::vec3(26.5f, -40.0f, 4.0f));
		block1->SetScale(glm::vec3(4.0f, 2.5f, 0.8f));

		RenderComponent::Sptr renderer = block1->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Gray"));

		RigidBody::Sptr physics = block1->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block1->GetScale()); 
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);  
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);  
	}

	GameObject::Sptr block2 = SceneManager::GetCurrentScene()->CreateGameObject("Block 2");
	{
		block2->SetPosition(glm::vec3(40.0f, -40.0f, 4.0f));
		block2->SetScale(glm::vec3(4.0f, 2.5f, 0.8f));

		RenderComponent::Sptr renderer = block2->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Gray"));

		RigidBody::Sptr physics = block2->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block2->GetScale()); 
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	//Left Jumping Puzzle Room
	GameObject::Sptr block3 = SceneManager::GetCurrentScene()->CreateGameObject("block3");
	{
		block3->SetPosition(glm::vec3(-40.0f, 9.5f, 5.5f));
		block3->SetScale(glm::vec3(2.0f, 2.0f, 1.0f));

		RenderComponent::Sptr renderer = block3->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Gray"));

		RigidBody::Sptr physics = block3->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block3->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block4 = SceneManager::GetCurrentScene()->CreateGameObject("block4");
	{
		block4->SetPosition(glm::vec3(-47.0f, 9.5f, 6.6f));
		block4->SetScale(glm::vec3(4.25f, 2.0f, 1.0f));

		RenderComponent::Sptr renderer = block4->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Gray"));

		RigidBody::Sptr physics = block4->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block4->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block5 = SceneManager::GetCurrentScene()->CreateGameObject("block5");
	{
		block5->SetPosition(glm::vec3(-55.0f, 0.0f, 7.5f));
		block5->SetScale(glm::vec3(2.0f, 4.5f, 1.0f));

		RenderComponent::Sptr renderer = block5->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Gray"));

		RigidBody::Sptr physics = block5->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block5->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block6 = SceneManager::GetCurrentScene()->CreateGameObject("block6");
	{
		block6->SetPosition(glm::vec3(-54.5f, -12.0f, 8.5f));
		block6->SetScale(glm::vec3(2.0f, 4.0f, 1.0f));

		RenderComponent::Sptr renderer = block6->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Gray"));

		RigidBody::Sptr physics = block6->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block6->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block7 = SceneManager::GetCurrentScene()->CreateGameObject("block7");
	{
		block7->SetPosition(glm::vec3(-46.0f, -14.0f, 10.5f));
		block7->SetRotation(glm::vec3(0.0f, -11.0f, 0.0f));
		block7->SetScale(glm::vec3(7.0f, 0.7f, 0.2f));

		RenderComponent::Sptr renderer = block7->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Gray"));

		RigidBody::Sptr physics = block7->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block7->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr block8 = SceneManager::GetCurrentScene()->CreateGameObject("block8"); 
	{
		block8->SetPosition(glm::vec3(-37.0f, -15.5f, 12.0f));
		block8->SetScale(glm::vec3(2.0f, 2.2f, 1.0f));

		RenderComponent::Sptr renderer = block8->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Cube"));
		renderer->SetMaterial(Resources::GetMaterial("Gray"));

		RigidBody::Sptr physics = block8->Add<RigidBody>(RigidBodyType::Static);
		BoxCollider::Sptr collider = BoxCollider::Create();
		collider->SetScale(block8->GetScale());
		collider->SetPosition(collider->GetPosition());
		physics->AddCollider(collider);
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}
	 
#pragma endregion

#pragma region WALLS

	GameObject::Sptr wall1 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 1");
	{
		wall1->SetPosition(glm::vec3(-8.590, -20.0f, 4.0f));
		wall1->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall1->SetScale(glm::vec3(1.0f, 1.0f, 1.67f));

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

	/*
	GameObject::Sptr wall10 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 10");
	{
		wall10->SetPosition(glm::vec3(18.0f, -49.75f, 4.0f));
		wall10->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall10->SetScale(glm::vec3(1.0f, 1.0f, 0.125f));

		RenderComponent::Sptr renderer = wall10->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}
	*/


	GameObject::Sptr wall11 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 11");
	{
		wall11->SetPosition(glm::vec3(44.91f, -49.66f, 4.0f));
		wall11->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall11->SetScale(glm::vec3(1.0f, 1.0f, 1.73f));

		RenderComponent::Sptr renderer = wall11->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall12 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 12");
	{
		wall12->SetPosition(glm::vec3(69.58f, -34.66f, 4.0f));
		wall12->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall12->SetScale(glm::vec3(1.0f, 1.0f, 0.92f));

		RenderComponent::Sptr renderer = wall12->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall13 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 13");
	{
		wall13->SetPosition(glm::vec3(64.32f, -19.78f, 4.0f));
		wall13->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall13->SetScale(glm::vec3(1.0f, 1.0f, 0.4f));

		RenderComponent::Sptr renderer = wall13->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall14 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 14");
	{
		wall14->SetPosition(glm::vec3(47.41f, -29.4f, 4.0f));
		wall14->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall14->SetScale(glm::vec3(1.0f, 1.0f, 0.55f));

		RenderComponent::Sptr renderer = wall14->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}
	/*
	GameObject::Sptr wall15 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 15");
	{
		wall15->SetPosition(glm::vec3(49.5f, -23.0f, 4.0f));
		wall15->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall15->SetScale(glm::vec3(1.0f, 1.0f, 0.225f));

		RenderComponent::Sptr renderer = wall15->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}
	*/

	GameObject::Sptr wall16 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 16");
	{
		wall16->SetPosition(glm::vec3(37.5f, 3.0f, 4));
		wall16->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall16->SetScale(glm::vec3(1.0f, 1.0f, 1.65f));

		RenderComponent::Sptr renderer = wall16->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}
	/*
	GameObject::Sptr wall17 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 17");
	{
		wall17->SetPosition(glm::vec3(33.0f, -36.25f, 4.0f));
		wall17->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall17->SetScale(glm::vec3(1.0f, 1.0f, 1.02f));

		RenderComponent::Sptr renderer = wall17->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}
	*/

	GameObject::Sptr wall18 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 18");
	{
		wall18->SetPosition(glm::vec3(64.0f, -7.0f, 4.0f));
		wall18->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall18->SetScale(glm::vec3(1.0f, 1.0f, 0.75f));

		RenderComponent::Sptr renderer = wall18->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	/*
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

	*/

	GameObject::Sptr wall22 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 22");
	{
		wall22->SetPosition(glm::vec3(-10.740f, 2.75f, 4.0f));
		wall22->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall22->SetScale(glm::vec3(1.0f, 1.0f, 1.48f));

		RenderComponent::Sptr renderer = wall22->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}
	/*
	GameObject::Sptr wall23 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 23");
	{
		wall23->SetPosition(glm::vec3(-13.75f, -17.96f, 4.0f));
		wall23->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall23->SetScale(glm::vec3(1.0f, 1.0f, 1.55f));

		RenderComponent::Sptr renderer = wall23->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}



	/*
	GameObject::Sptr wall24 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 24");
	{
		wall24->SetPosition(glm::vec3(-46.5f, 10.5f, 9.75f));
		wall24->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall24->SetScale(glm::vec3(1.0f, 1.35f, 0.75f));

		RenderComponent::Sptr renderer = wall24->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}
	*/



	GameObject::Sptr wall25 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 25");
	{
		wall25->SetPosition(glm::vec3(-34.91f, 2.75f, 9.75f));
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
		wall27->SetPosition(glm::vec3(-34.830, -24.5f, 16.75f));
		wall27->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall27->SetScale(glm::vec3(1.0f, 0.5f, 0.605f));

		RenderComponent::Sptr renderer = wall27->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall28 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 28");
	{
		wall28->SetPosition(glm::vec3(-53.36, -24.5f, 16.75f));
		wall28->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall28->SetScale(glm::vec3(1.0f, 0.5f, 0.235f));

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
		wall30->SetPosition(glm::vec3(-35.02f, -18.5f, 9.75f));
		wall30->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall30->SetScale(glm::vec3(1.0f, 1.35f, 0.4f));

		RenderComponent::Sptr renderer = wall30->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall31 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 31");
	{
		wall31->SetPosition(glm::vec3(-50.700f, -32.45f, 12.54f));
		wall31->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall31->SetScale(glm::vec3(1.0f, 1.0f, 0.43f));

		RenderComponent::Sptr renderer = wall31->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall32 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 32");
	{
		wall32->SetPosition(glm::vec3(-41.01, -40.04f, 12.5f));
		wall32->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall32->SetScale(glm::vec3(1.0f, 1.0f, 0.65f));

		RenderComponent::Sptr renderer = wall32->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}


	/*
	GameObject::Sptr wall33 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 33");
	{
		wall33->SetPosition(glm::vec3(-43.5f, -27.75f, 12.5f));
		wall33->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall33->SetScale(glm::vec3(1.0f, 1.0f, 0.15f));

		RenderComponent::Sptr renderer = wall33->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	*/

	/*
	GameObject::Sptr wall34 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 34");
	{
		wall34->SetPosition(glm::vec3(-36.75f, -28.75f, 12.5f));
		wall34->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall34->SetScale(glm::vec3(1.0f, 1.0f, 0.35f));

		RenderComponent::Sptr renderer = wall34->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	*/

	GameObject::Sptr wall35 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 35");
	{
		wall35->SetPosition(glm::vec3(-32.32, -45.68f, 9.62f));
		wall35->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall35->SetScale(glm::vec3(1.0f, 1.35f, 0.29f));

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
		wall37->SetPosition(glm::vec3(-16.32, -32.58f, 9.75f));
		wall37->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall37->SetScale(glm::vec3(1.0f, 1.35f, 0.56f));

		RenderComponent::Sptr renderer = wall37->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	/*
	GameObject::Sptr wall38 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 38");
	{
		wall38->SetPosition(glm::vec3(-32.75f, -26, 12.5f));
		wall38->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall38->SetScale(glm::vec3(1.0f, 1.0f, 0.15f));

		RenderComponent::Sptr renderer = wall38->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}
	*/

	/*
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
	*/
	GameObject::Sptr wall41 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 41");
	{
		wall41->SetPosition(glm::vec3(-32.86f, -6.53f, 15.72f));
		wall41->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall41->SetScale(glm::vec3(1.0f, 0.65f, 1.1f));

		RenderComponent::Sptr renderer = wall41->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	/*
	GameObject::Sptr wall42 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 42");
	{
		wall42->SetPosition(glm::vec3(-34.5f, 10.25f, 12.5f));
		wall42->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall42->SetScale(glm::vec3(1.0f, 1.0f, 0.2f));

		RenderComponent::Sptr renderer = wall42->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}
	*/

	/*
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

	*/

	GameObject::Sptr wall45 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 45");
	{
		wall45->SetPosition(glm::vec3(-24.78f, 31.22f, 12.5f));
		wall45->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall45->SetScale(glm::vec3(1.00f, 0.94f, 0.95f));

		RenderComponent::Sptr renderer = wall45->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	/*

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

	*/

	GameObject::Sptr wall49 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 49");
	{
		wall49->SetPosition(glm::vec3(-16.46f, -6.91f, 15.72f));
		wall49->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall49->SetScale(glm::vec3(1.0f, 0.65f, 1.09f));

		RenderComponent::Sptr renderer = wall49->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	/*
	GameObject::Sptr wall50 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 50");
	{
		wall50->SetPosition(glm::vec3(-19.5f, -23.0f, 12.5f));
		wall50->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall50->SetScale(glm::vec3(1.0f, 1.0f, 0.125f));

		RenderComponent::Sptr renderer = wall50->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}
	*/

	GameObject::Sptr wall51 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 51");
	{
		wall51->SetPosition(glm::vec3(-51.82f, 11.83f, 8.73f));
		wall51->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall51->SetScale(glm::vec3(1.0f, 1.48f, 1.35f));

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
		wall53->SetPosition(glm::vec3(7.64f, 32.1f, 12.5f));
		wall53->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall53->SetScale(glm::vec3(1.0f, 1.0f, 1.14f));

		RenderComponent::Sptr renderer = wall53->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

	}

	GameObject::Sptr wall54 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 54");
	{
		wall54->SetPosition(glm::vec3(5.1f, 11.190f, 12.5f));
		wall54->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall54->SetScale(glm::vec3(1.0f, 1.0f, 1.410f));

		RenderComponent::Sptr renderer = wall54->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	/*
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

	*/

	GameObject::Sptr wall57 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 57");
	{
		wall57->SetPosition(glm::vec3(38.530f, 11.1f, 12.5f));
		wall57->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall57->SetScale(glm::vec3(1.0f, 1.0f, 0.765f));

		RenderComponent::Sptr renderer = wall57->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall58 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 58");
	{
		wall58->SetPosition(glm::vec3(38.0f, 32.04f, 12.5f));
		wall58->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall58->SetScale(glm::vec3(1.0f, 1.0f, 0.85f));

		RenderComponent::Sptr renderer = wall58->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall60 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 60");
	{
		wall60->SetPosition(glm::vec3(51.310f, 21.5f, 12.5f));
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
		wall59->SetPosition(glm::vec3(32.810f, -36.58f, 4.5f));
		wall59->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
		wall59->SetScale(glm::vec3(1.0f, 1.0f, 0.89f));

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
			cageDoor_1->SetPosition(glm::vec3(55.630f, -19.510f, 4.5f));
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
			pressure_plate_1->SetScale(glm::vec3(0.8f, 0.5f, 0.8f));

			// Renderer
			RenderComponent::Sptr renderer = pressure_plate_1->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Pressure Plate"));
			renderer->SetMaterial(Resources::GetMaterial("Pressure Plate"));

			// Trigger Volume
			TriggerVolume::Sptr volume = pressure_plate_1->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
			collider->SetScale(glm::vec3(2.0f, 1.8f, 2.0f));
			volume->AddCollider(collider);
			volume->SetCollisionGroup(PHYSICAL_GROUP);
			volume->SetCollisionMask(PHYSICAL_MASK);

			AudioSource::Sptr audio = pressure_plate_1->Add<AudioSource>();
			{
				audio->playOnAwake = false;
				audio->LoadEvent("Pressure Plate");
				audio->volume = 0.75f;
				/*audio->m_Resource = Resources::GetSound("Click");
				audio->m_Settings = AudioSettings{
					false, false, false
				};*/
				audio->Init();
			}

			// Trigger Event
			TriggerVolumeEnterBehaviour::Sptr trigger = pressure_plate_1->Add<TriggerVolumeEnterBehaviour>();
			trigger->onTriggerEnterEvent = [pressure_plate_1, cageDoor_1, audio, NO_GROUP, NO_MASK]
			{					
				// Open The Door
				cageDoor_1->Get<RenderComponent>()->IsEnabled = false;
				RigidBody::Sptr rigid = cageDoor_1->Get<RigidBody>();
				rigid->SetCollisionGroup(NO_GROUP);
				rigid->SetCollisionMask(NO_MASK);	

				//AudioEngine::Instance().GetEvent("Pressure Plate").SetPosition(pressure_plate_1->GetPosition());
				AudioEngine::Instance().GetEvent("Pressure Plate").SetParameter("Powered", 0);
				//AudioEngine::Instance().GetEvent("Pressure Plate").Play();

				audio->Play();
			};

			trigger->onTriggerExitEvent = [pressure_plate_1, cageDoor_1, audio, PHYSICAL_GROUP, SHADOW_GROUP, PHYSICAL_MASK, SHADOW_MASK]
			{
				// Close The Door
				cageDoor_1->Get<RenderComponent>()->IsEnabled = true;
				RigidBody::Sptr rigid = cageDoor_1->Get<RigidBody>();				
				rigid->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				rigid->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);	

				//AudioEngine::Instance().GetEvent("Pressure Plate").SetPosition(pressure_plate_1->GetPosition());
				AudioEngine::Instance().GetEvent("Pressure Plate").SetParameter("Powered", 1);
				//AudioEngine::Instance().GetEvent("Pressure Plate").Play();

				audio->Play();
			};
		}

	}
	{
		{

			GameObject::Sptr spike_trap_1 = SceneManager::GetCurrentScene()->CreateGameObject("SpikeTrap");
			{
				spike_trap_1->SetPosition(glm::vec3(46.0f, -1.0f, 5.8f));
				spike_trap_1->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
				spike_trap_1->SetScale(glm::vec3(0.6));

				RenderComponent::Sptr renderer = spike_trap_1->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Spike Trap"));
				renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

				// Animator
				AnimatorComponent::Sptr animator = spike_trap_1->Add<AnimatorComponent>();
				animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
				animator->SetRenderer(*renderer);
				animator->SetLooping(false);

				// Trigger Volume
				TriggerVolume::Sptr volume = spike_trap_1->Add<TriggerVolume>();
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
				collider->SetScale(glm::vec3(2.0f, 1.8f, 2.0f));
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

				AudioSource::Sptr audio = spike_trap_1->Add<AudioSource>();
				{
					audio->playOnAwake = false;
					audio->LoadEvent("Spikes");
					audio->volume = 0.5f;
					/*audio->m_Resource = Resources::GetSound("Spike Trap");
					audio->m_Settings = AudioSettings{
						true, false, false
					};*/
					audio->Init();
				}

				// Spike Behavior
				SpikeTrapBehavior::Sptr spikeBehavior = spike_trap_1->Add<SpikeTrapBehavior>();
				spikeBehavior->SetAnimator(animator);
				spikeBehavior->SetTrigger(volume);
				spikeBehavior->Initialize(3, 0, false);

			}

			GameObject::Sptr spike_trap_2 = SceneManager::GetCurrentScene()->CreateGameObject("SpikeTrap");
			{
				spike_trap_2->SetPosition(glm::vec3(46.0f, -6.5f, 5.8f));
				spike_trap_2->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				spike_trap_2->SetScale(glm::vec3(0.6));


				RenderComponent::Sptr renderer = spike_trap_2->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Spike Trap"));
				renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

				// Animator
				AnimatorComponent::Sptr animator = spike_trap_2->Add<AnimatorComponent>();
				animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
				animator->SetRenderer(*renderer);
				animator->SetLooping(false);

				// Trigger Volume
				TriggerVolume::Sptr volume = spike_trap_2->Add<TriggerVolume>();
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
				collider->SetScale(glm::vec3(2.0f, 1.8f, 2.0f));
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

				AudioSource::Sptr audio = spike_trap_2->Add<AudioSource>();
				{
					audio->playOnAwake = false;
					audio->LoadEvent("Spikes");
					audio->volume = 0.5f;
					/*audio->m_Resource = Resources::GetSound("Spike Trap");
					audio->m_Settings = AudioSettings{
						true, false, false
					};*/
					audio->Init();
				}

				// Spike Behavior
				SpikeTrapBehavior::Sptr spikeBehavior = spike_trap_2->Add<SpikeTrapBehavior>();
				spikeBehavior->SetAnimator(animator);
				spikeBehavior->SetTrigger(volume);
				spikeBehavior->Initialize(3, 0, false);
			}

			GameObject::Sptr spike_trap_3 = SceneManager::GetCurrentScene()->CreateGameObject("SpikeTrap");
			{
				spike_trap_3->SetPosition(glm::vec3(46.0f, -12.0f, 5.8f));
				spike_trap_3->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
				spike_trap_3->SetScale(glm::vec3(0.6));


				RenderComponent::Sptr renderer = spike_trap_3->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Spike Trap"));
				renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

				// Animator
				AnimatorComponent::Sptr animator = spike_trap_3->Add<AnimatorComponent>();
				animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
				animator->SetRenderer(*renderer);
				animator->SetLooping(false);

				// Trigger Volume
				TriggerVolume::Sptr volume = spike_trap_3->Add<TriggerVolume>();
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
				collider->SetScale(glm::vec3(2.0f, 1.8f, 2.0f));
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

				AudioSource::Sptr audio = spike_trap_3->Add<AudioSource>();
				{
					audio->playOnAwake = false;
					audio->LoadEvent("Spikes");
					audio->volume = 0.5f;
					/*audio->m_Resource = Resources::GetSound("Spike Trap");
					audio->m_Settings = AudioSettings{
						true, false, false
					};*/
					audio->Init();
				}

				// Spike Behavior
				SpikeTrapBehavior::Sptr spikeBehavior = spike_trap_3->Add<SpikeTrapBehavior>();
				spikeBehavior->SetAnimator(animator);
				spikeBehavior->SetTrigger(volume);
				spikeBehavior->Initialize(3, 0, false);
			}

			GameObject::Sptr spike_trap_x = SceneManager::GetCurrentScene()->CreateGameObject("SpikeTrap");
			{
				spike_trap_x->SetPosition(glm::vec3(46.0f, -17.5f, 5.8f));
				spike_trap_x->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
				spike_trap_x->SetScale(glm::vec3(0.6));


				RenderComponent::Sptr renderer = spike_trap_x->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Spike Trap"));
				renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

				// Animator
				AnimatorComponent::Sptr animator = spike_trap_x->Add<AnimatorComponent>();
				animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
				animator->SetRenderer(*renderer);
				animator->SetLooping(false);

				// Trigger Volume
				TriggerVolume::Sptr volume = spike_trap_x->Add<TriggerVolume>();
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
				collider->SetScale(glm::vec3(2.0f, 1.8f, 2.0f));
				volume->AddCollider(collider);
				volume->SetCollisionGroup(PHYSICAL_GROUP);
				volume->SetCollisionMask(PHYSICAL_MASK);

				// Trigger Event
				TriggerVolumeEnterBehaviour::Sptr trigger = spike_trap_x->Add<TriggerVolumeEnterBehaviour>();
				trigger->onTriggerEnterEvent = [body] {
					if (!SceneManager::GetCurrentScene()->PC.isShadow) {
						body->Get<HealthComponent>()->DealDamage(10.0f);
					}
				};

				AudioSource::Sptr audio = spike_trap_x->Add<AudioSource>();
				{
					audio->playOnAwake = false;
					audio->LoadEvent("Spikes");
					audio->volume = 0.5f;
					/*audio->m_Resource = Resources::GetSound("Spike Trap");
					audio->m_Settings = AudioSettings{
						true, false, false
					};*/
					audio->Init();
				}

				// Spike Behavior
				SpikeTrapBehavior::Sptr spikeBehavior = spike_trap_x->Add<SpikeTrapBehavior>();
				spikeBehavior->SetAnimator(animator);
				spikeBehavior->SetTrigger(volume);
				spikeBehavior->Initialize(3, 0, false);
			}
		}
		
		{
			GameObject::Sptr spike_trap_4 = SceneManager::GetCurrentScene()->CreateGameObject("SpikeTrap");
			{
				spike_trap_4->SetPosition(glm::vec3(35.5f, -1.0f, 5.8f));
				spike_trap_4->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
				spike_trap_4->SetScale(glm::vec3(0.6));

	
				RenderComponent::Sptr renderer = spike_trap_4->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Spike Trap"));
				renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

				// Animator
				AnimatorComponent::Sptr animator = spike_trap_4->Add<AnimatorComponent>();
				animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
				animator->SetRenderer(*renderer);
				animator->SetLooping(false);

				// Trigger Volume
				TriggerVolume::Sptr volume = spike_trap_4->Add<TriggerVolume>();
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
				collider->SetScale(glm::vec3(2.0f, 1.8f, 2.0f));
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

				AudioSource::Sptr audio = spike_trap_4->Add<AudioSource>();
				{
					audio->playOnAwake = false;
					audio->LoadEvent("Spikes");
					audio->volume = 0.5f;
					/*audio->m_Resource = Resources::GetSound("Spike Trap");
					audio->m_Settings = AudioSettings{
						true, false, false
					};*/
					audio->Init();
				}

				// Spike Behavior
				SpikeTrapBehavior::Sptr spikeBehavior = spike_trap_4->Add<SpikeTrapBehavior>();
				spikeBehavior->SetAnimator(animator);
				spikeBehavior->SetTrigger(volume);
				spikeBehavior->Initialize(3, 3, false);
			}

			GameObject::Sptr spike_trap_5 = SceneManager::GetCurrentScene()->CreateGameObject("SpikeTrap");
			{
				spike_trap_5->SetPosition(glm::vec3(35.5f, -6.5f, 5.8f));
				spike_trap_5->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
				spike_trap_5->SetScale(glm::vec3(0.6));

		
				RenderComponent::Sptr renderer = spike_trap_5->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Spike Trap"));
				renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

				// Animator
				AnimatorComponent::Sptr animator = spike_trap_5->Add<AnimatorComponent>();
				animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
				animator->SetRenderer(*renderer);
				animator->SetLooping(false);

				// Trigger Volume
				TriggerVolume::Sptr volume = spike_trap_5->Add<TriggerVolume>();
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
				collider->SetScale(glm::vec3(2.0f, 1.8f, 2.0f));
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

				AudioSource::Sptr audio = spike_trap_5->Add<AudioSource>();
				{
					audio->playOnAwake = false;
					audio->LoadEvent("Spikes");
					audio->volume = 0.5f;
					/*audio->m_Resource = Resources::GetSound("Spike Trap");
					audio->m_Settings = AudioSettings{
						true, false, false
					};*/
					audio->Init();
				}

				// Spike Behavior
				SpikeTrapBehavior::Sptr spikeBehavior = spike_trap_5->Add<SpikeTrapBehavior>();
				spikeBehavior->SetAnimator(animator);
				spikeBehavior->SetTrigger(volume);
				spikeBehavior->Initialize(3, 3, false);
			}

			GameObject::Sptr spike_trap_6 = SceneManager::GetCurrentScene()->CreateGameObject("SpikeTrap");
			{
				spike_trap_6->SetPosition(glm::vec3(35.5f, -12.0f, 5.8f));
				spike_trap_6->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				spike_trap_6->SetScale(glm::vec3(0.6));


				RenderComponent::Sptr renderer = spike_trap_6->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Spike Trap"));
				renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

				// Animator
				AnimatorComponent::Sptr animator = spike_trap_6->Add<AnimatorComponent>();
				animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
				animator->SetRenderer(*renderer);
				animator->SetLooping(false);

				// Trigger Volume
				TriggerVolume::Sptr volume = spike_trap_6->Add<TriggerVolume>();
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
				collider->SetScale(glm::vec3(2.0f, 1.8f, 2.0f));
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

				AudioSource::Sptr audio = spike_trap_6->Add<AudioSource>();
				{
					audio->playOnAwake = false;
					audio->LoadEvent("Spikes");
					audio->volume = 0.5f;
					/*audio->m_Resource = Resources::GetSound("Spike Trap");
					audio->m_Settings = AudioSettings{
						true, false, false
					};*/
					audio->Init();
				}

				// Spike Behavior
				SpikeTrapBehavior::Sptr spikeBehavior = spike_trap_6->Add<SpikeTrapBehavior>();
				spikeBehavior->SetAnimator(animator);
				spikeBehavior->SetTrigger(volume);
				spikeBehavior->Initialize(3, 3, false);
			}

			GameObject::Sptr spike_trap_y = SceneManager::GetCurrentScene()->CreateGameObject("SpikeTrap");
			{
				spike_trap_y->SetPosition(glm::vec3(35.5f, -17.5f, 5.8f));
				spike_trap_y->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				spike_trap_y->SetScale(glm::vec3(0.6));


				RenderComponent::Sptr renderer = spike_trap_y->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Spike Trap"));
				renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

				// Animator
				AnimatorComponent::Sptr animator = spike_trap_y->Add<AnimatorComponent>();
				animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
				animator->SetRenderer(*renderer);
				animator->SetLooping(false);

				// Trigger Volume
				TriggerVolume::Sptr volume = spike_trap_y->Add<TriggerVolume>();
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
				collider->SetScale(glm::vec3(2.0f, 1.8f, 2.0f));
				volume->AddCollider(collider);
				volume->SetCollisionGroup(PHYSICAL_GROUP);
				volume->SetCollisionMask(PHYSICAL_MASK);

				// Trigger Event
				TriggerVolumeEnterBehaviour::Sptr trigger = spike_trap_y->Add<TriggerVolumeEnterBehaviour>();
				trigger->onTriggerEnterEvent = [body] {
					if (!SceneManager::GetCurrentScene()->PC.isShadow) {
						body->Get<HealthComponent>()->DealDamage(10.0f);
					}
				};

				AudioSource::Sptr audio = spike_trap_y->Add<AudioSource>();
				{
					audio->playOnAwake = false;
					audio->LoadEvent("Spikes");
					audio->volume = 0.5f;
					/*audio->m_Resource = Resources::GetSound("Spike Trap");
					audio->m_Settings = AudioSettings{
						true, false, false
					};*/
					audio->Init();
				}

				// Spike Behavior
				SpikeTrapBehavior::Sptr spikeBehavior = spike_trap_y->Add<SpikeTrapBehavior>();
				spikeBehavior->SetAnimator(animator);
				spikeBehavior->SetTrigger(volume);
				spikeBehavior->Initialize(3, 3, false);
			}
		}

		// Platform 3
		{
			GameObject::Sptr spike_trap_7 = SceneManager::GetCurrentScene()->CreateGameObject("SpikeTrap");
			{
				spike_trap_7->SetPosition(glm::vec3(26.25f, -1.0f, 5.8f));
				spike_trap_7->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
				spike_trap_7->SetScale(glm::vec3(0.6));

		
				RenderComponent::Sptr renderer = spike_trap_7->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Spike Trap"));
				renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

				// Animator
				AnimatorComponent::Sptr animator = spike_trap_7->Add<AnimatorComponent>();
				animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
				animator->SetRenderer(*renderer);
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

				AudioSource::Sptr audio = spike_trap_7->Add<AudioSource>();
				{
					audio->playOnAwake = false;
					audio->LoadEvent("Spikes");
					audio->volume = 0.5f;
					/*audio->m_Resource = Resources::GetSound("Spike Trap");
					audio->m_Settings = AudioSettings{
						true, false, false
					};*/
					audio->Init();
				}

				// Spike Behavior
				SpikeTrapBehavior::Sptr spikeBehavior = spike_trap_7->Add<SpikeTrapBehavior>();
				spikeBehavior->SetAnimator(animator);
				spikeBehavior->SetTrigger(volume);
				spikeBehavior->Initialize(3, 0, false);
			}

			GameObject::Sptr spike_trap_8 = SceneManager::GetCurrentScene()->CreateGameObject("SpikeTrap");
			{
				spike_trap_8->SetPosition(glm::vec3(26.25f, -6.5f, 5.8f));
				spike_trap_8->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
				spike_trap_8->SetScale(glm::vec3(0.6));

				RenderComponent::Sptr renderer = spike_trap_8->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Spike Trap"));
				renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

				// Animator
				AnimatorComponent::Sptr animator = spike_trap_8->Add<AnimatorComponent>();
				animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
				animator->SetRenderer(*renderer);
				animator->SetLooping(false);

				// Trigger Volume
				TriggerVolume::Sptr volume = spike_trap_8->Add<TriggerVolume>();
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
				collider->SetScale(glm::vec3(2.0f, 1.8f, 2.0f));
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

				AudioSource::Sptr audio = spike_trap_8->Add<AudioSource>();
				{
					audio->playOnAwake = false;
					audio->LoadEvent("Spikes");
					audio->volume = 0.5f;
					/*audio->m_Resource = Resources::GetSound("Spike Trap");
					audio->m_Settings = AudioSettings{
						true, false, false
					};*/
					audio->Init();
				}

				// Spike Behavior
				SpikeTrapBehavior::Sptr spikeBehavior = spike_trap_8->Add<SpikeTrapBehavior>();
				spikeBehavior->SetAnimator(animator);
				spikeBehavior->SetTrigger(volume);
				spikeBehavior->Initialize(3, 0, false);
			}

			GameObject::Sptr spike_trap_9 = SceneManager::GetCurrentScene()->CreateGameObject("SpikeTrap");
			{
				spike_trap_9->SetPosition(glm::vec3(26.25f, -12.0f, 5.8f));
				spike_trap_9->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
				spike_trap_9->SetScale(glm::vec3(0.6));

		
				RenderComponent::Sptr renderer = spike_trap_9->Add<RenderComponent>();
				renderer->SetMesh(Resources::GetMesh("Spike Trap"));
				renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

				// Animator
				AnimatorComponent::Sptr animator = spike_trap_9->Add<AnimatorComponent>();
				animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
				animator->SetRenderer(*renderer);
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

				AudioSource::Sptr audio = spike_trap_9->Add<AudioSource>();
				{
					audio->playOnAwake = false;
					audio->LoadEvent("Spikes");
					audio->volume = 0.5f;
					/*audio->m_Resource = Resources::GetSound("Spike Trap");
					audio->m_Settings = AudioSettings{
						true, false, false
					};*/
					audio->Init();
				}

				// Spike Behavior
				SpikeTrapBehavior::Sptr spikeBehavior = spike_trap_9->Add<SpikeTrapBehavior>();
				spikeBehavior->SetAnimator(animator);
				spikeBehavior->SetTrigger(volume);
				spikeBehavior->Initialize(3, 0, false);
			}

			GameObject::Sptr spike_trap_z = SceneManager::GetCurrentScene()->CreateGameObject("SpikeTrap"); 
			{
				spike_trap_z->SetPosition(glm::vec3(26.25f, -17.5f, 5.8f)); 
				spike_trap_z->SetRotation(glm::vec3(90.f, 0.0f, -90.0f)); 
				spike_trap_z->SetScale(glm::vec3(0.6)); 


				RenderComponent::Sptr renderer = spike_trap_z->Add<RenderComponent>(); 
				renderer->SetMesh(Resources::GetMesh("Spike Trap")); 
				renderer->SetMaterial(Resources::GetMaterial("Spike Trap")); 

				// Animator
				AnimatorComponent::Sptr animator = spike_trap_z->Add<AnimatorComponent>();
				animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
				animator->SetRenderer(*renderer);
				animator->SetLooping(false);

				// Trigger Volume
				TriggerVolume::Sptr volume = spike_trap_z->Add<TriggerVolume>();
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
				collider->SetScale(glm::vec3(2.0f, 1.0f, 2.0f));
				volume->AddCollider(collider);
				volume->SetCollisionGroup(PHYSICAL_GROUP);
				volume->SetCollisionMask(PHYSICAL_MASK);

				// Trigger Event
				TriggerVolumeEnterBehaviour::Sptr trigger = spike_trap_z->Add<TriggerVolumeEnterBehaviour>();
				trigger->onTriggerEnterEvent = [body] {
					if (!SceneManager::GetCurrentScene()->PC.isShadow) {
						body->Get<HealthComponent>()->DealDamage(10.0f);
					}
				};

				AudioSource::Sptr audio = spike_trap_z->Add<AudioSource>();
				{
					audio->playOnAwake = false;
					audio->LoadEvent("Spikes");
					audio->volume = 0.5f;
					/*audio->m_Resource = Resources::GetSound("Spike Trap");
					audio->m_Settings = AudioSettings{
						true, false, false
					};*/
					audio->Init();
				}

				// Spike Behavior
				SpikeTrapBehavior::Sptr spikeBehavior = spike_trap_z->Add<SpikeTrapBehavior>();
				spikeBehavior->SetAnimator(animator);
				spikeBehavior->SetTrigger(volume);
				spikeBehavior->Initialize(3, 0, false);
			}


		}	
	}

	// Upper Access
	{
		// Upper Access
		GameObject::Sptr cageDoor_2 = SceneManager::GetCurrentScene()->CreateGameObject("Upper Access Door");
		{
			// Transform
			cageDoor_2->SetPosition(glm::vec3(-21.250f, -23.0f, 15.0f));
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
			collider->SetScale(glm::vec3(1.0f, 4.0f, 5.0f));
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
			pressure_plate_2->SetScale(glm::vec3(0.8f, 0.5f, 0.8f));

			// Renderer
			RenderComponent::Sptr renderer = pressure_plate_2->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Pressure Plate"));
			renderer->SetMaterial(Resources::GetMaterial("Pressure Plate"));

			// Trigger Volume
			TriggerVolume::Sptr volume = pressure_plate_2->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
			collider->SetScale(glm::vec3(2.0f, 1.8f, 2.0f));
			volume->AddCollider(collider);
			volume->SetCollisionGroup(PHYSICAL_GROUP);
			volume->SetCollisionMask(PHYSICAL_MASK);

			AudioSource::Sptr audio = pressure_plate_2->Add<AudioSource>();
			{
				audio->playOnAwake = false;
				audio->LoadEvent("Pressure Plate");
				audio->volume = 0.75f;
				/*audio->m_Resource = Resources::GetSound("Click");
				audio->m_Settings = AudioSettings{
					false, false, false
				};*/
				audio->Init();
			}

			// Trigger Event
			TriggerVolumeEnterBehaviour::Sptr trigger = pressure_plate_2->Add<TriggerVolumeEnterBehaviour>();
			trigger->onTriggerEnterEvent = [pressure_plate_2, cageDoor_2, audio, NO_GROUP, NO_MASK]
			{
				// Open The Door
				cageDoor_2->Get<RenderComponent>()->IsEnabled = false;
				RigidBody::Sptr rigid = cageDoor_2->Get<RigidBody>();
				rigid->SetCollisionGroup(NO_GROUP);
				rigid->SetCollisionMask(NO_MASK);	

				//AudioEngine::Instance().GetEvent("Pressure Plate").SetPosition(pressure_plate_2->GetPosition());
				AudioEngine::Instance().GetEvent("Pressure Plate").SetParameter("Powered", 0);
				//AudioEngine::Instance().GetEvent("Pressure Plate").Play();

				audio->Play();
			};

			trigger->onTriggerExitEvent = [pressure_plate_2, cageDoor_2, audio, PHYSICAL_GROUP, SHADOW_GROUP, PHYSICAL_MASK, SHADOW_MASK]
			{
				// Close The Door
				cageDoor_2->Get<RenderComponent>()->IsEnabled = true;
				RigidBody::Sptr rigid = cageDoor_2->Get<RigidBody>();
				rigid->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				rigid->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);

				//AudioEngine::Instance().GetEvent("Pressure Plate").SetPosition(pressure_plate_2->GetPosition());
				AudioEngine::Instance().GetEvent("Pressure Plate").SetParameter("Powered", 1);
				//AudioEngine::Instance().GetEvent("Pressure Plate").Play();

				audio->Play();
			};
		}
	}

	// Elevator Access
	{
		// Elevator Access
		GameObject::Sptr cageDoor_3 = SceneManager::GetCurrentScene()->CreateGameObject("Elevator Access Door 1");
		{
			// Transform
			cageDoor_3->SetPosition(glm::vec3(-0.5f, -31.5f, 5.0f));
			cageDoor_3->SetRotation(glm::vec3(90, 0, 90));
			cageDoor_3->SetScale(glm::vec3(0.25f, 0.125f, 0.058f));

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
			lever_1->SetPosition(glm::vec3(-19.5f, -31.0f, 15.78f));
			lever_1->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			lever_1->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

			// Renderer
			RenderComponent::Sptr renderer = lever_1->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Lever"));
			renderer->SetMaterial(Resources::GetMaterial("LeverTex"));
			
			// Collider
			RigidBody::Sptr physics = lever_1->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 0.0f, 1.5f));			
			physics->AddCollider(collider);
			physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
			physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);			

			AudioSource::Sptr audio = lever_1->Add<AudioSource>();
			{
				audio->playOnAwake = false;
				audio->LoadEvent("Lever");
				audio->volume = 0.75f;
				/*audio->m_Resource = Resources::GetSound("Lever");
				audio->m_Settings = AudioSettings{
					false, false, false
				};*/
				audio->Init();
			}

			// Interaction Event
			InteractableComponent::Sptr interactable = lever_1->Add<InteractableComponent>();
			interactable->onInteractionEvent = [interactable, lever_1, audio, cageDoor_3, cageDoor_4, cageDoor_5, PHYSICAL_GROUP, PHYSICAL_MASK, SHADOW_GROUP, SHADOW_MASK, NO_GROUP, NO_MASK]
			{	
				interactable->isToggled = !interactable->isToggled;
				//audio->Play();

				AudioEngine::Instance().GetEvent("Lever").SetPosition(lever_1->GetPosition());
				AudioEngine::Instance().GetEvent("Lever").Play();

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

	// Abyss Room
	{
		GameObject::Sptr key = SceneManager::GetCurrentScene()->CreateGameObject("Key");
		{
			key->SetPosition(glm::vec3(-71.75f, 20.5f, 15.0f));
			key->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			key->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

			RenderComponent::Sptr renderer = key->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Key"));
			renderer->SetMaterial(Resources::GetMaterial("Key"));

			AudioSource::Sptr audio = key->Add<AudioSource>();
			{
				audio->playOnAwake = false;
				audio->LoadEvent("Key");
				audio->volume = 1.0f;
				/*audio->m_Resource = Resources::GetSound("Key");
				audio->m_Settings = AudioSettings{
					false, false, false
				};*/
				audio->Init();
			}

		}

		GameObject::Sptr pedestal = SceneManager::GetCurrentScene()->CreateGameObject("Key Pedestal");
		{
			pedestal->SetPosition(glm::vec3(-72.0f, 21.0f, 14.0f));
			pedestal->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
			pedestal->SetScale(glm::vec3(1, 2, 1));

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
				key->Get<AudioSource>()->Play();
				
				AudioEngine::Instance().GetEvent("Key").SetPosition(key->GetPosition());
				AudioEngine::Instance().GetEvent("Key").Play();
				
				
			};
		}
	}
	
	// Door Room
	{
		GameObject::Sptr interact_doorway = SceneManager::GetCurrentScene()->CreateGameObject("Interact Door");
		{
			// Transform
			interact_doorway->SetPosition(glm::vec3(-12.0f, 21.0f, 10.0f));
			interact_doorway->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			interact_doorway->SetScale(glm::vec3(1.0f, 1.0f, 0.9f));

			// Render
			RenderComponent::Sptr renderer = interact_doorway->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("WallGrate"));
			renderer->SetMaterial(Resources::GetMaterial("WallGrate"));

			// Animator
			AnimatorComponent::Sptr animator = interact_doorway->Add<AnimatorComponent>();
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
			RigidBody::Sptr physics = interact_doorway->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetScale(glm::vec3(1.5f, 5.0f, 10.0f));
			collider->SetPosition(collider->GetPosition() + glm::vec3(0, 7, 0));
			physics->AddCollider(collider);
			physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
			physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);

			AudioSource::Sptr audio = interact_doorway->Add<AudioSource>();
			{
				audio->playOnAwake = false;
				audio->LoadEvent("Door");
				audio->volume = 1.0f;
				/*audio->m_Resource = Resources::GetSound("Door");
				audio->m_Settings = AudioSettings{
					false, false, false
				};*/
				audio->Init();
			}

			// Interaction Event
			InteractableComponent::Sptr interactable = interact_doorway->Add<InteractableComponent>();
			interactable->onInteractionEvent = [interact_doorway, audio, PHYSICAL_GROUP, PHYSICAL_MASK, SHADOW_GROUP, SHADOW_MASK, NO_GROUP, NO_MASK] 
			{
				// If player doesn't have the key, do nothing.
				if (!GameManager::GetInstance().PlayerHasKey()) return;
				

				//AudioEngine::Instance().GetEvent("Door").SetPosition(interact_doorway->GetPosition());
				//AudioEngine::Instance().GetEvent("Door").Play();

				AnimatorComponent::Sptr anim = interact_doorway->Get<AnimatorComponent>();
				RigidBody::Sptr rigid = interact_doorway->Get<RigidBody>();

				if (anim->IsPaused() && anim->IsReversed()) {
					rigid->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
					rigid->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
					anim->SetPause(false);
					audio->Play();
				}
				else if (anim->IsPaused() && !anim->IsReversed()) {
					rigid->SetCollisionGroup(NO_GROUP);
					rigid->SetCollisionMask(NO_MASK);
					anim->SetPause(false);
					audio->Play();
				}
			};
		}

		//CobWebs
		GameObject::Sptr cob1 = SceneManager::GetCurrentScene()->CreateGameObject("cob1");
		{
			cob1->SetPosition(glm::vec3(-10.0, -33, 8));
			cob1->SetRotation(glm::vec3(0, 0, 20));
			cob1->SetScale(glm::vec3(2));

			RenderComponent::Sptr renderer = cob1->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Cob"));
			renderer->SetMaterial(Resources::GetMaterial("Cob"));

			// Trigger Volume
			TriggerVolume::Sptr volume = cob1->Add<TriggerVolume>(); 
			BoxCollider::Sptr collider = BoxCollider::Create(); 
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 0.5f, 0.0f)); 
			collider->SetScale(glm::vec3(2)); 
			volume->AddCollider(collider); 
			volume->SetCollisionGroup(PHYSICAL_GROUP); 
			volume->SetCollisionMask(PHYSICAL_MASK); 

			// Trigger Event
			TriggerVolumeEnterBehaviour::Sptr trigger = cob1->Add<TriggerVolumeEnterBehaviour>();  
			trigger->onTriggerEnterEvent = [body] { 
				if (!SceneManager::GetCurrentScene()->PC.isShadow) { 
					SceneManager::GetCurrentScene()->PC.playerSpeed = 0.15; //Slow Overtime when player interacts with cobweb
				}
			};
			trigger->onTriggerExitEvent = [body] { 
				if (!SceneManager::GetCurrentScene()->PC.isShadow) {
					SceneManager::GetCurrentScene()->PC.playerSpeed = 0.5; //Do not change this number this is character normal speed
				}
			};
		}

		//Healing Wells
		GameObject::Sptr well1 = SceneManager::GetCurrentScene()->CreateGameObject("well1");
		{
			well1->SetPosition(glm::vec3(15, -8, 5));
			well1->SetRotation(glm::vec3(90, 0, 0));
			well1->SetScale(glm::vec3(0.5));

			RenderComponent::Sptr renderer = well1->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("HealingWell"));
			renderer->SetMaterial(Resources::GetMaterial("HealingWell"));

			// Trigger Volume
			TriggerVolume::Sptr volume = well1->Add<TriggerVolume>();   
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));  
			collider->SetScale(glm::vec3(2.0f, 1.8f, 2.0f));
			volume->AddCollider(collider);
			volume->SetCollisionGroup(PHYSICAL_GROUP);
			volume->SetCollisionMask(PHYSICAL_MASK); 

			TriggerVolumeEnterBehaviour::Sptr trigger = well1->Add<TriggerVolumeEnterBehaviour>(); 
			trigger->onTriggerStayEvent = [body] {
				if (!SceneManager::GetCurrentScene()->PC.isShadow) {
					body->Get<HealthComponent>()->Heal(0.8f);  //Healing number overtime
				}
			};
		}

		//Crystal
		GameObject::Sptr Crystal1 = SceneManager::GetCurrentScene()->CreateGameObject("cry1"); 
		{
			Crystal1->SetPosition(glm::vec3(7, -45, 5)); 
			Crystal1->SetRotation(glm::vec3(90, 0, 0)); 
			Crystal1->SetScale(glm::vec3(0.5));  

			RenderComponent::Sptr renderer = Crystal1->Add<RenderComponent>(); 
			renderer->SetMesh(Resources::GetMesh("Crystal")); 
			renderer->SetMaterial(Resources::GetMaterial("Crystal")); 
		}

		GameObject::Sptr Crystal2 = SceneManager::GetCurrentScene()->CreateGameObject("cry2");
		{
			Crystal2->SetPosition(glm::vec3(-2, -23, 5));
			Crystal2->SetRotation(glm::vec3(90, 0, -60));
			Crystal2->SetScale(glm::vec3(0.6));

			RenderComponent::Sptr renderer = Crystal2->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Crystal"));
			renderer->SetMaterial(Resources::GetMaterial("Crystal"));
		}

		GameObject::Sptr Crystal3 = SceneManager::GetCurrentScene()->CreateGameObject("cry3");
		{
			Crystal3->SetPosition(glm::vec3(52, -23, 5));
			Crystal3->SetRotation(glm::vec3(90, 0, -30));
			Crystal3->SetScale(glm::vec3(0.5));

			RenderComponent::Sptr renderer = Crystal3->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Crystal"));
			renderer->SetMaterial(Resources::GetMaterial("Crystal"));
		}

		GameObject::Sptr Crystal4 = SceneManager::GetCurrentScene()->CreateGameObject("cry4");
		{
			Crystal4->SetPosition(glm::vec3(59, -43, 5));
			Crystal4->SetRotation(glm::vec3(90, 0, 0));
			Crystal4->SetScale(glm::vec3(0.5));

			RenderComponent::Sptr renderer = Crystal4->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Crystal"));
			renderer->SetMaterial(Resources::GetMaterial("Crystal"));
		}

		GameObject::Sptr Crystal5 = SceneManager::GetCurrentScene()->CreateGameObject("cry5");
		{
			Crystal5->SetPosition(glm::vec3(52, -2.2, 4.8));
			Crystal5->SetRotation(glm::vec3(120, 0, 0));
			Crystal5->SetScale(glm::vec3(0.5));

			RenderComponent::Sptr renderer = Crystal5->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Crystal"));
			renderer->SetMaterial(Resources::GetMaterial("Crystal"));
		}

		GameObject::Sptr Crystal6 = SceneManager::GetCurrentScene()->CreateGameObject("cry6");
		{
			Crystal6->SetPosition(glm::vec3(30, -10, 5));
			Crystal6->SetRotation(glm::vec3(90, 0, -20));
			Crystal6->SetScale(glm::vec3(0.5));

			RenderComponent::Sptr renderer = Crystal6->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Crystal"));
			renderer->SetMaterial(Resources::GetMaterial("Crystal"));
		}

		GameObject::Sptr Crystal7 = SceneManager::GetCurrentScene()->CreateGameObject("cry7");
		{
			Crystal7->SetPosition(glm::vec3(-8, -3, 5));
			Crystal7->SetRotation(glm::vec3(90, 0, 120));
			Crystal7->SetScale(glm::vec3(0.6));

			RenderComponent::Sptr renderer = Crystal7->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Crystal"));
			renderer->SetMaterial(Resources::GetMaterial("Crystal"));
		}

		GameObject::Sptr Crystal8 = SceneManager::GetCurrentScene()->CreateGameObject("cry8");
		{
			Crystal8->SetPosition(glm::vec3(-40, -28, 15));
			Crystal8->SetRotation(glm::vec3(90, 0, 60));
			Crystal8->SetScale(glm::vec3(0.6));

			RenderComponent::Sptr renderer = Crystal8->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Crystal"));
			renderer->SetMaterial(Resources::GetMaterial("Crystal"));
		}

		GameObject::Sptr Crystal9 = SceneManager::GetCurrentScene()->CreateGameObject("cry9");
		{
			Crystal9->SetPosition(glm::vec3(-50, 25, 13));
			Crystal9->SetRotation(glm::vec3(80, 0, 60));
			Crystal9->SetScale(glm::vec3(0.6));

			RenderComponent::Sptr renderer = Crystal9->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Crystal"));
			renderer->SetMaterial(Resources::GetMaterial("Crystal"));
		}

		GameObject::Sptr Crystal10 = SceneManager::GetCurrentScene()->CreateGameObject("cry10");
		{
			Crystal10->SetPosition(glm::vec3(-16, 16, 13));
			Crystal10->SetRotation(glm::vec3(90, 0, -100));
			Crystal10->SetScale(glm::vec3(0.5));

			RenderComponent::Sptr renderer = Crystal10->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Crystal"));
			renderer->SetMaterial(Resources::GetMaterial("Crystal"));
		}

		GameObject::Sptr Crystal11 = SceneManager::GetCurrentScene()->CreateGameObject("cry11");
		{
			Crystal11->SetPosition(glm::vec3(10, 25, 13));
			Crystal11->SetRotation(glm::vec3(90, 0, 60));
			Crystal11->SetScale(glm::vec3(0.6));

			RenderComponent::Sptr renderer = Crystal11->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Crystal"));
			renderer->SetMaterial(Resources::GetMaterial("Crystal"));
		}

		GameObject::Sptr Crystal12 = SceneManager::GetCurrentScene()->CreateGameObject("cry12");
		{
			Crystal12->SetPosition(glm::vec3(40, 15, 13));
			Crystal12->SetRotation(glm::vec3(90, 0, 60));
			Crystal12->SetScale(glm::vec3(0.6));

			RenderComponent::Sptr renderer = Crystal12->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Crystal"));
			renderer->SetMaterial(Resources::GetMaterial("Crystal"));
		}

		GameObject::Sptr Crystal13 = SceneManager::GetCurrentScene()->CreateGameObject("cry13"); 
		{
			Crystal13->SetPosition(glm::vec3(35, 29, 13)); 
			Crystal13->SetRotation(glm::vec3(90, 0, -30)); 
			Crystal13->SetScale(glm::vec3(0.5)); 

			RenderComponent::Sptr renderer = Crystal13->Add<RenderComponent>(); 
			renderer->SetMesh(Resources::GetMesh("Crystal")); 
			renderer->SetMaterial(Resources::GetMaterial("Crystal")); 
		}

		//Multiple Mushrooms
		GameObject::Sptr MultipleShrooms1 = SceneManager::GetCurrentScene()->CreateGameObject("ms1");
		{
			MultipleShrooms1->SetPosition(glm::vec3(20, -40, 1));
			MultipleShrooms1->SetRotation(glm::vec3(90, 0, 0));
			MultipleShrooms1->SetScale(glm::vec3(1.2));

			RenderComponent::Sptr renderer = MultipleShrooms1->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("MS"));
			renderer->SetMaterial(Resources::GetMaterial("MS"));
			
			// Trigger Volume
			TriggerVolume::Sptr volume = MultipleShrooms1->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.2f, 0.0f));
			collider->SetScale(glm::vec3(2.0f, 1.55f, 2.0f));
			volume->AddCollider(collider);
			volume->SetCollisionGroup(PHYSICAL_GROUP);
			volume->SetCollisionMask(PHYSICAL_MASK);

			TriggerVolumeEnterBehaviour::Sptr trigger = MultipleShrooms1->Add<TriggerVolumeEnterBehaviour>();
			trigger->onTriggerEnterEvent = [body] {
				if (!SceneManager::GetCurrentScene()->PC.isShadow) {
					SceneManager::GetCurrentScene()->PC.playerSpeed = 0.3;  //Slow over time
				}
			};
			trigger->onTriggerStayEvent = [body] {
				if (!SceneManager::GetCurrentScene()->PC.isShadow) {
					body->Get<HealthComponent>()->DealDamage(0.2f);  //Damage overtime poison
				}
			};
			trigger->onTriggerExitEvent = [body] { 
				if (!SceneManager::GetCurrentScene()->PC.isShadow) { 
					SceneManager::GetCurrentScene()->PC.playerSpeed = 0.5; //Do no change number character base speed
				}
			};
		}

		GameObject::Sptr MultipleShrooms2 = SceneManager::GetCurrentScene()->CreateGameObject("ms2");
		{
			MultipleShrooms2->SetPosition(glm::vec3(-40, 0, 5));
			MultipleShrooms2->SetRotation(glm::vec3(90, 0, 90));
			MultipleShrooms2->SetScale(glm::vec3(1.2));

			RenderComponent::Sptr renderer = MultipleShrooms2->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("MS"));
			renderer->SetMaterial(Resources::GetMaterial("MS"));
		}

		GameObject::Sptr MultipleShrooms3 = SceneManager::GetCurrentScene()->CreateGameObject("ms3");
		{
			MultipleShrooms3->SetPosition(glm::vec3(-54, -13, 5));
			MultipleShrooms3->SetRotation(glm::vec3(90, 0, -50));
			MultipleShrooms3->SetScale(glm::vec3(1.3));

			RenderComponent::Sptr renderer = MultipleShrooms3->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("MS"));
			renderer->SetMaterial(Resources::GetMaterial("MS"));
		}

		GameObject::Sptr MultipleShrooms4 = SceneManager::GetCurrentScene()->CreateGameObject("ms4");
		{
			MultipleShrooms4->SetPosition(glm::vec3(-19, -10, 15));
			MultipleShrooms4->SetRotation(glm::vec3(90, 0, -72));
			MultipleShrooms4->SetScale(glm::vec3(1.1));

			RenderComponent::Sptr renderer = MultipleShrooms4->Add<RenderComponent>(); 
			renderer->SetMesh(Resources::GetMesh("MS"));
			renderer->SetMaterial(Resources::GetMaterial("MS"));
		}

		//Single Mushrooms
		GameObject::Sptr SingleShrooms1 = SceneManager::GetCurrentScene()->CreateGameObject("ss1"); 
		{
			SingleShrooms1->SetPosition(glm::vec3(40, -47, 1)); 
			SingleShrooms1->SetRotation(glm::vec3(90, 0, -35));
			SingleShrooms1->SetScale(glm::vec3(1)); 

			RenderComponent::Sptr renderer = SingleShrooms1->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("SS"));
			renderer->SetMaterial(Resources::GetMaterial("SS"));
		}

		GameObject::Sptr SingleShrooms2 = SceneManager::GetCurrentScene()->CreateGameObject("ss2"); 
		{
			SingleShrooms2->SetPosition(glm::vec3(-51, 4, 5)); 
			SingleShrooms2->SetRotation(glm::vec3(90, 0, 160)); 
			SingleShrooms2->SetScale(glm::vec3(1.3));  
			 
			RenderComponent::Sptr renderer = SingleShrooms2->Add<RenderComponent>(); 
			renderer->SetMesh(Resources::GetMesh("SS")); 
			renderer->SetMaterial(Resources::GetMaterial("SS")); 
		}

		GameObject::Sptr SingleShrooms3 = SceneManager::GetCurrentScene()->CreateGameObject("ss3");
		{
			SingleShrooms3->SetPosition(glm::vec3(-45, 3, 5));
			SingleShrooms3->SetRotation(glm::vec3(90, 0, 120));
			SingleShrooms3->SetScale(glm::vec3(0.8));

			RenderComponent::Sptr renderer = SingleShrooms3->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("SS"));
			renderer->SetMaterial(Resources::GetMaterial("SS"));
		}

		GameObject::Sptr SingleShrooms4 = SceneManager::GetCurrentScene()->CreateGameObject("ss4");
		{
			SingleShrooms4->SetPosition(glm::vec3(-30, 0, 15));
			SingleShrooms4->SetRotation(glm::vec3(90, 0, 120));
			SingleShrooms4->SetScale(glm::vec3(1.1));

			RenderComponent::Sptr renderer = SingleShrooms4->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("SS"));
			renderer->SetMaterial(Resources::GetMaterial("SS"));
		}


		GameObject::Sptr SingleShrooms5 = SceneManager::GetCurrentScene()->CreateGameObject("ss5");
		{
			SingleShrooms5->SetPosition(glm::vec3(-26, -16, 15));
			SingleShrooms5->SetRotation(glm::vec3(90, 0, -160));
			SingleShrooms5->SetScale(glm::vec3(0.7));

			RenderComponent::Sptr renderer = SingleShrooms5->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("SS"));
			renderer->SetMaterial(Resources::GetMaterial("SS"));
		}
	}


#pragma endregion

#pragma region Scene Props

	GameObject::Sptr goblinShield1 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Shield 1");
	{
		goblinShield1->SetPosition(glm::vec3(7.16f, -33.91f, 6.05f));
		goblinShield1->SetRotation(glm::vec3(52.f, 0.0f, 0.0f));
		goblinShield1->SetScale(glm::vec3(0.2f, 0.2f, 0.2f));

		RenderComponent::Sptr renderer = goblinShield1->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Shield"));
		renderer->SetMaterial(Resources::GetMaterial("ShieldTex"));
	}

	GameObject::Sptr goblinShield2 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Shield 2");
	{
		goblinShield2->SetPosition(glm::vec3(46.01f, -43.75f, 1.25f));
		goblinShield2->SetRotation(glm::vec3(-13.f, 1.0f, -48.0f));
		goblinShield2->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

		RenderComponent::Sptr renderer = goblinShield2->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Shield"));
		renderer->SetMaterial(Resources::GetMaterial("ShieldTex"));
	}

	GameObject::Sptr goblinShield3 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Shield 3");
	{
		goblinShield3->SetPosition(glm::vec3(32.0f, -42.33f, 1.22f));
		goblinShield3->SetRotation(glm::vec3(0.0f, 1.0f, 45.0f));
		goblinShield3->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

		RenderComponent::Sptr renderer = goblinShield3->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Shield"));
		renderer->SetMaterial(Resources::GetMaterial("ShieldTex"));
	}

	GameObject::Sptr goblinShield4 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Shield 4");
	{
		goblinShield4->SetPosition(glm::vec3(44.61f, -44.7f, 1.45f));
		goblinShield4->SetRotation(glm::vec3(-13.f, 1.0f, 131.0f));
		goblinShield4->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		RenderComponent::Sptr renderer = goblinShield4->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Shield"));
		renderer->SetMaterial(Resources::GetMaterial("ShieldTex"));
	}

	GameObject::Sptr goblinShield5 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Shield 5");
	{
		goblinShield5->SetPosition(glm::vec3(58.85f, -2.7f, 5.68f));
		goblinShield5->SetRotation(glm::vec3(0.f, 0.0f, 81.0f));
		goblinShield5->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

		RenderComponent::Sptr renderer = goblinShield5->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Shield"));
		renderer->SetMaterial(Resources::GetMaterial("ShieldTex"));
	}

	GameObject::Sptr goblinShield6 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Shield 6");
	{
		goblinShield6->SetPosition(glm::vec3(19.04f, -0.81f, 2.82f));
		goblinShield6->SetRotation(glm::vec3(-37.f, -41.0f, -134.0f));
		goblinShield6->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

		RenderComponent::Sptr renderer = goblinShield6->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Shield"));
		renderer->SetMaterial(Resources::GetMaterial("ShieldTex"));
	}

	GameObject::Sptr goblinShield7 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Shield 7");
	{
		goblinShield7->SetPosition(glm::vec3(3.51f, -5.88f, 5.63f));
		goblinShield7->SetRotation(glm::vec3(0.f, 0.0f, -70.0f));
		goblinShield7->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

		RenderComponent::Sptr renderer = goblinShield7->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Shield"));
		renderer->SetMaterial(Resources::GetMaterial("ShieldTex"));
	}

	GameObject::Sptr goblinShield8 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Shield 8");
	{
		goblinShield8->SetPosition(glm::vec3(-39.92f, -12.12f, 5.63f));
		goblinShield8->SetRotation(glm::vec3(0.f, 0.0f, -135.0f));
		goblinShield8->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

		RenderComponent::Sptr renderer = goblinShield8->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Shield"));
		renderer->SetMaterial(Resources::GetMaterial("ShieldTex"));
	}

	GameObject::Sptr goblinShield9 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Shield 9");
	{
		goblinShield9->SetPosition(glm::vec3(-57.87f, 25.28f, 14.f));
		goblinShield9->SetRotation(glm::vec3(0.f, 0.0f, -69.0f));
		goblinShield9->SetScale(glm::vec3(0.35f, 0.35f, 0.35f));

		RenderComponent::Sptr renderer = goblinShield9->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Shield"));
		renderer->SetMaterial(Resources::GetMaterial("ShieldTex"));
	}

	GameObject::Sptr goblinShield10 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Shield 10");
	{
		goblinShield10->SetPosition(glm::vec3(-51.02f, 17.12f, 14.f));
		goblinShield10->SetRotation(glm::vec3(0.f, 0.0f, 143.0f));
		goblinShield10->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));

		RenderComponent::Sptr renderer = goblinShield10->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Shield"));
		renderer->SetMaterial(Resources::GetMaterial("ShieldTex"));
	}

	GameObject::Sptr goblinStaff1 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Staff 1");
	{
		goblinStaff1->SetPosition(glm::vec3(37.48f, -44.88f, 1.0f));
		goblinStaff1->SetRotation(glm::vec3(-13.f, -87.0f, 56.0f));
		goblinStaff1->SetScale(glm::vec3(1.2f, 1.2f, 1.2f));

		RenderComponent::Sptr renderer = goblinStaff1->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Staff"));
		renderer->SetMaterial(Resources::GetMaterial("StaffTex"));
	}

	GameObject::Sptr goblinStaff2 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Staff 2");
	{
		goblinStaff2->SetPosition(glm::vec3(15.27f, -22.7f, 5.98f));
		goblinStaff2->SetRotation(glm::vec3(-4.f, -90.0f, -45.0f));
		goblinStaff2->SetScale(glm::vec3(1.2f, 1.2f, 1.2f));

		RenderComponent::Sptr renderer = goblinStaff2->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Staff"));
		renderer->SetMaterial(Resources::GetMaterial("StaffTex"));
	}

	GameObject::Sptr goblinStaff3 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Staff 3");
	{
		goblinStaff3->SetPosition(glm::vec3(29.88f, -8.08f, 1.89f));
		goblinStaff3->SetRotation(glm::vec3(0.f, -90.0f, 39.0f));
		goblinStaff3->SetScale(glm::vec3(1.2f, 1.2f, 1.2f));

		RenderComponent::Sptr renderer = goblinStaff3->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Staff"));
		renderer->SetMaterial(Resources::GetMaterial("StaffTex"));
	}

	GameObject::Sptr goblinStaff4 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Staff 4");
	{
		goblinStaff4->SetPosition(glm::vec3(-9.82f, -10.3f, 5.4f));
		goblinStaff4->SetRotation(glm::vec3(-34.f, -87.0f, -86.0f));
		goblinStaff4->SetScale(glm::vec3(1.2f, 1.2f, 1.2f));

		RenderComponent::Sptr renderer = goblinStaff4->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Staff"));
		renderer->SetMaterial(Resources::GetMaterial("StaffTex"));
	}

	GameObject::Sptr goblinStaff5 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Staff 5");
	{
		goblinStaff5->SetPosition(glm::vec3(-51.8f, 6.54f, 6.85f));
		goblinStaff5->SetRotation(glm::vec3(0.f, 17.0f, -9.0f));
		goblinStaff5->SetScale(glm::vec3(1.2f, 1.2f, 1.2f));

		RenderComponent::Sptr renderer = goblinStaff5->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Staff"));
		renderer->SetMaterial(Resources::GetMaterial("StaffTex"));
	}

	GameObject::Sptr goblinStaff6 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Staff 6");
	{
		goblinStaff6->SetPosition(glm::vec3(-19.36f, -42.0f, 5.3f));
		goblinStaff6->SetRotation(glm::vec3(-34.f, 90.0f, 173.0f));
		goblinStaff6->SetScale(glm::vec3(1.2f, 1.2f, 1.2f));

		RenderComponent::Sptr renderer = goblinStaff6->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Staff"));
		renderer->SetMaterial(Resources::GetMaterial("StaffTex"));
	}

	GameObject::Sptr goblinStaff7 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Staff 7");
	{
		goblinStaff7->SetPosition(glm::vec3(-28.39f, 3.f, 16.98f));
		goblinStaff7->SetRotation(glm::vec3(90.f, -24.0f, 0.0f));
		goblinStaff7->SetScale(glm::vec3(1.2f, 1.2f, 1.2f));

		RenderComponent::Sptr renderer = goblinStaff7->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Staff"));
		renderer->SetMaterial(Resources::GetMaterial("StaffTex"));
	}

	GameObject::Sptr goblinStaff8 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Staff 8");
	{
		goblinStaff8->SetPosition(glm::vec3(-43.81f, 26.83f,16.f));
		goblinStaff8->SetRotation(glm::vec3(0.f, 90.0f, -81.0f));
		goblinStaff8->SetScale(glm::vec3(1.2f, 1.2f, 1.2f));

		RenderComponent::Sptr renderer = goblinStaff8->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Staff"));
		renderer->SetMaterial(Resources::GetMaterial("StaffTex"));
	}

	GameObject::Sptr goblinSword1 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Sword 1");
	{
		goblinSword1->SetPosition(glm::vec3(24.48f, -45.57f, 1.15f));
		goblinSword1->SetRotation(glm::vec3(0.0f, 0.0f, -56.0f));
		goblinSword1->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

		RenderComponent::Sptr renderer = goblinSword1->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Sword"));
		renderer->SetMaterial(Resources::GetMaterial("SwordTex"));
	}

	GameObject::Sptr goblinSword2 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Sword 2");
	{
		goblinSword2->SetPosition(glm::vec3(61.61f, -2.7f, 5.68f));
		goblinSword2->SetRotation(glm::vec3(0.0f, 0.0f, -167.0f));
		goblinSword2->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

		RenderComponent::Sptr renderer = goblinSword2->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Sword"));
		renderer->SetMaterial(Resources::GetMaterial("SwordTex"));
	}

	GameObject::Sptr goblinSword3 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Sword 3");
	{
		goblinSword3->SetPosition(glm::vec3(49.53f, -24.34f, 6.93f));
		goblinSword3->SetRotation(glm::vec3(-45.0f, 0.0f, -180.0f));
		goblinSword3->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

		RenderComponent::Sptr renderer = goblinSword3->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Sword"));
		renderer->SetMaterial(Resources::GetMaterial("SwordTex"));
	}

	GameObject::Sptr goblinSword4 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Sword 4");
	{
		goblinSword4->SetPosition(glm::vec3(18.31f, -0.22f, 5.61f));
		goblinSword4->SetRotation(glm::vec3(0.0f, 0.0f, 57.0f));
		goblinSword4->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

		RenderComponent::Sptr renderer = goblinSword4->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Sword"));
		renderer->SetMaterial(Resources::GetMaterial("SwordTex"));
	}

	GameObject::Sptr goblinSword5 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Sword 5");
	{
		goblinSword5->SetPosition(glm::vec3(-2.f, -6.89f, 6.f));
		goblinSword5->SetRotation(glm::vec3(0.0f, -180.0f, 121.0f));
		goblinSword5->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

		RenderComponent::Sptr renderer = goblinSword5->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Sword"));
		renderer->SetMaterial(Resources::GetMaterial("SwordTex"));
	}

	GameObject::Sptr goblinSword6 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Sword 6");
	{
		goblinSword6->SetPosition(glm::vec3(-18.82f, -9.05f, 6.f));
		goblinSword6->SetRotation(glm::vec3(0.0f, 0.0f, -67.0f));
		goblinSword6->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

		RenderComponent::Sptr renderer = goblinSword6->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Sword"));
		renderer->SetMaterial(Resources::GetMaterial("SwordTex"));
	}

	GameObject::Sptr goblinSword7 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Sword 7");
	{
		goblinSword7->SetPosition(glm::vec3(-31.46f, -28.f, 17.18f));
		goblinSword7->SetRotation(glm::vec3(-124.0f, 0.0f, 90.0f));
		goblinSword7->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

		RenderComponent::Sptr renderer = goblinSword7->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Sword"));
		renderer->SetMaterial(Resources::GetMaterial("SwordTex"));
	}

	GameObject::Sptr goblinSword8 = SceneManager::GetCurrentScene()->CreateGameObject("Goblin Sword 8");
	{
		goblinSword8->SetPosition(glm::vec3(-26.76f, 22.72f, 15.89f));
		goblinSword8->SetRotation(glm::vec3(0.0f, 0.0f, 115.0f));
		goblinSword8->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

		RenderComponent::Sptr renderer = goblinSword8->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Sword"));
		renderer->SetMaterial(Resources::GetMaterial("SwordTex"));
	}

	GameObject::Sptr floorGrate1 = SceneManager::GetCurrentScene()->CreateGameObject("Floor Grate 1");
	{
		floorGrate1->SetPosition(glm::vec3(14.75f, -36.f, 5.71f));
		floorGrate1->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
		floorGrate1->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

		RenderComponent::Sptr renderer = floorGrate1->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Floor Grate"));
		renderer->SetMaterial(Resources::GetMaterial("Floor Grate"));
	}

	GameObject::Sptr intactPillar1 = SceneManager::GetCurrentScene()->CreateGameObject("Jump Puzzle Intact Pillar 1");
	{
		intactPillar1->SetPosition(glm::vec3(-54.01f, 6.05f, 5.45f));
		intactPillar1->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
		intactPillar1->SetScale(glm::vec3(1, 1, 1));

		RenderComponent::Sptr renderer = intactPillar1->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Intact Pillar"));
		renderer->SetMaterial(Resources::GetMaterial("Intact Pillar"));
	}

	GameObject::Sptr intactPillar2 = SceneManager::GetCurrentScene()->CreateGameObject("Jump Puzzle Intact Pillar 2");
	{
		intactPillar2->SetPosition(glm::vec3(-54.11f, -21.3f, 5.45f));
		intactPillar2->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
		intactPillar2->SetScale(glm::vec3(1, 1, 1));

		RenderComponent::Sptr renderer = intactPillar2->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Intact Pillar"));
		renderer->SetMaterial(Resources::GetMaterial("Intact Pillar"));
	}

	GameObject::Sptr intactPillar3 = SceneManager::GetCurrentScene()->CreateGameObject("Intact Pillar 3");
	{
		intactPillar3->SetPosition(glm::vec3(65.8f, -46.15f, 5.48f));
		intactPillar3->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
		intactPillar3->SetScale(glm::vec3(0.7f, 0.7f, 0.7f));

		RenderComponent::Sptr renderer = intactPillar3->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Intact Pillar"));
		renderer->SetMaterial(Resources::GetMaterial("Intact Pillar"));
	}

	GameObject::Sptr intactPillar4 = SceneManager::GetCurrentScene()->CreateGameObject("Intact Pillar 4");
	{
		intactPillar4->SetPosition(glm::vec3(30.62f, 27.76f, 9.6f));
		intactPillar4->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
		intactPillar4->SetScale(glm::vec3(1.5f, 1.12f, 1.5f));

		RenderComponent::Sptr renderer = intactPillar4->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Intact Pillar"));
		renderer->SetMaterial(Resources::GetMaterial("Intact Pillar"));
	}

	GameObject::Sptr intactPillar5 = SceneManager::GetCurrentScene()->CreateGameObject("Intact Pillar 5");
	{
		intactPillar5->SetPosition(glm::vec3(46.38f, 27.66f, 9.6f));
		intactPillar5->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
		intactPillar5->SetScale(glm::vec3(1.5f, 1.12f, 1.5f));

		RenderComponent::Sptr renderer = intactPillar5->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Intact Pillar"));
		renderer->SetMaterial(Resources::GetMaterial("Intact Pillar"));
	}

	GameObject::Sptr intactPillar6 = SceneManager::GetCurrentScene()->CreateGameObject("Intact Pillar 6");
	{
		intactPillar6->SetPosition(glm::vec3(30.57f, 15.8f, 9.6f));
		intactPillar6->SetRotation(glm::vec3(90.0f, 0.0f, -180.0f));
		intactPillar6->SetScale(glm::vec3(1.5f, 1.12f, 1.5f));

		RenderComponent::Sptr renderer = intactPillar6->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Intact Pillar"));
		renderer->SetMaterial(Resources::GetMaterial("Intact Pillar"));
	}

	GameObject::Sptr damagedPillar1 = SceneManager::GetCurrentScene()->CreateGameObject("Damaged Pillar 1");
	{
		damagedPillar1->SetPosition(glm::vec3(-12.12f, -34.46f, 5.45f));
		damagedPillar1->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
		damagedPillar1->SetScale(glm::vec3(1.f, 1.f, 1.f));

		RenderComponent::Sptr renderer = damagedPillar1->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Damaged Pillar"));
		renderer->SetMaterial(Resources::GetMaterial("Damaged Pillar"));
	}

	GameObject::Sptr damagedPillar2 = SceneManager::GetCurrentScene()->CreateGameObject("Jump Puzzle Damaged Pillar");
	{
		damagedPillar2->SetPosition(glm::vec3(-40.3f, -21.3f, 5.45f));
		damagedPillar2->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
		damagedPillar2->SetScale(glm::vec3(1.5f, 1.17f, 1.5f));

		RenderComponent::Sptr renderer = damagedPillar2->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Damaged Pillar"));
		renderer->SetMaterial(Resources::GetMaterial("Damaged Pillar"));
	}

	GameObject::Sptr damagedPillar3 = SceneManager::GetCurrentScene()->CreateGameObject("Damaged Pillar 3");
	{
		damagedPillar3->SetPosition(glm::vec3(60.71f, -16.91f, 5.45f));
		damagedPillar3->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
		damagedPillar3->SetScale(glm::vec3(0.7f, 0.7f, 0.7f));

		RenderComponent::Sptr renderer = damagedPillar3->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Damaged Pillar"));
		renderer->SetMaterial(Resources::GetMaterial("Damaged Pillar"));
	}

	GameObject::Sptr damagedPillar4 = SceneManager::GetCurrentScene()->CreateGameObject("Damaged Pillar 4");
	{
		damagedPillar4->SetPosition(glm::vec3(46.38f, 15.8f, 9.6f));
		damagedPillar4->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
		damagedPillar4->SetScale(glm::vec3(1.5f, 1.12f, 1.5f));

		RenderComponent::Sptr renderer = damagedPillar4->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Damaged Pillar"));
		renderer->SetMaterial(Resources::GetMaterial("Damaged Pillar"));
	}

	GameObject::Sptr destroyedPillar1 = SceneManager::GetCurrentScene()->CreateGameObject("Jump Puzzle Destroyed Pillar 1");
	{
		destroyedPillar1->SetPosition(glm::vec3(-40.4f, 6.25f, 5.45f));
		destroyedPillar1->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
		destroyedPillar1->SetScale(glm::vec3(1.f, 0.56f, 1.f));

		RenderComponent::Sptr renderer = destroyedPillar1->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Destroyed Pillar"));
		renderer->SetMaterial(Resources::GetMaterial("Destroyed Pillar"));
	}

	GameObject::Sptr destroyedPillar2 = SceneManager::GetCurrentScene()->CreateGameObject("Destroyed Pillar 2");
	{
		destroyedPillar2->SetPosition(glm::vec3(15.35f, -16.16f, 5.45f));
		destroyedPillar2->SetRotation(glm::vec3(90.0f, 0.0f, -180.0f));
		destroyedPillar2->SetScale(glm::vec3(1.f, 1.f, 1.f));

		RenderComponent::Sptr renderer = destroyedPillar2->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Destroyed Pillar"));
		renderer->SetMaterial(Resources::GetMaterial("Destroyed Pillar"));
	}

	GameObject::Sptr rockPile1 = SceneManager::GetCurrentScene()->CreateGameObject("Rock Pile 1");
	{
		rockPile1->SetPosition(glm::vec3(-6.47f, -44.0f, 5.9f));
		rockPile1->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
		rockPile1->SetScale(glm::vec3(1.25f, 1.25f, 1.25f));

		RenderComponent::Sptr renderer = rockPile1->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Rock Pile"));
		renderer->SetMaterial(Resources::GetMaterial("Rock Pile"));
	}

	GameObject::Sptr rockPile2 = SceneManager::GetCurrentScene()->CreateGameObject("Rock Pile 2");
	{
		rockPile2->SetPosition(glm::vec3(12.01f, -25.12f, 5.9f));
		rockPile2->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
		rockPile2->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

		RenderComponent::Sptr renderer = rockPile2->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Rock Pile"));
		renderer->SetMaterial(Resources::GetMaterial("Rock Pile"));
	}

	GameObject::Sptr rockPile3 = SceneManager::GetCurrentScene()->CreateGameObject("Rock Pile 3");
	{
		rockPile3->SetPosition(glm::vec3(9.53f, -8.01f, 5.7f));
		rockPile3->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
		rockPile3->SetScale(glm::vec3(1.25));

		RenderComponent::Sptr renderer = rockPile3->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Rock Pile"));
		renderer->SetMaterial(Resources::GetMaterial("Rock Pile"));
	}

	GameObject::Sptr rockPile4 = SceneManager::GetCurrentScene()->CreateGameObject("Rock Pile 4");
	{
		rockPile4->SetPosition(glm::vec3(-45.68f, -1.42f, 5.9f));
		rockPile4->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
		rockPile4->SetScale(glm::vec3(1.25f));

		RenderComponent::Sptr renderer = rockPile4->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Rock Pile"));
		renderer->SetMaterial(Resources::GetMaterial("Rock Pile"));
	}

	GameObject::Sptr rockPile5 = SceneManager::GetCurrentScene()->CreateGameObject("Rock Pile 5");
	{
		rockPile5->SetPosition(glm::vec3(-24.68f, 22.57f, 13.2f));
		rockPile5->SetRotation(glm::vec3(90.0f, 0.0f, 135.0f));
		rockPile5->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

		RenderComponent::Sptr renderer = rockPile5->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Rock Pile"));
		renderer->SetMaterial(Resources::GetMaterial("Rock Pile"));
	}

	GameObject::Sptr graveStone1 = SceneManager::GetCurrentScene()->CreateGameObject("Grave Stone 1");
	{
		graveStone1->SetPosition(glm::vec3(7.13f, -33.16f, 5.45f));
		graveStone1->SetRotation(glm::vec3(90.0f, 0.0f, -90.0f));
		graveStone1->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

		RenderComponent::Sptr renderer = graveStone1->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Grave Stone"));
		renderer->SetMaterial(Resources::GetMaterial("Grave Stone"));

	}

#pragma endregion


#pragma region Audio

	/////////////////////////////////////////////////////////
	//				   ENVIRONMENTAL AUDIO
	/////////////////////////////////////////////////////////

	//AudioEngine::Instance().GetEvent("Test").Play();

	GameObject::Sptr bgm = SceneManager::GetCurrentScene()->CreateGameObject("BGM");
	{
		AudioSource::Sptr audio = bgm->Add<AudioSource>();
		{


			audio->playOnAwake = true;
			audio->LoadEvent("Test");
			audio->volume = 0.1f;
		
			/*audio->m_Resource = Resources::GetSound("Mohit");
			audio->m_Settings = AudioSettings{
				false, true, false
			};*/
			//audio->volume = f;
			audio->Init();
		}
	}

#pragma endregion


#pragma region User Interface

	/////////////////////////////////////////////////////////
	//				   USER INTERFACE - GAME
	/////////////////////////////////////////////////////////

	GameObject::Sptr gameCanvas = SceneManager::GetCurrentScene()->CreateGameObject("Game Canvas");
	{
		GameObject::Sptr healthp = UIHelper::CreateImage(Resources::GetTexture("CharacterH"), "Health");
		healthp->Get<RectTransform>()->SetPosition({ 170, 90 });
		healthp->Get<RectTransform>()->SetSize({ 90, 40 }); 
		healthp->Get<GuiPanel>()->SetBorderRadius(0);
		gameCanvas->AddChild(healthp);

		GameObject::Sptr healthText = UIHelper::CreateText1("Body Health: ???", "Body Health Text");
		healthText->Get<RectTransform>()->SetPosition({ 350, 180 });
		gameCanvas->AddChild(healthText);

		GameObject::Sptr shadowhp = UIHelper::CreateImage(Resources::GetTexture("ShadowH"), "Shadow Health");
		shadowhp->Get<RectTransform>()->SetPosition({ 170, 180 });
		shadowhp->Get<RectTransform>()->SetSize({ 90, 40 });
		shadowhp->Get<GuiPanel>()->SetBorderRadius(0);
		gameCanvas->AddChild(shadowhp);

		GameObject::Sptr shadowText = UIHelper::CreateText("Shadow Health: ???", "Shadow Health Text");
		shadowText->Get<RectTransform>()->SetPosition({ 360, 360 });
		gameCanvas->AddChild(shadowText);

		SceneManager::GameInterface.SetGameUserInterface(*gameCanvas);
		SceneManager::GameInterface.InitializeGameUserInterface(*healthText->Get<GuiText>(), *shadowText->Get<GuiText>());
	}

	/////////////////////////////////////////////////////////
	//				   USER INTERFACE - Lose Screen
	/////////////////////////////////////////////////////////

	GameObject::Sptr loseCanvas = SceneManager::GetCurrentScene()->CreateGameObject("Lose Screen");
	{
		RectTransform::Sptr transform = loseCanvas->Add<RectTransform>();
		transform->SetMin({ 16, 16 });
		transform->SetMax({ 900, 900 });
		transform->SetPosition({ 400, 400 });

		GameObject::Sptr loseS = UIHelper::CreateImage(Resources::GetTexture("Lose"), "Lose Screen");
		loseS->Get<RectTransform>()->SetPosition({ 380, 400 });
		loseS->Get<RectTransform>()->SetSize({ 250, 200 });
		loseS->Get<GuiPanel>()->SetBorderRadius(0);
		loseCanvas->AddChild(loseS);

		loseCanvas->IsActive = false;
		SceneManager::GameInterface.SetLosePanel(*loseCanvas);
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

		GameObject::Sptr pa = UIHelper::CreateImage(Resources::GetTexture("Pause"), "Pausing");
		pa->Get<RectTransform>()->SetPosition({ 163, 40 });
		pa->Get<RectTransform>()->SetSize({ 100, 50 });
		pa->Get<GuiPanel>()->SetBorderRadius(0);
		pauseMenu->AddChild(pa);

		GameObject::Sptr resume = UIHelper::CreateImage(Resources::GetTexture("R"), "Resume");
		resume->Get<RectTransform>()->SetPosition({ transform->GetPosition().x / 2.5f, 120 });
		resume->Get<RectTransform>()->SetSize({ 50, 20 });
		resume->Get<GuiPanel>()->SetBorderRadius(0);
		pauseMenu->AddChild(resume);

		GameObject::Sptr restart = UIHelper::CreateImage(Resources::GetTexture("Re"), "Restart");
		restart->Get<RectTransform>()->SetPosition({ transform->GetPosition().x / 2.5f, 200 });
		restart->Get<RectTransform>()->SetSize({ 50, 20 });
		restart->Get<GuiPanel>()->SetBorderRadius(0);
		pauseMenu->AddChild(restart);

		GameObject::Sptr Options1 = UIHelper::CreateImage(Resources::GetTexture("Options"), "Options");
		Options1->Get<RectTransform>()->SetPosition({ transform->GetPosition().x / 2.5f, 280 });
		Options1->Get<RectTransform>()->SetSize({ 50, 20 });
		Options1->Get<GuiPanel>()->SetBorderRadius(0);
		pauseMenu->AddChild(Options1);

		GameObject::Sptr quit = UIHelper::CreateImage(Resources::GetTexture("Return"), "Quit");
		quit->Get<RectTransform>()->SetPosition({ transform->GetPosition().x / 2.5f, 360 });
		quit->Get<RectTransform>()->SetSize({ 50, 20 });
		quit->Get<GuiPanel>()->SetBorderRadius(0);
		pauseMenu->AddChild(quit);

		GameObject::Sptr exit = UIHelper::CreateImage(Resources::GetTexture("Exit"), "Exit");
		exit->Get<RectTransform>()->SetPosition({ transform->GetPosition().x / 2.5f, 440 });
		exit->Get<RectTransform>()->SetSize({ 50, 20 });
		exit->Get<GuiPanel>()->SetBorderRadius(0);
		pauseMenu->AddChild(exit);

		pauseMenu->IsActive = false;
		//SceneManager::GetCurrentScene()->PC.SetPauseMenu(*pauseMenu);
		SceneManager::GameInterface.SetPausePanel(*pauseMenu);
	}

	GuiBatcher::SetDefaultTexture(ResourceManager::CreateAsset<Texture2D>("textures/ui-sprite.png"));
	GuiBatcher::SetDefaultBorderRadius(8);

#pragma endregion

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