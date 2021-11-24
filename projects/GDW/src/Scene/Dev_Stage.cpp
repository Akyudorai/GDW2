#include "Dev_Stage.h"

#include "NOU/App.h"
#include "NOU/Input.h"
#include "NOU/Entity.h"
#include "NOU/CCamera.h"
#include "NOU/CMeshRenderer.h"
#include "NOU/Shader.h"
#include "NOU/GLTFLoader.h"

#include "Editor/Resources.h"

#include "Game/PlayerController.h"

#include "Physics/Physics.h"
#include "Physics/SphereCollider.h"

#include "GLFW/glfw3.h"
#include <iostream>
#include <GLFW/glfw3.h>

#include <stb_image.h>

using namespace GAME;

Dev_Stage::Dev_Stage() : Scene(), camera(Entity::Create("Camera")),
body(Entity::Create("Body")), shadow(Entity::Create("Shadow")),
wall_1(Entity::Create("Wall")), wall_2(Entity::Create("Wall 2")),
wall_3(Entity::Create("Wall 3"))
{		
	/*Entity test = Entity::Create("Test 1");
	Entity test2 = Entity::Create("Test 2");	
	entities.emplace(test.m_name, test);
	entities.emplace(test2.m_name, test2);*/

	/*for (int i = -50; i <= 50; i++)
	{		
		std::string name = "GameObject " + std::to_string(i);
		Entity e = Entity::Create(name);
		e.transform.m_scale = vec3(1.0f);
		e.transform.m_pos = vec3(i, 0, i);
		e.Add<CMeshRenderer>(e, *Resources::GetInstance().Meshes["Box"], *Resources::GetInstance().mat_unselected);

		entities.emplace(e.m_name, e);
	}*/
	
}

