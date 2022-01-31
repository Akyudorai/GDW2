#include "Main_Menu.h"
#include "Gameplay/SceneManagement/SceneManager.h"

// Gameplay
#include "Gameplay/MeshResource.h"

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

// GUI
#include "Gameplay/Components/GUI/RectTransform.h"
#include "Gameplay/Components/GUI/GuiPanel.h"
#include "Gameplay/Components/GUI/GuiText.h"
#include "Graphics/GuiBatcher.h"
#include "Utils/UIHelper.h"

using namespace Gameplay::Physics;

bool Main_Menu::PlayOnLoad = true;

Scene::Sptr Main_Menu::Load(GLFWwindow* window)
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
	SceneManager::GetCurrentScene()->Lights[0].Position = glm::vec3(0.0f, 0.0f, 13.0f);
	SceneManager::GetCurrentScene()->Lights[0].Color = glm::vec3(1.0f, 1.0f, 1.0f);
	SceneManager::GetCurrentScene()->Lights[0].Range = 500.0f;

	/////////////////////////////////////////////////////////
	//					  CONTROLLER
	/////////////////////////////////////////////////////////

	GameObject::Sptr camera = SceneManager::GetCurrentScene()->CreateGameObject("Main Camera");
	{
		camera->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
		camera->SetRotation(glm::vec3(75.0f, 0.0f, 33.0f));				

		Camera::Sptr cam = camera->Add<Camera>();
		cam->SetFovDegrees(60.0f);
		SceneManager::GetCurrentScene()->MainCamera = cam;
	}

	GameObject::Sptr body = SceneManager::GetCurrentScene()->CreateGameObject("Body");
	{
		// Set position in the SceneManager::GetCurrentScene()
		body->SetPosition(glm::vec3(-5.0f, 4.0f, 0.0f));
		body->SetRotation(glm::vec3(90.f, 0.0f, -110.0f));
		body->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

		RenderComponent::Sptr renderer = body->Add<RenderComponent>();

		renderer->SetMesh(Resources::GetMesh("Character"));
		renderer->SetMaterial(Resources::GetMaterial("Character"));

		AnimatorComponent::Sptr animator = body->Add<AnimatorComponent>();
		animator->AddAnimation("Idle", Resources::GetAnimation("Character Idle"));
		animator->SetRenderer(*renderer);
		animator->SetAnimation("Idle");
		animator->SetLooping(true);
		animator->SetPause(false);
		animator->SetSpeed(2.0f);
	}

	/////////////////////////////////////////////////////////
	//					  ENVIRONMENT
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

	GameObject::Sptr wall = SceneManager::GetCurrentScene()->CreateGameObject("Wall");
	{
		wall->SetPosition(glm::vec3(0, 9.5f, 0.0f));
		wall->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
		wall->SetScale(glm::vec3(1.0f, 1.0f, 0.6f));

		RenderComponent::Sptr renderer = wall->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	GameObject::Sptr wall2 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 2");
	{
		wall2->SetPosition(glm::vec3(0-9.5f, 8.0f, 0.0f));
		wall2->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
		wall2->SetScale(glm::vec3(1.0f, 1.0f, 0.6f));

		RenderComponent::Sptr renderer = wall2->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
	}

	/////////////////////////////////////////////////////////
	//					 USER INTERFACE
	/////////////////////////////////////////////////////////

	GameObject::Sptr pauseMenu = SceneManager::GetCurrentScene()->CreateGameObject("UI Menu Canvas");
	{
		RectTransform::Sptr transform = pauseMenu->Add<RectTransform>();
		transform->SetMin({ 16, 16 });
		transform->SetMax({ 350, 500 });
		transform->SetPosition({ 550, 400 });

		GuiPanel::Sptr backgroundPanel = pauseMenu->Add<GuiPanel>();
		backgroundPanel->SetColor(glm::vec4(0.3f, 0.3f, 0.3f, 0.0f));


		GameObject::Sptr upperGraphic = UIHelper::CreateImage(Resources::GetTexture("Menu Gloss"), "Upper Graphic");
		upperGraphic->Get<RectTransform>()->SetPosition({ 170, 90 });
		upperGraphic->Get<RectTransform>()->SetSize({ 80, 30 });
		upperGraphic->Get<GuiPanel>()->SetBorderRadius(1);
		pauseMenu->AddChild(upperGraphic);

		GameObject::Sptr menuTitle = UIHelper::CreateText("Into The Abyss");
		menuTitle->Get<GuiText>()->SetTextScale(2);
		menuTitle->Get<RectTransform>()->SetPosition({ 250, 75 });	
		pauseMenu->AddChild(menuTitle);

		GameObject::Sptr button1 = UIHelper::CreateButton("1. New Game");
		button1->Get<RectTransform>()->SetPosition({ 167.5f, 155 });
		button1->Get<GuiPanel>()->SetBorderRadius(0);
		pauseMenu->AddChild(button1);

		GameObject::Sptr button2 = UIHelper::CreateButton("2. Choose Level");
		button2->Get<RectTransform>()->SetPosition({ 167.5f, 215 });
		button2->Get<GuiPanel>()->SetBorderRadius(0);
		pauseMenu->AddChild(button2);

		GameObject::Sptr button3 = UIHelper::CreateButton("3. Options");
		button3->Get<RectTransform>()->SetPosition({ 167.5f, 275 });
		button3->Get<GuiPanel>()->SetBorderRadius(0);
		pauseMenu->AddChild(button3);

		GameObject::Sptr button4 = UIHelper::CreateButton("4. Quit to Desktop");
		button4->Get<RectTransform>()->SetPosition({ 167.5f, 335 });
		button4->Get<GuiPanel>()->SetBorderRadius(0);
		pauseMenu->AddChild(button4);

		GameObject::Sptr lowerGraphic = UIHelper::CreateImage(Resources::GetTexture("Menu Gloss Reverse"), "Lower Graphic");
		lowerGraphic->Get<RectTransform>()->SetPosition({ 160, 400 });
		lowerGraphic->Get<RectTransform>()->SetSize({ 40, 17.5f });
		lowerGraphic->Get<GuiPanel>()->SetBorderRadius(0);
		pauseMenu->AddChild(lowerGraphic);
	}

	/////////////////////////////////////////////////////////

	// Call SceneManager::GetCurrentScene() awake to start up all of our components
	SceneManager::GetCurrentScene()->Window = window;
	SceneManager::GetCurrentScene()->Awake();

	// Save the asset manifest for all the resources we just loaded
	ResourceManager::SaveManifest("manifest.json");
	// Save the SceneManager::GetCurrentScene() to a JSON file
	SceneManager::GetCurrentScene()->Save("Level_Three.json");

	return nullptr;
}