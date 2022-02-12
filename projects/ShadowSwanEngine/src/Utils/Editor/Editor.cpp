
#include "Editor.h"

#include "Gameplay/SceneManagement/SceneManager.h"
#include "Gameplay/InputManagement/InputHandler.h"

#include "Gameplay/Window.h"
#include "Utils/Editor/GameViewWindow.h"

#include <filesystem>
#include "Utils/ImGuiHelper.h"

#include "Gameplay/Prefabs.h"


using namespace Gameplay;

bool Editor::IsPlaying = false;
GameObject::Sptr Editor::selectedObject = nullptr;

void Editor::Initialize(GLFWwindow* windowRef)
{
	// Initialize our ImGui helper
	ImGuiHelper::Init(windowRef);
	window = windowRef;

	// String storage for our editor scene input field
	scenePath = "scene.json";
	scenePath.reserve(256);

	m_camera = SceneManager::GetCurrentScene()->MainCamera;
}

void Editor::Update(float deltaTime)
{
	// Editor Camera Controller
	if (!IsPlaying)
	{
		// XYZ Motion
		glm::vec3 cameraMotion = glm::vec3(0);
		if (InputHandler::GetKey(GLFW_KEY_W)) { cameraMotion += glm::vec3(0, 0.5, 0); }
		if (InputHandler::GetKey(GLFW_KEY_S)) { cameraMotion += glm::vec3(0, -0.5, 0); }
		if (InputHandler::GetKey(GLFW_KEY_A)) { cameraMotion += glm::vec3(-0.5, 0, 0); }
		if (InputHandler::GetKey(GLFW_KEY_D)) { cameraMotion += glm::vec3(0.5, 0, 0); }
		if (InputHandler::GetKey(GLFW_KEY_LEFT_CONTROL)) { cameraMotion += glm::vec3(0, 0, -0.5); }
		if (InputHandler::GetKey(GLFW_KEY_SPACE)) { cameraMotion += glm::vec3(0, 0, 0.5); }

		m_camera->GetGameObject()->SetPosition(m_camera->GetGameObject()->GetPosition() + cameraMotion * cameraSpeed * deltaTime);

		// Rotation
		glm::vec3 cameraRotation = glm::vec3(0);
		if (InputHandler::GetKey(GLFW_KEY_UP)) { cameraRotation += glm::vec3(0.5, 0, 0); }
		if (InputHandler::GetKey(GLFW_KEY_DOWN)) { cameraRotation += glm::vec3(-0.5, 0, 0); }
		if (InputHandler::GetKey(GLFW_KEY_LEFT)) { cameraRotation += glm::vec3(0, 0, 0.5); }
		if (InputHandler::GetKey(GLFW_KEY_RIGHT)) { cameraRotation += glm::vec3(0, 0, -0.5); }

		m_camera->GetGameObject()->SetRotation(m_camera->GetGameObject()->GetRotationEuler() + cameraRotation * 15.0f * deltaTime);

		// Raycasting (select Object in scene view)

	}
}

void Editor::Draw(float deltaTime)
{
	// Uncomment to view old ImGUI Editor (Sage's Code)
	//DrawOldGuiEditor(deltaTime);

	DrawNewGuiEditor(deltaTime);
}