bool Dev_Stage::OnCreate()
{
	// Texture
	int widthImg, heightImg, numColCh;
	unsigned char* bytes = stbi_load("bot.png", &widthImg, &heightImg, &numColCh, 0);
	
	GLuint texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(bytes);
	glBindTexture(GL_TEXTURE_2D, 0);

	/*GLuint tex0Uni = glGetUniformLocation(shaderProgram.ID, "tex0");
	shaderProgram.Activate();
	glUniform1i(tex0Uni, 0);*/

	glBindTexture(GL_TEXTURE_2D, texture);

	// Initialize Camera
	//camera = Entity::Create("Camera");
	CCamera& c_cam = camera.Add<CCamera>(camera);
	c_cam.Perspective(60.0f, 1.0f, 0.1f, 100.0f);
	camera.transform.m_pos = glm::vec3(0.0f, 3.0f, 5.0f);
	camera.transform.m_rotation = glm::angleAxis(glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//entities.push_back(&camera);

	//// Initialize Player
	//body = Entity::Create("Body");
	body.Add<CMeshRenderer>(body, *Resources::GetInstance().Meshes["Box"], *Resources::GetInstance().mat_ducky);
	body.transform.m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
	body.transform.m_pos = glm::vec3(-1.0f, 0.0f, 0.0f);
	//body.transform.m_rotation = glm::angleAxis(glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	body.Add<BoxCollider>(body, glm::vec3(1.0f, 1.0f, 1.0f));
	body.Add<PhysicsObject>(body);
	//entities.push_back(body);

	//shadow = Entity::Create("Shadow Ducky");
	shadow.Add<CMeshRenderer>(shadow, *Resources::GetInstance().Meshes["Box"], *Resources::GetInstance().mat_line);
	shadow.transform.m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
	shadow.transform.m_pos = glm::vec3(1.0f, 0.0f, 0.0f);
	//shadow.transform.m_rotation = glm::angleAxis(glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	shadow.Add<BoxCollider>(shadow, glm::vec3(1.0f, 1.0f, 1.0f));
	shadow.Add<PhysicsObject>(shadow);
	//entities.push_back(shadow);

	//// Initialize Environment Objects
	//Entity* wall_1 = &Entity::Create("Wall 1");
	wall_1.Add<CMeshRenderer>(wall_1, *Resources::GetInstance().Meshes["Box"], *Resources::GetInstance().mat_unselected);
	wall_1.transform.m_scale = glm::vec3(1.0f, 1.0f, 3.0f);
	wall_1.transform.m_pos = glm::vec3(-2.0f, 0.0f, 3.0f);
	wall_1.Add<BoxCollider>(wall_1, wall_1.transform.m_scale);
	//entities.push_back(*wall_1);

	//Entity* wall_2 = &Entity::Create("Wall 2");
	wall_2.Add<CMeshRenderer>(wall_2, *Resources::GetInstance().Meshes["Box"], *Resources::GetInstance().mat_selected);
	wall_2.transform.m_scale = glm::vec3(2.0f, 1.0f, 1.0f);
	wall_2.transform.m_pos = glm::vec3(0.0f, 0.0f, 3.0f);
	wall_2.Add<BoxCollider>(wall_2, wall_2.transform.m_scale);
	//entities.push_back(*wall_2);

	//Entity* wall_3 = &Entity::Create("Wall 3");
	wall_3.Add<CMeshRenderer>(wall_3, *Resources::GetInstance().Meshes["Box"], *Resources::GetInstance().mat_unselected);
	wall_3.transform.m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
	wall_3.transform.m_pos = glm::vec3(2.0f, 0.0f, 3.0f);
	wall_3.Add<BoxCollider>(wall_3, wall_3.transform.m_scale);
	//entities.push_back(*wall_3);

	// Initialize Player Controller
	pc = PlayerController(body, shadow, camera);

	entities.reserve(1000);
	for (int i = 0; i < 10; i++) {
		Entity e = Entity::Create("Test " + std::to_string(i+1));
		e.transform.m_scale = vec3(1.0f);
		e.transform.m_pos = vec3(i, 0, i);
		e.Add<CMeshRenderer>(e, *Resources::GetInstance().Meshes["Box"], *Resources::GetInstance().mat_unselected);
		entities.push_back(e);
	}

	/*entities = {
		Entity::Create("Test"),
		Entity::Create("Test 2")
	};*/

	return true;
}

void Dev_Stage::LoadScene()
{
	entities = {
		Entity::Create("Test"),
		Entity::Create("Test 2"),
		Entity::Create("Test 3"),
	};
}

void Dev_Stage::Update(const float deltaTime)
{
	// Update Environment Transform
	wall_1.transform.RecomputeGlobal();
	wall_2.transform.RecomputeGlobal();
	wall_3.transform.RecomputeGlobal();

	//// Draw Environment
	wall_1.Get<CMeshRenderer>().Draw();
	wall_2.Get<CMeshRenderer>().Draw();
	wall_3.Get<CMeshRenderer>().Draw();

	// Update Controller
	pc.Update(deltaTime);
	pc.Render();	

	for (auto &e : entities)
	{
		e.transform.RecomputeGlobal();
		
		// Cant load component?
		//e.Get<CMeshRenderer>().Draw();
		
	}

	// Player/Wall 1
	if (Physics::BoxBoxCollision(body.Get<BoxCollider>(), wall_1.Get<BoxCollider>()))
	{
		Physics::BoxBoxCollisionResponse(body.Get<BoxCollider>(), wall_1.Get<BoxCollider>());
	}

	// Player/Wall 2
	if (Physics::BoxBoxCollision(body.Get<BoxCollider>(), wall_2.Get<BoxCollider>()))
	{
		Physics::BoxBoxCollisionResponse(body.Get<BoxCollider>(), wall_2.Get<BoxCollider>());
	}

	// Player/Wall 3
	if (Physics::BoxBoxCollision(body.Get<BoxCollider>(), wall_3.Get<BoxCollider>()))
	{
		Physics::BoxBoxCollisionResponse(body.Get<BoxCollider>(), wall_3.Get<BoxCollider>());
	}

	// Shadow/Wall 1
	if (Physics::BoxBoxCollision(shadow.Get<BoxCollider>(), wall_1.Get<BoxCollider>()))
	{
		Physics::BoxBoxCollisionResponse(shadow.Get<BoxCollider>(), wall_1.Get<BoxCollider>());
	}

	// Shadow/Wall 3
	if (Physics::BoxBoxCollision(shadow.Get<BoxCollider>(), wall_3.Get<BoxCollider>()))
	{
		Physics::BoxBoxCollisionResponse(shadow.Get<BoxCollider>(), wall_3.Get<BoxCollider>());
	}


}

void Dev_Stage::Render() const
{

}

void Dev_Stage::HandleEvents()
{

}

void Dev_Stage::OnResizeWindow(int width, int height)
{

}

Dev_Stage::~Dev_Stage()
{
	OnDestroy();
}

void Dev_Stage::OnDestroy()
{
	// Cleanup Assets	

}