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
	AudioResource(const std::string& filepath)
		: Name("null"), Filepath("null")
	{
		// Extract name from filepath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		std::string filename = filepath.substr(lastSlash, count);

		Name = filename;
		Filepath = filepath;
	}
	
	virtual nlohmann::json ToJson() const override
	{
		return
		{
			{ "name", Name },
			{ "filepath", Filepath }
		};
	}

	static AudioResource::Sptr FromJson(const nlohmann::json& blob)
	{
		AudioResource::Sptr result = std::make_shared<AudioResource>();
		result->Name = blob["name"];
		result->Filepath = blob["filepath"];

		return result;
	}

	
};