void Editor::DrawNewGuiEditor(float deltaTime) 
{
	// Draw the panel here	
	ImGui::SetNextWindowSize(ImVec2(600, 700), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Editor", NULL, ImGuiWindowFlags_MenuBar))
	{		
		// Menubar
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{				
				ImGui::Separator();
				if (ImGui::MenuItem("Exit")) { glfwSetWindowShouldClose(Window::GetWindow(), true); }
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Scene")) {
				if (ImGui::MenuItem("Level One")) { SceneManager::LoadScene(SceneManager::Scenes::LevelOne, false); }
				if (ImGui::MenuItem("Level Two")) { SceneManager::LoadScene(SceneManager::Scenes::LevelTwo, false); }
				if (ImGui::MenuItem("Level Three")) { SceneManager::LoadScene(SceneManager::Scenes::LevelThree, false); }
				if (ImGui::MenuItem("Level Four")) { SceneManager::LoadScene(SceneManager::Scenes::BossLevel, false); }
				ImGui::Separator();
				if (ImGui::MenuItem("Main Menu")) { SceneManager::LoadScene(SceneManager::Scenes::MainMenu, false); }
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(125, 125, 125, 255));
				if (ImGui::MenuItem("Loading Menu")) { /* TO DO */ }
				ImGui::PopStyleColor();
				ImGui::Separator();
				if (ImGui::MenuItem("Sandbox")) { SceneManager::LoadScene(SceneManager::Scenes::Sandbox, false); }
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(125, 125, 125, 255));
				if (ImGui::MenuItem("Undo")) { /* Do stuff */ }
				if (ImGui::MenuItem("Redo")) { /* Do stuff */ }
				if (ImGui::MenuItem("Cut")) { /* Do stuff */ }
				if (ImGui::MenuItem("Copy")) { /* Do stuff */ }
				if (ImGui::MenuItem("Paste")) { /* Do stuff */ }
				if (ImGui::MenuItem("Duplicate")) { /* Do stuff */ }

				if (ImGui::MenuItem("Delete"))
				{
					
				}
				ImGui::PopStyleColor();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Empty Object")) { SceneManager::GetCurrentScene()->CreateGameObject("New GameObject"); }				
				ImGui::Separator();
				if (ImGui::MenuItem("Wall")) { Prefabs::Instantiate(Prefabs::Prefab::Wall); }
				if (ImGui::MenuItem("Ground")) { Prefabs::Instantiate(Prefabs::Prefab::Ground); }
				ImGui::Separator();
				if (ImGui::MenuItem("Crate")) { Prefabs::Instantiate(Prefabs::Prefab::Crate); }
				ImGui::Separator();
				if (ImGui::MenuItem("Pressure Plate")) { Prefabs::Instantiate(Prefabs::Prefab::PresurePlate); }				
				if (ImGui::MenuItem("Switch")) { Prefabs::Instantiate(Prefabs::Prefab::Switch); }
				ImGui::Separator();
				if (ImGui::MenuItem("Elevator")) { Prefabs::Instantiate(Prefabs::Prefab::Elevator); }
				if (ImGui::MenuItem("Spike Trap")) { Prefabs::Instantiate(Prefabs::Prefab::SpikeTrap); }
				if (ImGui::MenuItem("Turret")) { Prefabs::Instantiate(Prefabs::Prefab::Turret); }
				ImGui::Separator();
				if (ImGui::MenuItem("Key")) { Prefabs::Instantiate(Prefabs::Prefab::Key); }
				if (ImGui::MenuItem("Key Door")) { Prefabs::Instantiate(Prefabs::Prefab::KeyDoor); }
				

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Debug"))
			{
				if (ImGui::MenuItem("Default")) { SceneManager::GetCurrentScene()->SetPhysicsDebugDrawMode(BulletDebugMode::None); }
				if (ImGui::MenuItem("Wireframe")) { SceneManager::GetCurrentScene()->SetPhysicsDebugDrawMode(BulletDebugMode::DrawWireframe); }
				if (ImGui::MenuItem("AABB")) { SceneManager::GetCurrentScene()->SetPhysicsDebugDrawMode(BulletDebugMode::DrawAabb); }
				ImGui::EndMenu();
			}		

			if (ImGui::BeginMenu("Window"))
			{
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(125, 125, 125, 255));
				if (ImGui::MenuItem("Hierarchy")) { /* Do stuff */ }
				if (ImGui::MenuItem("Inspector")) { /* Do stuff */ }
				if (ImGui::MenuItem("Assets")) { /* Do stuff */ }
				if (ImGui::MenuItem("Console")) { /* Do stuff */ }
				ImGui::PopStyleColor();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		GameViewWindow::imgui(&sceneViewOpen);

		// Hierarchy				

		if (ImGui::BeginChild("Hierarchy", ImVec2(200, 500), true))
		{
			std::vector<GameObject::Sptr> objects = SceneManager::GetCurrentScene()->GetAllObjects();
			for (int i = 0; i < objects.size(); i++) {
				int index = i;
				
				if (ImGui::Selectable(objects[i]->Name.c_str(), selectedObjectIndex == index))
				{
					selectedObjectIndex = index;
					selectedObject = objects[i];
				}
			}

			ImGui::EndChild();
		}
		

		// Inspector
		ImGui::SameLine();
		if (ImGui::BeginChild("Inspector", ImVec2(300, 500), true))
		{
			if (selectedObject != nullptr) {
				selectedObject->DrawImGui();				
			}

			ImGui::EndChild();
		}
		

		// Assets
		if (ImGui::BeginChild("Assets", ImVec2(500, 0), true))
		{
			ImGui::EndChild();
		}
		

		ImGui::End();
	}

	
}

