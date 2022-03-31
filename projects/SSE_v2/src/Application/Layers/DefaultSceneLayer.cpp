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


		// Create some lights for our scene
		GameObject::Sptr lightParent = scene->CreateGameObject("Lights");

		for (int ix = 0; ix < 50; ix++) {
			GameObject::Sptr light = scene->CreateGameObject("Light");
			light->SetPosition(glm::vec3(glm::diskRand(25.0f), 1.0f));
			lightParent->AddChild(light);

			Light::Sptr lightComponent = light->Add<Light>();
			lightComponent->SetColor(glm::linearRand(glm::vec3(0.0f), glm::vec3(1.0f)));
			lightComponent->SetRadius(glm::linearRand(0.1f, 10.0f));
			lightComponent->SetIntensity(glm::linearRand(1.0f, 2.0f));
		}

		// Set up the scene's camera
		GameObject::Sptr camera = scene->MainCamera->GetGameObject()->SelfRef();
		{
			camera->SetPosition({ -9, -6, 15 });
			camera->LookAt(glm::vec3(0.0f));

			camera->Add<SimpleCameraControl>();
		}


		// Set up all our sample objects
		GameObject::Sptr plane = scene->CreateGameObject("Plane");
		{
			// Make a big tiled mesh
			MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
			tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(100.0f), glm::vec2(20.0f)));
			tiledMesh->GenerateMesh();

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = plane->Add<RenderComponent>();
			renderer->SetMesh(tiledMesh);
			renderer->SetMaterial(Resources::GetMaterial("Box"));

			// Attach a plane collider that extends infinitely along the X/Y axis
			RigidBody::Sptr physics = plane->Add<RigidBody>(/*static by default*/);
			physics->AddCollider(BoxCollider::Create(glm::vec3(50.0f, 50.0f, 1.0f)))->SetPosition({ 0,0,-1 });
		}

		// Add some walls :3
		{
			MeshResource::Sptr wall = ResourceManager::CreateAsset<MeshResource>();
			wall->AddParam(MeshBuilderParam::CreateCube(ZERO, ONE));
			wall->GenerateMesh();

			GameObject::Sptr wall1 = scene->CreateGameObject("Wall1");
			wall1->Add<RenderComponent>()->SetMesh(wall)->SetMaterial(Resources::GetMaterial("White Brick"));
			wall1->SetScale(glm::vec3(20.0f, 1.0f, 3.0f));
			wall1->SetPosition(glm::vec3(0.0f, 10.0f, 1.5f));
			plane->AddChild(wall1);

			GameObject::Sptr wall2 = scene->CreateGameObject("Wall2");
			wall2->Add<RenderComponent>()->SetMesh(wall)->SetMaterial(Resources::GetMaterial("White Brick"));
			wall2->SetScale(glm::vec3(20.0f, 1.0f, 3.0f));
			wall2->SetPosition(glm::vec3(0.0f, -10.0f, 1.5f));
			plane->AddChild(wall2);

			GameObject::Sptr wall3 = scene->CreateGameObject("Wall3");
			wall3->Add<RenderComponent>()->SetMesh(wall)->SetMaterial(Resources::GetMaterial("White Brick"));
			wall3->SetScale(glm::vec3(1.0f, 20.0f, 3.0f));
			wall3->SetPosition(glm::vec3(10.0f, 0.0f, 1.5f));
			plane->AddChild(wall3);

			GameObject::Sptr wall4 = scene->CreateGameObject("Wall4");
			wall4->Add<RenderComponent>()->SetMesh(wall)->SetMaterial(Resources::GetMaterial("White Brick"));
			wall4->SetScale(glm::vec3(1.0f, 20.0f, 3.0f));
			wall4->SetPosition(glm::vec3(-10.0f, 0.0f, 1.5f));
			plane->AddChild(wall4);
		}

		GameObject::Sptr monkey1 = scene->CreateGameObject("Monkey");
		{
			// Set position in the scene
			monkey1->SetPosition(glm::vec3(1.5f, 0.0f, 1.0f));

			// Add some behaviour that relies on the physics body
			monkey1->Add<JumpBehaviour>();

			// Create and attach a renderer for the monkey
			RenderComponent::Sptr renderer = monkey1->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Monkey"));
			renderer->SetMaterial(Resources::GetMaterial("Monkey"));

			// Example of a trigger that interacts with static and kinematic bodies as well as dynamic bodies
			TriggerVolume::Sptr trigger = monkey1->Add<TriggerVolume>();
			trigger->SetFlags(TriggerTypeFlags::Statics | TriggerTypeFlags::Kinematics);
			trigger->AddCollider(BoxCollider::Create(glm::vec3(1.0f)));

			monkey1->Add<TriggerVolumeEnterBehaviour>();
			
			AudioSource::Sptr audio = monkey1->Add<AudioSource>();
			{	
				audio->Init({ false });				
			} 
		}

