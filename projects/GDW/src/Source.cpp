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

#include "Editor/DevTools.h"
#include "Game/FPS_Controller.h"

using namespace nou;
using namespace OMG;
using namespace GAME;

std::unique_ptr<ShaderProgram> prog_texLit, prog_lit, prog_unlit;
std::unique_ptr<Mesh> mesh_ducky, mesh_box;
std::unique_ptr<Texture2D> tex2D_ducky;
std::unique_ptr<Material> mat_ducky, mat_unselected, mat_selected, mat_line;

// Keep our main cleaner
void LoadDefaultResources();

// Templated LERP function
template<typename T>
T Lerp(const T& p0, const T& p1, float t)
{
	return (1.0f - t) * p0 + t * p1;
}

// **DO NOT RENAME THE CATMULL OR BEZIER FUNCTIONS (things will break)**

// TODO: Templated Catmull-Rom function.
template<typename T>
T Catmull(const T& p0, const T& p1, const T& p2, const T& p3, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;

	T a = p1;
	T b = 0.5f * (-p0 + p2);
	T c = 0.5f * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3);
	T d = 0.5f * (-p0 + 3.0f * p1 - 3.0f * p2 + p3);

	return a + b * t + c * t2 + d * t3;
}

// TODO: Templated Bezier function
template<typename T>
T Bezier(const T& p0, const T& p1, const T& p2, const T& p3, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;
	float a2 = (1.0f - t) * (1.0f - t);
	float a3 = a2 * (1.0f - t);

	T a = a3 * p0;
	T b = 3 * t * a2 * p1;
	T c = 3 * t2 * (1 - t) * p2;
	T d = t3 * p3;

	return a + b + c + d;
}

int main()
{
	// Create window and set clear color
	App::Init("Game", 800, 600);
	App::SetClearColor(glm::vec4(0.0f, 0.27f, 0.4f, 1.0f));

	// Initialize ImGui
	App::InitImgui();

	// Load in our model/texture resources
	LoadDefaultResources();

	// Create and set up camera
	Entity ent_camera = Entity::Create("Camera");
	//ent_camera.m_name = "Camera";
	CCamera& cam = ent_camera.Add<CCamera>(ent_camera);
	cam.Perspective(60.0f, 1.0f, 0.1f, 100.0f);
	ent_camera.transform.m_pos = glm::vec3(0.0f, 0.0f, 4.0f);

	// Creating duck entity
	Entity ent_ducky = Entity::Create("Ducky");
	//ent_ducky.m_name = "Ducky";
	ent_ducky.Add<CMeshRenderer>(ent_ducky, *mesh_ducky, *mat_ducky);
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

		// Update transforms on all our points
		for (int i = 0; i < points.size(); i++)
		{
			points[i]->transform.RecomputeGlobal();
		}

		// Update duck transform
		ent_ducky.transform.RecomputeGlobal();

		// Draw our points
		for (int i = 0; i < points.size(); i++)
		{
			points[i]->Get<CMeshRenderer>().Draw();
		}

		// Draw duck
		ent_ducky.Get<CMeshRenderer>().Draw();


		////////////////////////////////////////////////
		////////////////////////////////////////////////

		// Update FPS Controller
		ent_camera.Get<FPS_Controller>().UpdateMotion(deltaTime);
		ent_camera.Get<FPS_Controller>().UpdateCamera(deltaTime);

		App::StartImgui();
	
		DevTools::GetInstance().Render();
		
		App::EndImgui();		

		// Draw everything we queued up to the screen
		App::SwapBuffers();
	}

	

	// Destroy window
	App::Cleanup();

	return 0;
}

void LoadDefaultResources()
{
	// Load in the shaders we will need and activate them
	// Textured lit shader
	std::unique_ptr vs_texLitShader = std::make_unique<Shader>("shaders/texturedlit.vert", GL_VERTEX_SHADER);
	std::unique_ptr fs_texLitShader = std::make_unique<Shader>("shaders/texturedlit.frag", GL_FRAGMENT_SHADER);
	std::vector<Shader*> texLit = { vs_texLitShader.get(), fs_texLitShader.get() };
	prog_texLit = std::make_unique<ShaderProgram>(texLit);

	// Untextured lit shader
	std::unique_ptr vs_litShader = std::make_unique<Shader>("shaders/lit.vert", GL_VERTEX_SHADER);
	std::unique_ptr fs_litShader = std::make_unique<Shader>("shaders/lit.frag", GL_FRAGMENT_SHADER);
	std::vector<Shader*> lit = { vs_litShader.get(), fs_litShader.get() };
	prog_lit = std::make_unique<ShaderProgram>(lit);

	// Untextured unlit shader
	std::unique_ptr vs_unlitShader = std::make_unique<Shader>("shaders/unlit.vert", GL_VERTEX_SHADER);
	std::unique_ptr fs_unlitShader = std::make_unique<Shader>("shaders/unlit.frag", GL_FRAGMENT_SHADER);
	std::vector<Shader*> unlit = { vs_unlitShader.get(), vs_unlitShader.get() };
	prog_unlit = std::make_unique<ShaderProgram>(unlit);

	// Set up duck
	mesh_ducky = std::make_unique<Mesh>();
	GLTF::LoadMesh("duck/Duck.gltf", *mesh_ducky);
	tex2D_ducky = std::make_unique<Texture2D>("duck/DuckCM.png");

	// Set up box
	mesh_box = std::make_unique<Mesh>();
	GLTF::LoadMesh("box/Box.gltf", *mesh_box);

	// Set up duck material
	mat_ducky = std::make_unique<Material>(*prog_texLit);
	mat_ducky->AddTexture("albedo", *tex2D_ducky);

	// Set up point and line materials
	mat_unselected = std::make_unique<Material>(*prog_lit);
	mat_unselected->m_color = glm::vec3(0.5f, 0.5f, 0.5f);

	mat_selected = std::make_unique<Material>(*prog_lit);
	mat_selected->m_color = glm::vec3(1.0f, 0.0f, 0.0f);

	mat_line = std::make_unique<Material>(*prog_unlit);
	mat_line->m_color = glm::vec3(1.0f, 1.0f, 1.0f);
}