#pragma once
#include "IComponent.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Physics/RigidBody.h"

class UpdatedBoxCollider : public Gameplay::IComponent 
{
public:
	typedef std::shared_ptr<UpdatedBoxCollider> Sptr;

	UpdatedBoxCollider();
	UpdatedBoxCollider(glm::vec3 extents);
	virtual ~UpdatedBoxCollider();

	virtual void Awake() override;
	virtual void Update(float deltaTime) override;

public:
	virtual void RenderImGui() override;
	MAKE_TYPENAME(UpdatedBoxCollider);
	virtual nlohmann::json ToJson() const override;
	static UpdatedBoxCollider::Sptr FromJson(const nlohmann::json& blob);

public:
	void UpdatePosition()
	{
		glm::vec3 p = GetGameObject()->GetPosition();
		glm::vec3 e = m_extents;

		/* Vertex Pts
		0 - Left/Bottom/Back = Vec3(p-e.x, p-e.y, p-e.z)
		1 - Right/Bottom/Back = Vec3(p+e.x, p-e.y, p-e.z)
		2 - Left/Top/Back = Vec3(p-e.x, p+e.y, p-e.z)
		3 - Right/Top/Back = Vec3(p+e.x, p+e.y, p-e.z)
		4 - Left/Bottom/Front = Vec3(p-e.x, p-e.y, p+e.z)
		5 - Right/Bottom/Front = Vec3(p+e.x, p-e.y, p+e.z)
		6 - Left/Top/Front = Vec3(p-e.x, p+e.y, p+e.z)
		7 - Right/Top/Front = Vec3(p+e.x, p+e.y, p+e.z)
		*/

		vertices[0] = glm::vec3(p.x - e.x, p.y - e.y, p.z - e.z);
		vertices[1] = glm::vec3(p.x + e.x, p.y - e.y, p.z - e.z);
		vertices[2] = glm::vec3(p.x - e.x, p.y + e.y, p.z - e.z);
		vertices[3] = glm::vec3(p.x + e.x, p.y + e.y, p.z - e.z);
		vertices[4] = glm::vec3(p.x - e.x, p.y - e.y, p.z + e.z);
		vertices[5] = glm::vec3(p.x + e.x, p.y - e.y, p.z + e.z);
		vertices[6] = glm::vec3(p.x - e.x, p.y + e.y, p.z + e.z);
		vertices[7] = glm::vec3(p.x + e.x, p.y + e.y, p.z + e.z);
	}

protected:
	// variables
	glm::vec3 m_extents;
	glm::vec3 vertices[8];

	bool isTrigger = false;
	std::function<void()> onTriggerEvent;
};