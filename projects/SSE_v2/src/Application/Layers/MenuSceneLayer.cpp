#include "MenuSceneLayer.h"

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

MenuSceneLayer::MenuSceneLayer() :
	ApplicationLayer()
{
	Name = "Menu Scene";
	Overrides = AppLayerFunctions::OnAppLoad;
}

MenuSceneLayer::~MenuSceneLayer() = default;

void MenuSceneLayer::OnAppLoad(const nlohmann::json & config) {
	LoadScene();
}

void MenuSceneLayer::LoadScene()
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
	}
	else {

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
			camera->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
			camera->SetRotation(glm::vec3(75.0f, 0.0f, 33.0f));
			camera->Get<Camera>()->SetFovDegrees(60.0f);
		}

		GameObject::Sptr character = Prefabs::Load(scene, "Character Body");
		{
			character->SetPosition(glm::vec3(-5.0f, 4.0f, 0.0f));
			character->SetRotation(glm::vec3(90.f, 0.0f, -115.0f));
		}

		// Environment
		// =========================================================================
#pragma region Environment

		GameObject::Sptr environment = scene->CreateGameObject("Environment");		

		GameObject::Sptr floor = Prefabs::Load(scene, "Floor");
		{
			floor->SetPosition(glm::vec3(0));
			floor->SetScale(glm::vec3(0.67f, 0.65f, 1.0f));
			floor->Get<RigidBody>()->AddCollider(BoxCollider::Create(glm::vec3(16.75f, 16.25f, 1.5f)))->SetPosition({ 0.3,-0.1,-2.3 });
			environment->AddChild(floor);
		}

		GameObject::Sptr wall1 = Prefabs::Load(scene, "Wall");
		{
			wall1->SetPosition(glm::vec3(-3.05, 9.670f, 3.0f));
			wall1->SetRotation(glm::vec3(90.0f, 0.0f, -100.0f));
			wall1->SetScale(glm::vec3(0.5f));
			environment->AddChild(wall1);
		}

		GameObject::Sptr wall2 = Prefabs::Load(scene, "Wall");
		{
			wall2->SetPosition(glm::vec3(-6.6f, 5.75f, 3.0f));
			wall2->SetRotation(glm::vec3(90.0f, 0.0f, -20.0f));
			wall2->SetScale(glm::vec3(0.5f));
			environment->AddChild(wall2);
		}

		GameObject::Sptr light = scene->CreateGameObject("Light");
		{
			light->SetPosition(glm::vec3(glm::vec3(0, 0, 1)));

			Light::Sptr lightComponent = light->Add<Light>();
			lightComponent->SetColor(glm::vec3(0.5f, 0.7f, 1.0f));
			lightComponent->SetRadius(1);
			lightComponent->SetIntensity(17);
		}


#pragma endregion

#pragma region Audio

		AudioEngine::Instance().GetEvent("Mohit").SetVolume(0.25f);
		AudioEngine::Instance().GetEvent("Mohit").Play();

		AudioEngine::Instance().GetEvent("Dialogue").Play();

#pragma endregion

#pragma region User Interface

		GameObject::Sptr ui = scene->CreateGameObject("User Interface");

		GameObject::Sptr menuCanvas = scene->CreateGameObject("Menu Canvas");
		{
			GameObject::Sptr t = UIHelper::CreateImage(scene, Resources::GetTexture2D("TitleT"), "title");
			t->Get<RectTransform>()->SetPosition({ 1100, 120 });
			t->Get<RectTransform>()->SetSize({ 120, 60 });
			t->Get<GuiPanel>()->SetBorderRadius(0);
			menuCanvas->AddChild(t);

			GameObject::Sptr upperGraphic = UIHelper::CreateImage(scene, Resources::GetTexture2D("Upper"), "Upper Graphic");
			upperGraphic->Get<RectTransform>()->SetPosition({ 1090, 280 });
			upperGraphic->Get<RectTransform>()->SetSize({ 100, 80 });
			upperGraphic->Get<GuiPanel>()->SetBorderRadius(0);
			menuCanvas->AddChild(upperGraphic);

			GameObject::Sptr button1 = UIHelper::CreateImage(scene, Resources::GetTexture2D("MainMenu"), "MainMenu");
			button1->Get<RectTransform>()->SetPosition({ 1090, 500 });
			button1->Get<RectTransform>()->SetSize({ 100, 50 });
			button1->Get<GuiPanel>()->SetBorderRadius(0);
			menuCanvas->AddChild(button1);

			GameObject::Sptr lowerGraphic = UIHelper::CreateImage(scene, Resources::GetTexture2D("Lower"), "Lower Graphic");
			lowerGraphic->Get<RectTransform>()->SetPosition({ 1090, 700 });
			lowerGraphic->Get<RectTransform>()->SetSize({ 100, 80 });
			lowerGraphic->Get<GuiPanel>()->SetBorderRadius(0);
			menuCanvas->AddChild(lowerGraphic);
		}

#pragma endregion

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

void MenuSceneLayer::UnloadScene()
{
	Application& app = Application::Get();

	// Stop all Audio Events
	AudioEngine::Instance().GetEvent("Mohit").StopImmediately();

	// Unload the Scene by loading empty scene
	app.LoadScene(std::make_shared<Gameplay::Scene>());
}