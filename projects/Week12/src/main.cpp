#include <Logging.h>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <json.hpp>
#include <fstream>
#include <sstream>
#include <typeindex>
#include <optional>
#include <string>

// GLM math library
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/gtx/common.hpp> // for fmod (floating modulus)

// Graphics
#include "Graphics/IndexBuffer.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/VertexArrayObject.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture2D.h"
#include "Graphics/TextureCube.h"
#include "Graphics/VertexTypes.h"
#include "Graphics/Font.h"
#include "Graphics/GuiBatcher.h"

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

// Gameplay
#include "Gameplay/Material.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Gameplay/PlayerController.h"

// Components
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/Components/Camera.h"
#include "Gameplay/Components/RotatingBehaviour.h"
#include "Gameplay/Components/JumpBehaviour.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/MaterialSwapBehaviour.h"
#include "Gameplay/Components/HealthComponent.h"
#include "Gameplay/Components/InteractableComponent.h"
#include "Gameplay/Components/AnimatorComponent.h"
#include "Gameplay/Components/Enemy.h"

// Physics
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Physics/Colliders/PlaneCollider.h"
#include "Gameplay/Physics/Colliders/SphereCollider.h"
#include "Gameplay/Physics/Colliders/ConvexMeshCollider.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Graphics/DebugDraw.h"
#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Components/SimpleCameraControl.h"
#include "Gameplay/Physics/Colliders/CylinderCollider.h"

// GUI
#include "Gameplay/Components/GUI/RectTransform.h"
#include "Gameplay/Components/GUI/GuiPanel.h"
#include "Gameplay/Components/GUI/GuiText.h"

#include "Utils/UIHelper.h"

//#define LOG_GL_NOTIFICATIONS 

/*
	Handles debug messages from OpenGL
	https://www.khronos.org/opengl/wiki/Debug_Output#Message_Components
	@param source    Which part of OpenGL dispatched the message
	@param type      The type of message (ex: error, performance issues, deprecated behavior)
	@param id        The ID of the error or message (to distinguish between different types of errors, like nullref or index out of range)
	@param severity  The severity of the message (from High to Notification)
	@param length    The length of the message
	@param message   The human readable message from OpenGL
	@param userParam The pointer we set with glDebugMessageCallback (should be the game pointer)
*/



void GlDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::string sourceTxt;
	switch (source) {
		case GL_DEBUG_SOURCE_API: sourceTxt = "DEBUG"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceTxt = "WINDOW"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceTxt = "SHADER"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: sourceTxt = "THIRD PARTY"; break;
		case GL_DEBUG_SOURCE_APPLICATION: sourceTxt = "APP"; break;
		case GL_DEBUG_SOURCE_OTHER: default: sourceTxt = "OTHER"; break;
	}
	switch (severity) {
		case GL_DEBUG_SEVERITY_LOW:          LOG_INFO("[{}] {}", sourceTxt, message); break;
		case GL_DEBUG_SEVERITY_MEDIUM:       LOG_WARN("[{}] {}", sourceTxt, message); break;
		case GL_DEBUG_SEVERITY_HIGH:         LOG_ERROR("[{}] {}", sourceTxt, message); break;
			#ifdef LOG_GL_NOTIFICATIONS
		case GL_DEBUG_SEVERITY_NOTIFICATION: LOG_INFO("[{}] {}", sourceTxt, message); break;
			#endif
		default: break;
	}
}  

// Stores our GLFW window in a global variable for now
GLFWwindow* window;
// The current size of our window in pixels
glm::ivec2 windowSize = glm::ivec2(800, 800);
// The title of our GLFW window
std::string windowTitle = "Into the Abyss";


// using namespace should generally be avoided, and if used, make sure it's ONLY in cpp files
using namespace Gameplay;
using namespace Gameplay::Physics;

// The scene that we will be rendering
Scene::Sptr scene = Scene::currentScene = nullptr;

void GlfwWindowResizedCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	windowSize = glm::ivec2(width, height);
	if (windowSize.x * windowSize.y > 0) {
		scene->MainCamera->ResizeWindow(width, height);
	}
	GuiBatcher::SetWindowSize({ width, height });
}

/// <summary>
/// Handles intializing GLFW, should be called before initGLAD, but after Logger::Init()
/// Also handles creating the GLFW window
/// </summary>
/// <returns>True if GLFW was initialized, false if otherwise</returns>
bool initGLFW() {
	// Initialize GLFW
	if (glfwInit() == GLFW_FALSE) {
		LOG_ERROR("Failed to initialize GLFW");
		return false;
	}

	//Create a new GLFW window and make it current
	window = glfwCreateWindow(windowSize.x, windowSize.y, windowTitle.c_str(), nullptr, nullptr);
	glfwMakeContextCurrent(window);
	
	// Set our window resized callback
	glfwSetWindowSizeCallback(window, GlfwWindowResizedCallback);

	GuiBatcher::SetWindowSize(windowSize);

	return true;
}

/// <summary>
/// Handles initializing GLAD and preparing our GLFW window for OpenGL calls
/// </summary>
/// <returns>True if GLAD is loaded, false if there was an error</returns>
bool initGLAD() {
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		LOG_ERROR("Failed to initialize Glad");
		return false;
	}
	return true;
}