// OLD STUFF
void Editor::DrawOldGuiEditor(float deltaTime) 
{
	bool isDebugWindowOpen = ImGui::Begin("Debugging");
	if (isDebugWindowOpen) {
		// Draws a button to control whether or not the game is currently playing
		static char buttonLabel[64];
		sprintf_s(buttonLabel, "%s###playmode", SceneManager::GetCurrentScene()->IsPlaying ? "Exit Play Mode" : "Enter Play Mode");
		if (ImGui::Button(buttonLabel)) {
			// Save scene so it can be restored when exiting play mode
			if (!SceneManager::GetCurrentScene()->IsPlaying) {
				editorSceneState = SceneManager::GetCurrentScene()->ToJson();
			}

			// Toggle state
			SceneManager::GetCurrentScene()->IsPlaying = !SceneManager::GetCurrentScene()->IsPlaying;

			// If we've gone from playing to not playing, restore the state from before we started playing
			if (!SceneManager::GetCurrentScene()->IsPlaying) {
				SceneManager::GetCurrentScene() = nullptr;
				// We reload to scene from our cached state
				SceneManager::GetCurrentScene() = Scene::FromJson(editorSceneState);
				// Don't forget to reset the scene's window and wake all the objects!
				SceneManager::GetCurrentScene()->Window = window;
				SceneManager::GetCurrentScene()->Awake();
			}
		}

		// Make a new area for the scene saving/loading
		ImGui::Separator();
		if (DrawSaveLoadImGui(SceneManager::GetCurrentScene(), scenePath)) {
			// C++ strings keep internal lengths which can get annoying
			// when we edit it's underlying datastore, so recalcualte size
			scenePath.resize(strlen(scenePath.c_str()));

			// We have loaded a new scene, call awake to set
			// up all our components
			SceneManager::GetCurrentScene()->Window = window;
			SceneManager::GetCurrentScene()->Awake();
		}
		ImGui::Separator();
		// Draw a dropdown to select our physics debug draw mode
		if (BulletDebugDraw::DrawModeGui("Physics Debug Mode:", physicsDebugMode)) {
			SceneManager::GetCurrentScene()->SetPhysicsDebugDrawMode(physicsDebugMode);
		}
		LABEL_LEFT(ImGui::SliderFloat, "Playback Speed:    ", &playbackSpeed, 0.0f, 10.0f);
		ImGui::Separator();

		// Draw some ImGui stuff for the lights
		if (isDebugWindowOpen) {
			for (int ix = 0; ix < SceneManager::GetCurrentScene()->Lights.size(); ix++) {
				char buff[256];
				sprintf_s(buff, "Light %d##%d", ix, ix);
				// DrawLightImGui will return true if the light was deleted
				if (DrawLightImGui(SceneManager::GetCurrentScene(), buff, ix)) {
					// Remove light from scene, restore all lighting data
					SceneManager::GetCurrentScene()->Lights.erase(SceneManager::GetCurrentScene()->Lights.begin() + ix);
					SceneManager::GetCurrentScene()->SetupShaderAndLights();
					// Move back one element so we don't skip anything!
					ix--;
				}
			}
			// As long as we don't have max lights, draw a button
			// to add another one
			if (SceneManager::GetCurrentScene()->Lights.size() < SceneManager::GetCurrentScene()->MAX_LIGHTS) {
				if (ImGui::Button("Add Light")) {
					SceneManager::GetCurrentScene()->Lights.push_back(Light());
					SceneManager::GetCurrentScene()->SetupShaderAndLights();
				}
			}
			// Split lights from the objects in ImGui
			ImGui::Separator();
		}

		// Draw object GUIs
		if (isDebugWindowOpen) {
			SceneManager::GetCurrentScene()->DrawAllGameObjectGUIs();
		}

		ImGui::End();
	}
}

bool Editor::DrawSaveLoadImGui(Scene::Sptr& scene, std::string& path) {
	// Since we can change the internal capacity of an std::string,
	// we can do cool things like this!
	ImGui::InputText("Path", path.data(), path.capacity());

	// Draw a save button, and save when pressed
	if (ImGui::Button("Save")) {
		scene->Save(path);

		std::string newFilename = std::filesystem::path(path).stem().string() + "-manifest.json";
		ResourceManager::SaveManifest(newFilename);
	}
	ImGui::SameLine();
	// Load scene from file button
	if (ImGui::Button("Load")) {
		// Since it's a reference to a ptr, this will
		// overwrite the existing scene!
		scene = nullptr;

		std::string newFilename = std::filesystem::path(path).stem().string() + "-manifest.json";
		ResourceManager::LoadManifest(newFilename);
		scene = Scene::Load(path);

		return true;
	}
	return false;
}

bool Editor::DrawLightImGui(const Scene::Sptr& scene, const char* title, int ix) {
	bool isEdited = false;
	bool result = false;
	Light& light = scene->Lights[ix];
	ImGui::PushID(&light); // We can also use pointers as numbers for unique IDs
	if (ImGui::CollapsingHeader(title)) {
		isEdited |= ImGui::DragFloat3("Pos", &light.Position.x, 0.01f);
		isEdited |= ImGui::ColorEdit3("Col", &light.Color.r);
		isEdited |= ImGui::DragFloat("Range", &light.Range, 0.1f);

		result = ImGui::Button("Delete");
	}
	if (isEdited) {
		scene->SetShaderLight(ix);
	}

	ImGui::PopID();
	return result;
}
