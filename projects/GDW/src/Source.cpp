/*
Week 4 tutorial sample - Splines and steering behaviours
Quinn Daggett 2021
*/

#include "NOU/App.h"
#include "NOU/Input.h"
#include "NOU/Entity.h"
#include "NOU/CCamera.h"
#include "NOU/CMeshRenderer.h"
#include "NOU/Shader.h"
#include "NOU/GLTFLoader.h"

#include <iostream>

#include "imgui.h"

#include <memory>
#include <cmath>

#include "Editor/Editor.h"
#include "Game/FPS_Controller.h"

#include "Editor/Resources.h"

using namespace nou;
using namespace OMG;
using namespace GAME;


int main()
{
	// Create window and set clear color
	App::Init("Game", 800, 600);
	App::SetClearColor(glm::vec4(0.0f, 0.27f, 0.4f, 1.0f));

	// Initialize ImGui
	App::InitImgui();

	// Load in our model/texture resources
	Resources::GetInstance().LoadResources();

	// Create and set up camera
	Entity ent_camera = Entity::Create("Camera");
	//ent_camera.m_name = "Camera";
	CCamera& cam = ent_camera.Add<CCamera>(ent_camera);
	cam.Perspective(60.0f, 1.0f, 0.1f, 100.0f);
	ent_camera.transform.m_pos = glm::vec3(0.0f, 0.0f, 4.0f);

	// Creating duck entity
	Entity ent_ducky = Entity::Create("Ducky");
	//ent_ducky.m_name = "Ducky";
	ent_ducky.Add<CMeshRenderer>(ent_ducky, *Resources::GetInstance().Meshes["Duck"], *Resources::GetInstance().mat_ducky);
	ent_ducky.transform.m_scale = glm::vec3(0.005f, 0.005f, 0.005f);
	ent_ducky.transform.m_pos = glm::vec3(0.0f, -1.0f, 0.0f);
	ent_ducky.transform.m_rotation = glm::angleAxis(glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	ent_camera.Add<FPS_Controller>(ent_camera, cam);

	// Set up waypoint container
	std::vector<std::unique_ptr<Entity>> points;

	// Interpolation mode value
	static int interp_type = 0;

	App::Tick();

	// Main loop
	while (!App::IsClosing() && !Input::GetKeyDown(GLFW_KEY_ESCAPE))
	{
		App::FrameStart();

		// Update our LERP timers
		float deltaTime = App::GetDeltaTime();

		// Update camera
		ent_camera.Get<CCamera>().Update();		

		// Update duck transform
		ent_ducky.transform.RecomputeGlobal();

		// Draw duck
		ent_ducky.Get<CMeshRenderer>().Draw();		

		////////////////////////////////////////////////
		////////////////////////////////////////////////

		// Update FPS Controller
		ent_camera.Get<FPS_Controller>().UpdateMotion(deltaTime);
		ent_camera.Get<FPS_Controller>().UpdateCamera(deltaTime);

		App::StartImgui();
	
		Editor::GetInstance().Render();
		
		App::EndImgui();	

		// Draw everything we queued up to the screen
		App::SwapBuffers();
	}

	

	// Destroy window
	App::Cleanup();

	return 0;
}