#pragma region Character Setup

		GameObject::Sptr character = scene->CreateGameObject("ShadowRend");
		{
			// Transform
			character->SetPosition(glm::vec3(0.0f, 0.0f, 1.0f));
			character->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			character->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

			// Render Component
			RenderComponent::Sptr renderer = character->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Character"));
			renderer->SetMaterial(Resources::GetMaterial("Character"));

			// Animation Component
			AnimatorComponent::Sptr animator = character->Add<AnimatorComponent>();
			animator->AddAnimation("Walk", Resources::GetAnimation("Character Walk"));
			animator->AddAnimation("Idle", Resources::GetAnimation("Character Idle"));
			
			animator->SetRenderer(*renderer);
			animator->SetAnimation("Idle");
			animator->SetLooping(true);
			animator->SetPause(false);
			animator->SetSpeed(4.0f);
			
			// Audio Source
			AudioSource::Sptr audio = character->Add<AudioSource>();
			{				
				audio->Init({ false });
			}
			
			// Physics Collider
			RigidBody::Sptr physics = character->Add<RigidBody>(RigidBodyType::Dynamic);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);
			physics->SetCollisionGroup(PHYSICAL_GROUP);
			physics->SetCollisionMask(PHYSICAL_MASK);

			// Interaction Collider
			TriggerVolume::Sptr volume = character->Add<TriggerVolume>();
			BoxCollider::Sptr i_collider = BoxCollider::Create();
			i_collider->SetPosition(i_collider->GetPosition() + glm::vec3(0.0f, 0.0f, -2.5f));
			volume->AddCollider(i_collider);
			TriggerVolumeEnterBehaviour::Sptr trigger = character->Add<TriggerVolumeEnterBehaviour>();

			// Health Component
			character->Add<HealthComponent>(100);
		}

		GameObject::Sptr shadow = scene->CreateGameObject("Shadow");
		{
			// Set position in the SceneManager::GetCurrentScene()
			shadow->SetPosition(glm::vec3(0.0f, -40.0f, 5.4f));
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
			animator->SetLooping(true);
			animator->SetPause(false);
			animator->SetSpeed(4.0f);

			AudioSource::Sptr audio = shadow->Add<AudioSource>();
			{
				audio->Init({ false });
			}

			// Add a dynamic rigid body to this monkey
			RigidBody::Sptr physics = shadow->Add<RigidBody>(RigidBodyType::Dynamic);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);
			physics->SetCollisionGroup(SHADOW_GROUP);
			physics->SetCollisionMask(SHADOW_MASK);

			shadow->Add<HealthComponent>(100);
		}

		GameObject::Sptr controller = scene->CreateGameObject("Controller");
		{
			PlayerController::Sptr pc = controller->Add<PlayerController>();
			pc->SetCharacterBody(*character);
			pc->SetCharacterShadow(*shadow);
			pc->SetInteractionCollider(*character->Get<TriggerVolume>());
			pc->SetCamera(*camera);
		}

