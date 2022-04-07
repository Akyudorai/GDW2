#include "DefaultSceneLayer.h"

// GLM math library
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtc/random.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/gtx/common.hpp> // for fmod (floating modulus)

#include <filesystem>

// Graphics
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/VertexArrayObject.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Textures/Texture2D.h"
#include "Graphics/Textures/TextureCube.h"
#include "Graphics/VertexTypes.h"
#include "Graphics/Font.h"
#include "Graphics/GuiBatcher.h"
#include "Graphics/Framebuffer.h"

// Utilities
#include "Utils/MeshBuilder.h"
#include "Utils/MeshFactory.h"
#include "Utils/ObjLoader.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/ResourceManager/ResourceManager.h"
#include "Utils/FileHelpers.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/StringUtils.h"
#include "Utils/GlmDefines.h"
#include "Utils/ResourceManager/Resources.h"
#include "Audio/AudioManager.h"
#include "Utils/UIHelper.h"

// Gameplay
#include "Gameplay/Material.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Gameplay/Components/Light.h"

// Components
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/Components/Camera.h"
#include "Gameplay/Components/RotatingBehaviour.h"
#include "Gameplay/Components/JumpBehaviour.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/MaterialSwapBehaviour.h"
#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Components/SimpleCameraControl.h"
#include "Gameplay/Components/AudioSource.h"
#include "Gameplay/Components/AnimatorComponent.h"
#include "Gameplay/Components/HealthComponent.h"
#include "Gameplay/Components/InteractableComponent.h"
#include "Gameplay/Components/SpikeTrapBehavior.h"
#include "Gameplay/Components/MovingPlatformBehavior.h"
#include "Gameplay/PlayerController.h"

// Physics
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Physics/Colliders/PlaneCollider.h"
#include "Gameplay/Physics/Colliders/SphereCollider.h"
#include "Gameplay/Physics/Colliders/ConvexMeshCollider.h"
#include "Gameplay/Physics/Colliders/CylinderCollider.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Graphics/DebugDraw.h"

// GUI
#include "Gameplay/Components/GUI/RectTransform.h"
#include "Gameplay/Components/GUI/GuiPanel.h"
#include "Gameplay/Components/GUI/GuiText.h"
#include "Gameplay/InputEngine.h"

#include "Application/Application.h"
#include "Gameplay/Components/ParticleSystem.h"
#include "Graphics/Textures/Texture3D.h"
#include "Graphics/Textures/Texture1D.h"
#include "Application/Layers/ImGuiDebugLayer.h"
#include "Application/Windows/DebugWindow.h"
#include "Gameplay/Components/ShadowCamera.h"

#include "Utils/ResourceManager/Prefabs.h"
#include "Gameplay/GameManager.h"

DefaultSceneLayer::DefaultSceneLayer() :
	ApplicationLayer()
{
	Name = "Default Scene";
	Overrides = AppLayerFunctions::OnAppLoad;
}

DefaultSceneLayer::~DefaultSceneLayer() = default;

void DefaultSceneLayer::OnAppLoad(const nlohmann::json& config) {
	_CreateScene();
}