/// <summary>
/// Draws a widget for saving or loading our scene
/// </summary>
/// <param name="scene">Reference to scene pointer</param>
/// <param name="path">Reference to path string storage</param>
/// <returns>True if a new scene has been loaded</returns>
bool DrawSaveLoadImGui(Scene::Sptr& scene, std::string& path) {
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

bool DrawLightImGui(const Scene::Sptr& scene, const char* title, int ix) {
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

/// <summary>
/// handles creating or loading the scene
/// </summary>
void CreateScene() {
	bool loadScene = false;  
	// For now we can use a toggle to generate our scene vs load from file
	if (loadScene) {
		ResourceManager::LoadManifest("manifest.json");
		scene = Scene::Load("scene.json");

		// Call scene awake to start up all of our components
		scene->Window = window;
		scene->Awake();
	} 
	else {
		// This time we'll have 2 different shaders, and share data between both of them using the UBO
		// This shader will handle reflective materials 
		Shader::Sptr reflectiveShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_environment_reflective.glsl" }
		});

		// This shader handles our basic materials without reflections (cause they expensive)
		Shader::Sptr basicShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_blinn_phong_textured.glsl" }
		});

		// This shader handles our basic materials without reflections (cause they expensive)
		Shader::Sptr specShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/textured_specular.glsl" }
		});

		// This shader handles our foliage vertex shader example
		Shader::Sptr foliageShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/foliage.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/screendoor_transparency.glsl" }
		});

		// This shader handles our cel shading example
		Shader::Sptr toonShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/toon_shading.glsl" }
		});

		// This shader handles our displacement mapping example
		Shader::Sptr displacementShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/displacement_mapping.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_tangentspace_normal_maps.glsl" }
		});

		// This shader handles our displacement mapping example
		Shader::Sptr tangentSpaceMapping = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_tangentspace_normal_maps.glsl" }
		});

		// This shader handles our multitexturing example
		Shader::Sptr multiTextureShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/vert_multitextured.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_multitextured.glsl" }
		});

		// Load in the meshes
		MeshResource::Sptr characterMesh = ResourceManager::CreateAsset<MeshResource>("Character.obj");

		MeshResource::Sptr darkTreeMesh = ResourceManager::CreateAsset<MeshResource>("DarkPineTree.obj");
		MeshResource::Sptr lightTreeMesh = ResourceManager::CreateAsset<MeshResource>("LightPineTree.obj");
		MeshResource::Sptr graveStoneMesh = ResourceManager::CreateAsset<MeshResource>("GraveStone.obj");
		MeshResource::Sptr keymesh = ResourceManager::CreateAsset<MeshResource>("key.obj");
		MeshResource::Sptr rockMesh = ResourceManager::CreateAsset<MeshResource>("Rock.obj");
		MeshResource::Sptr stoneWallMesh = ResourceManager::CreateAsset<MeshResource>("StoneWall.obj");
		MeshResource::Sptr TurretProj = ResourceManager::CreateAsset<MeshResource>("TurretProjectile.obj");
		MeshResource::Sptr TorchMesh = ResourceManager::CreateAsset<MeshResource>("StandingTorch.obj");
		MeshResource::Sptr stoneSlabMesh = ResourceManager::CreateAsset<MeshResource>("stoneslabs.obj");
		MeshResource::Sptr EnemyMesh = ResourceManager::CreateAsset<MeshResource>("Enemy.obj");

		//Cages
		MeshResource::Sptr circleCageMesh = ResourceManager::CreateAsset<MeshResource>("CircleCage.obj");
		MeshResource::Sptr boxCageMesh = ResourceManager::CreateAsset<MeshResource>("BoxCage.obj");
		MeshResource::Sptr LongCageMesh = ResourceManager::CreateAsset<MeshResource>("CageLong.obj");
		MeshResource::Sptr sCageMesh = ResourceManager::CreateAsset<MeshResource>("SCage.obj");
		MeshResource::Sptr UCageMesh = ResourceManager::CreateAsset<MeshResource>("CageU.obj");

		//Symbols
		MeshResource::Sptr S1 = ResourceManager::CreateAsset<MeshResource>("Symbol1.obj");
		MeshResource::Sptr S2 = ResourceManager::CreateAsset<MeshResource>("Symbol2.obj");
		MeshResource::Sptr S3 = ResourceManager::CreateAsset<MeshResource>("Symbol3.obj");
		MeshResource::Sptr S4 = ResourceManager::CreateAsset<MeshResource>("Symbol4.obj");
		MeshResource::Sptr S5 = ResourceManager::CreateAsset<MeshResource>("Symbol5.obj");

		//Animated Props
		MeshResource::Sptr pressurePlateMesh = ResourceManager::CreateAsset<MeshResource>("PressurePlate.obj");
		MeshResource::Sptr turretMesh = ResourceManager::CreateAsset<MeshResource>("Turret.obj");
		MeshResource::Sptr doorMesh = ResourceManager::CreateAsset<MeshResource>("door.obj");
		MeshResource::Sptr spikeMesh = ResourceManager::CreateAsset<MeshResource>("SpikedTrap.obj");

		// Load in some textures
		Texture2D::Sptr    stoneWallTex = ResourceManager::CreateAsset<Texture2D>("textures/StoneWallUVs.png");
		Texture2D::Sptr    stoneSlabTex = ResourceManager::CreateAsset<Texture2D>("textures/StoneSlabUVs.png");
		Texture2D::Sptr    stoneSlabTex2 = ResourceManager::CreateAsset<Texture2D>("textures/StoneSlabsTexture.png");
		Texture2D::Sptr    darkTreeTex = ResourceManager::CreateAsset<Texture2D>("textures/DarkPineTreeUVS.png");
		Texture2D::Sptr    lightTreeTex = ResourceManager::CreateAsset<Texture2D>("textures/LightTreeTextureUVS.png");
		Texture2D::Sptr    tprojtex = ResourceManager::CreateAsset<Texture2D>("textures/TurretProjectileUVS.png");
		Texture2D::Sptr    keyTex = ResourceManager::CreateAsset<Texture2D>("textures/KeyTextureUVS.png");
		Texture2D::Sptr    rockTex = ResourceManager::CreateAsset<Texture2D>("textures/RockUVS.png");
		Texture2D::Sptr    torchTex = ResourceManager::CreateAsset<Texture2D>("textures/TorchUVS.png");
		Texture2D::Sptr    graveStoneTex = ResourceManager::CreateAsset<Texture2D>("textures/GraveStoneUVS.png");
		Texture2D::Sptr    enemyTex = ResourceManager::CreateAsset<Texture2D>("textures/EnemyUVS.png");
		Texture2D::Sptr    characterTex = ResourceManager::CreateAsset<Texture2D>("textures/CharacterUVS.png");
		Texture2D::Sptr    shadowTex = ResourceManager::CreateAsset<Texture2D>("textures/ShadowUVS.png");

		//Cages Textures
		Texture2D::Sptr    circleCageTex = ResourceManager::CreateAsset<Texture2D>("textures/CircleCageUVS.png");
		Texture2D::Sptr    boxCageTex = ResourceManager::CreateAsset<Texture2D>("textures/BoxcageUVS.png");
		Texture2D::Sptr    longCageTex = ResourceManager::CreateAsset<Texture2D>("textures/LongCageUVS.png");
		Texture2D::Sptr    smallCageTex = ResourceManager::CreateAsset<Texture2D>("textures/SmallCageUVS.png");
		Texture2D::Sptr    uCageTex = ResourceManager::CreateAsset<Texture2D>("textures/UCageUVS.png");

		//Animated Prop Textures
		Texture2D::Sptr    pressurePlateTex = ResourceManager::CreateAsset<Texture2D>("textures/PressurePlateUVs.png");
		Texture2D::Sptr    turretTex = ResourceManager::CreateAsset<Texture2D>("textures/TurretUVS.png");
		Texture2D::Sptr    doorTex = ResourceManager::CreateAsset<Texture2D>("textures/DoorUVs.png");
		Texture2D::Sptr    spikeTrapTex = ResourceManager::CreateAsset<Texture2D>("textures/SpikedTrapUVS.png");

		//Symbols
		Texture2D::Sptr    firstSymbolTex = ResourceManager::CreateAsset<Texture2D>("textures/StoneTablet1UVS.png");
		Texture2D::Sptr    secondSymbolTex = ResourceManager::CreateAsset<Texture2D>("textures/StoneTablet2UVS.png");
		Texture2D::Sptr    thirdSymbolTex = ResourceManager::CreateAsset<Texture2D>("textures/StoneTablet3UVS.png");
		Texture2D::Sptr    fourthSymbolTex = ResourceManager::CreateAsset<Texture2D>("textures/StoneTablet4UVS.png");
		Texture2D::Sptr    fifthSymbolTex = ResourceManager::CreateAsset<Texture2D>("textures/StoneTablet5UVS.png");

		//U.I Assets
		Texture2D::Sptr    menuGlossTex = ResourceManager::CreateAsset<Texture2D>("textures/MenuGloss.png");
		Texture2D::Sptr menuGlossReverseTex = ResourceManager::CreateAsset<Texture2D>("textures/MenuGlossReverse.png");

		// Create an empty scene
		scene = std::make_shared<Scene>();
		Scene::currentScene = scene;

		Material::Sptr stoneWallMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			stoneWallMaterial->Name = "Stone Wall";
			stoneWallMaterial->Set("u_Material.Diffuse", stoneWallTex);
			stoneWallMaterial->Set("u_Material.Shininess", 0.1f);
		}

		Material::Sptr pressurePlateMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			pressurePlateMaterial->Name = "Pressure Plate";
			pressurePlateMaterial->Set("u_Material.Diffuse", pressurePlateTex);
			pressurePlateMaterial->Set("u_Material.Shininess", 0.1f);
		}

		Material::Sptr stoneSlabMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			stoneSlabMaterial->Name = "Stone Slab";
			stoneSlabMaterial->Set("u_Material.Diffuse", stoneSlabTex2);
			stoneSlabMaterial->Set("u_Material.Shininess", 0.1f);
		}

		Material::Sptr sCageMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			sCageMaterial->Name = "Small Cage";
			sCageMaterial->Set("u_Material.Diffuse", smallCageTex);
			sCageMaterial->Set("u_Material.Shininess", 0.1f);
		}

		Material::Sptr TurretProjMat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			TurretProjMat->Name = "Turret Projectile";
			TurretProjMat->Set("u_Material.Diffuse", tprojtex);
			TurretProjMat->Set("u_Material.Shininess", 0.1f);
		}

		Material::Sptr turretMat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			turretMat->Name = "Turret";
			turretMat->Set("u_Material.Diffuse", turretTex);
			turretMat->Set("u_Material.Shininess", 0.1f);
		}

		Material::Sptr doorMat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			doorMat->Name = "Door";
			doorMat->Set("u_Material.Diffuse", doorTex);
			doorMat->Set("u_Material.Shininess", 0.1f);
		}	

		Material::Sptr characterMat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			characterMat->Name = "Character";
			characterMat->Set("u_Material.Diffuse", characterTex);
			characterMat->Set("u_Material.Shininess", 0.1f);
		}		

		//Symbol Materials
		Material::Sptr symbol1Mat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			symbol1Mat->Name = "Symbol 1";
			symbol1Mat->Set("u_Material.Diffuse", firstSymbolTex);
			symbol1Mat->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr symbol2Mat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			symbol2Mat->Name = "Symbol 2";
			symbol2Mat->Set("u_Material.Diffuse", secondSymbolTex);
			symbol2Mat->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr symbol3Mat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			symbol3Mat->Name = "Symbol 3";
			symbol3Mat->Set("u_Material.Diffuse", thirdSymbolTex);
			symbol3Mat->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr symbol4Mat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			symbol4Mat->Name = "Symbol 4";
			symbol4Mat->Set("u_Material.Diffuse", fourthSymbolTex);
			symbol4Mat->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr symbol5Mat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			symbol5Mat->Name = "Symbol 5";
			symbol5Mat->Set("u_Material.Diffuse", fifthSymbolTex);
			symbol5Mat->Set("u_Material.Shininess", 0.1f);
		}

		Material::Sptr shadowMat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			shadowMat->Name = "Shadow";
			shadowMat->Set("u_Material.Diffuse", shadowTex);
			shadowMat->Set("u_Material.Shininess", 0.1f);
		}
		//Tree Materials
		Material::Sptr darkTreeMat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			darkTreeMat->Name = "Dark Pine Tree";
			darkTreeMat->Set("u_Material.Diffuse", darkTreeTex);
			darkTreeMat->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr lightTreeMat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			lightTreeMat->Name = "Light Pine Tree";
			lightTreeMat->Set("u_Material.Diffuse", lightTreeTex);
			lightTreeMat->Set("u_Material.Shininess", 0.1f);
		}

		//Environment Object Materials
		Material::Sptr torchMat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			torchMat->Name = "Torch";
			torchMat->Set("u_Material.Diffuse", torchTex);
			torchMat->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr rockMat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			torchMat->Name = "Rock";
			torchMat->Set("u_Material.Diffuse", rockTex);
			torchMat->Set("u_Material.Shininess", 0.1f);
		}

		//Cage Materials
		Material::Sptr boxCageMat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			boxCageMat->Name = "Box Cage";
			boxCageMat->Set("u_Material.Diffuse", boxCageTex);
			boxCageMat->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr circleCageMat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			circleCageMat->Name = "Circle Cage";
			circleCageMat->Set("u_Material.Diffuse", circleCageTex);
			circleCageMat->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr longCageMat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			longCageMat->Name = "Long Cage";
			longCageMat->Set("u_Material.Diffuse", longCageTex);
			longCageMat->Set("u_Material.Shininess", 0.1f);
		}
		Material::Sptr uCageMat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			uCageMat->Name = "U Cage";
			uCageMat->Set("u_Material.Diffuse", uCageTex);
			uCageMat->Set("u_Material.Shininess", 0.1f);
		}

		Material::Sptr graveStoneMat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			graveStoneMat->Name = "Grave Stone";
			graveStoneMat->Set("u_Material.Diffuse", graveStoneTex);
			graveStoneMat->Set("u_Material.Shininess", 0.1f);
		}

		// Create some lights for our scene
		scene->Lights.resize(3);
		scene->Lights[0].Position = glm::vec3(0.0f, 12.0f, 13.0f);
		scene->Lights[0].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[0].Range = 200.0f;

		Material::Sptr enemyMat = ResourceManager::CreateAsset<Material>(basicShader);
		{
			enemyMat->Name = "Enemy";
			enemyMat->Set("u_Material.Diffuse", characterTex);
			enemyMat->Set("u_Material.Shininess", 0.1f);
		}

		MeshResource::Sptr planeMesh = ResourceManager::CreateAsset<MeshResource>();
		planeMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(1.0f)));
		planeMesh->GenerateMesh();

		std::vector<MeshResource::Sptr> WalkAnimation;
		for (int i = 0; i < 8; ++i)
		{
			std::string file;
			file.append("models/character_walk/Walk");
			file.append(std::to_string((i + 1)));
			file.append(".obj");
			WalkAnimation.push_back(ResourceManager::CreateAsset<MeshResource>(file));
		}

		std::vector<MeshResource::Sptr> IdleAnimation;
		for (int i = 0; i < 6; ++i)
		{
			std::string file;
			file.append("models/character_idle/Idle");
			file.append(std::to_string((i + 1)));
			file.append(".obj");
			IdleAnimation.push_back(ResourceManager::CreateAsset<MeshResource>(file));
		}
		

int PHYSICAL_MASK = 0xFFFFFFFF;
int PHYSICAL_GROUP = 0x01;
int SHADOW_MASK = 0xFFFFFFFE;
int SHADOW_GROUP = 0x02;
int NO_MASK = 0xFFFFFFFD;
int NO_GROUP = 0x03;