#pragma endregion

		// =========================================================================

		GameObject::Sptr pressure_plate = scene->CreateGameObject("Pressure Plate");
		{
			// Transform
			pressure_plate->SetPosition(glm::vec3(0, 5, 1));
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

			AudioSource::Sptr audio = pressure_plate->Add<AudioSource>();
			{
				audio->LoadEvent("Pressure Plate");								
				audio->Init({ false });
			}

			// Trigger Event
			TriggerVolumeEnterBehaviour::Sptr trigger = pressure_plate->Add<TriggerVolumeEnterBehaviour>();
			trigger->onTriggerEnterEvent = [audio]
			{
				AudioEngine::Instance().GetEvent("Pressure Plate").SetParameter("Powered", 0);								
				audio->Play();
			};

			trigger->onTriggerExitEvent = [audio]
			{				
				AudioEngine::Instance().GetEvent("Pressure Plate").SetParameter("Powered", 1);				
				audio->Play();
			};
		}

		GameObject::Sptr spike_trap = scene->CreateGameObject("SpikeTrap");
		{
			spike_trap->SetPosition(glm::vec3(5, 0, 1));
			spike_trap->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			spike_trap->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

			// Create and attach a renderer for the monkey
			RenderComponent::Sptr renderer = spike_trap->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Spike Trap"));
			renderer->SetMaterial(Resources::GetMaterial("Spike Trap"));

			// Animator
			AnimatorComponent::Sptr animator = spike_trap->Add<AnimatorComponent>();
			animator->AddAnimation("Spikes", Resources::GetAnimation("Spikes"));
			animator->SetRenderer(*renderer);
			animator->SetLooping(false);

			// Trigger Volume
			TriggerVolume::Sptr volume = spike_trap->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
			collider->SetScale(glm::vec3(2.0f, 1.0f, 2.0f));
			volume->AddCollider(collider);
			volume->SetCollisionGroup(PHYSICAL_GROUP);
			volume->SetCollisionMask(PHYSICAL_MASK);

			// Trigger Event
			TriggerVolumeEnterBehaviour::Sptr trigger = spike_trap->Add<TriggerVolumeEnterBehaviour>();
			trigger->onTriggerEnterEvent = [character] {
				//if (!SceneManager::GetCurrentScene()->PC.isShadow) {
					//body->Get<HealthComponent>()->DealDamage(10.0f);
				//}
			};

			AudioSource::Sptr audio = spike_trap->Add<AudioSource>();
			{
				audio->LoadEvent("Spikes");
				audio->volume = 0.5f;				
				audio->Init({ false });
			}

			// Spike Behavior
			SpikeTrapBehavior::Sptr spikeBehavior = spike_trap->Add<SpikeTrapBehavior>();
			spikeBehavior->SetAnimator(animator);
			spikeBehavior->SetTrigger(volume);
			spikeBehavior->Initialize(3, 0, false);

		}

		GameObject::Sptr elevator = scene->CreateGameObject("Elevator");
		{
			elevator->SetPosition(glm::vec3(-5, 0, 1));
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
			elevatorBehavior->SetStartPosition(glm::vec3(-5, 0, 1));
			elevatorBehavior->SetEndPosition(glm::vec3(-5, 0, 5));
			elevatorBehavior->SetLooping(true);
			elevatorBehavior->SetCollider(collider);

		}

		GameObject::Sptr lever_2 = scene->CreateGameObject("Turret Activation Lever");
		{
			// Transform
			lever_2->SetPosition(glm::vec3(0, 5, 1));
			lever_2->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
			lever_2->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

			// Renderer
			RenderComponent::Sptr renderer = lever_2->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Lever"));
			renderer->SetMaterial(Resources::GetMaterial("LeverTex"));

			// Collider
			RigidBody::Sptr physics = lever_2->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);

			AudioSource::Sptr audio = lever_2->Add<AudioSource>();
			{
				audio->LoadEvent("Lever");
				audio->volume = 0.75f;				
				audio->Init({ false });
			}

			// Interaction Event
			InteractableComponent::Sptr interactable = lever_2->Add<InteractableComponent>();
			interactable->onInteractionEvent = [interactable, audio]
			{
				interactable->isToggled = !interactable->isToggled;
				audio->Play();


				if (interactable->isToggled) {
					// Shut off Turrets
				}

				else {
					// Turn on Turrets
				}
			};
		}


		GameObject::Sptr ship = scene->CreateGameObject("Fenrir");
		{
			// Set position in the scene
			ship->SetPosition(glm::vec3(1.5f, 0.0f, 4.0f));
			ship->SetScale(glm::vec3(0.1f));

			// Create and attach a renderer for the monkey
			RenderComponent::Sptr renderer = ship->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Fenrir"));
			renderer->SetMaterial(Resources::GetMaterial("Grey"));
		}

		GameObject::Sptr demoBase = scene->CreateGameObject("Demo Parent");

		// Box to showcase the specular material
		GameObject::Sptr specBox = scene->CreateGameObject("Specular Object");
		{
			MeshResource::Sptr boxMesh = ResourceManager::CreateAsset<MeshResource>();
			boxMesh->AddParam(MeshBuilderParam::CreateCube(ZERO, ONE));
			boxMesh->GenerateMesh();

			// Set and rotation position in the scene
			specBox->SetPosition(glm::vec3(0, -4.0f, 1.0f));

			// Add a render component
			RenderComponent::Sptr renderer = specBox->Add<RenderComponent>();
			renderer->SetMesh(boxMesh);
			renderer->SetMaterial(Resources::GetMaterial("Test"));

			demoBase->AddChild(specBox);
		}

		// sphere to showcase the foliage material
		GameObject::Sptr foliageBall = scene->CreateGameObject("Foliage Sphere");
		{
			// Set and rotation position in the scene
			foliageBall->SetPosition(glm::vec3(-4.0f, -4.0f, 1.0f));

			// Add a render component
			RenderComponent::Sptr renderer = foliageBall->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Sphere"));
			renderer->SetMaterial(Resources::GetMaterial("Foliage"));

			demoBase->AddChild(foliageBall);
		}

		// Box to showcase the foliage material
		GameObject::Sptr foliageBox = scene->CreateGameObject("Foliage Box");
		{
			MeshResource::Sptr box = ResourceManager::CreateAsset<MeshResource>();
			box->AddParam(MeshBuilderParam::CreateCube(glm::vec3(0, 0, 0.5f), ONE));
			box->GenerateMesh();

			// Set and rotation position in the scene
			foliageBox->SetPosition(glm::vec3(-6.0f, -4.0f, 1.0f));

			// Add a render component
			RenderComponent::Sptr renderer = foliageBox->Add<RenderComponent>();
			renderer->SetMesh(box);
			renderer->SetMaterial(Resources::GetMaterial("Foliage"));

			demoBase->AddChild(foliageBox);
		}

		// Box to showcase the specular material
		GameObject::Sptr toonBall = scene->CreateGameObject("Toon Object");
		{
			// Set and rotation position in the scene
			toonBall->SetPosition(glm::vec3(-2.0f, -4.0f, 1.0f));

			// Add a render component
			RenderComponent::Sptr renderer = toonBall->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Sphere"));
			renderer->SetMaterial(Resources::GetMaterial("Toon"));

			demoBase->AddChild(toonBall);
		}

		GameObject::Sptr displacementBall = scene->CreateGameObject("Displacement Object");
		{
			// Set and rotation position in the scene
			displacementBall->SetPosition(glm::vec3(2.0f, -4.0f, 1.0f));

			// Add a render component
			RenderComponent::Sptr renderer = displacementBall->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Sphere"));
			renderer->SetMaterial(Resources::GetMaterial("Displacement"));

			demoBase->AddChild(displacementBall);
		}

		GameObject::Sptr multiTextureBall = scene->CreateGameObject("Multitextured Object");
		{
			// Set and rotation position in the scene 
			multiTextureBall->SetPosition(glm::vec3(4.0f, -4.0f, 1.0f));

			// Add a render component 
			RenderComponent::Sptr renderer = multiTextureBall->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Sphere"));
			renderer->SetMaterial(Resources::GetMaterial("Multi-Texture"));

			demoBase->AddChild(multiTextureBall);
		}

		GameObject::Sptr normalMapBall = scene->CreateGameObject("Normal Mapped Object");
		{
			// Set and rotation position in the scene 
			normalMapBall->SetPosition(glm::vec3(6.0f, -4.0f, 1.0f));

			// Add a render component 
			RenderComponent::Sptr renderer = normalMapBall->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Sphere"));
			renderer->SetMaterial(Resources::GetMaterial("Normal Map"));

			demoBase->AddChild(normalMapBall);
		}

		// Create a trigger volume for testing how we can detect collisions with objects!
		GameObject::Sptr trigger = scene->CreateGameObject("Trigger");
		{
			TriggerVolume::Sptr volume = trigger->Add<TriggerVolume>();
			CylinderCollider::Sptr collider = CylinderCollider::Create(glm::vec3(3.0f, 3.0f, 1.0f));
			collider->SetPosition(glm::vec3(0.0f, 0.0f, 0.5f));
			volume->AddCollider(collider);

			trigger->Add<TriggerVolumeEnterBehaviour>();
		}

		GameObject::Sptr shadowCaster = scene->CreateGameObject("Shadow Light");
		{
			// Set position in the scene
			shadowCaster->SetPosition(glm::vec3(3.0f, 3.0f, 5.0f));
			shadowCaster->LookAt(glm::vec3(0.0f));

			// Create and attach a renderer for the monkey
			ShadowCamera::Sptr shadowCam = shadowCaster->Add<ShadowCamera>();
			//shadowCam->SetOrthographic(false);
		}

		/////////////////////////// UI //////////////////////////////
		/*
		GameObject::Sptr canvas = scene->CreateGameObject("UI Canvas");
		{
			RectTransform::Sptr transform = canvas->Add<RectTransform>();
			transform->SetMin({ 16, 16 });
			transform->SetMax({ 256, 256 });

			GuiPanel::Sptr canPanel = canvas->Add<GuiPanel>();


			GameObject::Sptr subPanel = scene->CreateGameObject("Sub Item");
			{
				RectTransform::Sptr transform = subPanel->Add<RectTransform>();
				transform->SetMin({ 10, 10 });
				transform->SetMax({ 128, 128 });

				GuiPanel::Sptr panel = subPanel->Add<GuiPanel>();
				panel->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

				panel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/upArrow.png"));

				Font::Sptr font = ResourceManager::CreateAsset<Font>("fonts/Roboto-Medium.ttf", 16.0f);
				font->Bake();

				GuiText::Sptr text = subPanel->Add<GuiText>();
				text->SetText("Hello world!");
				text->SetFont(font);

				monkey1->Get<JumpBehaviour>()->Panel = text;
			}

			canvas->AddChild(subPanel);
		}
		*/

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
