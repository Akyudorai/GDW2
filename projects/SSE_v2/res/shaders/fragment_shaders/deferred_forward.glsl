#version 430

#include "../fragments/fs_common_inputs.glsl"

// We output a single color to the color buffer
layout(location = 0) out vec4 albedo_specPower;
layout(location = 1) out vec4 normal_metallic;
layout(location = 2) out vec4 emissive;
layout(location = 3) out vec3 view_pos;

// Represents a collection of attributes that would define a material
// For instance, you can think of this like material settings in 
// Unity
struct Material {
	sampler2D AlbedoMap;
	sampler2D EmissiveMap;
	sampler2D NormalMap;
	sampler2D MetallicShininessMap;
	float     DiscardThreshold;
};

struct Lights {
	vec3 Color;

	bool	ToggleAmbience;
	float	AmbienceStrength;
	bool	ToggleDiffuse;
	bool	ToggleSpecular;
	float	SpecularStrength;

	bool ToggleInversion;
};

// Create a uniform for the material
uniform Material u_Material;
uniform Lights u_Lights;

#include "../fragments/frame_uniforms.glsl"

// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
void main() {
	// Get albedo from the material
	vec4 albedoColor = texture(u_Material.AlbedoMap, inUV);

	// ======================================================

	vec3 result = vec3(0, 0, 0);
	vec3 lightDir = vec3(1, 0, 0);

	// Ambient
	if (u_Lights.ToggleAmbience) {		
		vec3 ambient = u_Lights.AmbienceStrength * u_Lights.Color; 
		result += ambient;
	}

	// Diffuse
	if (u_Lights.ToggleDiffuse)
	{
		float diff = max(dot(inNormal.xyz, lightDir), 0.0);
		vec3 diffuse = diff * u_Lights.Color;
		result += diffuse;
	}

	// Specular
	if (u_Lights.ToggleSpecular)
	{
		vec3 viewDir = normalize(inViewPos);
		vec3 reflectDir = reflect(-lightDir, inNormal.xyz);

		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		vec3 specular = u_Lights.SpecularStrength * spec * u_Lights.Color;
		result += specular;
	}
		
	// Inversion
	if (u_Lights.ToggleInversion)
	{
		albedoColor = vec4(vec3(1.0 - texture(u_Material.AlbedoMap, inUV)), 1.0);
	}

	// RESULT
	albedoColor *= vec4(result, 1.0);

	// ======================================================

	// We can use another texture to store things like our lighting settings
	vec4 lightingParams = texture(u_Material.MetallicShininessMap, inUV);

	// Discarding fragments who's alpha is below the material's threshold
	if (albedoColor.a < u_Material.DiscardThreshold) {
		discard;
	}

	// Extract albedo from material, and store shininess
	albedo_specPower = vec4(albedoColor.rgb, 1.0f);//lightingParams.x);
	
	// Normalize our input normal
    // Read our tangent from the map, and convert from the [0,1] range to [-1,1] range
    vec3 normal = texture(u_Material.NormalMap, inUV).rgb;
    normal = normal * 2.0 - 1.0;

    // Here we apply the TBN matrix to transform the normal from tangent space to view space
    normal = normalize(inTBN * normal);
	
	// Map [-1, 1] to [0, 1]
	normal = clamp((normal + 1) / 2.0, 0, 1);
	normal_metallic = vec4(normal, lightingParams.y);

	// Extract emissive from the material
	emissive = texture(u_Material.EmissiveMap, inUV);

	view_pos = inViewPos;
}