#pragma once

#include "ResourceManager.h"
#include <map>

// Resource Types
#include "Gameplay/MeshResource.h"
#include "Gameplay/Material.h"
#include "Graphics/Textures/Texture1D.h"
#include "Graphics/Textures/Texture2D.h"
#include "Graphics/Textures/Texture3D.h"
#include "Graphics/Textures/TextureCube.h"
#include "Graphics/ShaderProgram.h"

class Resources
{

public: // Singleton Format 
	static Resources& Instance() { static Resources resources; return resources; }

protected: // Protected Vars
	std::map<std::string, ShaderProgram::Sptr>							shaders			= std::map<std::string, ShaderProgram::Sptr>();
	std::map<std::string, Gameplay::MeshResource::Sptr>					meshes			= std::map<std::string, Gameplay::MeshResource::Sptr>();
	std::map<std::string, Texture1D::Sptr>								textures_1D		= std::map<std::string, Texture1D::Sptr>();
	std::map<std::string, Texture2D::Sptr>								textures_2D		= std::map<std::string, Texture2D::Sptr>();
	std::map<std::string, Texture3D::Sptr>								textures_3D		= std::map<std::string, Texture3D::Sptr>();
	std::map<std::string, TextureCube::Sptr>							textures_Cube	= std::map<std::string, TextureCube::Sptr>();
	std::map<std::string, Gameplay::Material::Sptr>						materials		= std::map<std::string, Gameplay::Material::Sptr>();
	std::map<std::string, std::vector<Gameplay::MeshResource::Sptr>>	animations		= std::map<std::string, std::vector<Gameplay::MeshResource::Sptr>>();

protected: // Protected Funcs
	Resources() {};

public: // Public Vars
	int PHYSICAL_MASK = 0xFFFFFFFF;
	int PHYSICAL_GROUP = 0x01;
	int SHADOW_MASK = 0xFFFFFFFE;
	int SHADOW_GROUP = 0x02;
	int NO_MASK = 0xFFFFFFFD;
	int NO_GROUP = 0x03;

public: // Public Funcs

	void Initialize();

	static std::map<std::string, Gameplay::Material::Sptr> GetMaterialMap() { return Instance().materials; }

	static ShaderProgram::Sptr GetShader(std::string name);
	static Gameplay::MeshResource::Sptr GetMesh(std::string name);
	static Texture1D::Sptr GetTexture1D(std::string name);
	static Texture2D::Sptr GetTexture2D(std::string name);
	static Texture3D::Sptr GetTexture3D(std::string name);
	static TextureCube::Sptr GetCubeMap(std::string name);
	static Gameplay::Material::Sptr GetMaterial(std::string name);
	static std::vector<Gameplay::MeshResource::Sptr> GetAnimation(std::string name);

};


