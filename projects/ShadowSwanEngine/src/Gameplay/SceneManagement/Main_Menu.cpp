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

// Audio
#include "Audio/AudioSource.h"

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
	SceneManager::GetCurrentScene()->Lights[0].Position = glm::vec3(0.8f, -3.5f, 10.0f);
	SceneManager::GetCurrentScene()->Lights[0].Color = glm::vec3(1, 1, 1);
	SceneManager::GetCurrentScene()->Lights[0].Range = 100.0f;

	SceneManager::GetCurrentScene()->Lights[1].Position = glm::vec3(-6.0f, 9.0f, 7.0f);
	SceneManager::GetCurrentScene()->Lights[1].Color = glm::vec3(0.5f, 0.7f, 1.0f);
	SceneManager::GetCurrentScene()->Lights[1].Range = 50.0f;

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
		body->SetPosition(glm::vec3(-5.0f, 4.0f, 0.1f));
		body->SetRotation(glm::vec3(90.f, 0.0f, -110.0f));
		body->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

		RenderComponent::Sptr renderer = body->Add<RenderComponent>();

		renderer->SetMesh(Resources::GetMesh("Door"));
		renderer->SetMaterial(Resources::GetMaterial("Door"));

		AnimatorComponent::Sptr animator = body->Add<AnimatorComponent>();
		animator->AddAnimation("Open", Resources::GetAnimation("Door"), 1.0f);
		animator->SetRenderer(*renderer);
		animator->SetLooping(true);
		animator->SetSpeed(1.0f);
		animator->Play("Open");
	}

	/////////////////////////////////////////////////////////
	//					  ENVIRONMENT
	/////////////////////////////////////////////////////////

	// Set up all our sample objects
	GameObject::Sptr floor = SceneManager::GetCurrentScene()->CreateGameObject("Floor");
	{
		MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
		tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(50.0f), glm::vec2(8.0f)));
		tiledMesh->GenerateMesh();

		// Create and attach a RenderComponent to the object to draw our mesh
		RenderComponent::Sptr renderer = floor->Add<RenderComponent>();
		renderer->SetMesh(tiledMesh);
		renderer->SetMaterial(Resources::GetMaterial("StoneTex"));

		RigidBody::Sptr physics = floor->Add<RigidBody>(/*static by default*/);
		physics->AddCollider(BoxCollider::Create(glm::vec3(100.0f, 100.0f, 1.5f)))->SetPosition({ 0,0,-1 });
		physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
		physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
	}

	GameObject::Sptr wall = SceneManager::GetCurrentScene()->CreateGameObject("Wall");
	{
		wall->SetPosition(glm::vec3(-3.05, 9.670f, 3.0f));
		wall->SetRotation(glm::vec3(90.0f, 0.0f, -100.0f));
		wall->SetScale(glm::vec3(0.5f));

		RenderComponent::Sptr renderer = wall->Add<RenderComponent>();
		renderer->SetMesh(Resources::GetMesh("Stone Wall"));
		renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));

		GameObject::Sptr wall2 = SceneManager::GetCurrentScene()->CreateGameObject("Wall 2");
		{
			wall2->SetPosition(glm::vec3(-6.6f, 5.75f, 3.0f));
			wall2->SetRotation(glm::vec3(90.0f, 0.0f, -20.0f));
			wall2->SetScale(glm::vec3(0.5f));

			RenderComponent::Sptr renderer = wall2->Add<RenderComponent>();
			renderer->SetMesh(Resources::GetMesh("Stone Wall"));
			renderer->SetMaterial(Resources::GetMaterial("Stone Wall"));
		}

	/////////////////////////////////////////////////////////
	//					 AUDIO
	/////////////////////////////////////////////////////////

	GameObject::Sptr bgm = SceneManager::GetCurrentScene()->CreateGameObject("BGM");
	{
		bgm->SetPosition(glm::vec3(-5.0f, 4.0f, 0.0f));
		
		AudioSource::Sptr audio = bgm->Add<AudioSource>();		
		audio->m_Resource = Resources::GetSound("Mohit");
		audio->m_Settings = AudioSettings{
			false, true, false
		};
		audio->playOnAwake = false;
		audio->Init();
	}

	/////////////////////////////////////////////////////////
	//					 USER INTERFACE
	/////////////////////////////////////////////////////////

		GameObject::Sptr mainMenu = SceneManager::GetCurrentScene()->CreateGameObject("UI Menu Canvas");
		{
			GameObject::Sptr t = UIHelper::CreateImage(Resources::GetTexture("TitleT"), "title");
			t->Get<RectTransform>()->SetPosition({ 550, 82 });
			t->Get<RectTransform>()->SetSize({ 100, 40 });
			t->Get<GuiPanel>()->SetBorderRadius(0);
			mainMenu->AddChild(t);

			GameObject::Sptr upperGraphic = UIHelper::CreateImage(Resources::GetTexture("Upper"), "Upper Graphic");
			upperGraphic->Get<RectTransform>()->SetPosition({ 550, 180 });
			upperGraphic->Get<RectTransform>()->SetSize({ 80, 60 });
			upperGraphic->Get<GuiPanel>()->SetBorderRadius(0);
			mainMenu->AddChild(upperGraphic);

			GameObject::Sptr button1 = UIHelper::CreateImage(Resources::GetTexture("New"), "New Game");
			button1->Get<RectTransform>()->SetPosition({ 550, 260 });
			button1->Get<RectTransform>()->SetSize({ 60, 30 });
			button1->Get<GuiPanel>()->SetBorderRadius(0);
			mainMenu->AddChild(button1);

			GameObject::Sptr button2 = UIHelper::CreateImage(Resources::GetTexture("ChooseLevel"), "ChooseLevel");
			button2->Get<RectTransform>()->SetPosition({ 550, 360 });
			button2->Get<RectTransform>()->SetSize({ 60, 30 });
			button2->Get<GuiPanel>()->SetBorderRadius(0);
			mainMenu->AddChild(button2);

			GameObject::Sptr b3 = UIHelper::CreateImage(Resources::GetTexture("Options"), "Options");
			b3->Get<RectTransform>()->SetPosition({ 550, 460 });
			b3->Get<RectTransform>()->SetSize({ 60, 30 });
			b3->Get<GuiPanel>()->SetBorderRadius(0);
			mainMenu->AddChild(b3);

			GameObject::Sptr b4 = UIHelper::CreateImage(Resources::GetTexture("Credits"), "Credits");
			b4->Get<RectTransform>()->SetPosition({ 550, 560 });
			b4->Get<RectTransform>()->SetSize({ 60, 30 });
			b4->Get<GuiPanel>()->SetBorderRadius(0);
			mainMenu->AddChild(b4);

			GameObject::Sptr b5 = UIHelper::CreateImage(Resources::GetTexture("Exit"), "Exit");
			b5->Get<RectTransform>()->SetPosition({ 550, 660 });
			b5->Get<RectTransform>()->SetSize({ 60, 30 });
			b5->Get<GuiPanel>()->SetBorderRadius(0);
			mainMenu->AddChild(b5);

			GameObject::Sptr lowerGraphic = UIHelper::CreateImage(Resources::GetTexture("Lower"), "Lower Graphic");
			lowerGraphic->Get<RectTransform>()->SetPosition({ 550, 740 });
			lowerGraphic->Get<RectTransform>()->SetSize({ 80, 60 });
			lowerGraphic->Get<GuiPanel>()->SetBorderRadius(0);
			mainMenu->AddChild(lowerGraphic);
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
}