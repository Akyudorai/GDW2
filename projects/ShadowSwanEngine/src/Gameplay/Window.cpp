#include "Window.h"

#include <glad/glad.h>

#include "Utils/MouseListener.h"
#include "SceneManagement/SceneManager.h"
#include "Graphics/GuiBatcher.h"

#include "Utils/ImGuiHelper.h"
#include "Gameplay/InputManagement/InputHandler.h"
#include "Gameplay/GameManager.h"
#include "Utils/Editor/Editor.h"
#include "Graphics/DebugDraw.h"

#include "Utils/ResourceManagement/Resources.h"
#include "Utils/ResourceManagement/ResourceManager.h"
#include "Gameplay/Components/RenderComponent.h"

#include "Utils/Editor/GameViewWindow.h"

Window::Window()
{
	this->width = 1920;
	this->height = 1080;
	this->title = "Into The Abyss";
	r = 0.2;
	g = 0.2;
	b = 0.2;
	a = 1;
}

void Window::Run()
{
	Init();
	Loop();

	// Free the memory
	glfwDestroyWindow(glfwWindow);

	// Terminate GLFW and free the error callback
	glfwTerminate();
	glfwSetErrorCallback(NULL);
}


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

void GlfwWindowResizedCallback(GLFWwindow* window, int width, int height) {
	
	Window::SetWidth(width);
	Window::SetHeight(height);

	SceneManager::GetCurrentScene()->MainCamera->ResizeWindow(width, height);
	GuiBatcher::SetWindowSize({ width, height });
}

void Window::Init()
{
	Logger::Init();

	// Initialize GLFW
	if (!glfwInit()) {
		LOG_ERROR("Unable to initialize GLFW.");
		exit(0);
	}

	// Create the Window
	glfwWindow = glfwCreateWindow(this->width, this->height, this->title.c_str(), nullptr, nullptr);
	if (glfwWindow == nullptr) {
		LOG_ERROR("Failed to create the GLFW Window.");
	}

	// Make the OpenGL context current
	glfwMakeContextCurrent(glfwWindow);

	glfwSetWindowSizeCallback(glfwWindow, GlfwWindowResizedCallback);
	GuiBatcher::SetWindowSize(glm::ivec2(this->width, this->height));

	// Initialize GLAD
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		LOG_ERROR("Failed to initialie GLAD.");
		exit(0);
	}

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(GlDebugMessage, nullptr);

	// Configure GLFW
	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	// Enable v-sync
	glfwSwapInterval(1);

	// Make the window visible
	glfwShowWindow(glfwWindow);

	this->framebuffer = Framebuffer(1980, 1080);
	glViewport(0, 0, 1980, 1090);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

void Window::Loop()
{

	// Structure for our frame - level uniforms, matches layout from
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

	// Initialize our Resources
	Resources::GetInstance().Initialize();

	// Initialize GameManager
	GameManager::GetInstance().Initialize();

	// Initialize Scene Manager
	SceneManager::Initialize(glfwWindow);
	SceneManager::LoadScene(SceneManager::Scenes::LevelTwo, true);
	SceneManager::GetCurrentScene()->IsPlaying = true;

	// Initialize Input Manager
	InputHandler::Initialize();

	// Initialize Editor
	Editor editor = Editor();
	editor.Initialize(glfwWindow);

	float lastFrame = (float)glfwGetTime();

	while (!glfwWindowShouldClose(glfwWindow)) {
		// Poll Events
		glfwPollEvents();

		ImGuiHelper::StartFrame();
		
		// Calculate DeltaTime;
		float thisFrame = (float)glfwGetTime();
		float dt = static_cast<float>(thisFrame - lastFrame);

		//this->framebuffer.Bind();

		// Perform updates for all components
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);		
			
		// Clear the color and depth buffers
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update Scene
		SceneManager::Update(dt);
		SceneManager::Draw();
		InputHandler::Update(dt);
		GameManager::GetInstance().Update(dt);
		editor.Update(dt);
		editor.Draw(dt);

		// Grab shorthand to camera
		Camera::Sptr camera = SceneManager::GetCurrentScene()->MainCamera;

		// Cache the camera's viewprojection
		glm::mat4 viewProj = camera->GetViewProjection();
		DebugDrawer::Get().SetViewProjection(viewProj);

		// The current Material that is bound for rendering
		Material::Sptr currentMat = nullptr;
		Shader::Sptr shader = nullptr;

		// Make sure depth testing and colling are re-enabled;
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		// Here we'll bind all the UBOs to their corresponding slots
		SceneManager::GetCurrentScene()->PreRender();
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
				if (SceneManager::GetCurrentScene()->DefaultMaterial != nullptr) {
					renderable->SetMaterial(SceneManager::GetCurrentScene()->DefaultMaterial);
				}
				else {
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
		glm::mat4 proj = glm::ortho(0.0f, (float)this->width, (float)this->height, 0.0f, -1.0f, 1.0f);
		GuiBatcher::SetProjection(proj);		

		// Iterate over and render all the GUI objects
		SceneManager::GetCurrentScene()->RenderGUI();

		this->framebuffer.Unbind();

		// Flush the Gui Batch renderer
		GuiBatcher::Flush();

		// Disable alpha blending
		glDisable(GL_BLEND);
		// Disable scissor testing
		glDisable(GL_SCISSOR_TEST);
		// Re-enable depth writing
		glDepthMask(GL_TRUE);
		
		VertexArrayObject::Unbind();		

		lastFrame = thisFrame;
		ImGuiHelper::EndFrame();

		glfwSwapBuffers(glfwWindow);
	}

	// Clean up the ImGui library
	ImGuiHelper::Cleanup();

	// Clean up the resource manager
	ResourceManager::Cleanup();

	// Clean up the toolkit logger so we don't leak memory
	Logger::Uninitialize();
}

void ChangeScene(int index)
{

}