// Create some lights for our scene
scene->Lights.resize(8);
scene->Lights[0].Position = glm::vec3(0.0f, 12.0f, 13.0f);
scene->Lights[0].Color = glm::vec3(1.0f, 1.0f, 1.0f);
scene->Lights[0].Range = 100.0f;

		/////////////////////////////////////////////////////////
		//					PLAYER ELEMENTS
		/////////////////////////////////////////////////////////
		
		// Set up the scene's camera
		GameObject::Sptr camera = scene->CreateGameObject("Main Camera");
		{
			camera->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
			camera->SetRotation(glm::vec3(45.0f, 0.0f, 0.0f));
			//camera->LookAt(glm::vec3(0.0f));			

			Camera::Sptr cam = camera->Add<Camera>();
			cam->SetFovDegrees(60.0f);
			scene->MainCamera = cam;
		}
		
		GameObject::Sptr body = scene->CreateGameObject("Body");
		{
			// Set position in the scene
			body->SetPosition(glm::vec3(-4.0f, 17.0f, 0.0f));
			body->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			body->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

			RenderComponent::Sptr renderer = body->Add<RenderComponent>();
			renderer->SetMesh(characterMesh);
			renderer->SetMaterial(characterMat);

			AnimatorComponent::Sptr animator = body->Add<AnimatorComponent>();
			std::vector<MeshResource::Sptr> walkFrames;		
			animator->AddAnimation("Walk", WalkAnimation);			
			animator->AddAnimation("Idle", IdleAnimation);

			animator->SetRenderer(*renderer);
			animator->SetAnimation("Idle");
			//animator->SetFrames(std::move(frames));
			animator->SetLooping(true);
			animator->SetPause(false);
			animator->SetSpeed(10.0f);

			//std::cout << std::to_string(frames.size()) << std::endl;
			
			RigidBody::Sptr physics = body->Add<RigidBody>(RigidBodyType::Dynamic);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);
			physics->SetCollisionGroup(PHYSICAL_GROUP);
			physics->SetCollisionMask(PHYSICAL_MASK);

			TriggerVolume::Sptr volume = body->Add<TriggerVolume>();
			BoxCollider::Sptr i_collider = BoxCollider::Create();
			i_collider->SetPosition(i_collider->GetPosition() + glm::vec3(0.0f, 0.0f, -2.5f));
			volume->AddCollider(i_collider);
			TriggerVolumeEnterBehaviour::Sptr trigger = body->Add<TriggerVolumeEnterBehaviour>();

			body->Add<HealthComponent>(100.0f);
		}

		GameObject::Sptr shadow = scene->CreateGameObject("Shadow");
		{
			// Set position in the scene
			shadow->SetPosition(glm::vec3(4.0f, 17.0f, 0.0f));
			shadow->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			shadow->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

			// Create and attach a renderer for the monkey
			RenderComponent::Sptr renderer = shadow->Add<RenderComponent>();
			renderer->SetMesh(characterMesh);
			renderer->SetMaterial(shadowMat);

			AnimatorComponent::Sptr animator = shadow->Add<AnimatorComponent>();		
			animator->AddAnimation("Walk", WalkAnimation);
			animator->AddAnimation("Idle", IdleAnimation);

			animator->SetRenderer(*renderer);
			animator->SetAnimation("Idle");
			//animator->SetFrames(std::move(frames));
			animator->SetLooping(true);
			animator->SetPause(false);
			animator->SetSpeed(10.0f);

			// Add a dynamic rigid body to this monkey
			RigidBody::Sptr physics = shadow->Add<RigidBody>(RigidBodyType::Dynamic);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);
			physics->SetCollisionGroup(SHADOW_GROUP);
			physics->SetCollisionMask(SHADOW_MASK);


			shadow->Add<HealthComponent>(100.0f);
		}

		scene->PC.Initialize(*body, *shadow, *camera, scene->Lights[0], *body->Get<TriggerVolume>());

		/////////////////////////////////////////////////////////
		//			BASIC ENEMY - Steering Behavior
		/////////////////////////////////////////////////////////

		GameObject::Sptr nav1 = scene->CreateGameObject("Nav 1");
		{
			// Transform
			nav1->SetPosition(glm::vec3(-30.0f, -10.0f, 4));
			nav1->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			nav1->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

			// Renderer
			//RenderComponent::Sptr renderer = nav1->Add<RenderComponent>();
			//renderer->SetMesh(pressurePlateMesh);
			//renderer->SetMaterial(stoneWallMaterial);
		}

		GameObject::Sptr nav2 = scene->CreateGameObject("Nav 2");
		{
			// Transform
			nav2->SetPosition(glm::vec3(-30.0f, -20.0f, 4));
			nav2->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			nav2->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

			// Renderer
			//RenderComponent::Sptr renderer = nav2->Add<RenderComponent>();
			//renderer->SetMesh(pressurePlateMesh);
			//renderer->SetMaterial(stoneWallMaterial);
		}

		GameObject::Sptr nav3 = scene->CreateGameObject("Nav 3");
		{
			// Transform
			nav3->SetPosition(glm::vec3(-20.0f, -20.0f, 4));
			nav3->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			nav3->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

			// Renderer
			//RenderComponent::Sptr renderer = nav3->Add<RenderComponent>();
			//renderer->SetMesh(pressurePlateMesh);
			//renderer->SetMaterial(stoneWallMaterial);
		}

		GameObject::Sptr nav4 = scene->CreateGameObject("Nav 4");
		{
			// Transform
			nav4->SetPosition(glm::vec3(-20.0f, -10.0f, 4));
			nav4->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			nav4->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

			// Renderer
			//RenderComponent::Sptr renderer = nav4->Add<RenderComponent>();
			//renderer->SetMesh(pressurePlateMesh);
			//renderer->SetMaterial(stoneWallMaterial);
		}

		GameObject::Sptr enemy = scene->CreateGameObject("Enemy");
		{
			enemy->SetPosition(glm::vec3(-20.0f, -25.0f, 0.0f));
			enemy->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			enemy->SetScale(glm::vec3(1.0f, 1.0f, 1.0));

			// Create and attach a renderer for the monkey
			RenderComponent::Sptr renderer = enemy->Add<RenderComponent>();
			renderer->SetMesh(EnemyMesh);
			renderer->SetMaterial(enemyMat);	

			// Enemy Navigation
			Enemy::Sptr enemyScript = enemy->Add<Enemy>();
			enemyScript->SetOwner(enemy);
			std::vector<GameObject> navPts;
			navPts.push_back(*nav1);
			navPts.push_back(*nav2);
			navPts.push_back(*nav3);
			navPts.push_back(*nav4);
			enemyScript->SetNavPoints(navPts);
		}

		/////////////////////////////////////////////////////////
		//					CORE LEVEL
		/////////////////////////////////////////////////////////

		// Set up all our sample objects
		GameObject::Sptr floor = scene->CreateGameObject("Floor");
		{
			MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
			tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(100.0f), glm::vec2(20.0f)));
			tiledMesh->GenerateMesh();

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = floor->Add<RenderComponent>();
			renderer->SetMesh(tiledMesh);
			renderer->SetMaterial(stoneSlabMaterial);

			RigidBody::Sptr physics = floor->Add<RigidBody>(/*static by default*/);
			physics->AddCollider(BoxCollider::Create(glm::vec3(100.0f, 100.0f, 1.5f)))->SetPosition({ 0,0,-1 });
			physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
			physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
		}

		///////////////////    WALLS     ///////////////////////////
		{
			GameObject::Sptr wall1 = scene->CreateGameObject("Wall 1");
			{
				wall1->SetPosition(glm::vec3(0, 30.0f, 0.0f));
				wall1->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
				wall1->SetScale(glm::vec3(1.0f, 1.0f, 1.5f));

				RenderComponent::Sptr renderer = wall1->Add<RenderComponent>();
				renderer->SetMesh(stoneWallMesh);
				renderer->SetMaterial(stoneWallMaterial);

				RigidBody::Sptr physics = wall1->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(glm::vec3(3.0f, 4.0f, 24.0f));
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
			}

			GameObject::Sptr wall2 = scene->CreateGameObject("Wall 2");
			{
				wall2->SetPosition(glm::vec3(-24.0f, 35.0f, 0.0f));
				wall2->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall2->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

				RenderComponent::Sptr renderer = wall2->Add<RenderComponent>();
				renderer->SetMesh(stoneWallMesh);
				renderer->SetMaterial(stoneWallMaterial);

				RigidBody::Sptr physics = wall2->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(glm::vec3(3.0f, 4.0, 15.5));
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
			}

			GameObject::Sptr wall3 = scene->CreateGameObject("Wall 3");
			{
				wall3->SetPosition(glm::vec3(-24.0f, 8.0f, 0.0f));
				wall3->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall3->SetScale(glm::vec3(1.0f, 1.0f, 0.3f));

				RenderComponent::Sptr renderer = wall3->Add<RenderComponent>();
				renderer->SetMesh(stoneWallMesh);
				renderer->SetMaterial(stoneWallMaterial);

				RigidBody::Sptr physics = wall3->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(glm::vec3(3, 4, 4.6));
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
			}

			GameObject::Sptr wall4 = scene->CreateGameObject("Wall 4");
			{
				wall4->SetPosition(glm::vec3(-28.0f, 2.0f, 0.0f));
				wall4->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
				wall4->SetScale(glm::vec3(1.0f, 1.0f, 1.5f));

				RenderComponent::Sptr renderer = wall4->Add<RenderComponent>();
				renderer->SetMesh(stoneWallMesh);
				renderer->SetMaterial(stoneWallMaterial);

				RigidBody::Sptr physics = wall4->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(glm::vec3(3.0f, 4.0f, 24.0f));
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
			}

			GameObject::Sptr wall5 = scene->CreateGameObject("Wall 5");
			{
				wall5->SetPosition(glm::vec3(28.0f, 2.0f, 0.0f));
				wall5->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
				wall5->SetScale(glm::vec3(1.0f, 1.0f, 1.5f));

				RenderComponent::Sptr renderer = wall5->Add<RenderComponent>();
				renderer->SetMesh(stoneWallMesh);
				renderer->SetMaterial(stoneWallMaterial);

				RigidBody::Sptr physics = wall5->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(glm::vec3(3.0f, 4.0f, 24.0f));
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
			}

			GameObject::Sptr wall6 = scene->CreateGameObject("Wall 6");
			{
				wall6->SetPosition(glm::vec3(24.0f, 8.0f, 0.0f));
				wall6->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall6->SetScale(glm::vec3(1.0f, 1.0f, 0.3f));

				RenderComponent::Sptr renderer = wall6->Add<RenderComponent>();
				renderer->SetMesh(stoneWallMesh);
				renderer->SetMaterial(stoneWallMaterial);

				RigidBody::Sptr physics = wall6->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(glm::vec3(3, 4, 4.6));
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
			}

			GameObject::Sptr wall7 = scene->CreateGameObject("Wall 7");
			{
				wall7->SetPosition(glm::vec3(24.0f, 35.0f, 0.0f));
				wall7->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall7->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

				RenderComponent::Sptr renderer = wall7->Add<RenderComponent>();
				renderer->SetMesh(stoneWallMesh);
				renderer->SetMaterial(stoneWallMaterial);

				RigidBody::Sptr physics = wall7->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(glm::vec3(3.0, 4, 15.5));
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
			}

			GameObject::Sptr wall8 = scene->CreateGameObject("Wall 8");
			{
				wall8->SetPosition(glm::vec3(-50, 26.5f, 0.0f));
				wall8->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
				wall8->SetScale(glm::vec3(1.0f, 1.0f, 1.55f));

				RenderComponent::Sptr renderer = wall8->Add<RenderComponent>();
				renderer->SetMesh(stoneWallMesh);
				renderer->SetMaterial(stoneWallMaterial);

				RigidBody::Sptr physics = wall8->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(glm::vec3(3.0f, 4.0f, 24.0f));
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
			}

			GameObject::Sptr wall9 = scene->CreateGameObject("Wall 9");
			{
				wall9->SetPosition(glm::vec3(-37, 48.5f, 0.0f));
				wall9->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
				wall9->SetScale(glm::vec3(1.0f, 1.0f, 0.75f));

				RenderComponent::Sptr renderer = wall9->Add<RenderComponent>();
				renderer->SetMesh(stoneWallMesh);
				renderer->SetMaterial(stoneWallMaterial);

				RigidBody::Sptr physics = wall9->Add<RigidBody>(RigidBodyType::Static);
				BoxCollider::Sptr collider = BoxCollider::Create();
				collider->SetScale(glm::vec3(3.0f, 4.0f, 15.0f));
				collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
				physics->AddCollider(collider);
				physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
			}

GameObject::Sptr wall10 = scene->CreateGameObject("Wall 10");
{
	wall10->SetPosition(glm::vec3(50, -15.0f, 0.0f));
	wall10->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
	wall10->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

	RenderComponent::Sptr renderer = wall10->Add<RenderComponent>();
	renderer->SetMesh(stoneWallMesh);
	renderer->SetMaterial(stoneWallMaterial);

	RigidBody::Sptr physics = wall10->Add<RigidBody>(RigidBodyType::Static);
	BoxCollider::Sptr collider = BoxCollider::Create();
	collider->SetScale(glm::vec3(3.0, 4, 15.5));
	collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
	physics->AddCollider(collider);
	physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
	physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
}

GameObject::Sptr wall11 = scene->CreateGameObject("Wall 11");
{
	wall11->SetPosition(glm::vec3(37.0f, 48.5f, 0.0f));
	wall11->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
	wall11->SetScale(glm::vec3(1.0f, 1.0f, 0.75f));

	RenderComponent::Sptr renderer = wall11->Add<RenderComponent>();
	renderer->SetMesh(stoneWallMesh);
	renderer->SetMaterial(stoneWallMaterial);

	RigidBody::Sptr physics = wall11->Add<RigidBody>(RigidBodyType::Static);
	BoxCollider::Sptr collider = BoxCollider::Create();
	collider->SetScale(glm::vec3(3.0, 4, 15));
	collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
	physics->AddCollider(collider);
	physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
	physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
}

GameObject::Sptr wall12 = scene->CreateGameObject("Wall 12");
{
	wall12->SetPosition(glm::vec3(-50.0f, -15.0f, 0.0f));
	wall12->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
	wall12->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

	RenderComponent::Sptr renderer = wall12->Add<RenderComponent>();
	renderer->SetMesh(stoneWallMesh);
	renderer->SetMaterial(stoneWallMaterial);

	RigidBody::Sptr physics = wall12->Add<RigidBody>(RigidBodyType::Static);
	BoxCollider::Sptr collider = BoxCollider::Create();
	collider->SetScale(glm::vec3(3.0, 4, 15.5));
	collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
	physics->AddCollider(collider);
	physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
	physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
}

GameObject::Sptr wall13 = scene->CreateGameObject("Wall 13");
{
	wall13->SetPosition(glm::vec3(50.0f, 26.5f, 0.0f));
	wall13->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
	wall13->SetScale(glm::vec3(1.0f, 1.0f, 1.55f));

	RenderComponent::Sptr renderer = wall13->Add<RenderComponent>();
	renderer->SetMesh(stoneWallMesh);
	renderer->SetMaterial(stoneWallMaterial);

	RigidBody::Sptr physics = wall13->Add<RigidBody>(RigidBodyType::Static);
	BoxCollider::Sptr collider = BoxCollider::Create();
	collider->SetScale(glm::vec3(3.0f, 4.0f, 24.0f));
	collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
	physics->AddCollider(collider);
	physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
	physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
}

GameObject::Sptr wall14 = scene->CreateGameObject("Wall 14");
{
	wall14->SetPosition(glm::vec3(0, -31.5f, 0.0f));
	wall14->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
	wall14->SetScale(glm::vec3(1.0f, 1.0f, 3.35f));

	RenderComponent::Sptr renderer = wall14->Add<RenderComponent>();
	renderer->SetMesh(stoneWallMesh);
	renderer->SetMaterial(stoneWallMaterial);

	RigidBody::Sptr physics = wall14->Add<RigidBody>(RigidBodyType::Static);
	BoxCollider::Sptr collider = BoxCollider::Create();
	collider->SetScale(glm::vec3(3.0f, 4.0f, 50.0f));
	collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 4.0f, 0.0f));
	physics->AddCollider(collider);
	physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
	physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
}
		}
		/////////////////// END OF WALLS ///////////////////////////

		///////////////////    Lights    ///////////////////////////		

		GameObject::Sptr torch1 = scene->CreateGameObject("Torch 1");
		{
			torch1->SetPosition(glm::vec3(0.0f, 15.0f, 0.0f));
			torch1->SetRotation(glm::vec3(90, 0, 90));
			torch1->SetScale(glm::vec3(1.0f, 0.3f, 1.0f));

			RenderComponent::Sptr renderer = torch1->Add<RenderComponent>();
			renderer->SetMesh(TorchMesh);
			renderer->SetMaterial(pressurePlateMaterial);

			scene->Lights[1].Position = torch1->GetPosition() + glm::vec3(0.0f, 0.0f, 10.5f);
			scene->Lights[1].Color = glm::vec3(0.89f, 0.345f, 0.13f);
			scene->Lights[1].Range = 100.0f;
		}

		GameObject::Sptr torch2 = scene->CreateGameObject("Torch 2");
		{
			torch2->SetPosition(glm::vec3(-25.0f, -15.0f, 0.0f));
			torch2->SetRotation(glm::vec3(90, 0, 90));
			torch2->SetScale(glm::vec3(1.0f, 0.3f, 1.0f));

			RenderComponent::Sptr renderer = torch2->Add<RenderComponent>();
			renderer->SetMesh(TorchMesh);
			renderer->SetMaterial(pressurePlateMaterial);

			scene->Lights[2].Position = torch2->GetPosition() + glm::vec3(0.0f, 0.0f, 10.5f);
			scene->Lights[2].Color = glm::vec3(0.89f, 0.345f, 0.13f);
			scene->Lights[2].Range = 100.0f;

		}

		GameObject::Sptr torch3 = scene->CreateGameObject("Torch 3");
		{
			torch3->SetPosition(glm::vec3(35.0f, -15.0f, 0.0f));
			torch3->SetRotation(glm::vec3(90, 0, 90));
			torch3->SetScale(glm::vec3(1.0f, 0.3f, 1.0f));

			RenderComponent::Sptr renderer = torch3->Add<RenderComponent>();
			renderer->SetMesh(TorchMesh);
			renderer->SetMaterial(pressurePlateMaterial);

			scene->Lights[3].Position = torch3->GetPosition() + glm::vec3(0.0f, 0.0f, 10.5f);
			scene->Lights[3].Color = glm::vec3(0.89f, 0.345f, 0.13f);
			scene->Lights[3].Range = 100.0f;

		}

		GameObject::Sptr torch4 = scene->CreateGameObject("Torch 4");
		{
			torch4->SetPosition(glm::vec3(-46.5f, 15.0f, 0.0f));
			torch4->SetRotation(glm::vec3(90, 0, 90));
			torch4->SetScale(glm::vec3(1.0f, 0.3f, 1.0f));

			RenderComponent::Sptr renderer = torch4->Add<RenderComponent>();
			renderer->SetMesh(TorchMesh);
			renderer->SetMaterial(pressurePlateMaterial);

			scene->Lights[4].Position = torch4->GetPosition() + glm::vec3(0.0f, 0.0f, 10.5f);
			scene->Lights[4].Color = glm::vec3(0.89f, 0.345f, 0.13f);
			scene->Lights[4].Range = 100.0f;

		}

		GameObject::Sptr torch5 = scene->CreateGameObject("Torch 5");
		{
			torch5->SetPosition(glm::vec3(-46.5f, 35.0f, 0.0f));
			torch5->SetRotation(glm::vec3(90, 0, 90));
			torch5->SetScale(glm::vec3(1.0f, 0.3f, 1.0f));

			RenderComponent::Sptr renderer = torch5->Add<RenderComponent>();
			renderer->SetMesh(TorchMesh);
			renderer->SetMaterial(pressurePlateMaterial);

			scene->Lights[5].Position = torch5->GetPosition() + glm::vec3(0.0f, 0.0f, 10.5f);
			scene->Lights[5].Color = glm::vec3(0.89f, 0.345f, 0.13f);
			scene->Lights[5].Range = 100.0f;

		}

		GameObject::Sptr torch6 = scene->CreateGameObject("Torch 6");
		{
			torch6->SetPosition(glm::vec3(46.5f, 15.0f, 0.0f));
			torch6->SetRotation(glm::vec3(90, 0, 90));
			torch6->SetScale(glm::vec3(1.0f, 0.3f, 1.0f));

			RenderComponent::Sptr renderer = torch6->Add<RenderComponent>();
			renderer->SetMesh(TorchMesh);
			renderer->SetMaterial(pressurePlateMaterial);

			scene->Lights[6].Position = torch6->GetPosition() + glm::vec3(0.0f, 0.0f, 10.5f);
			scene->Lights[6].Color = glm::vec3(0.89f, 0.345f, 0.13f);
			scene->Lights[6].Range = 100.0f;

		}

		GameObject::Sptr torch7 = scene->CreateGameObject("Torch 7");
		{
			torch7->SetPosition(glm::vec3(46.5f, 35.0f, 0.0f));
			torch7->SetRotation(glm::vec3(90, 0, 90));
			torch7->SetScale(glm::vec3(1.0f, 0.3f, 1.0f));

			RenderComponent::Sptr renderer = torch7->Add<RenderComponent>();
			renderer->SetMesh(TorchMesh);
			renderer->SetMaterial(pressurePlateMaterial);

			scene->Lights[7].Position = torch7->GetPosition() + glm::vec3(0.0f, 0.0f, 10.5f);
			scene->Lights[7].Color = glm::vec3(0.89f, 0.345f, 0.13f);
			scene->Lights[7].Range = 100.0f;

		}

		/////////////////// END OF Lights ///////////////////////////
		// 
		// 
		/////////////////////////////////////////////////////////
		//					ENVIRONMENT
		/////////////////////////////////////////////////////////		

		

		GameObject::Sptr sCage = scene->CreateGameObject("S_Cage");
		{
			sCage->SetPosition(glm::vec3(0, 2, 0));
			sCage->SetRotation(glm::vec3(90, 0, 90));
			sCage->SetScale(glm::vec3(0.25f, 0.125f, 0.125f));

			RenderComponent::Sptr renderer = sCage->Add<RenderComponent>();
			renderer->SetMesh(sCageMesh);
			renderer->SetMaterial(sCageMaterial);

			RigidBody::Sptr physics = sCage->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetScale(glm::vec3(2.0f, 5.0f, 4.2f));
			collider->SetPosition(collider->GetPosition() + glm::vec3(0, 3.25f, 0));
			physics->AddCollider(collider);
		}		

		GameObject::Sptr interact_doorway = scene->CreateGameObject("Interact Door");
		{
			interact_doorway->SetPosition(glm::vec3(-24.0f, 16.0f, 2.0f));
			interact_doorway->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			interact_doorway->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

			RenderComponent::Sptr renderer = interact_doorway->Add<RenderComponent>();
			renderer->SetMesh(doorMesh);
			renderer->SetMaterial(doorMat);

			AnimatorComponent::Sptr animator = interact_doorway->Add<AnimatorComponent>();
			std::vector<MeshResource::Sptr> frames;
			for (int i = 0; i < 4; ++i)
			{
				std::string file;
				file.append("models/door/door");
				file.append(std::to_string((i + 1)));
				file.append(".obj");
				frames.push_back(ResourceManager::CreateAsset<MeshResource>(file));
			}
			animator->AddAnimation("Open", frames);

			animator->SetRenderer(*renderer);
			animator->SetAnimation("Open");
			animator->SetLooping(false);
			animator->SetPause(true);
			animator->onAnimationCompleted = [animator] {
				animator->SetPause(true);
				animator->SetReverse(!animator->IsReversed());
			};

			RigidBody::Sptr physics = interact_doorway->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetScale(glm::vec3(1.5f, 3.8f, 4.3f));
			collider->SetPosition(collider->GetPosition() + glm::vec3(0, 0, 0));
			physics->AddCollider(collider);
			physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
			physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
		}

		GameObject::Sptr interact = scene->CreateGameObject("Lever PlaceHolder");
		{
			interact->SetPosition(glm::vec3(-18.0f, 22.0f, 0.0f));
			interact->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			interact->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

			// Create and attach a renderer for the monkey
			RenderComponent::Sptr renderer = interact->Add<RenderComponent>();
			renderer->SetMesh(TurretProj);
			renderer->SetMaterial(TurretProjMat);

			// Add a dynamic rigid body to this monkey
			RigidBody::Sptr physics = interact->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);


			// Add Interactable Component
			InteractableComponent::Sptr interactable = interact->Add<InteractableComponent>();
			interactable->onInteractionEvent = [interact_doorway, PHYSICAL_GROUP, PHYSICAL_MASK, SHADOW_GROUP, SHADOW_MASK, NO_GROUP, NO_MASK] {
				//bool isEnabled = interact_doorway->Get<RenderComponent>()->IsEnabled;
				//interact_doorway->Get<RenderComponent>()->IsEnabled = !isEnabled;
				//interact_doorway->Get<RigidBody>()->IsEnabled = !isEnabled;

				AnimatorComponent::Sptr anim = interact_doorway->Get<AnimatorComponent>();
				RigidBody::Sptr rigid = interact_doorway->Get<RigidBody>();

				if (anim->IsPaused() && anim->IsReversed()) {
					rigid->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
					rigid->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);					
					anim->SetPause(false);
				}
				else if (anim->IsPaused() && !anim->IsReversed()) {
					rigid->SetCollisionGroup(NO_GROUP);
					rigid->SetCollisionMask(NO_MASK);					
					anim->SetPause(false);
				}
			};
		}

		GameObject::Sptr pressure_door = scene->CreateGameObject("Pressure Door");
		{
			pressure_door->SetPosition(glm::vec3(24.0f, 16.0f, 2.5f));
			pressure_door->SetRotation(glm::vec3(90.0f, 0.0f, 180.0f));
			pressure_door->SetScale(glm::vec3(1.0f, 0.5f, 0.65f));

			// Create and attach a renderer for the monkey
			RenderComponent::Sptr renderer = pressure_door->Add<RenderComponent>();
			renderer->SetMesh(doorMesh);
			renderer->SetMaterial(doorMat);

			AnimatorComponent::Sptr animator = pressure_door->Add<AnimatorComponent>();
			std::vector<MeshResource::Sptr> frames;
			for (int i = 0; i < 4; ++i)
			{
				std::string file;
				file.append("models/door/door");
				file.append(std::to_string((i + 1)));
				file.append(".obj");
				frames.push_back(ResourceManager::CreateAsset<MeshResource>(file));
			}
			animator->AddAnimation("Open", frames);

			animator->SetRenderer(*renderer);
			animator->SetAnimation("Open");
			animator->SetLooping(false);
			animator->SetPause(true);
			

			// Add a dynamic rigid body to this monkey
			RigidBody::Sptr physics = pressure_door->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 2.0f, 0.0f));
			collider->SetScale(glm::vec3(1.0f, 4.0f, 3.5f));			
			physics->AddCollider(collider);
			physics->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
			physics->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
		}

		// Cody :: About Trigger Volumes, I modified them with three types of event triggers.
		// onTriggerEnterEvent occurs when our character touches the TriggerVolume's collider.
		// onTriggerExitEvent occurs when our character leaves the TriggerVolume's collider.
		// onTriggerStayEvent occurs periodically while our character remains inside the TriggerVolume's collider. 

		GameObject::Sptr pressure_plate = scene->CreateGameObject("Pressure Plate");
		{
			pressure_plate->SetPosition(glm::vec3(15.0f, 16.0f, 0.0f));
			pressure_plate->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			pressure_plate->SetScale(glm::vec3(1.25f, 0.5f, 1.25f));

			// Create and attach a renderer for the monkey
			RenderComponent::Sptr renderer = pressure_plate->Add<RenderComponent>();
			renderer->SetMesh(pressurePlateMesh);
			renderer->SetMaterial(pressurePlateMaterial);

			AnimatorComponent::Sptr animator = pressure_plate->Add<AnimatorComponent>();
			std::vector<MeshResource::Sptr> frames;
			for (int i = 0; i < 2; ++i)
			{
				std::string file;
				file.append("models/pressurePlate/PressurePlate");
				file.append(std::to_string((i + 1)));
				file.append(".obj");
				frames.push_back(ResourceManager::CreateAsset<MeshResource>(file));
			}
			animator->AddAnimation("Activate", frames);

			animator->SetRenderer(*renderer);
			animator->SetAnimation("Activate");
			animator->SetLooping(false);
			animator->SetPause(true);
			animator->onAnimationCompleted = [animator] {
				animator->SetPause(true);
			};

			// Add a dynamic rigid body to this monkey
			TriggerVolume::Sptr volume = pressure_plate->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 2.5f, 0.0f));
			collider->SetScale(glm::vec3(2.0f, 1.0f, 2.0f));
			volume->AddCollider(collider);
			volume->SetCollisionGroup(PHYSICAL_GROUP);
			volume->SetCollisionMask(PHYSICAL_MASK);

			TriggerVolumeEnterBehaviour::Sptr trigger = pressure_plate->Add<TriggerVolumeEnterBehaviour>();
			trigger->onTriggerEnterEvent = [pressure_door, animator, NO_GROUP, NO_MASK] {
				AnimatorComponent::Sptr anim = pressure_door->Get<AnimatorComponent>();
				RigidBody::Sptr rigid = pressure_door->Get<RigidBody>();

				animator->SetReverse(false);
				animator->SetPause(false);

				// Open Door
				rigid->SetCollisionGroup(NO_GROUP);
				rigid->SetCollisionMask(NO_MASK);				
				anim->SetReverse(false);
				anim->SetPause(false);
			};

			trigger->onTriggerExitEvent = [pressure_door, animator, PHYSICAL_GROUP, SHADOW_GROUP, PHYSICAL_MASK, SHADOW_MASK] {
				AnimatorComponent::Sptr anim = pressure_door->Get<AnimatorComponent>();
				RigidBody::Sptr rigid = pressure_door->Get<RigidBody>();

				animator->SetReverse(true);
				animator->SetPause(false);

				// Close Door
				rigid->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				rigid->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
				anim->SetReverse(true);
				anim->SetPause(false);
			};
			
			// Cody :: To create a trigger event, start with square brackets '[]'.  If you want to pass in any references, put them inside the square brackets '[body]'
			// Then you can open it like a function with braces '{  }'.  That's because we just created a function with no name.  This works because we bind this created function
			// to the triggerEvent inside the TriggerVolume class.
		}

		GameObject::Sptr pressure_plate2 = scene->CreateGameObject("Pressure Plate 2");
		{
			pressure_plate2->SetPosition(glm::vec3(35.0f, 16.0f, 0.0f));
			pressure_plate2->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			pressure_plate2->SetScale(glm::vec3(1.25f, 0.5f, 1.25f));

			// Create and attach a renderer for the monkey
			RenderComponent::Sptr renderer = pressure_plate2->Add<RenderComponent>();
			renderer->SetMesh(pressurePlateMesh);
			renderer->SetMaterial(pressurePlateMaterial);

			AnimatorComponent::Sptr animator = pressure_plate2->Add<AnimatorComponent>();
			std::vector<MeshResource::Sptr> frames;
			for (int i = 0; i < 2; ++i)
			{
				std::string file;
				file.append("models/pressurePlate/PressurePlate");
				file.append(std::to_string((i + 1)));
				file.append(".obj");
				frames.push_back(ResourceManager::CreateAsset<MeshResource>(file));
			}
			animator->AddAnimation("Activate", frames);

			animator->SetRenderer(*renderer);
			animator->SetAnimation("Activate");
			animator->SetLooping(false);
			animator->SetPause(true);
			animator->onAnimationCompleted = [animator] {
				animator->SetPause(true);
			};

			// Add a dynamic rigid body to this monkey
			TriggerVolume::Sptr volume = pressure_plate2->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 2.5f, 0.0f));
			collider->SetScale(glm::vec3(2.0f, 1.0f, 2.0f));
			volume->AddCollider(collider);
			volume->SetCollisionGroup(PHYSICAL_GROUP);
			volume->SetCollisionMask(PHYSICAL_MASK);

			TriggerVolumeEnterBehaviour::Sptr trigger = pressure_plate2->Add<TriggerVolumeEnterBehaviour>();
			trigger->onTriggerEnterEvent = [pressure_door, animator, NO_GROUP, NO_MASK] {
				AnimatorComponent::Sptr anim = pressure_door->Get<AnimatorComponent>();
				RigidBody::Sptr rigid = pressure_door->Get<RigidBody>();

				animator->SetReverse(false);
				animator->SetPause(false);

				// Open Door
				rigid->SetCollisionGroup(NO_GROUP);
				rigid->SetCollisionMask(NO_MASK);
				anim->SetReverse(false);
				anim->SetPause(false);
			};

			trigger->onTriggerExitEvent = [pressure_door, animator, PHYSICAL_GROUP, SHADOW_GROUP, PHYSICAL_MASK, SHADOW_MASK] {
				AnimatorComponent::Sptr anim = pressure_door->Get<AnimatorComponent>();
				RigidBody::Sptr rigid = pressure_door->Get<RigidBody>();

				animator->SetReverse(true);
				animator->SetPause(false);

				// Close Door
				rigid->SetCollisionGroupMulti(PHYSICAL_GROUP | SHADOW_GROUP);
				rigid->SetCollisionMask(PHYSICAL_MASK | SHADOW_MASK);
				anim->SetReverse(true);
				anim->SetPause(false);
			};

			// Cody :: To create a trigger event, start with square brackets '[]'.  If you want to pass in any references, put them inside the square brackets '[body]'
			// Then you can open it like a function with braces '{  }'.  That's because we just created a function with no name.  This works because we bind this created function
			// to the triggerEvent inside the TriggerVolume class.
		}

		GameObject::Sptr spike_trap = scene->CreateGameObject("Spike Trap");
		{
			spike_trap->SetPosition(glm::vec3(-6.0f, 25.0f, 0.0f));
			spike_trap->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			spike_trap->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

			// Create and attach a renderer for the monkey
			RenderComponent::Sptr renderer = spike_trap->Add<RenderComponent>();
			renderer->SetMesh(pressurePlateMesh);
			renderer->SetMaterial(pressurePlateMaterial);

			AnimatorComponent::Sptr animator = spike_trap->Add<AnimatorComponent>();
			std::vector<MeshResource::Sptr> frames;
			for (int i = 0; i < 3; ++i)
			{
				std::string file;
				file.append("models/spikedTrap/SpikedTrap");
				file.append(std::to_string((i + 1)));
				file.append(".obj");
				frames.push_back(ResourceManager::CreateAsset<MeshResource>(file));
			}
			animator->AddAnimation("Activate", frames);

			animator->SetRenderer(*renderer);
			animator->SetAnimation("Activate");
			animator->SetLooping(false);
			animator->SetPause(true);
			animator->onAnimationCompleted = [animator] {
												
				if (animator->IsReversed()) {
					animator->SetPause(true);
				}

				animator->SetReverse(!animator->IsReversed());
			};

			// Add a dynamic rigid body to this monkey
			TriggerVolume::Sptr volume = spike_trap->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 2.5f, 0.0f));
			volume->AddCollider(collider);
			volume->SetCollisionGroup(PHYSICAL_GROUP);
			volume->SetCollisionMask(PHYSICAL_MASK);

			TriggerVolumeEnterBehaviour::Sptr trigger = spike_trap->Add<TriggerVolumeEnterBehaviour>();
			trigger->onTriggerEnterEvent = [body, animator] {
				if (!scene->PC.isShadow) {
					body->Get<HealthComponent>()->DealDamage(10.0f);
				}

				animator->SetPause(false);
			};
		}

		GameObject::Sptr light_trap = scene->CreateGameObject("Light Trap");
		{
			light_trap->SetPosition(glm::vec3(0.0f, 25.0f, 0.0f));
			light_trap->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			light_trap->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

			// Create and attach a renderer for the monkey
			RenderComponent::Sptr renderer = light_trap->Add<RenderComponent>();
			renderer->SetMesh(pressurePlateMesh);
			renderer->SetMaterial(pressurePlateMaterial);

			// Add a dynamic rigid body to this monkey
			TriggerVolume::Sptr volume = light_trap->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 2.5f, 0.0f));
			volume->AddCollider(collider);			
			volume->SetCollisionGroup(SHADOW_GROUP);
			volume->SetCollisionMask(SHADOW_MASK);

			TriggerVolumeEnterBehaviour::Sptr trigger = light_trap->Add<TriggerVolumeEnterBehaviour>();
			
			trigger->onTriggerStayEvent = [shadow] {
				if (scene->PC.isShadow) {
					shadow->Get<HealthComponent>()->DealDamage(0.01f);
				}
			};
		}

		GameObject::Sptr gas_trap = scene->CreateGameObject("Gas Trap");
		{
			gas_trap->SetPosition(glm::vec3(6.0f, 25.0f, 0.0f));
			gas_trap->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			gas_trap->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

			// Create and attach a renderer for the monkey
			RenderComponent::Sptr renderer = gas_trap->Add<RenderComponent>();
			renderer->SetMesh(pressurePlateMesh);
			renderer->SetMaterial(pressurePlateMaterial);

			AnimatorComponent::Sptr animator = gas_trap->Add<AnimatorComponent>();
			std::vector<MeshResource::Sptr> frames;
			for (int i = 0; i < 2; ++i)
			{
				std::string file;
				file.append("models/pressurePlate/PressurePlate");
				file.append(std::to_string((i + 1)));
				file.append(".obj");
				frames.push_back(ResourceManager::CreateAsset<MeshResource>(file));
			}
			animator->AddAnimation("Activate", frames);

			animator->SetRenderer(*renderer);			
			animator->SetAnimation("Activate");
			animator->SetLooping(false);
			animator->SetPause(true);
			animator->onAnimationCompleted = [animator] {
				animator->SetPause(true);
			};

			// Add a dynamic rigid body to this monkey
			TriggerVolume::Sptr volume = gas_trap->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create();
			collider->SetPosition(collider->GetPosition() + glm::vec3(0.0f, 2.5f, 0.0f));
			volume->AddCollider(collider);
			volume->SetCollisionGroup(PHYSICAL_GROUP);
			volume->SetCollisionMask(PHYSICAL_MASK);

			TriggerVolumeEnterBehaviour::Sptr trigger = gas_trap->Add<TriggerVolumeEnterBehaviour>();
			
			trigger->onTriggerEnterEvent = [animator] {
				animator->SetReverse(false);
				animator->SetPause(false);
			};

			trigger->onTriggerStayEvent = [body, animator] {
				if (!scene->PC.isShadow) {
					body->Get<HealthComponent>()->DealDamage(0.01f);
				}
			};

			trigger->onTriggerExitEvent = [animator] {
				animator->SetReverse(true);
				animator->SetPause(false);
			};

		}

		/////////////////////////////////////////////////////////
		//					ENVIRONMENT - DECORATIONS
		/////////////////////////////////////////////////////////
		//Cages

		GameObject::Sptr darkPineTree1 = scene->CreateGameObject("Dark Tree 1");
		{
			darkPineTree1->SetPosition(glm::vec3(-17.91f, 8.05f, 0.55f));
			darkPineTree1->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			darkPineTree1->SetScale(glm::vec3(0.15f, 0.15f, 0.15f));

			RenderComponent::Sptr renderer = darkPineTree1->Add<RenderComponent>();
			renderer->SetMesh(darkTreeMesh);
			renderer->SetMaterial(darkTreeMat);

			RigidBody::Sptr physics = darkPineTree1->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);
		}

		GameObject::Sptr lightPineTree1 = scene->CreateGameObject("Light Tree 1");
		{
			lightPineTree1->SetPosition(glm::vec3(37.04f, 7.43f, 0.55f));
			lightPineTree1->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			lightPineTree1->SetScale(glm::vec3(0.15f, 0.15f, 0.15f));

			RenderComponent::Sptr renderer = lightPineTree1->Add<RenderComponent>();
			renderer->SetMesh(lightTreeMesh);
			renderer->SetMaterial(lightTreeMat);

			RigidBody::Sptr physics = lightPineTree1->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);
		}

		GameObject::Sptr symbol1 = scene->CreateGameObject("Symbol 1");
		{
			symbol1->SetPosition(glm::vec3(0, 28.24f, 3.85f));
			symbol1->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			symbol1->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

			RenderComponent::Sptr renderer = symbol1->Add<RenderComponent>();
			renderer->SetMesh(S1);
			renderer->SetMaterial(symbol1Mat);

			RigidBody::Sptr physics = symbol1->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);
		}

		GameObject::Sptr symbol2 = scene->CreateGameObject("Symbol 2");
		{
			symbol2->SetPosition(glm::vec3(5.0f, 28.24f, 3.85f));
			symbol2->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			symbol2->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

			RenderComponent::Sptr renderer = symbol2->Add<RenderComponent>();
			renderer->SetMesh(S2);
			renderer->SetMaterial(symbol2Mat);

			RigidBody::Sptr physics = symbol2->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);
		}

		GameObject::Sptr symbol3 = scene->CreateGameObject("Symbol 3");
		{
			symbol3->SetPosition(glm::vec3(-5.0f, 28.24f, 3.85f));
			symbol3->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			symbol3->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

			RenderComponent::Sptr renderer = symbol3->Add<RenderComponent>();
			renderer->SetMesh(S3);
			renderer->SetMaterial(symbol3Mat);

			RigidBody::Sptr physics = symbol3->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);
		}

		GameObject::Sptr symbol4 = scene->CreateGameObject("Symbol 4");
		{
			symbol4->SetPosition(glm::vec3(10.0f, 28.24f, 3.85f));
			symbol4->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			symbol4->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

			RenderComponent::Sptr renderer = symbol4->Add<RenderComponent>();
			renderer->SetMesh(S4);
			renderer->SetMaterial(symbol4Mat);

			RigidBody::Sptr physics = symbol4->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);
		}

		GameObject::Sptr symbol5 = scene->CreateGameObject("Symbol 5");
		{
			symbol5->SetPosition(glm::vec3(-10.0f, 28.24f, 3.85f));
			symbol5->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			symbol5->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

			RenderComponent::Sptr renderer = symbol5->Add<RenderComponent>();
			renderer->SetMesh(S5);
			renderer->SetMaterial(symbol5Mat);

			RigidBody::Sptr physics = symbol5->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);
		}

		GameObject::Sptr torch = scene->CreateGameObject("Torch");
		{
			torch->SetPosition(glm::vec3(20.43f, 26.59f, 2.6f));
			torch->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			torch->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

			RenderComponent::Sptr renderer = torch->Add<RenderComponent>();
			renderer->SetMesh(TorchMesh);
			renderer->SetMaterial(torchMat);

			RigidBody::Sptr physics = torch->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);
		}

		GameObject::Sptr rock = scene->CreateGameObject("Rock");
		{
			rock->SetPosition(glm::vec3(32.34f, 5.71f, 0.85f));
			rock->SetRotation(glm::vec3(-74.f, 0.0f, -90.0f));
			rock->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

			RenderComponent::Sptr renderer = rock->Add<RenderComponent>();
			renderer->SetMesh(rockMesh);
			renderer->SetMaterial(rockMat);

			RigidBody::Sptr physics = rock->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);
		}

		GameObject::Sptr turret = scene->CreateGameObject("Turret");
		{
			turret->SetPosition(glm::vec3(-36.72f, 40.79f, 0.5f));
			turret->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			turret->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

			RenderComponent::Sptr renderer = turret->Add<RenderComponent>();
			renderer->SetMesh(turretMesh);
			renderer->SetMaterial(turretMat);

			RigidBody::Sptr physics = turret->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);
		}

		GameObject::Sptr boxCage = scene->CreateGameObject("Box Cage");
		{
			boxCage->SetPosition(glm::vec3(31.01f, 38.56f, 0.14f));
			boxCage->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			boxCage->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

			RenderComponent::Sptr renderer = boxCage->Add<RenderComponent>();
			renderer->SetMesh(boxCageMesh);
			renderer->SetMaterial(boxCageMat);

			RigidBody::Sptr physics = boxCage->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);
		}

		GameObject::Sptr circleCage = scene->CreateGameObject("Circle Cage");
		{
			circleCage->SetPosition(glm::vec3(42.0f, 42.0f, 0.14f));
			circleCage->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			circleCage->SetScale(glm::vec3(0.075f, 0.075f, 0.075f));

			RenderComponent::Sptr renderer = circleCage->Add<RenderComponent>();
			renderer->SetMesh(circleCageMesh);
			renderer->SetMaterial(circleCageMat);

			RigidBody::Sptr physics = circleCage->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);
		}

		GameObject::Sptr longCage = scene->CreateGameObject("Long Cage");
		{
			longCage->SetPosition(glm::vec3(20, -11.75f, 0.14f));
			longCage->SetRotation(glm::vec3(90.f, 0.0f, 0.0f));
			longCage->SetScale(glm::vec3(0.1f, 0.1f, 0.225f));

			RenderComponent::Sptr renderer = longCage->Add<RenderComponent>();
			renderer->SetMesh(LongCageMesh);
			renderer->SetMaterial(longCageMat);

			RigidBody::Sptr physics = longCage->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);
		}

		GameObject::Sptr uCage = scene->CreateGameObject("U Cage");
		{
			uCage->SetPosition(glm::vec3(-36.77f, 37.03f, 0.14f));
			uCage->SetRotation(glm::vec3(90.f, 0.0f, -90.0f));
			uCage->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

			RenderComponent::Sptr renderer = uCage->Add<RenderComponent>();
			renderer->SetMesh(UCageMesh);
			renderer->SetMaterial(uCageMat);

			RigidBody::Sptr physics = uCage->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);
		}

		GameObject::Sptr graveStone = scene->CreateGameObject("Grave Stone");
		{
			graveStone->SetPosition(glm::vec3(-12.43f, 11.08f, 0.33f));
			graveStone->SetRotation(glm::vec3(90.f, 0.0f, 90.0f));
			graveStone->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

			RenderComponent::Sptr renderer = graveStone->Add<RenderComponent>();
			renderer->SetMesh(graveStoneMesh);
			renderer->SetMaterial(graveStoneMat);

			RigidBody::Sptr physics = graveStone->Add<RigidBody>(RigidBodyType::Static);
			BoxCollider::Sptr collider = BoxCollider::Create();
			physics->AddCollider(collider);
		}

		/////////////////////////////////////////////////////////
		//				   USER INTERFACE - GAME
		/////////////////////////////////////////////////////////

		GameObject::Sptr gameCanvas = scene->CreateGameObject("Game Canvas");
		{
			RectTransform::Sptr transform = gameCanvas->Add<RectTransform>();
			transform->SetMin({ 0, 0 });
			transform->SetMax({ 1000, 1000 });
			transform->SetPosition({ 500, 500 });

			GuiPanel::Sptr canPanel = gameCanvas->Add<GuiPanel>();
			canPanel->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));

			GameObject::Sptr healthText = UIHelper::CreateText("Body Health: ???", "Body Health Text");
			healthText->Get<RectTransform>()->SetPosition({ 120, 50 });
			scene->PC.SetBodyHealthUI(*healthText->Get<GuiText>());			
			gameCanvas->AddChild(healthText);

			GameObject::Sptr shadowText = UIHelper::CreateText("Shadow Health: ???", "Shadow Health Text");
			shadowText->Get<RectTransform>()->SetPosition({ 120, 100 });
			scene->PC.SetShadowHealthUI(*shadowText->Get<GuiText>());
			gameCanvas->AddChild(shadowText);
		}

		/////////////////////////////////////////////////////////
		//				   USER INTERFACE - MENU
		/////////////////////////////////////////////////////////

		// Cody :: Everything for pause menu is stored in one game object.  Refer to UIHelper.h to see the full code to create Images, Text, and "Buttons"
		// Note that I call them buttons but they dont do anything.  Hope you guys can figure out that part by the time I pick this back up Saturday night.

		GameObject::Sptr pauseMenu = scene->CreateGameObject("UI Menu Canvas");
		{
			RectTransform::Sptr transform = pauseMenu->Add<RectTransform>();
			transform->SetMin({ 16, 16 });
			transform->SetMax({ 350, 500 });
			transform->SetPosition({ 400, 300 });

			GuiPanel::Sptr backgroundPanel = pauseMenu->Add<GuiPanel>();
			backgroundPanel->SetColor(glm::vec4(0.3f, 0.3f, 0.3f, 0.5f));			
		

			/*GameObject::Sptr upperGraphic = UIHelper::CreateImage(menuGlossTex, "Upper Graphic");
			upperGraphic->Get<RectTransform>()->SetPosition({ transform->GetPosition().x / 2.5f, 70 });
			upperGraphic->Get<RectTransform>()->SetSize({ 80, 30 });
			upperGraphic->Get<GuiPanel>()->SetBorderRadius(0);
			pauseMenu->AddChild(upperGraphic);*/

			GameObject::Sptr menuTitle = UIHelper::CreateText("Paused");
			menuTitle->Get<GuiText>()->SetTextScale(2);			
			menuTitle->Get<RectTransform>()->SetPosition({ 280.0f, 175 });					
			pauseMenu->AddChild(menuTitle);

			GameObject::Sptr button1 = UIHelper::CreateButton("Resume Game");
			button1->Get<RectTransform>()->SetPosition({ transform->GetPosition().x / 2.5f, 150 });
			pauseMenu->AddChild(button1);

			GameObject::Sptr button2 = UIHelper::CreateButton("Restart Level");
			button2->Get<RectTransform>()->SetPosition({ transform->GetPosition().x / 2.5f, 200 });
			pauseMenu->AddChild(button2);

			GameObject::Sptr button3 = UIHelper::CreateButton("Options");
			button3->Get<RectTransform>()->SetPosition({ transform->GetPosition().x / 2.5f, 250 });
			pauseMenu->AddChild(button3);

			GameObject::Sptr button4 = UIHelper::CreateButton("Quit to Menu");
			button4->Get<RectTransform>()->SetPosition({ transform->GetPosition().x / 2.5f, 300 });
			pauseMenu->AddChild(button4);

			GameObject::Sptr button5 = UIHelper::CreateButton("Quit Game");
			button5->Get<RectTransform>()->SetPosition({ transform->GetPosition().x / 2.5f, 350 });
			pauseMenu->AddChild(button5);			

			/*GameObject::Sptr lowerGraphic = UIHelper::CreateImage(menuGlossReverseTex, "Lower Graphic");
			lowerGraphic->Get<RectTransform>()->SetPosition({ transform->GetPosition().x / 2.5f, 400 });
			lowerGraphic->Get<RectTransform>()->SetSize({ 40, 17.5f });	
			lowerGraphic->Get<GuiPanel>()->SetBorderRadius(0);
			pauseMenu->AddChild(lowerGraphic);*/


			pauseMenu->IsActive = false;
			scene->PC.SetPauseMenu(*pauseMenu);
		}	


		/////////////////////////////////////////////////////////
		//				USER INTERFACE - Guide
		/////////////////////////////////////////////////////////

		// Cody :: Everything for pause menu is stored in one game object.  Refer to UIHelper.h to see the full code to create Images, Text, and "Buttons"
		// Note that I call them buttons but they dont do anything.  Hope you guys can figure out that part by the time I pick this back up Saturday night.

		GameObject::Sptr guideCanvas = scene->CreateGameObject("UI Menu Canvas");
		{
			RectTransform::Sptr transform = guideCanvas->Add<RectTransform>();
			transform->SetMin({ 16, 16 });
			transform->SetMax({ 350, 130 });
			transform->SetPosition({ 400, 100 });

			GuiPanel::Sptr backgroundPanel = guideCanvas->Add<GuiPanel>();
			backgroundPanel->SetColor(glm::vec4(0.3f, 0.3f, 0.3f, 0.5f));

			GameObject::Sptr menuTitle = UIHelper::CreateText("\t\t\t\tWelcome to Into The Abyss Demo. \nTo move, use the keys [ W ], [ A ], [ S ], [ D ]. \n[ E ] to interact with objects.  \n[ Q ] to switch between your body and shadow character. \n[ R ] to recall your shadow back to your body.  \n[ F ] to swap the positions of your shadow and your body");
			menuTitle->Get<GuiText>()->SetTextScale(1.0f);
			menuTitle->Get<RectTransform>()->SetPosition({ 350.0f, 100.0f });
			guideCanvas->AddChild(menuTitle);			

			//guideCanvas->IsActive = false;
			scene->PC.SetGuideCanvas(*pauseMenu);
		}


		GuiBatcher::SetDefaultTexture(ResourceManager::CreateAsset<Texture2D>("textures/ui-sprite.png"));
		GuiBatcher::SetDefaultBorderRadius(8);


		// Call scene awake to start up all of our components
		scene->Window = window;
		scene->Awake();

		// Save the asset manifest for all the resources we just loaded
		ResourceManager::SaveManifest("manifest.json");
		// Save the scene to a JSON file
		scene->Save("scene.json");
	}
}

