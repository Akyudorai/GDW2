#include "Level_3.h"

#include "NOU/App.h"
#include "NOU/Input.h"
#include "NOU/Entity.h"
#include "NOU/CCamera.h"
#include "NOU/CMeshRenderer.h"
#include "NOU/Shader.h"
#include "NOU/GLTFLoader.h"

#include "Editor/Resources.h"

#include "GameSceneManager.h"
#include "Game/PlayerController.h"

#include "Physics/Physics.h"
#include "Physics/SphereCollider.h"

using namespace GAME;

Level_3::Level_3() : Scene(), camera(Entity::Create("Camera")),
body(Entity::Create("Body")), shadow(Entity::Create("Shadow")),
wall_1(Entity::Create("Wall")), wall_2(Entity::Create("Wall 2")),
wall_3(Entity::Create("Wall 3")), winBox(Entity::Create("Win Condition"))
{

}

bool Level_3::OnCreate()
{
	// Initialize Camera
	//camera = Entity::Create("Camera");
	CCamera& c_cam = camera.Add<CCamera>(camera);
	c_cam.Perspective(60.0f, 1.0f, 0.1f, 100.0f);
	camera.transform.m_pos = glm::vec3(0.0f, 3.0f, 5.0f);
	camera.transform.m_rotation = glm::angleAxis(glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//entities.push_back(&camera);

	//// Initialize Player
	//body = Entity::Create("Body");
	body.Add<CMeshRenderer>(body, *Resources::GetInstance().Meshes["Duck"], *Resources::GetInstance().mat_ducky);
	body.transform.m_scale = glm::vec3(0.005f, 0.005f, 0.005f);
	body.transform.m_pos = glm::vec3(0.0f, 0.0f, 0.0f);
	body.transform.m_rotation = glm::angleAxis(glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	body.Add<SphereCollider>(body, 0.5f);
	body.Add<PhysicsObject>(body);
	//entities.push_back(body);

	//shadow = Entity::Create("Shadow Ducky");
	shadow.Add<CMeshRenderer>(shadow, *Resources::GetInstance().Meshes["Duck"], *Resources::GetInstance().mat_line);
	shadow.transform.m_scale = glm::vec3(0.005f, 0.005f, 0.005f);
	shadow.transform.m_pos = glm::vec3(0.0f, 0.0f, 0.0f);
	shadow.transform.m_rotation = glm::angleAxis(glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	shadow.Add<SphereCollider>(shadow, 0.5f);
	shadow.Add<PhysicsObject>(shadow);
	//entities.push_back(shadow);

	//// Initialize Environment Objects
	//Entity* wall_1 = &Entity::Create("Wall 1");
	wall_1.Add<CMeshRenderer>(wall_1, *Resources::GetInstance().Meshes["Box"], *Resources::GetInstance().mat_unselected);
	wall_1.transform.m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
	wall_1.transform.m_pos = glm::vec3(3.25f, 0.5f, 3.25f);
	//entities.push_back(*wall_1);

	//Entity* wall_2 = &Entity::Create("Wall 2");
	wall_2.Add<CMeshRenderer>(wall_2, *Resources::GetInstance().Meshes["Box"], *Resources::GetInstance().mat_selected);
	wall_2.transform.m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
	wall_2.transform.m_pos = glm::vec3(-2.25f, 0.5f, -2.25f);
	wall_2.Add<SphereCollider>(wall_2, 0.5f);
	wall_2.Get<SphereCollider>().isTrigger = true;
	//entities.push_back(*wall_2);

	//Entity* wall_3 = &Entity::Create("Wall 3");
	wall_3.Add<CMeshRenderer>(wall_3, *Resources::GetInstance().Meshes["Box"], *Resources::GetInstance().mat_unselected);
	wall_3.transform.m_scale = glm::vec3(3.0f, 1.0f, 1.0f);
	wall_3.transform.m_pos = glm::vec3(1.25f, 0.5f, -1.25f);
	//entities.push_back(*wall_3);

	winBox.Add<CMeshRenderer>(winBox, *Resources::GetInstance().Meshes["Box"], *Resources::GetInstance().mat_ducky);
	winBox.transform.m_scale = glm::vec3(0.5f, 0.5f, 0.5f);
	winBox.transform.m_pos = glm::vec3(-1.0f, 0.5f, 1.0f);
	winBox.Add<SphereCollider>(winBox, 0.25f);
	winBox.Get<SphereCollider>().isTrigger = true;
	winBox.Get<SphereCollider>().onTriggerEvent = []
	{
		Debug::Log("Level 3 completed.");
	};

	// Initialize Player Controller
	pc = PlayerController(body, shadow, camera);

	return true;
}


void Level_3::Update(const float deltaTime)
{
	// Update Environment Transform
	wall_1.transform.RecomputeGlobal();
	wall_2.transform.RecomputeGlobal();
	wall_3.transform.RecomputeGlobal();

	//// Draw Environment
	wall_1.Get<CMeshRenderer>().Draw();
	wall_2.Get<CMeshRenderer>().Draw();
	wall_3.Get<CMeshRenderer>().Draw();

	winBox.transform.RecomputeGlobal();
	winBox.Get<CMeshRenderer>().Draw();

	// Update Controller
	pc.Update(deltaTime);
	pc.Render();

	// Sample Collision Event
	if (Physics::SphereSphereCollision(body.Get<SphereCollider>(), shadow.Get<SphereCollider>()))
	{
		Physics::SphereSphereCollisionResponse(body.Get<SphereCollider>(), shadow.Get<SphereCollider>());
	}

	// Sample Trigger Event
	if (Physics::SphereSphereCollision(body.Get<SphereCollider>(), wall_2.Get<SphereCollider>()))
	{
		Physics::SphereSphereCollisionResponse(body.Get<SphereCollider>(), wall_2.Get<SphereCollider>());
	}

	// Win Condition
	if (Physics::SphereSphereCollision(body.Get<SphereCollider>(), winBox.Get<SphereCollider>()))
	{
		Physics::SphereSphereCollisionResponse(body.Get<SphereCollider>(), winBox.Get<SphereCollider>());
	}
}

void Level_3::Render() const
{

}

void Level_3::HandleEvents()
{

}

void Level_3::OnResizeWindow(int width, int height)
{

}

Level_3::~Level_3()
{
	OnDestroy();
}

void Level_3::OnDestroy()
{
	// Cleanup Assets	

}