void DefaultSceneLayer::_CreateScene()
{
	using namespace Gameplay;
	using namespace Gameplay::Physics;

	// Should be moved to Game Settings or Physics Class
	int PHYSICAL_MASK = 0xFFFFFFFF;
	int PHYSICAL_GROUP = 0x01;
	int SHADOW_MASK = 0xFFFFFFFE;
	int SHADOW_GROUP = 0x02;
	int NO_MASK = 0xFFFFFFFD;
	int NO_GROUP = 0x03;

	Application& app = Application::Get();

	bool loadScene = false;
	// For now we can use a toggle to generate our scene vs load from file
	if (loadScene && std::filesystem::exists("scene.json")) {
		app.LoadScene("scene.json");
	} else {
		 
		DebugWindow::Sptr debugWindow = app.GetLayer<ImGuiDebugLayer>()->GetWindow<DebugWindow>();

		// Create an empty scene
		Scene::Sptr scene = std::make_shared<Scene>();  

		// Setting up our enviroment map
		scene->SetSkyboxTexture(Resources::GetCubeMap("Ocean"));
		scene->SetSkyboxShader(Resources::GetShader("Skybox"));
		// Since the skybox I used was for Y-up, we need to rotate it 90 deg around the X-axis to convert it to z-up 
		scene->SetSkyboxRotation(glm::rotate(MAT4_IDENTITY, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)));

		// Configure the color correction LUT
		scene->SetColorLUT(Resources::GetTexture3D("Cool Lut"));

		// Character
		// =========================================================================

		GameObject::Sptr camera = scene->MainCamera->GetGameObject()->SelfRef();
		{
			camera->SetPosition({ -9, -6, 15 });
			camera->LookAt(glm::vec3(0.0f));
		}

		GameObject::Sptr character = Prefabs::Load(scene, "Character Body");
		{
			character->SetPosition(glm::vec3(0.0f, -40.0f, 5.4f));
		}


		GameObject::Sptr shadow = Prefabs::Load(scene, "Shadow");
		{
			shadow->SetPosition(glm::vec3(0.0f, -40.0f, 5.4f));
		}

		GameObject::Sptr controller = scene->CreateGameObject("Controller");
		{
			PlayerController::Sptr pc = controller->Add<PlayerController>();
			pc->SetCharacterBody(character);
			pc->SetCharacterShadow(shadow);
			pc->SetInteractionCollider(character->Get<TriggerVolume>());
			pc->SetCamera(camera);

			GameManager::GetInstance().SetPC(pc);
		}

		// Environment
		// =========================================================================

		GameObject::Sptr environment = scene->CreateGameObject("Environment");
		GameObject::Sptr section1 = scene->CreateGameObject("Section 1");
		GameObject::Sptr section2 = scene->CreateGameObject("Section 2");
		GameObject::Sptr section3 = scene->CreateGameObject("Section 3");
		GameObject::Sptr section4 = scene->CreateGameObject("Section 4");
		GameObject::Sptr section5 = scene->CreateGameObject("Section 5");
		GameObject::Sptr section6 = scene->CreateGameObject("Section 6");
		GameObject::Sptr section7 = scene->CreateGameObject("Section 7");

		environment->AddChild(section1);
		environment->AddChild(section2);
		environment->AddChild(section3);
		environment->AddChild(section4);
		environment->AddChild(section5);
		environment->AddChild(section6);
		environment->AddChild(section7);

#pragma region Section 1

		// Interactables
		{
			section1->AddChild(Prefabs::Load(scene, "Cobweb", glm::vec3(-10.0, -33, 8)));

			section1->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(-11.5f, -36.5, 5.75f)));
			section1->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(-9.5f, -35, 5.75f)));
			section1->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(-14, -48, 5.75f)));			
			section1->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(15.75f, -47, 5.75f)));
			section1->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(14, -48, 5.75f)));
			section1->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(15.75, -37, 5.75f)));
			section1->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(3, -33.5, 5.75f)));
			
			section1->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(46.6, -36.9, 5.75f)));
			section1->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(60.3, -22.5, 5.75f)));
			section1->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(67.6, -39.5, 5.75f)));
			section1->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(67.6, -37.7, 5.75f)));
			section1->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(66.6, -38.6, 5.75f)));
			section1->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(61.9, -46.9, 5.75f)));
			section1->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(48.6, -46.9, 5.75f)));

			section1->AddChild(Prefabs::Load(scene, "Shroom Trap Multi", glm::vec3(20, -40, 1)));
			section1->AddChild(Prefabs::Load(scene, "Shroom Trap Single", glm::vec3(40, -47, 1)));

			GameObject::Sptr s1_door = Prefabs::Load(scene, "Cage Door");
			{
				s1_door->SetPosition(glm::vec3(55.630f, -19.510f, 4.5f));
				section1->AddChild(s1_door);

				
			}

			GameObject::Sptr s1_pressure_plate_1 = Prefabs::Load(scene, "Pressure Plate");
			{
				s1_pressure_plate_1->SetPosition(glm::vec3(62.5f, -29, 6));

				s1_pressure_plate_1->Get<TriggerVolumeEnterBehaviour>()->onTriggerEnterEvent.push_back([s1_door]
				{
					s1_door->Get<RigidBody>()->SetCollisionGroup(Resources::Instance().NO_GROUP);
					s1_door->Get<RigidBody>()->SetCollisionMask(Resources::Instance().NO_MASK);
					s1_door->Get<RenderComponent>()->IsEnabled = false;
				});

				s1_pressure_plate_1->Get<TriggerVolumeEnterBehaviour>()->onTriggerExitEvent.push_back([s1_door]
				{
					s1_door->Get<RigidBody>()->SetCollisionGroupMulti(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
					s1_door->Get<RigidBody>()->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);
					s1_door->Get<RenderComponent>()->IsEnabled = true;
				});

				section1->AddChild(s1_pressure_plate_1);
			}
		}

		// Props
		{
			section1->AddChild(Prefabs::Load(scene, "Crystal", glm::vec3(7, -45, 5)));
			
			section1->AddChild(Prefabs::Load(scene, "Crystal", glm::vec3(-2, -23, 5)));
			section1->AddChild(Prefabs::Load(scene, "Crystal", glm::vec3(52, -23, 5)));
			section1->AddChild(Prefabs::Load(scene, "Crystal", glm::vec3(59, -43, 5)));

			section1->AddChild(Prefabs::Load(scene, "Floor Grate", glm::vec3(14.75f, -36.f, 5.71f)));
			section1->AddChild(Prefabs::Load(scene, "Pillar", glm::vec3(65.8f, -46.15f, 5.48f)));
			section1->AddChild(Prefabs::Load(scene, "Gravestone", glm::vec3(7.13f, -33.16f, 5.45f)));

			GameObject::Sptr swordProp_1 = Prefabs::Load(scene, "Sword");
			{
				swordProp_1->SetPosition(glm::vec3(24.48f, -45.57f, 1.15f));
				swordProp_1->SetRotation(glm::vec3(0.0f, 0.0f, -56.0f));
				section1->AddChild(swordProp_1);
			}

			GameObject::Sptr swordProp_3 = Prefabs::Load(scene, "Sword");
			{
				swordProp_3->SetPosition(glm::vec3(49.53f, -24.34f, 6.93f));
				swordProp_3->SetRotation(glm::vec3(-45.0f, 0.0f, -180.0f));
				section1->AddChild(swordProp_3);
			}

			GameObject::Sptr shieldProp_1 = Prefabs::Load(scene, "Shield");
			{
				shieldProp_1->SetPosition(glm::vec3(7.16f, -33.91f, 6.05f));
				shieldProp_1->SetRotation(glm::vec3(52.f, 0.0f, 0.0f));
				shieldProp_1->SetScale(glm::vec3(0.2f));
				section1->AddChild(shieldProp_1);
			}

			GameObject::Sptr shieldProp_2 = Prefabs::Load(scene, "Shield");
			{
				shieldProp_2->SetPosition(glm::vec3(46.01f, -43.75f, 1.25f));
				shieldProp_2->SetRotation(glm::vec3(-13.f, 1.0f, -48.0f));
				shieldProp_2->SetScale(glm::vec3(0.3f));
				section1->AddChild(shieldProp_2);
			}

			GameObject::Sptr shieldProp_3 = Prefabs::Load(scene, "Shield");
			{
				shieldProp_3->SetPosition(glm::vec3(32.0f, -42.33f, 1.22f));
				shieldProp_3->SetRotation(glm::vec3(0.0f, 1.0f, 45.0f));
				shieldProp_3->SetScale(glm::vec3(0.3f));
				section1->AddChild(shieldProp_3);
			}

			GameObject::Sptr shieldProp_4 = Prefabs::Load(scene, "Shield");
			{
				shieldProp_4->SetPosition(glm::vec3(44.61f, -44.7f, 1.45f));
				shieldProp_4->SetRotation(glm::vec3(-13.f, 1.0f, 131.0f));
				shieldProp_4->SetScale(glm::vec3(0.5f));
				section1->AddChild(shieldProp_4);
			}

			GameObject::Sptr staffProp_1 = Prefabs::Load(scene, "Staff");
			{
				staffProp_1->SetPosition(glm::vec3(37.48f, -44.88f, 1.0f));
				staffProp_1->SetRotation(glm::vec3(-13.f, -87.0f, 56.0f));
				section1->AddChild(staffProp_1);
			}

			GameObject::Sptr staffProp_2 = Prefabs::Load(scene, "Staff");
			{
				staffProp_2->SetPosition(glm::vec3(15.27f, -22.7f, 5.98f));
				staffProp_2->SetRotation(glm::vec3(-4.f, -90.0f, -45.0f));
				section1->AddChild(staffProp_2);
			}

			GameObject::Sptr damaged_pillar_1 = Prefabs::Load(scene, "Damaged Pillar");
			{
				damaged_pillar_1->SetPosition(glm::vec3(-12.12f, -34.46f, 5.45f));
				damaged_pillar_1->SetScale(glm::vec3(1.0f));
				section1->AddChild(damaged_pillar_1);
			}

			GameObject::Sptr rocks_1 = Prefabs::Load(scene, "Rocks");
			{
				rocks_1->SetPosition(glm::vec3(-6.47f, -44.0f, 5.9f));
				rocks_1->SetScale(glm::vec3(1.25f));
				section1->AddChild(rocks_1);
			}

			GameObject::Sptr rocks_2 = Prefabs::Load(scene, "Rocks");
			{
				rocks_2->SetPosition(glm::vec3(12.01f, -25.12f, 5.9f));
				rocks_2->SetScale(glm::vec3(1.5f));
				section1->AddChild(rocks_2);
			}
		}

		// Floors
		{
			GameObject::Sptr floor0 = Prefabs::Load(scene, "Floor");
			{
				floor0->SetPosition(glm::vec3(0.3f, -35.0f, 5.4f));
				floor0->SetScale(glm::vec3(0.67f, 0.65f, 1.0f));
				floor0->Get<RigidBody>()->AddCollider(BoxCollider::Create(glm::vec3(16.75f, 16.25f, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
				section1->AddChild(floor0);
			}

			GameObject::Sptr floor1 = Prefabs::Load(scene, "Floor");
			{
				floor1->SetPosition(glm::vec3(31.0f, -33.0f, 1.0f));
				floor1->SetScale(glm::vec3(0.75f, 0.65f, 1.0f));
				floor1->Get<RigidBody>()->AddCollider(BoxCollider::Create(glm::vec3(18.75f, 16.25f, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
				floor1->Get<RenderComponent>()->SetMaterial(Resources::GetMaterial("Brown"));
				section1->AddChild(floor1);
			}

			GameObject::Sptr floor2 = Prefabs::Load(scene, "Floor");
			{
				floor2->SetPosition(glm::vec3(59.0f, -33.5f, 5.5f));
				floor2->SetScale(glm::vec3(0.5f, 0.6f, 1.0f));
				floor2->Get<RigidBody>()->AddCollider(BoxCollider::Create(glm::vec3(12.5f, 15.0f, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
				section1->AddChild(floor2);
			}
		}

		//Platforms
		{
			GameObject::Sptr Platform1 = Prefabs::Load(scene, "Platform");
			{
				Platform1->SetPosition(glm::vec3(26.5f, -40.0f, 4.0f));
				Platform1->SetScale(glm::vec3(4.0f, 2.5f, 0.8f));
				section1->AddChild(Platform1);

				// Collider
				RigidBody::Sptr physics = Platform1->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(Platform1->GetScale());
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
				physics->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);
			}

			GameObject::Sptr Platform2 = Prefabs::Load(scene, "Platform");
			{
				Platform2->SetPosition(glm::vec3(27.0f, -45.0f, 3.0f));
				Platform2->SetRotation(glm::vec3(20.0f, 0.0f, 0.0f));
				Platform2->SetScale(glm::vec3(0.8f, 3.0f, 0.3f));
				section1->AddChild(Platform2);

				// Collider
				RigidBody::Sptr physics = Platform2->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(Platform2->GetScale());
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
				physics->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);
			}

			GameObject::Sptr Platform3 = Prefabs::Load(scene, "Platform");
			{
				Platform3->SetPosition(glm::vec3(40.0f, -40.0f, 4.0f));
				Platform3->SetScale(glm::vec3(4.0f, 2.5f, 0.8f));
				section1->AddChild(Platform1);

				// Collider
				RigidBody::Sptr physics = Platform3->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(Platform3->GetScale());
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
				physics->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);
			}
		}

		// Walls
		{
			GameObject::Sptr wall2 = Prefabs::Load(scene, "Wall");
			{
			wall2->SetPosition(glm::vec3(18.0f, -28.5f, 4.0f));
			wall2->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			wall2->SetScale(glm::vec3(1.0f, 1.0f, 0.6f));
			BoxCollider::Sptr collider = BoxCollider::Create();
			// Example to modify scale and position of collider
			// collider->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
			// collider->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
			wall2->Get<RigidBody>()->AddCollider(collider);
			collider->SetScale(glm::vec3(2.03f,6.42f,9.16f));
			collider->SetPosition(glm::vec3(0.65f, 1.49f, -0.46f));
			section1->AddChild(wall2);
			}

			GameObject::Sptr wall3 = Prefabs::Load(scene, "Wall");
			{
				wall3->SetPosition(glm::vec3(9.5f, -31.5f, 4.0f));
				wall3->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall3->SetScale(glm::vec3(1.0f, 1.0f, 0.5f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall3->Get<RigidBody>()->AddCollider(collider);
				collider->SetScale(glm::vec3(1.52f, 5.81f,9.46f));
				collider->SetPosition(glm::vec3(-0.1f, 4.32f, 1.74f));
				section1->AddChild(wall3);
			}

			GameObject::Sptr wall4 = Prefabs::Load(scene, "Wall");
			{
				wall4->SetPosition(glm::vec3(-10.5f, -31.5f, 4.0f));
				wall4->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall4->SetScale(glm::vec3(1.0f, 1.0f, 0.5f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall4->Get<RigidBody>()->AddCollider(collider);
				collider->SetScale(glm::vec3(1.3f,6.16f,7.43f));
				collider->SetPosition(glm::vec3(0.31f,3.13f,0.0f));
				section1->AddChild(wall4);
			}

			GameObject::Sptr wall6 = Prefabs::Load(scene, "Wall");
			{
				wall6->SetPosition(glm::vec3(-16.5f, -49.375f, 4.0f));
				wall6->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall6->SetScale(glm::vec3(1.0f, 1.0f, 0.125f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall6->Get<RigidBody>()->AddCollider(collider);
				collider->SetScale(glm::vec3(1.0f,4.59f,2.16f));
				collider->SetPosition(glm::vec3(-0.24f,3.33f, 0.0f));
				section1->AddChild(wall6);
			}

			GameObject::Sptr wall7 = Prefabs::Load(scene, "Wall");
			{
				wall7->SetPosition(glm::vec3(-9.95f, -50.0f, 4.0f));
				wall7->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall7->SetScale(glm::vec3(1.0f, 1.0f, 0.35f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall7->Get<RigidBody>()->AddCollider(collider);
				collider->SetScale(glm::vec3(1.0f,6.030f,5.52f));
				collider->SetPosition(glm::vec3(0.1f, 1.74f, 0.0f));
				section1->AddChild(wall7);
			}

			GameObject::Sptr wall8 = Prefabs::Load(scene, "Wall");
			{
				wall8->SetPosition(glm::vec3(10.0f, -50.0f, 4.0f));
				wall8->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall8->SetScale(glm::vec3(1.0f, 1.0f, 0.5f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall8->Get<RigidBody>()->AddCollider(collider);
				collider->SetScale(glm::vec3(1.09f,6.16f,10.03f));
				collider->SetPosition(glm::vec3(-0.210f,1.47,2.1f));
				section1->AddChild(wall8);
			}

			GameObject::Sptr wall11 = Prefabs::Load(scene, "Wall");
			{
				wall11->SetPosition(glm::vec3(44.91f, -49.66f, 4.0f));
				wall11->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall11->SetScale(glm::vec3(1.0f, 1.0f, 1.73f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall11->Get<RigidBody>()->AddCollider(collider);
				collider->SetScale(glm::vec3(1.25f,7.13f,26.42f));
				collider->SetPosition(glm::vec3(0.13f,1.25f,-0.95f));
				section1->AddChild(wall11);
			}

			GameObject::Sptr wall12 = Prefabs::Load(scene, "Wall");
			{
				wall12->SetPosition(glm::vec3(69.58f, -34.66f, 4.0f));
				wall12->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall12->SetScale(glm::vec3(1.0f, 1.0f, 0.92f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall12->Get<RigidBody>()->AddCollider(collider);
				collider->SetScale(glm::vec3(1.49f, 6.4f, 20.840f));
				collider->SetPosition(glm::vec3(-0.45f,0.74f,0.0f));
				section1->AddChild(wall12);
			}

			GameObject::Sptr wall13 = Prefabs::Load(scene, "Wall");
			{
				wall13->SetPosition(glm::vec3(64.32f, -19.78f, 4.0f));
				wall13->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall13->SetScale(glm::vec3(1.0f, 1.0f, 0.4f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall13->Get<RigidBody>()->AddCollider(collider);
				collider->SetScale(glm::vec3(1.25f,4.48f,-7.28f));
				collider->SetPosition(glm::vec3(0.96f,3.5f,1.02f));
				section1->AddChild(wall13);
			}

			GameObject::Sptr wall14 = Prefabs::Load(scene, "Wall");
			{
				wall14->SetPosition(glm::vec3(47.41f, -29.4f, 4.0f));
				wall14->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall14->SetScale(glm::vec3(1.0f, 1.0f, 0.55f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall14->Get<RigidBody>()->AddCollider(collider);
				collider->SetScale(glm::vec3(1.39f,5.96f,9.57f));
				collider->SetPosition(glm::vec3(0.0f,2.99f,-0.97f));
				section1->AddChild(wall14);
			}

			GameObject::Sptr wall59 = Prefabs::Load(scene, "Wall");
			{
				wall59->SetPosition(glm::vec3(32.810f, -36.58f, 4.5f));
				wall59->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall59->SetScale(glm::vec3(1.0f, 1.0f, 0.89f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall59->Get<RigidBody>()->AddCollider(collider);
				collider->SetScale(glm::vec3(1.28f,8.77f,16.06f));
				section1->AddChild(wall59);
			}

			GameObject::Sptr s1_wall_1 = Prefabs::Load(scene, "Wall");
			{
				s1_wall_1->SetPosition(glm::vec3(15.7f, -42.9f, -2.93f));
				s1_wall_1->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				s1_wall_1->SetScale(glm::vec3(1.0f, 0.97f, 0.42f));
				s1_wall_1->Get<RigidBody>()->AddCollider(BoxCollider::Create(glm::vec3(1)))->SetPosition(s1_wall_1->GetPosition());
				section1->AddChild(s1_wall_1);
			}

			GameObject::Sptr s1_wall_2 = Prefabs::Load(scene, "Wall");
			{
				s1_wall_2->SetPosition(glm::vec3(47.51f, -42.9f, -2.93f));
				s1_wall_2->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				s1_wall_2->SetScale(glm::vec3(1.0f, 1.0f, 0.6f));
				s1_wall_2->Get<RigidBody>()->AddCollider(BoxCollider::Create(glm::vec3(1)))->SetPosition(s1_wall_1->GetPosition());
				section1->AddChild(s1_wall_2);
			}
		}

#pragma endregion

#pragma region Section 2

		// Interactables		
		{
			section2->AddChild(Prefabs::Load(scene, "Healing Well", glm::vec3(15, -8, 5)));

			section2->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(60.3f, -1.35, 5.75f)));
			section2->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(58.2f, 0.4, 5.75f)));

			GameObject::Sptr s2_torch_1 = Prefabs::Load(scene, "Torch", glm::vec3(40.75, -8.5, 5));
			{				
				for (auto& c : s2_torch_1->GetChildren())
				{
					if (c->Get<Light>() != nullptr) {
						c->Get<Light>()->SetRadius(10);
					}
				}

				section2->AddChild(s2_torch_1);
			}

			GameObject::Sptr s2_torch_2 = Prefabs::Load(scene, "Torch", glm::vec3(30.75, -8.5, 5));
			{
				for (auto& c : s2_torch_2->GetChildren())
				{
					if (c->Get<Light>() != nullptr) {
						c->Get<Light>()->SetRadius(10);
					}
				}

				section2->AddChild(s2_torch_1);
			}
			
			section2->AddChild(Prefabs::Load(scene, "Spike Trap", glm::vec3(46.0f, -1.0f, 5.8f)));
			section2->AddChild(Prefabs::Load(scene, "Spike Trap", glm::vec3(46.0f, -6.5f, 5.8f)));
			section2->AddChild(Prefabs::Load(scene, "Spike Trap", glm::vec3(46.0f, -12.0f, 5.8f)));
			section2->AddChild(Prefabs::Load(scene, "Spike Trap", glm::vec3(46.0f, -17.5f, 5.8f)));

			section2->AddChild(Prefabs::Load(scene, "Spike Trap", glm::vec3(35.5f, -1.0f, 5.8f)));
			section2->AddChild(Prefabs::Load(scene, "Spike Trap", glm::vec3(35.5f, -6.5f, 5.8f)));
			section2->AddChild(Prefabs::Load(scene, "Spike Trap", glm::vec3(35.5f, -12.0f, 5.8f)));
			section2->AddChild(Prefabs::Load(scene, "Spike Trap", glm::vec3(35.5f, -17.5f, 5.8f)));

			section2->AddChild(Prefabs::Load(scene, "Spike Trap", glm::vec3(26.25f, -1.0f, 5.8f)));
			section2->AddChild(Prefabs::Load(scene, "Spike Trap", glm::vec3(26.25f, -6.5f, 5.8f)));
			section2->AddChild(Prefabs::Load(scene, "Spike Trap", glm::vec3(26.25f, -12.0f, 5.8f)));
			section2->AddChild(Prefabs::Load(scene, "Spike Trap", glm::vec3(26.25f, -17.5f, 5.8f)));
		}

		// Props
		{
			section2->AddChild(Prefabs::Load(scene, "Crystal", glm::vec3(52, -2.2, 4.8)));
			section2->AddChild(Prefabs::Load(scene, "Crystal", glm::vec3(30, -10, 5)));
			section2->AddChild(Prefabs::Load(scene, "Crystal", glm::vec3(-8, -3, 5)));

			GameObject::Sptr swordProp_2 = Prefabs::Load(scene, "Sword");
			{
				swordProp_2->SetPosition(glm::vec3(61.61f, -2.7f, 5.68f));
				swordProp_2->SetRotation(glm::vec3(0.0f, 0.0f, -167.0f));
				section2->AddChild(swordProp_2);
			}

			GameObject::Sptr swordProp_4 = Prefabs::Load(scene, "Sword");
			{
				swordProp_4->SetPosition(glm::vec3(18.31f, -0.22f, 5.61f));
				swordProp_4->SetRotation(glm::vec3(0.0f, 0.0f, 57.0f));
				section2->AddChild(swordProp_4);
			}

			GameObject::Sptr swordProp_5 = Prefabs::Load(scene, "Sword");
			{
				swordProp_5->SetPosition(glm::vec3(-2.f, -6.89f, 6.f));
				swordProp_5->SetRotation(glm::vec3(0.0f, -180.0f, 121.0f));
				section2->AddChild(swordProp_5);
			}

			GameObject::Sptr swordProp_6 = Prefabs::Load(scene, "Sword");
			{
				swordProp_6->SetPosition(glm::vec3(-18.82f, -9.05f, 6.f));
				swordProp_6->SetRotation(glm::vec3(0.0f, 0.0f, -67.0f));
				section2->AddChild(swordProp_6);
			}

			GameObject::Sptr shieldProp_5 = Prefabs::Load(scene, "Shield");
			{
				shieldProp_5->SetPosition(glm::vec3(58.85f, -2.7f, 5.68f));
				shieldProp_5->SetRotation(glm::vec3(0.f, 0.0f, 81.0f));
				shieldProp_5->SetScale(glm::vec3(0.25f));
				section2->AddChild(shieldProp_5);
			}

			GameObject::Sptr shieldProp_6 = Prefabs::Load(scene, "Shield");
			{
				shieldProp_6->SetPosition(glm::vec3(19.04f, -0.81f, 2.82f));
				shieldProp_6->SetRotation(glm::vec3(-37.f, -41.0f, -134.0f));
				shieldProp_6->SetScale(glm::vec3(0.25f));
				section2->AddChild(shieldProp_6);
			}

			GameObject::Sptr shieldProp_7 = Prefabs::Load(scene, "Shield");
			{
				shieldProp_7->SetPosition(glm::vec3(3.51f, -5.88f, 5.63f));
				shieldProp_7->SetRotation(glm::vec3(0.f, 0.0f, -70.0f));
				shieldProp_7->SetScale(glm::vec3(0.3f));
				section2->AddChild(shieldProp_7);
			}

			GameObject::Sptr staffProp_3 = Prefabs::Load(scene, "Staff");
			{
				staffProp_3->SetPosition(glm::vec3(29.88f, -8.08f, 1.89f));
				staffProp_3->SetRotation(glm::vec3(0.f, -90.0f, 39.0f));
				section2->AddChild(staffProp_3);
			}

			GameObject::Sptr staffProp_4 = Prefabs::Load(scene, "Staff");
			{
				staffProp_4->SetPosition(glm::vec3(-9.82f, -10.3f, 5.4f));
				staffProp_4->SetRotation(glm::vec3(-34.f, -87.0f, -86.0f));
				section2->AddChild(staffProp_4);
			}

			GameObject::Sptr damaged_pillar_3 = Prefabs::Load(scene, "Damaged Pillar");
			{
				damaged_pillar_3->SetPosition(glm::vec3(60.71f, -16.91f, 5.45f));
				damaged_pillar_3->SetScale(glm::vec3(0.7f));
				section2->AddChild(damaged_pillar_3);
			}

			GameObject::Sptr destroyed_pillar_2 = Prefabs::Load(scene, "Destroyed Pillar");
			{
				destroyed_pillar_2->SetPosition(glm::vec3(15.35f, -16.16f, 5.45f));
				destroyed_pillar_2->SetScale(glm::vec3(1.0f));
				section2->AddChild(destroyed_pillar_2);
			}
		}

		// Floors
		{
			GameObject::Sptr floor5 = Prefabs::Load(scene, "Floor");
			{
				floor5->SetPosition(glm::vec3(-13.0f, -2.5f, 5.0f));
				floor5->SetScale(glm::vec3(0.6f, 0.72f, 1.0f));
				floor5->Get<RigidBody>()->AddCollider(BoxCollider::Create(glm::vec3(15.0f, 18, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
				section2->AddChild(floor5);
			}

			GameObject::Sptr floor6 = Prefabs::Load(scene, "Floor");
			{
				floor6->SetPosition(glm::vec3(17.0f, -2.5f, 5.0f));
				floor6->SetScale(glm::vec3(0.6f, 0.72f, 1.0f));
				floor6->Get<RigidBody>()->AddCollider(BoxCollider::Create(glm::vec3(15.0f, 18, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
				section2->AddChild(floor6);
			}

			GameObject::Sptr floor7 = Prefabs::Load(scene, "Floor");
			{
				floor7->SetPosition(glm::vec3(47.0f, -2.5f, 5.0f));
				floor7->SetScale(glm::vec3(0.6f, 0.72f, 1.0f));
				floor7->Get<RigidBody>()->AddCollider(BoxCollider::Create(glm::vec3(15.0f, 18, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
				section2->AddChild(floor7);
			}
		}

		// Walls
		{
			GameObject::Sptr wall1 = Prefabs::Load(scene, "Wall");
			{
				wall1->SetPosition(glm::vec3(-8.590, -20.0f, 4.0f));
				wall1->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall1->SetScale(glm::vec3(1.0f, 1.0f, 1.67f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall1->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(0.25f,-3.63f, 17.410f));
				collider->SetScale(glm::vec3(1.5f, 14.55f, 43.07f));	

				section2->AddChild(wall1);
			}
			
			GameObject::Sptr wall9 = Prefabs::Load(scene, "Wall");
			{
				wall9->SetPosition(glm::vec3(36.25f, -19.75f, 4.0f));
				wall9->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall9->SetScale(glm::vec3(1.0f, 1.0f, 1.1f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall9->Get<RigidBody>()->AddCollider(collider);
				section2->AddChild(wall9);
			}

			GameObject::Sptr wall16 = Prefabs::Load(scene, "Wall");
			{
				wall16->SetPosition(glm::vec3(37.5f, 3.0f, 4));
				wall16->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall16->SetScale(glm::vec3(1.0f, 1.0f, 1.65f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall16->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(0.46f, 1.95f, 0.0f));
				collider->SetScale(glm::vec3(1.5f,6.03f, 29.95f));
				section2->AddChild(wall16);
			}

			GameObject::Sptr wall18 = Prefabs::Load(scene, "Wall");
			{
				wall18->SetPosition(glm::vec3(63.190f, -7.0f, 4.0f));
				wall18->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall18->SetScale(glm::vec3(1.0f, 1.0f, 0.75f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall18->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(-1.05f,0.00f,-1.57f));
				collider->SetScale(glm::vec3(1.25f, 7.88f, 14.59f));
				section2->AddChild(wall18);
			}

			GameObject::Sptr wall22 = Prefabs::Load(scene, "Wall");
			{
				wall22->SetPosition(glm::vec3(-10.740f, 2.75f, 4.0f));
				wall22->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall22->SetScale(glm::vec3(1.0f, 1.0f, 1.48f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall22->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(0.45f, 0.0f, 0.0f));
				collider->SetScale(glm::vec3(1.75f, 8.15f, 24.12f));
				section2->AddChild(wall22);
			}
		}

#pragma endregion

#pragma region Section 3

		// Interactables
		{
			section3->AddChild(Prefabs::Load(scene, "Shroom Trap Multi", glm::vec3(-40, 0, 5)));
			section3->AddChild(Prefabs::Load(scene, "Shroom Trap Multi", glm::vec3(-54, -13, 5)));
			section3->AddChild(Prefabs::Load(scene, "Shroom Trap Single", glm::vec3(-51, 4, 5)));
			section3->AddChild(Prefabs::Load(scene, "Shroom Trap Single", glm::vec3(-45, 3, 5)));
			section3->AddChild(Prefabs::Load(scene, "Torch", glm::vec3(-37.6, -3.4, 5)));
			section3->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(-53.85f, -15.3, 9.84f)));
			section3->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(-37, -17.2, 13.3f)));
			section3->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(-48.1, -20.9, 15.2f)));
			section3->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(-36.8, -22.6, 15.2f)));
		}

		// Props
		{
			section3->AddChild(Prefabs::Load(scene, "Pillar", glm::vec3(-54.01f, 6.05f, 5.45f)));
			section3->AddChild(Prefabs::Load(scene, "Pillar", glm::vec3(-54.11f, -21.3f, 5.45f)));

			GameObject::Sptr shieldProp_8 = Prefabs::Load(scene, "Shield");
			{
				shieldProp_8->SetPosition(glm::vec3(-39.92f, -12.12f, 5.63f));
				shieldProp_8->SetRotation(glm::vec3(0.f, 0.0f, -135.0f));
				shieldProp_8->SetScale(glm::vec3(0.25f));
				section3->AddChild(shieldProp_8);
			}

			GameObject::Sptr staffProp_5 = Prefabs::Load(scene, "Staff");
			{
				staffProp_5->SetPosition(glm::vec3(-51.8f, 6.54f, 6.85f));
				staffProp_5->SetRotation(glm::vec3(0.f, 17.0f, -9.0f));
				section3->AddChild(staffProp_5);
			}

			GameObject::Sptr damaged_pillar_2 = Prefabs::Load(scene, "Damaged Pillar");
			{
				damaged_pillar_2->SetPosition(glm::vec3(-40.3f, -21.3f, 5.45f));
				damaged_pillar_2->SetScale(glm::vec3(1.5f, 1.17f, 1.5f));
				section3->AddChild(damaged_pillar_2);
			}

			GameObject::Sptr destroyed_pillar_1 = Prefabs::Load(scene, "Destroyed Pillar");
			{
				destroyed_pillar_1->SetPosition(glm::vec3(-40.4f, 6.25f, 5.45f));
				destroyed_pillar_1->SetScale(glm::vec3(1.f, 0.56f, 1.f));
				section3->AddChild(destroyed_pillar_1);
			}

			GameObject::Sptr rocks_3 = Prefabs::Load(scene, "Rocks");
			{
				rocks_3->SetPosition(glm::vec3(9.53f, -8.01f, 5.7f));
				rocks_3->SetScale(glm::vec3(1.25f));
				section3->AddChild(rocks_3);
			}
		}

		// Floors
		{
			GameObject::Sptr floor3 = Prefabs::Load(scene, "Floor");
			{
				floor3->SetPosition(glm::vec3(-43.0f, -7.0f, 5.0f));
				floor3->SetScale(glm::vec3(0.6f, 0.72f, 1.0f));
				floor3->Get<RigidBody>()->AddCollider(BoxCollider::Create(glm::vec3(15.0f, 18, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
				floor3->Get<RenderComponent>()->SetMaterial(Resources::GetMaterial("Brown"));
				section3->AddChild(floor3);
			}
		}

		//Platforms
		{
			GameObject::Sptr Platform4 = Prefabs::Load(scene, "Platform");
			{
				Platform4->SetPosition(glm::vec3(-40.0f, 9.5f, 5.3f));
				Platform4->SetScale(glm::vec3(2.0f, 2.0f, 1.0f));
				section3->AddChild(Platform4);

				// Collider
				RigidBody::Sptr physics = Platform4->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(Platform4->GetScale());
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
				physics->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);
			}

			GameObject::Sptr Platform5 = Prefabs::Load(scene, "Platform");
			{
				Platform5->SetPosition(glm::vec3(-47.0f, 9.5f, 6.6f));
				Platform5->SetScale(glm::vec3(4.25f, 2.0f, 1.0f));
				section3->AddChild(Platform5);

				// Collider
				RigidBody::Sptr physics = Platform5->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(Platform5->GetScale());
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
				physics->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);
			}

			GameObject::Sptr Platform6 = Prefabs::Load(scene, "Platform");
			{
				Platform6->SetPosition(glm::vec3(-55.0f, 0.0f, 7.5f));
				Platform6->SetScale(glm::vec3(2.0f, 4.5f, 1.0f));
				section3->AddChild(Platform6);

				// Collider
				RigidBody::Sptr physics = Platform6->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(Platform6->GetScale());
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
				physics->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);
			}

			GameObject::Sptr Platform7 = Prefabs::Load(scene, "Platform");
			{
				Platform7->SetPosition(glm::vec3(-54.5f, -12.0f, 8.5f));
				Platform7->SetScale(glm::vec3(2.0f, 4.0f, 1.0f));
				section3->AddChild(Platform7);

				// Collider
				RigidBody::Sptr physics = Platform7->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(Platform7->GetScale());
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
				physics->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);
			}

			GameObject::Sptr Platform8 = Prefabs::Load(scene, "Platform");
			{
				Platform8->SetPosition(glm::vec3(-46.0f, -14.0f, 10.5f));
				Platform8->SetRotation(glm::vec3(0.0f, -11.0f, 0.0f));
				Platform8->SetScale(glm::vec3(7.0f, 0.7f, 0.2f));
				section3->AddChild(Platform8);

				// Collider
				RigidBody::Sptr physics = Platform8->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(Platform8->GetScale());
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
				physics->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);
			}

			GameObject::Sptr Platform9 = Prefabs::Load(scene, "Platform");
			{
				Platform9->SetPosition(glm::vec3(-37.0f, -15.5f, 12.0f));
				Platform9->SetScale(glm::vec3(2.0f, 2.2f, 1.0f));
				section3->AddChild(Platform9);

				// Collider
				RigidBody::Sptr physics = Platform9->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(Platform9->GetScale());
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
				physics->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);
			}
		}

		// Walls
		{
			GameObject::Sptr wall25 = Prefabs::Load(scene, "Wall");
			{
				wall25->SetPosition(glm::vec3(-34.91f, 2.75f, 9.75f));
				wall25->SetRotation(glm::vec3(90.f, 0.0f, 00.0f));
				wall25->SetScale(glm::vec3(1.0f, 1.35f, 0.5f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall25->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(0.09f, -5.24f, -6.76f));
				collider->SetScale(glm::vec3(1.30f, 8.070f, 14.44f));
				section3->AddChild(wall25);
			}

			GameObject::Sptr wall26 = Prefabs::Load(scene, "Wall");
			{
				wall26->SetPosition(glm::vec3(-46.5f, -24.5f, 7.0f));
				wall26->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall26->SetScale(glm::vec3(1.0f, 1.0f, 0.75f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall26->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(0.0f, -1.94f, 0.0f));
				collider->SetScale(glm::vec3(1.38f, 8.43f, 20.55f));
				section3->AddChild(wall26);
			}

			GameObject::Sptr wall28 = Prefabs::Load(scene, "Wall");
			{
				wall28->SetPosition(glm::vec3(-53.36, -24.5f, 16.75f));
				wall28->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall28->SetScale(glm::vec3(1.0f, 0.5f, 0.235f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall28->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(0.19f, 0.0f, 0.0f));
				collider->SetScale(glm::vec3(1.47f, 4.22f, 4.00f));
				section3->AddChild(wall28);
			}

			GameObject::Sptr wall29 = Prefabs::Load(scene, "Wall");
			{
				wall29->SetPosition(glm::vec3(-58, -6.76f, 9.75f));
				wall29->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall29->SetScale(glm::vec3(1.0f, 1.35f, 1.25f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall29->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(0.0f, -0.68f, 0.0f));
				collider->SetScale(glm::vec3(1.54f, 20.94f, 19.77f));
				section3->AddChild(wall29);
			}

			GameObject::Sptr wall30 = Prefabs::Load(scene, "Wall");
			{
				wall30->SetPosition(glm::vec3(-35.02f, -18.5f, 9.75f));
				wall30->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall30->SetScale(glm::vec3(1.0f, 1.35f, 0.4f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall30->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(0.0f, -1.64f, 0.0f));
				collider->SetScale(glm::vec3(1.55f, 13.1f, 6.27f));
				section3->AddChild(wall30);
			}
		}

#pragma endregion

#pragma region Section 4

		// Interactables
		{
			section4->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(-32.8, -38.1, 15.2f)));
			section4->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(-42.5, -26.5, 15.2f)));
			section4->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(-26.7, -26.5, 15.2f)));
			section4->AddChild(Prefabs::Load(scene, "Candle", glm::vec3(-46.9, -37.6, 15.2f)));
			GameObject::Sptr s4_candle_1  = Prefabs::Load(scene, "Candle", glm::vec3(-18.75, -41.4, 5.2f));
			{

				section4->AddChild(s4_candle_1);
			}

			GameObject::Sptr s4_plate_door = Prefabs::Load(scene, "Cage Door");
			{
				s4_plate_door->SetPosition(glm::vec3(-21.250f, -23.0f, 15.0f));
				s4_plate_door->SetRotation(glm::vec3(90, 0, 90));
				section4->AddChild(s4_plate_door);
			}

			GameObject::Sptr s4_pressure_plate = Prefabs::Load(scene, "Pressure Plate");
			{
				s4_pressure_plate->SetPosition(glm::vec3(-10.5f, -26.5f, 6.0f));
				s4_pressure_plate->SetScale(glm::vec3(0.75f, 0.5f, 0.75f));

				s4_pressure_plate->Get<TriggerVolumeEnterBehaviour>()->onTriggerEnterEvent.push_back([s4_plate_door]
				{
					s4_plate_door->Get<RigidBody>()->SetCollisionGroup(Resources::Instance().NO_GROUP);
					s4_plate_door->Get<RigidBody>()->SetCollisionMask(Resources::Instance().NO_MASK);
					s4_plate_door->Get<RenderComponent>()->IsEnabled = false;
				});

				s4_pressure_plate->Get<TriggerVolumeEnterBehaviour>()->onTriggerExitEvent.push_back([s4_plate_door]
				{
					s4_plate_door->Get<RigidBody>()->SetCollisionGroupMulti(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
					s4_plate_door->Get<RigidBody>()->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);
					s4_plate_door->Get<RenderComponent>()->IsEnabled = true;
				});

				section4->AddChild(s4_pressure_plate);
			}

			GameObject::Sptr s4_lever_door_1 = Prefabs::Load(scene, "Cage Door");
			{
				s4_lever_door_1->SetPosition(glm::vec3(-0.5f, -31.5f, 5.0f));
				s4_lever_door_1->SetRotation(glm::vec3(90, 0, 90));
				section4->AddChild(s4_lever_door_1);
			}

			GameObject::Sptr s4_lever_door_2 = Prefabs::Load(scene, "Cage Door");
			{
				s4_lever_door_1->SetPosition(glm::vec3(-16.0f, -44.5f, 5.0f));
				s4_lever_door_2->SetRotation(glm::vec3(90, 0, 0));
				section4->AddChild(s4_lever_door_2);
			}

			GameObject::Sptr s4_lever_door_3 = Prefabs::Load(scene, "Cage Door");
			{
				s4_lever_door_1->SetPosition(glm::vec3(-24.25f, -39.0f, 15.0f));
				s4_lever_door_3->SetRotation(glm::vec3(90, 0, 90));
				section4->AddChild(s4_lever_door_3);
			}

			GameObject::Sptr s4_lever = Prefabs::Load(scene, "Lever");
			{
				s4_lever->SetPosition(glm::vec3(-19.5f, -31.0f, 15.78f));

				s4_lever->Get<InteractableComponent>()->onInteractionEvent.push_back([s4_lever, s4_lever_door_1, s4_lever_door_2, s4_lever_door_3] {

					if (s4_lever->Get<InteractableComponent>()->isToggled)
					{
						s4_lever_door_1->Get<RigidBody>()->SetCollisionGroup(Resources::Instance().NO_GROUP);
						s4_lever_door_1->Get<RigidBody>()->SetCollisionMask(Resources::Instance().NO_MASK);
						s4_lever_door_1->Get<RenderComponent>()->IsEnabled = false;

						s4_lever_door_2->Get<RigidBody>()->SetCollisionGroup(Resources::Instance().NO_GROUP);
						s4_lever_door_2->Get<RigidBody>()->SetCollisionMask(Resources::Instance().NO_MASK);
						s4_lever_door_2->Get<RenderComponent>()->IsEnabled = false;

						s4_lever_door_3->Get<RigidBody>()->SetCollisionGroup(Resources::Instance().NO_GROUP);
						s4_lever_door_3->Get<RigidBody>()->SetCollisionMask(Resources::Instance().NO_MASK);
						s4_lever_door_3->Get<RenderComponent>()->IsEnabled = false;
					}

					else
					{
						s4_lever_door_1->Get<RigidBody>()->SetCollisionGroupMulti(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
						s4_lever_door_1->Get<RigidBody>()->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);
						s4_lever_door_1->Get<RenderComponent>()->IsEnabled = true;

						s4_lever_door_2->Get<RigidBody>()->SetCollisionGroupMulti(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
						s4_lever_door_2->Get<RigidBody>()->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);
						s4_lever_door_2->Get<RenderComponent>()->IsEnabled = true;

						s4_lever_door_3->Get<RigidBody>()->SetCollisionGroupMulti(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
						s4_lever_door_3->Get<RigidBody>()->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);
						s4_lever_door_3->Get<RenderComponent>()->IsEnabled = true;
					}
				});

				section4->AddChild(s4_lever);
			}

			GameObject::Sptr s4_elevator = Prefabs::Load(scene, "Moving Platform", glm::vec3(-26.75f, -44.75f, 15.0f));
			{
				s4_elevator->Get<MovingPlatformBehavior>()->SetStartPosition(glm::vec3(-26.75f, -44.75f, 15.0f));
				s4_elevator->Get<MovingPlatformBehavior>()->SetEndPosition(glm::vec3(-26.75f, -46.75f, 5.5f));
				section4->AddChild(s4_elevator);
			}
		}

		// Props
		{
			section4->AddChild(Prefabs::Load(scene, "Crystal", glm::vec3(-40, -28, 15)));

			GameObject::Sptr swordProp_7 = Prefabs::Load(scene, "Sword");
			{
				swordProp_7->SetPosition(glm::vec3(-31.46f, -28.f, 17.18f));
				swordProp_7->SetRotation(glm::vec3(-124.0f, 0.0f, 90.0f));
				section4->AddChild(swordProp_7);
			}

			GameObject::Sptr staffProp_6 = Prefabs::Load(scene, "Staff");
			{
				staffProp_6->SetPosition(glm::vec3(-19.36f, -42.0f, 5.3f));
				staffProp_6->SetRotation(glm::vec3(-34.f, 90.0f, 173.0f));
				section4->AddChild(staffProp_6);
			}

			GameObject::Sptr rocks_4 = Prefabs::Load(scene, "Rocks");
			{
				rocks_4->SetPosition(glm::vec3(-45.68f, -1.42f, 5.9f));
				rocks_4->SetScale(glm::vec3(1.25f));
				section4->AddChild(rocks_4);
			}
		}
		
		// Floors
		{
			GameObject::Sptr floor10 = Prefabs::Load(scene, "Floor");
			{
				floor10->SetPosition(glm::vec3(-33.0f, -29.8f, 15.0f));
				floor10->SetScale(glm::vec3(0.67f, 0.4f, 1.0f));
				floor10->Get<RigidBody>()->AddCollider(BoxCollider::Create(glm::vec3(16.75f, 10.0f, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
				section4->AddChild(floor10);
			}

			GameObject::Sptr s4_floor_2 = Prefabs::Load(scene, "Floor");
			{
				s4_floor_2->SetPosition(glm::vec3(-24,-43, 5));
				s4_floor_2->SetScale(glm::vec3(0.36f, 0.28f, 1.0f));
				s4_floor_2->Get<RigidBody>()->AddCollider(BoxCollider::Create(glm::vec3(1)))->SetPosition(glm::vec3(-24,-43,5));
				section4->AddChild(s4_floor_2);
			}
		}
		
		// Walls
		{
			GameObject::Sptr wall5 = Prefabs::Load(scene, "Wall");
			{
				wall5->SetPosition(glm::vec3(-16.5f, -45.75f, 16.0f));
				wall5->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall5->SetScale(glm::vec3(1.0f, 0.6f, 0.3f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall5->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(0.04f, 1.2f, 0.0f));
				collider->SetScale(glm::vec3(1.3f, 4.44f, 9.79f));
				section4->AddChild(wall5);
			}

			GameObject::Sptr wall27 = Prefabs::Load(scene, "Wall");
			{
				wall27->SetPosition(glm::vec3(-34.830, -24.5f, 16.75f));
				wall27->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall27->SetScale(glm::vec3(1.0f, 0.5f, 0.605f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall27->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(-0.16f, 0.0f, 0.0f));
				collider->SetScale(glm::vec3(1.3f, 8.22f, 9.72f));
				section4->AddChild(wall27);
			}

			GameObject::Sptr wall31 = Prefabs::Load(scene, "Wall");
			{
				wall31->SetPosition(glm::vec3(-50.700f, -32.45f, 12.54f));
				wall31->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall31->SetScale(glm::vec3(1.0f, 1.0f, 0.43f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall31->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				collider->SetScale(glm::vec3(1.36f, 9.08f, 9.29f));
				section4->AddChild(wall31);
			}

			GameObject::Sptr wall32 = Prefabs::Load(scene, "Wall");
			{
				wall32->SetPosition(glm::vec3(-41.01, -40.04f, 12.5f));
				wall32->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall32->SetScale(glm::vec3(1.0f, 1.0f, 0.65f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall32->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(0.17f, -0.07f, 0.0f));
				collider->SetScale(glm::vec3(1.0f, 9.93f, 12.840f));
				section4->AddChild(wall32);
			}

			GameObject::Sptr wall35 = Prefabs::Load(scene, "Wall");
			{
				wall35->SetPosition(glm::vec3(-32.32, -45.68f, 9.62f));
				wall35->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall35->SetScale(glm::vec3(1.0f, 1.35f, 0.29f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall35->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(0.38f, 0.0f, 0.0f));
				collider->SetScale(glm::vec3(1.61f, 11.86f, 7.68f));
				section4->AddChild(wall35);
			}

			GameObject::Sptr wall36 = Prefabs::Load(scene, "Wall");
			{
				wall36->SetPosition(glm::vec3(-24, -51, 9.75f));
				wall36->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall36->SetScale(glm::vec3(1.0f, 1.35f, 0.5f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall36->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				collider->SetScale(glm::vec3(1.34f, 20.57f, 15.64f));
				section4->AddChild(wall36);
			}

			GameObject::Sptr wall37 = Prefabs::Load(scene, "Wall");
			{
				wall37->SetPosition(glm::vec3(-16.32, -32.58f, 9.75f));
				wall37->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall37->SetScale(glm::vec3(1.0f, 1.35f, 0.7f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall37->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(0.0f, 0.290f, -1.830f));
				collider->SetScale(glm::vec3(1.47f, 14.42f, 12.09f));
				section4->AddChild(wall37);
			}

			GameObject::Sptr s4_wall_1 = Prefabs::Load(scene, "Wall");
			{
				s4_wall_1->SetPosition(glm::vec3(-23.5f, -40, 5));
				s4_wall_1->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				s4_wall_1->SetScale(glm::vec3(1.0f, 1.35f, 0.56f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				s4_wall_1->Get<RigidBody>()->AddCollider(collider);
				section4->AddChild(s4_wall_1);
			}
		}
		

#pragma endregion

#pragma region Section 5

		// Interactables
		{
			section5->AddChild(Prefabs::Load(scene, "Shroom Trap Multi", glm::vec3(-19, -10, 15)));
			section5->AddChild(Prefabs::Load(scene, "Shroom Trap Single", glm::vec3(-30, 0, 15)));
			section5->AddChild(Prefabs::Load(scene, "Shroom Trap Single", glm::vec3(-26, -16, 15)));
		}

		// Props
		{
			GameObject::Sptr staffProp_7 = Prefabs::Load(scene, "Staff");
			{
				staffProp_7->SetPosition(glm::vec3(-28.39f, 3.f, 16.98f));
				staffProp_7->SetRotation(glm::vec3(90.f, -24.0f, 0.0f));
				section5->AddChild(staffProp_7);
			}
		}

		// Floors
		{

			GameObject::Sptr floor11 = Prefabs::Load(scene, "Floor");
			{
				floor11->SetPosition(glm::vec3(-24.77f, -4.0f, 15.0f));
				floor11->SetScale(glm::vec3(0.36f, 0.63f, 1.0f));
				floor11->Get<RigidBody>()->AddCollider(BoxCollider::Create(glm::vec3(9.0f, 15.75f, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
				floor11->Get<RenderComponent>()->SetMaterial(Resources::GetMaterial("Brown"));
				section5->AddChild(floor11);
			}
		}

		// Walls
		{
			GameObject::Sptr wall41 = Prefabs::Load(scene, "Wall");
			{
				wall41->SetPosition(glm::vec3(-32.86f, -6.53f, 15.72f));
				wall41->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall41->SetScale(glm::vec3(1.0f, 0.65f, 1.1f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall41->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(0.07f, 0.0f, 0.0f));
				collider->SetScale(glm::vec3(1.45f, 6.94f, 20.26f));
				section5->AddChild(wall41);
			}

			GameObject::Sptr wall49 = Prefabs::Load(scene, "Wall");
			{
				wall49->SetPosition(glm::vec3(-16.46f, -6.91f, 15.72f));
				wall49->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall49->SetScale(glm::vec3(1.0f, 0.65f, 1.09f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall49->Get<RigidBody>()->AddCollider(collider);
				collider->SetScale(glm::vec3(1.39f, 5.43f, 18.43f));
				section5->AddChild(wall49);
			}
		}

#pragma endregion

#pragma region Section 6

		// Interactables
		{
			section6->AddChild(Prefabs::Load(scene, "Key", glm::vec3(-71.75f, 20.5f, 15.0f)));

			GameObject::Sptr keyDoor = Prefabs::Load(scene, "Key Door", glm::vec3(-12.0f, 21.0f, 10.0f));
			{
				keyDoor->SetRotation(glm::vec3(90, 0, 0));
				section6->AddChild(keyDoor);
			}
		}

		// Props
		{
			section6->AddChild(Prefabs::Load(scene, "Crystal", glm::vec3(-50, 25, 13)));
			section6->AddChild(Prefabs::Load(scene, "Crystal", glm::vec3(-16, 16, 13)));
			section6->AddChild(Prefabs::Load(scene, "Crystal", glm::vec3(10, 25, 13)));
			section6->AddChild(Prefabs::Load(scene, "Crystal", glm::vec3(40, 15, 13)));
			section6->AddChild(Prefabs::Load(scene, "Crystal", glm::vec3(35, 29, 13)));

			GameObject::Sptr staffProp_8 = Prefabs::Load(scene, "Staff");
			{
				staffProp_8->SetPosition(glm::vec3(-43.81f, 26.83f, 16.f));
				staffProp_8->SetRotation(glm::vec3(0.f, 90.0f, -81.0f));
				section6->AddChild(staffProp_8);
			}

			GameObject::Sptr shieldProp_9 = Prefabs::Load(scene, "Shield");
			{
				shieldProp_9->SetPosition(glm::vec3(-57.87f, 25.28f, 14.f));
				shieldProp_9->SetRotation(glm::vec3(0.f, 0.0f, -69.0f));
				shieldProp_9->SetScale(glm::vec3(0.35f));
				section6->AddChild(shieldProp_9);
			}

			GameObject::Sptr shieldProp_10 = Prefabs::Load(scene, "Shield");
			{
				shieldProp_10->SetPosition(glm::vec3(-51.02f, 17.12f, 14.f));
				shieldProp_10->SetRotation(glm::vec3(0.f, 0.0f, 143.0f));
				shieldProp_10->SetScale(glm::vec3(0.25f));
				section6->AddChild(shieldProp_10);
			}

			GameObject::Sptr swordProp_8 = Prefabs::Load(scene, "Sword");
			{
				swordProp_8->SetPosition(glm::vec3(-26.76f, 22.72f, 15.89f));
				swordProp_8->SetRotation(glm::vec3(0.0f, 0.0f, 115.0f));
				section6->AddChild(swordProp_8);
			}

			section6->AddChild(Prefabs::Load(scene, "Pillar", glm::vec3(30.62f, 27.76f, 9.6f)));
			section6->AddChild(Prefabs::Load(scene, "Pillar", glm::vec3(46.38f, 27.66f, 9.6f)));
			section6->AddChild(Prefabs::Load(scene, "Pillar", glm::vec3(30.57f, 15.8f, 9.6f)));

			GameObject::Sptr damaged_pillar_4 = Prefabs::Load(scene, "Damaged Pillar");
			{
				damaged_pillar_4->SetPosition(glm::vec3(46.38f, 15.8f, 9.6f));
				damaged_pillar_4->SetScale(glm::vec3(1.5f, 1.12f, 1.5f));
				section6->AddChild(damaged_pillar_4);
			}

			GameObject::Sptr rocks_5 = Prefabs::Load(scene, "Rocks");
			{
				rocks_5->SetPosition(glm::vec3(-24.68f, 22.57f, 13.2f));
				rocks_5->SetRotation(glm::vec3(90, 0, 135));
				rocks_5->SetScale(glm::vec3(1.5f));
				section6->AddChild(rocks_5);
			}
		}

		// Floors
		{
			GameObject::Sptr floor4 = Prefabs::Load(scene, "Floor");
			{
				floor4->SetPosition(glm::vec3(-56.0f, 27.0f, 13.0f));
				floor4->SetScale(glm::vec3(0.85f, 0.6f, 1.0f));
				floor4->Get<RigidBody>()->AddCollider(BoxCollider::Create(glm::vec3(21.25f, 15, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
				section6->AddChild(floor4);
			}

			GameObject::Sptr floor8 = Prefabs::Load(scene, "Floor");
			{
				floor8->SetPosition(glm::vec3(-13.5f, 27.0f, 13.0f));
				floor8->SetScale(glm::vec3(0.85f, 0.6f, 1.0f));
				floor8->Get<RigidBody>()->AddCollider(BoxCollider::Create(glm::vec3(21.25f, 15, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
				section6->AddChild(floor8);
			}

			GameObject::Sptr floor9 = Prefabs::Load(scene, "Floor");
			{
				floor9->SetPosition(glm::vec3(29.0f, 27.0f, 13.0f));
				floor9->SetScale(glm::vec3(0.85f, 0.6f, 1.0f));
				floor9->Get<RigidBody>()->AddCollider(BoxCollider::Create(glm::vec3(21.25f, 15, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
				section6->AddChild(floor9);
			}
		}

		// Walls
		{
			GameObject::Sptr wall45 = Prefabs::Load(scene, "Wall");
			{
				wall45->SetPosition(glm::vec3(-24.78f, 31.22f, 12.5f));
				wall45->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall45->SetScale(glm::vec3(1.00f, 0.94f, 0.95f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall45->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(0.730f, 0.01f, 10.1f));
				collider->SetScale(glm::vec3(1.87f, 23.87f, 93.48f));
				section6->AddChild(wall45);
			}

			GameObject::Sptr wall51 = Prefabs::Load(scene, "Wall");
			{
				wall51->SetPosition(glm::vec3(-51.82f, 11.83f, 8.73f));
				wall51->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall51->SetScale(glm::vec3(1.0f, 1.48f, 1.35f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall51->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(-0.260f, 0.01f, -5.19f));
				collider->SetScale(glm::vec3(1.43f, 21.04f, 23.13f));
				section6->AddChild(wall51);
			}

			GameObject::Sptr wall52 = Prefabs::Load(scene, "Wall");
			{
				wall52->SetPosition(glm::vec3(-74.0f, 20.75f, 12.5f));
				wall52->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall52->SetScale(glm::vec3(1.0f, 1.0f, 0.625f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall52->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				collider->SetScale(glm::vec3(1.37f, 21.61f, 20.81f));
				section6->AddChild(wall52);
			}

			GameObject::Sptr wall53 = Prefabs::Load(scene, "Wall");
			{
				wall53->SetPosition(glm::vec3(7.64f, 32.1f, 12.5f));
				wall53->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall53->SetScale(glm::vec3(1.0f, 1.0f, 1.14f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall53->Get<RigidBody>()->AddCollider(collider);
				section6->AddChild(wall53);

			}

			GameObject::Sptr wall54 = Prefabs::Load(scene, "Wall");
			{
				wall54->SetPosition(glm::vec3(5.1f, 11.190f, 12.5f));
				wall54->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall54->SetScale(glm::vec3(1.0f, 1.0f, 1.410f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall54->Get<RigidBody>()->AddCollider(collider);
				section6->AddChild(wall54);
			}

			GameObject::Sptr wall57 = Prefabs::Load(scene, "Wall");
			{
				wall57->SetPosition(glm::vec3(38.530f, 11.1f, 12.5f));
				wall57->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall57->SetScale(glm::vec3(1.0f, 1.0f, 0.765f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall57->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(-0.32f, 0.0f, -17.05f));
				collider->SetScale(glm::vec3(1.37f, 21.19f, 39.8f));
				section6->AddChild(wall57);
			}

			GameObject::Sptr wall58 = Prefabs::Load(scene, "Wall");
			{
				wall58->SetPosition(glm::vec3(38.0f, 32.04f, 12.5f));
				wall58->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall58->SetScale(glm::vec3(1.0f, 1.0f, 0.85f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall58->Get<RigidBody>()->AddCollider(collider);
				section6->AddChild(wall58);
			}

			GameObject::Sptr wall61 = Prefabs::Load(scene, "Wall");
			{
				wall61->SetPosition(glm::vec3(-56.25f, 29.5f, 12.5f));
				wall61->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall61->SetScale(glm::vec3(1.0f, 1.0f, 1.1f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall61->Get<RigidBody>()->AddCollider(collider);
				collider->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				collider->SetScale(glm::vec3(1.63f, 21.59f, 37.25f));
				section6->AddChild(wall61);
			}
		}

#pragma endregion

#pragma region Section 7

		// Interactables
		{
			section7->AddChild(Prefabs::Load(scene, "Spike Trap", glm::vec3(120.0f, 50.0f, 15.4f)));
			section7->AddChild(Prefabs::Load(scene, "Spike Trap", glm::vec3(120.0f, 44.0f, 15.4f)));
			section7->AddChild(Prefabs::Load(scene, "Spike Trap", glm::vec3(120.0f, 57.0f, 15.4f)));
			section7->AddChild(Prefabs::Load(scene, "Spike Trap", glm::vec3(170.0f, -6.0f, 15.4f)));
			section7->AddChild(Prefabs::Load(scene, "Spike Trap", glm::vec3(170.0f, -12.0f, 15.4f)));

			section7->AddChild(Prefabs::Load(scene, "Cobweb", glm::vec3(127.130, 33.140, 17.920)));
			section7->AddChild(Prefabs::Load(scene, "Cobweb", glm::vec3(138.0, 3.5, 17.920)));
			section7->AddChild(Prefabs::Load(scene, "Cobweb", glm::vec3(123.0, 20.0, 17.920)));
			section7->AddChild(Prefabs::Load(scene, "Cobweb", glm::vec3(117.000, 10.0, 17.920)));
			section7->AddChild(Prefabs::Load(scene, "Cobweb", glm::vec3(228.000, 18.0, 17.920)));

			section7->AddChild(Prefabs::Load(scene, "Healing Well", glm::vec3(255.0f, 20.0f, 15.0f)));

			section7->AddChild(Prefabs::Load(scene, "Shroom Trap Multi", glm::vec3(118.0f, -5.0f, 15.4f)));
			section7->AddChild(Prefabs::Load(scene, "Shroom Trap Multi", glm::vec3(118.0f, -5.0f, 15.4f)));
			section7->AddChild(Prefabs::Load(scene, "Shroom Trap Multi", glm::vec3(200.0f, -5.0f, 15.4f)));
			section7->AddChild(Prefabs::Load(scene, "Shroom Trap Multi", glm::vec3(225.0f, -3.0f, 15.4f)));

			section7->AddChild(Prefabs::Load(scene, "Shroom Trap Single", glm::vec3(120, -13, 15)));
			section7->AddChild(Prefabs::Load(scene, "Shroom Trap Single", glm::vec3(140, -10, 15)));
		}

		// Props
		{
			section7->AddChild(Prefabs::Load(scene, "Crystal", glm::vec3(58.800, 28.450, 14.9)));
			section7->AddChild(Prefabs::Load(scene, "Crystal", glm::vec3(92.240, 17.290, 14.9)));
			section7->AddChild(Prefabs::Load(scene, "Crystal", glm::vec3(140.0, 15.0, 15.0)));
			section7->AddChild(Prefabs::Load(scene, "Crystal", glm::vec3(160.0, -4.5, 15.0)));
			section7->AddChild(Prefabs::Load(scene, "Crystal", glm::vec3(224.5, 35.0, 15.0)));
			section7->AddChild(Prefabs::Load(scene, "Crystal", glm::vec3(210.5, 35.0, 15.0)));
		}

		// Floors
		{
			GameObject::Sptr s7_floor_1 = Prefabs::Load(scene, "Floor");
			{
				s7_floor_1->SetPosition(glm::vec3(125.920f, 29.600f, 15.0f));
				s7_floor_1->SetScale(glm::vec3(1.1f, 2.3f, 1.0f));
				s7_floor_1->Get<RigidBody>()->AddCollider(BoxCollider::Create(glm::vec3(24.0f, 50.0f, 2.5f)))->SetPosition({ 0.3,-7.0,-2.3 });
				section7->AddChild(s7_floor_1);
			}

			GameObject::Sptr s7_floor_2 = Prefabs::Load(scene, "Floor");
			{
				s7_floor_2->SetPosition(glm::vec3(72.610f, 33.0f, 15.0f));
				s7_floor_2->SetScale(glm::vec3(1.040f, 1.150f, 1.0f));
				s7_floor_2->Get<RigidBody>()->AddCollider(BoxCollider::Create(glm::vec3(30.0f, 18.0f, 3.0f)))->SetPosition({ 2.0,-5.0,-2.3 });
				section7->AddChild(s7_floor_2);
			}

			GameObject::Sptr s7_floor_3= Prefabs::Load(scene, "Floor");
			{
				s7_floor_3->SetPosition(glm::vec3(228.030f, 25.440f, 15.0f));
				s7_floor_3->SetScale(glm::vec3(3.0f, 2.0f, 1.0f));
				s7_floor_3->Get<RigidBody>()->AddCollider(BoxCollider::Create(glm::vec3(65.0f, 50.0f, 2.5f)))->SetPosition({ -20.0,-15.0,-2.3 });
				section7->AddChild(s7_floor_3);
			}
		}

		// Walls
		{
			GameObject::Sptr wall62 = Prefabs::Load(scene, "Wall");
			{
				wall62->SetPosition(glm::vec3(66.520f, 32.050f, 12.470f));
				wall62->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall62->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall62->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall62);
			}

			GameObject::Sptr wall63 = Prefabs::Load(scene, "Wall");
			{
				wall63->SetPosition(glm::vec3(65.630f, 11.090f, 12.470f));
				wall63->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall63->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall63->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall63);
			}

			GameObject::Sptr wall64 = Prefabs::Load(scene, "Wall");
			{
				wall64->SetPosition(glm::vec3(97.570f, 32.030f, 12.470f));
				wall64->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall64->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall64->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall64);
			}

			GameObject::Sptr wall65 = Prefabs::Load(scene, "Wall");
			{
				wall65->SetPosition(glm::vec3(96.460f, 11.060f, 12.470f));
				wall65->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall65->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall65->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall65);
			}

			GameObject::Sptr wall66 = Prefabs::Load(scene, "Wall");
			{
				wall66->SetPosition(glm::vec3(111.960f, 48.700f, 12.470f));
				wall66->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall66->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall66->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall66);
			}

			GameObject::Sptr wall67 = Prefabs::Load(scene, "Wall");
			{
				wall67->SetPosition(glm::vec3(128.490f, 62.900f, 12.470f));
				wall67->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall67->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall67->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall67);
			}

			GameObject::Sptr wall68 = Prefabs::Load(scene, "Wall");
			{
				wall68->SetPosition(glm::vec3(142.800f, 47.160f, 12.470f));
				wall68->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall68->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall68->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall68);
			}

			GameObject::Sptr wall69 = Prefabs::Load(scene, "Wall");
			{
				wall69->SetPosition(glm::vec3(110.710f, -5.170f, 12.470f));
				wall69->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall69->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall69->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall69);
			}

			GameObject::Sptr wall70 = Prefabs::Load(scene, "Wall");
			{
				wall70->SetPosition(glm::vec3(142.600f, 16.140f, 12.470f));
				wall70->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall70->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall70->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall70);
			}

			GameObject::Sptr wall71 = Prefabs::Load(scene, "Wall");
			{
				wall71->SetPosition(glm::vec3(157.010f, -0.370f, 12.470f));
				wall71->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall71->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall71->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall71);
			}

			GameObject::Sptr wall72 = Prefabs::Load(scene, "Wall");
			{
				wall72->SetPosition(glm::vec3(127.090f, -19.730f, 12.470f));
				wall72->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall72->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall72->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall72);
			}

			GameObject::Sptr wall73 = Prefabs::Load(scene, "Wall");
			{
				wall73->SetPosition(glm::vec3(32.870f, -36.850f, 3.960f));
				wall73->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall73->SetScale(glm::vec3(1.0f, 1.0f, 0.9f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall73->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall73);
			}

			GameObject::Sptr wall74 = Prefabs::Load(scene, "Wall");
			{
				wall74->SetPosition(glm::vec3(187.700f, -0.300f, 12.470f));
				wall74->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall74->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall74->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall74);
			}

			GameObject::Sptr wall75 = Prefabs::Load(scene, "Wall");
			{
				wall75->SetPosition(glm::vec3(188.650f, -19.650f, 12.470f));
				wall75->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall75->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall75->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall75);
			}

			GameObject::Sptr wall76 = Prefabs::Load(scene, "Wall");
			{
				wall76->SetPosition(glm::vec3(202.110f, 15.990f, 12.470f));
				wall76->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall76->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall76->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall76);
			}

			GameObject::Sptr wall77 = Prefabs::Load(scene, "Wall");
			{
				wall77->SetPosition(glm::vec3(120.200f, 21.950f, 12.470f));
				wall77->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall77->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall77->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall77);
			}

			GameObject::Sptr wall78 = Prefabs::Load(scene, "Wall");
			{
				wall78->SetPosition(glm::vec3(202.140f, 46.570f, 12.470f));
				wall78->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall78->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall78->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall78);
			}

			GameObject::Sptr wall79 = Prefabs::Load(scene, "Wall");
			{
				wall79->SetPosition(glm::vec3(218.680f, 60.710f, 12.470f));
				wall79->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall79->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall79->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall79);
			}

			GameObject::Sptr wall80 = Prefabs::Load(scene, "Wall");
			{
				wall80->SetPosition(glm::vec3(232.690f, 44.400f, 12.470f));
				wall80->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall80->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall80->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall80);
			}

			GameObject::Sptr wall81 = Prefabs::Load(scene, "Wall");
			{
				wall81->SetPosition(glm::vec3(219.410f, -19.610f, 12.470f));
				wall81->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall81->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall81->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall81);
			}

			GameObject::Sptr wall82 = Prefabs::Load(scene, "Wall");
			{
				wall82->SetPosition(glm::vec3(233.600f, -3.180f, 12.470f));
				wall82->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall82->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall82->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall82);
			}

			GameObject::Sptr wall83 = Prefabs::Load(scene, "Wall");
			{
				wall83->SetPosition(glm::vec3(248.120f, 27.840f, 12.470f));
				wall83->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall83->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall83->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall83);
			}

			GameObject::Sptr wall84 = Prefabs::Load(scene, "Wall");
			{
				wall84->SetPosition(glm::vec3(248.120f, 12.880f, 12.470f));
				wall84->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
				wall84->SetScale(glm::vec3(1));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall84->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall84);
			}

			GameObject::Sptr wall85 = Prefabs::Load(scene, "Wall");
			{
				wall85->SetPosition(glm::vec3(264.210f, 20.240f, 12.470f));
				wall85->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall85->SetScale(glm::vec3(1, 1, 0.55f));
				BoxCollider::Sptr collider = BoxCollider::Create();
				wall85->Get<RigidBody>()->AddCollider(collider);
				section7->AddChild(wall85);
			}
		}


#pragma endregion

		// Misc
		// =========================================================================
		
		// Create some lights for our scene
		GameObject::Sptr skylight_1 = scene->CreateGameObject("Sky Light");
		{
			skylight_1->SetPosition(glm::vec3(glm::vec3(0, 0, 100)));

			Light::Sptr lightComponent = skylight_1->Add<Light>();
			lightComponent->SetColor(glm::vec3(0.3686, 0.3451, 0.3451));
			lightComponent->SetRadius(10000);
			lightComponent->SetIntensity(2);
		}

		GameObject::Sptr skylight_2 = scene->CreateGameObject("Sky Light");
		{
			skylight_2->SetPosition(glm::vec3(glm::vec3(175, 50, 100)));

			Light::Sptr lightComponent = skylight_2->Add<Light>();
			lightComponent->SetColor(glm::vec3(0.3686, 0.3451, 0.3451));
			lightComponent->SetRadius(10000);
			lightComponent->SetIntensity(2);
		}
		

		GameObject::Sptr shadowCaster = scene->CreateGameObject("Shadow Light");
		{
			// Set position in the scene
			shadowCaster->SetPosition(glm::vec3(0, -40, 0));
			shadowCaster->SetRotation(glm::vec3(75, 0, 0));
			
			ShadowCamera::Sptr shadowCam = shadowCaster->Add<ShadowCamera>();
			shadowCam->SetOrthographic(true);
			shadowCam->Bias = 0.001f;
			shadowCam->SetSize(glm::vec4(-20, 20, 20, -20));
		}

		// UI
		// =========================================================================
		GameObject::Sptr gameCanvas = scene->CreateGameObject("Game Canvas");
		{
			GameObject::Sptr healthp = UIHelper::CreateImage(scene, Resources::GetTexture2D("CharacterH"), "Health");
			healthp->Get<RectTransform>()->SetPosition({ 170, 90 });
			healthp->Get<RectTransform>()->SetSize({ 90, 40 });
			healthp->Get<GuiPanel>()->SetBorderRadius(0);
			gameCanvas->AddChild(healthp);

			GameObject::Sptr shadowhp = UIHelper::CreateImage(scene, Resources::GetTexture2D("ShadowH"), "Shadow Health");
			shadowhp->Get<RectTransform>()->SetPosition({ 170, 180 });
			shadowhp->Get<RectTransform>()->SetSize({ 90, 40 });
			shadowhp->Get<GuiPanel>()->SetBorderRadius(0);
			gameCanvas->AddChild(shadowhp);

			GameObject::Sptr healthText = UIHelper::CreateText1(scene, "Body Health: ???", "Body Health Text");
			healthText->Get<RectTransform>()->SetPosition({ 170, 90 });
			gameCanvas->AddChild(healthText);

			GameObject::Sptr shadowText = UIHelper::CreateText(scene, "Shadow Health: ???", "Shadow Health Text");
			shadowText->Get<RectTransform>()->SetPosition({ 176, 181 });
			gameCanvas->AddChild(shadowText);

			GameManager::GameInterface.SetGameUserInterface(*gameCanvas);
			GameManager::GameInterface.InitializeGameUserInterface(*healthText->Get<GuiText>(), *shadowText->Get<GuiText>());
		}

		//Particles
		GameObject::Sptr particles = scene->CreateGameObject("Particles");
		{
			ParticleSystem::Sptr particleManager = particles->Add<ParticleSystem>();
			particleManager->AddEmitter(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 10.0f), 10.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		}

		GuiBatcher::SetDefaultTexture(ResourceManager::CreateAsset<Texture2D>("textures/ui-sprite.png"));
		GuiBatcher::SetDefaultBorderRadius(8);

		// Save the asset manifest for all the resources we just loaded
		ResourceManager::SaveManifest("scene-manifest.json");
		// Save the scene to a JSON file
		scene->Save("scene.json");

		// Send the scene to the application
		app.LoadScene(scene);
	}
}