int main() {
	Logger::Init(); // We'll borrow the logger from the toolkit, but we need to initialize it

	//Initialize GLFW
	if (!initGLFW())
		return 1;

	//Initialize GLAD
	if (!initGLAD())
		return 1;

	// Let OpenGL know that we want debug output, and route it to our handler function
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(GlDebugMessage, nullptr);

	// Initialize our ImGui helper
	ImGuiHelper::Init(window);

	// Initialize our resource manager
	ResourceManager::Init();

	// Register all our resource types so we can load them from manifest files
	ResourceManager::RegisterType<Texture2D>();
	ResourceManager::RegisterType<TextureCube>();
	ResourceManager::RegisterType<Shader>();
	ResourceManager::RegisterType<Material>();
	ResourceManager::RegisterType<MeshResource>();

	// Register all of our component types so we can load them from files
	ComponentManager::RegisterType<Camera>();
	ComponentManager::RegisterType<RenderComponent>();
	ComponentManager::RegisterType<RigidBody>();
	ComponentManager::RegisterType<TriggerVolume>();
	ComponentManager::RegisterType<RotatingBehaviour>();
	ComponentManager::RegisterType<JumpBehaviour>();
	ComponentManager::RegisterType<MaterialSwapBehaviour>();
	ComponentManager::RegisterType<TriggerVolumeEnterBehaviour>();
	ComponentManager::RegisterType<SimpleCameraControl>();
	ComponentManager::RegisterType<HealthComponent>();
	ComponentManager::RegisterType<InteractableComponent>();
	ComponentManager::RegisterType<AnimatorComponent>();
	ComponentManager::RegisterType<Enemy>();

	ComponentManager::RegisterType<RectTransform>();
	ComponentManager::RegisterType<GuiPanel>();
	ComponentManager::RegisterType<GuiText>();

	// GL states, we'll enable depth testing and backface fulling

	
	// Structure for our frame-level uniforms, matches layout from
	// fragments/frame_uniforms.glsl
	// For use with a UBO.
	struct FrameLevelUniforms {
		// The camera's view matrix
		glm::mat4 u_View;
		// The camera's projection matrix
		glm::mat4 u_Projection;
		// The combined viewProject matrix
		glm::mat4 u_ViewProjection;
		// The camera's position in world space
		glm::vec4 u_CameraPos;
		// The time in seconds since the start of the application
		float u_Time;
	};
	// This uniform buffer will hold all our frame level uniforms, to be shared between shaders
	UniformBuffer<FrameLevelUniforms>::Sptr frameUniforms = std::make_shared<UniformBuffer<FrameLevelUniforms>>(BufferUsage::DynamicDraw);
	// The slot that we'll bind our frame level UBO to
	const int FRAME_UBO_BINDING = 0;

	// Structure for our isntance-level uniforms, matches layout from
	// fragments/frame_uniforms.glsl
	// For use with a UBO.
	struct InstanceLevelUniforms {
		// Complete MVP
		glm::mat4 u_ModelViewProjection;
		// Just the model transform, we'll do worldspace lighting
		glm::mat4 u_Model;
		// Normal Matrix for transforming normals
		glm::mat4 u_NormalMatrix;
	};

	// This uniform buffer will hold all our instance level uniforms, to be shared between shaders
	UniformBuffer<InstanceLevelUniforms>::Sptr instanceUniforms = std::make_shared<UniformBuffer<InstanceLevelUniforms>>(BufferUsage::DynamicDraw);

	// The slot that we'll bind our instance level UBO to
	const int INSTANCE_UBO_BINDING = 1;

	////////////////////////////////
	///// SCENE CREATION MOVED /////
	////////////////////////////////
	CreateScene();

	// We'll use this to allow editing the save/load path
	// via ImGui, note the reserve to allocate extra space
	// for input!
	std::string scenePath = "scene.json"; 
	scenePath.reserve(256); 

	// Our high-precision timer
	double lastFrame = glfwGetTime();

	BulletDebugMode physicsDebugMode = BulletDebugMode::None;
	float playbackSpeed = 1.0f;

	nlohmann::json editorSceneState;

	scene->IsPlaying = true;

	///// Game loop /////
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		ImGuiHelper::StartFrame();

		// Calculate the time since our last frame (dt)
		double thisFrame = glfwGetTime();
		float dt = static_cast<float>(thisFrame - lastFrame);

		// Showcasing how to use the imGui library!
		bool isDebugWindowOpen = ImGui::Begin("Debugging");
		if (isDebugWindowOpen) {
			// Draws a button to control whether or not the game is currently playing
			static char buttonLabel[64];
			sprintf_s(buttonLabel, "%s###playmode", scene->IsPlaying ? "Exit Play Mode" : "Enter Play Mode");
			if (ImGui::Button(buttonLabel)) {
				// Save scene so it can be restored when exiting play mode
				if (!scene->IsPlaying) {
					editorSceneState = scene->ToJson();
				}

				// Toggle state
				scene->IsPlaying = !scene->IsPlaying;

				// If we've gone from playing to not playing, restore the state from before we started playing
				if (!scene->IsPlaying) {
					scene = nullptr;
					// We reload to scene from our cached state
					scene = Scene::FromJson(editorSceneState);
					// Don't forget to reset the scene's window and wake all the objects!
					scene->Window = window;
					scene->Awake();
				}
			}

			// Make a new area for the scene saving/loading
			ImGui::Separator();
			if (DrawSaveLoadImGui(scene, scenePath)) {
				// C++ strings keep internal lengths which can get annoying
				// when we edit it's underlying datastore, so recalcualte size
				scenePath.resize(strlen(scenePath.c_str()));

				// We have loaded a new scene, call awake to set
				// up all our components
				scene->Window = window;
				scene->Awake();
			}
			ImGui::Separator();
			// Draw a dropdown to select our physics debug draw mode
			if (BulletDebugDraw::DrawModeGui("Physics Debug Mode:", physicsDebugMode)) {
				scene->SetPhysicsDebugDrawMode(physicsDebugMode);
			}
			LABEL_LEFT(ImGui::SliderFloat, "Playback Speed:    ", &playbackSpeed, 0.0f, 10.0f);
			ImGui::Separator();
		}

		// Clear the color and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw some ImGui stuff for the lights
		if (isDebugWindowOpen) {
			for (int ix = 0; ix < scene->Lights.size(); ix++) {
				char buff[256];
				sprintf_s(buff, "Light %d##%d", ix, ix);
				// DrawLightImGui will return true if the light was deleted
				if (DrawLightImGui(scene, buff, ix)) {
					// Remove light from scene, restore all lighting data
					scene->Lights.erase(scene->Lights.begin() + ix);
					scene->SetupShaderAndLights();
					// Move back one element so we don't skip anything!
					ix--;
				}
			}
			// As long as we don't have max lights, draw a button
			// to add another one
			if (scene->Lights.size() < scene->MAX_LIGHTS) {
				if (ImGui::Button("Add Light")) {
					scene->Lights.push_back(Light());
					scene->SetupShaderAndLights();
				}
			}
			// Split lights from the objects in ImGui
			ImGui::Separator();
		}

		dt *= playbackSpeed;

		// Perform updates for all components
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		scene->DoPhysics(dt);
		scene->Update(dt);

		// Grab shorthands to the camera and shader from the scene
		Camera::Sptr camera = scene->MainCamera;

		// Cache the camera's viewprojection
		glm::mat4 viewProj = camera->GetViewProjection();
		DebugDrawer::Get().SetViewProjection(viewProj);

		// Update our worlds physics!
		scene->DoPhysics(dt);

		// Draw object GUIs
		if (isDebugWindowOpen) {
			scene->DrawAllGameObjectGUIs();
		}
		
		// The current material that is bound for rendering
		Material::Sptr currentMat = nullptr;
		Shader::Sptr shader = nullptr;

		// Make sure depth testing and culling are re-enabled
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		// Here we'll bind all the UBOs to their corresponding slots
		scene->PreRender();
		frameUniforms->Bind(FRAME_UBO_BINDING);
		instanceUniforms->Bind(INSTANCE_UBO_BINDING);

		// Upload frame level uniforms
		auto& frameData = frameUniforms->GetData();
		frameData.u_Projection = camera->GetProjection();
		frameData.u_View = camera->GetView();
		frameData.u_ViewProjection = camera->GetViewProjection();
		frameData.u_CameraPos = glm::vec4(camera->GetGameObject()->GetPosition(), 1.0f);
		frameData.u_Time = static_cast<float>(thisFrame);
		frameUniforms->Update();

		// Render all our objects
		ComponentManager::Each<RenderComponent>([&](const RenderComponent::Sptr& renderable) {
			// Early bail if mesh not set
			if (renderable->GetMesh() == nullptr) { 
				return;
			}

			// If we don't have a material, try getting the scene's fallback material
			// If none exists, do not draw anything
			if (renderable->GetMaterial() == nullptr) {
				if (scene->DefaultMaterial != nullptr) {
					renderable->SetMaterial(scene->DefaultMaterial);
				} else {
					return;
				}
			}

			// If the material has changed, we need to bind the new shader and set up our material and frame data
			// Note: This is a good reason why we should be sorting the render components in ComponentManager
			if (renderable->GetMaterial() != currentMat) {
				currentMat = renderable->GetMaterial();
				shader = currentMat->GetShader();

				shader->Bind();
				currentMat->Apply();
			}

			// Grab the game object so we can do some stuff with it
			GameObject* object = renderable->GetGameObject();
			 
			// Use our uniform buffer for our instance level uniforms
			auto& instanceData = instanceUniforms->GetData();
			instanceData.u_Model = object->GetTransform();
			instanceData.u_ModelViewProjection = viewProj * object->GetTransform();
			instanceData.u_NormalMatrix = glm::mat3(glm::transpose(glm::inverse(object->GetTransform())));
			
			instanceUniforms->Update();  

			// Draw the object
			renderable->GetMesh()->Draw();
		});

		// Disable culling
		glDisable(GL_CULL_FACE);
		// Disable depth testing, we're going to use order-dependant layering
		glDisable(GL_DEPTH_TEST);
		// Disable depth writing
		glDepthMask(GL_FALSE);

		// Enable alpha blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Enable the scissor test;
		glEnable(GL_SCISSOR_TEST);

		// Our projection matrix will be our entire window for now
		glm::mat4 proj = glm::ortho(0.0f, (float)windowSize.x, (float)windowSize.y, 0.0f, -1.0f, 1.0f);
		GuiBatcher::SetProjection(proj);

		// Iterate over and render all the GUI objects
		scene->RenderGUI();

		// Flush the Gui Batch renderer
		GuiBatcher::Flush();

		// Disable alpha blending
		glDisable(GL_BLEND);
		// Disable scissor testing
		glDisable(GL_SCISSOR_TEST);
		// Re-enable depth writing
		glDepthMask(GL_TRUE);

		// End our ImGui window
		ImGui::End();

		VertexArrayObject::Unbind();

		lastFrame = thisFrame;
		ImGuiHelper::EndFrame();
		glfwSwapBuffers(window);
	}

	// Clean up the ImGui library
	ImGuiHelper::Cleanup();

	// Clean up the resource manager
	ResourceManager::Cleanup();

	// Clean up the toolkit logger so we don't leak memory
	Logger::Uninitialize();
	return 0;
}