#include "AudioResource.h"
#include "AudioManager.h"

AudioResource::AudioResource(const std::string& filepath)
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

nlohmann::json AudioResource::ToJson() const
{
	return
	{
		{ "name", Name },
		{ "filepath", Filepath }
	};
}

AudioResource::Sptr AudioResource::FromJson(const nlohmann::json& blob)
{
	AudioResource::Sptr result = std::make_shared<AudioResource>();
	result->Name = blob["name"];
	result->Filepath = blob["filepath"];

	return result;
}