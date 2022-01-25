
#include "Editor.h"

#include "Gameplay/SceneManagement/SceneManager.h"

#include <filesystem>
#include "Utils/ImGuiHelper.h"

using namespace Gameplay;

void Editor::Initialize(GLFWwindow* windowRef)
{
	// Initialize our ImGui helper
	ImGuiHelper::Init(windowRef);
	window = windowRef;

	// String storage for our editor scene input field
	scenePath = "scene.json";
	scenePath.reserve(256);
}

void Editor::Update(float deltaTime)
{

}

void Editor::Draw(float deltaTime)
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
	}
}

//// Showcasing how to use the imGui library!
	

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
