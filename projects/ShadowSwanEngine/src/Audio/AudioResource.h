#pragma once

#include "fmod.hpp"
#include <string>

#include "Utils/ResourceManagement/IResource.h"

class AudioResource : public IResource
{
public:
	typedef std::shared_ptr<AudioResource> Sptr;
	
	std::string Name;
	std::string Filepath;
	FMOD::Sound* Sound = nullptr;

	AudioResource() {}
	AudioResource(const std::string& filepath);
	
	virtual nlohmann::json ToJson() const override;
	static AudioResource::Sptr FromJson(const nlohmann::json& blob);
};