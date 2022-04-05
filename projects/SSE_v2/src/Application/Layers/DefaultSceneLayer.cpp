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
		GameObject::Sptr shadow = Prefabs::Load(scene, "Shadow");

		GameObject::Sptr controller = scene->CreateGameObject("Controller");
		{
			PlayerController::Sptr pc = controller->Add<PlayerController>();
			pc->SetCharacterBody(character);
			pc->SetCharacterShadow(shadow);
			pc->SetInteractionCollider(character->Get<TriggerVolume>());
			pc->SetCamera(camera);

			GameManager::GetInstance().SetPC(pc);
		}

		// Floors
		// =========================================================================

		GameObject::Sptr plane = scene->CreateGameObject("Plane");
		{
			// Make a big tiled mesh
			MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
			tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(100.0f), glm::vec2(20.0f)));
			tiledMesh->GenerateMesh();

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = plane->Add<RenderComponent>();
			renderer->SetMesh(tiledMesh);
			renderer->SetMaterial(Resources::GetMaterial("StoneTex"));

			// Attach a plane collider that extends infinitely along the X/Y axis
			RigidBody::Sptr physics = plane->Add<RigidBody>(/*static by default*/);
			physics->AddCollider(BoxCollider::Create(glm::vec3(50.0f, 50.0f, 1.0f)))->SetPosition({ 0,0,-1 });
			physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
			physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
		}

		/// Walls
		// =========================================================================


		// Interactables
		// =========================================================================

		Prefabs::Load(scene, "Spike Trap", glm::vec3(10.0f, 0.0f, 1.0f));
		Prefabs::Load(scene, "Key", glm::vec3(-8, 11, 0));
		
		GameObject::Sptr Healing1 = Prefabs::Load(scene, "Healing Well", glm::vec3(0, -6.5f, 0));
		{
			Healing1->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
		}

		GameObject::Sptr Shroom1 = Prefabs::Load(scene, "Shroom Trap Multi", glm::vec3(10, 7, 0));
		{

		}

		GameObject::Sptr C1 = Prefabs::Load(scene, "Crystaling", glm::vec3(0, 6.5f, 0));
		{

		}

		GameObject::Sptr plate_door = Prefabs::Load(scene, "Cage Door", glm::vec3(-12.5f, -4, 0));
		{
			plate_door->SetRotation(glm::vec3(90, 0, 0));
		}

		GameObject::Sptr pressure_plate = Prefabs::Load(scene, "Pressure Plate", glm::vec3(-11, -10, 0));
		{
			pressure_plate->SetScale(glm::vec3(0.75f, 0.5f, 0.75f));

			pressure_plate->Get<TriggerVolumeEnterBehaviour>()->onTriggerEnterEvent.push_back([plate_door]
			{
				plate_door->Get<RigidBody>()->SetCollisionGroup(Resources::Instance().NO_GROUP);
				plate_door->Get<RigidBody>()->SetCollisionMask(Resources::Instance().NO_MASK);
				plate_door->Get<RenderComponent>()->IsEnabled = false;
			});

			pressure_plate->Get<TriggerVolumeEnterBehaviour>()->onTriggerExitEvent.push_back([plate_door]
			{
				plate_door->Get<RigidBody>()->SetCollisionGroupMulti(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
				plate_door->Get<RigidBody>()->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);
				plate_door->Get<RenderComponent>()->IsEnabled = true;
			});
		}

		GameObject::Sptr elevator = Prefabs::Load(scene, "Moving Platform", glm::vec3(10, -12.5f, 0));
		{
			elevator->SetScale(glm::vec3(2, 2, 0.5f));

			elevator->Get<MovingPlatformBehavior>()->SetStartPosition(glm::vec3(10, -12.5f, 0));
			elevator->Get<MovingPlatformBehavior>()->SetEndPosition(glm::vec3(10, -12.5f, 5));
		}

		GameObject::Sptr lever_door = Prefabs::Load(scene, "Cage Door", glm::vec3(-12.5f, 8, 0));
		{
			lever_door->SetRotation(glm::vec3(90, 0, 0));
		}		

		GameObject::Sptr lever = Prefabs::Load(scene, "Lever", glm::vec3(-12, 5, 0));
		{
			lever->Get<InteractableComponent>()->onInteractionEvent.push_back([lever, lever_door] {
				
				if (lever->Get<InteractableComponent>()->isToggled)
				{
					lever_door->Get<RigidBody>()->SetCollisionGroup(Resources::Instance().NO_GROUP);
					lever_door->Get<RigidBody>()->SetCollisionMask(Resources::Instance().NO_MASK);
				
					lever_door->Get<RenderComponent>()->IsEnabled = false;
				}

				else
				{
					lever_door->Get<RigidBody>()->SetCollisionGroupMulti(Resources::Instance().PHYSICAL_GROUP | Resources::Instance().SHADOW_GROUP);
					lever_door->Get<RigidBody>()->SetCollisionMask(Resources::Instance().PHYSICAL_MASK | Resources::Instance().SHADOW_MASK);

					lever_door->Get<RenderComponent>()->IsEnabled = true;
				}
			});
		}

		GameObject::Sptr keyDoor = Prefabs::Load(scene, "Key Door", glm::vec3(0, 12, 0));
		{
			keyDoor->SetRotation(glm::vec3(90, 0, 90));
		}

		GameObject::Sptr cobweb = Prefabs::Load(scene, "Cobweb", glm::vec3(11.5f, -6.5f, 1.0));
		{
			cobweb->SetRotation(glm::vec3(-65, 0, -90));
		}

		GameObject::Sptr wall1 = Prefabs::Load(scene, "Wall", glm::vec3(-15, 0, 0));
		{
			wall1->SetRotation(glm::vec3(90, 0, 0));
			wall1->SetScale(glm::vec3(1));
		}
		
		GameObject::Sptr wall2 = Prefabs::Load(scene, "Wall", glm::vec3(15, 0, 0));
		{
			wall2->SetRotation(glm::vec3(90, 0, 0));
			wall2->SetScale(glm::vec3(1));
		}

		GameObject::Sptr wall3 = Prefabs::Load(scene, "Wall", glm::vec3(0, 15, 0));
		{
			wall3->SetRotation(glm::vec3(90, 0, 90));
			wall3->SetScale(glm::vec3(1));
		}
		

		// Misc
		// =========================================================================
		
		// Create some lights for our scene
		GameObject::Sptr light = scene->CreateGameObject("Light");
		{
			light->SetPosition(glm::vec3(glm::vec3(0, 0, 10)));			

			Light::Sptr lightComponent = light->Add<Light>();
			lightComponent->SetColor(glm::vec4(1));
			lightComponent->SetRadius(100);
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
			shadowText->Get<RectTransform>()->SetPosition({ 175, 180 });
			gameCanvas->AddChild(shadowText);

			GameManager::GameInterface.SetGameUserInterface(*gameCanvas);
			GameManager::GameInterface.InitializeGameUserInterface(*healthText->Get<GuiText>(), *shadowText->Get<GuiText>());
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
