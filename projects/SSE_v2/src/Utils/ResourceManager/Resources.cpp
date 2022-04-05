#include "Resources.h"

#include "Utils/GlmDefines.h"

// Audio Manager handles its own resources
#include "Audio/AudioManager.h"

using namespace Gameplay;

void Resources::Initialize()
{
#pragma region Shaders

	// Basic gbuffer generation with no vertex manipulation
	ShaderProgram::Sptr deferredForward = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
		{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
		{ ShaderPartType::Fragment, "shaders/fragment_shaders/deferred_forward.glsl" }
	});
	deferredForward->SetDebugName("Deferred - GBuffer Generation");
	shaders.emplace("Deferred Forward", deferredForward);

	// Our foliage shader which manipulates the vertices of the mesh
	ShaderProgram::Sptr foliageShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
		{ ShaderPartType::Vertex, "shaders/vertex_shaders/foliage.glsl" },
		{ ShaderPartType::Fragment, "shaders/fragment_shaders/deferred_forward.glsl" }
	});
	foliageShader->SetDebugName("Foliage");
	shaders.emplace("Foliage", foliageShader);

	// This shader handles our multitexturing example
	ShaderProgram::Sptr multiTextureShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
		{ ShaderPartType::Vertex, "shaders/vertex_shaders/vert_multitextured.glsl" },
		{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_multitextured.glsl" }
	});
	multiTextureShader->SetDebugName("Multitexturing");
	shaders.emplace("Multi-Texture", multiTextureShader);

	// This shader handles our displacement mapping example
	ShaderProgram::Sptr displacementShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
		{ ShaderPartType::Vertex, "shaders/vertex_shaders/displacement_mapping.glsl" },
		{ ShaderPartType::Fragment, "shaders/fragment_shaders/deferred_forward.glsl" }
	});
	displacementShader->SetDebugName("Displacement Mapping");
	shaders.emplace("Displacement", displacementShader);

	// This shader handles our cel shading example
	ShaderProgram::Sptr celShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
		{ ShaderPartType::Vertex, "shaders/vertex_shaders/displacement_mapping.glsl" },
		{ ShaderPartType::Fragment, "shaders/fragment_shaders/cel_shader.glsl" }
	});
	celShader->SetDebugName("Cel Shader");
	shaders.emplace("Cel", celShader);

	ShaderProgram::Sptr skyboxShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
		{ ShaderPartType::Vertex, "shaders/vertex_shaders/skybox_vert.glsl" },
		{ ShaderPartType::Fragment, "shaders/fragment_shaders/skybox_frag.glsl" }
	});
	skyboxShader->SetDebugName("Skybox");
	shaders.emplace("Skybox", skyboxShader);

#pragma endregion

#pragma region 1D Textures

	Texture1D::Sptr toonLut = ResourceManager::CreateAsset<Texture1D>("luts/toon-1D.png");
	{
		toonLut->SetWrap(WrapMode::ClampToEdge);
		textures_1D.emplace("Toon Lut", toonLut);
	}	

#pragma endregion

#pragma region 2D Textures
	
            textures_2D.emplace("Stone Wall", ResourceManager::CreateAsset<Texture2D>("textures/2D/StoneWallUVs.png"));
			textures_2D.emplace("Key", ResourceManager::CreateAsset<Texture2D>("textures/2D/KeyTextureUVS.png"));
			textures_2D.emplace("Grave Stone", ResourceManager::CreateAsset<Texture2D>("textures/2D/GraveStoneUVS.png"));
			textures_2D.emplace("CharacterTex", ResourceManager::CreateAsset<Texture2D>("textures/2D/CharacterTexture.png"));
			textures_2D.emplace("ShadowTex", ResourceManager::CreateAsset<Texture2D>("textures/2D/ShadowUVS.png"));
			textures_2D.emplace("Small Cage", ResourceManager::CreateAsset<Texture2D>("textures/2D/SmallCageUVS.png"));
			textures_2D.emplace("Pressure Plate", ResourceManager::CreateAsset<Texture2D>("textures/2D/PressurePlateUVs.png"));
			textures_2D.emplace("Spike Trap", ResourceManager::CreateAsset<Texture2D>("textures/2D/SpikedTrapUVS.png"));
			textures_2D.emplace("Character Dagger", ResourceManager::CreateAsset<Texture2D>("textures/2D/DaggerTexture.png"));
			textures_2D.emplace("WallGrate", ResourceManager::CreateAsset<Texture2D>("textures/2D/WallGrateUVS.png"));
			textures_2D.emplace("Floor Grate", ResourceManager::CreateAsset<Texture2D>("textures/2D/FloorGrateUVS.png"));
			textures_2D.emplace("Brown", ResourceManager::CreateAsset<Texture2D>("textures/2D/Brown.png"));
			textures_2D.emplace("ShieldTex", ResourceManager::CreateAsset<Texture2D>("textures/2D/ShieldTexture.png"));
			textures_2D.emplace("StaffTex", ResourceManager::CreateAsset<Texture2D>("textures/2D/StaffTexture.png"));
			textures_2D.emplace("SwordTex", ResourceManager::CreateAsset<Texture2D>("textures/2D/GoblinSwordUV.png"));
			textures_2D.emplace("RockPileTex", ResourceManager::CreateAsset<Texture2D>("textures/2D/Rock3Textures.png"));
			textures_2D.emplace("LeverTex", ResourceManager::CreateAsset<Texture2D>("textures/2D/LeverTextures.png"));
			textures_2D.emplace("Intact Pillar", ResourceManager::CreateAsset<Texture2D>("textures/2D/PillarUV.png"));
			textures_2D.emplace("Damaged Pillar", ResourceManager::CreateAsset<Texture2D>("textures/2D/DamagedPillarUV.png"));
			textures_2D.emplace("Destroyed Pillar", ResourceManager::CreateAsset<Texture2D>("textures/2D/DestroyedPillarUV.png"));
			textures_2D.emplace("Brown", ResourceManager::CreateAsset<Texture2D>("textures/2D/Brown.png"));
			textures_2D.emplace("StoneTex", ResourceManager::CreateAsset<Texture2D>("textures/2D/Stone.png"));
			textures_2D.emplace("Gray", ResourceManager::CreateAsset<Texture2D>("textures/2D/Gray.png"));
			textures_2D.emplace("cTex", ResourceManager::CreateAsset<Texture2D>("textures/2D/CobTex.png"));
			textures_2D.emplace("Shroom", ResourceManager::CreateAsset<Texture2D>("textures/2D/MushroomTexture.png"));
			textures_2D.emplace("Crystal", ResourceManager::CreateAsset<Texture2D>("textures/2D/CrystalTexture.png"));
			textures_2D.emplace("HealingWell", ResourceManager::CreateAsset<Texture2D>("textures/2D/HealingWellTexture.png"));


	//USER INTERFACE
			textures_2D.emplace("Upper", ResourceManager::CreateAsset<Texture2D>("textures/UI/UpperGraphic.png"));
			textures_2D.emplace("Lower", ResourceManager::CreateAsset<Texture2D>("textures/UI/LowerGraphic.png"));
			textures_2D.emplace("New", ResourceManager::CreateAsset<Texture2D>("textures/UI/NewGame.png"));
			textures_2D.emplace("Options", ResourceManager::CreateAsset<Texture2D>("textures/UI/Options.png"));
			textures_2D.emplace("ChooseLevel", ResourceManager::CreateAsset<Texture2D>("textures/UI/ChooseLevel.png"));
			textures_2D.emplace("Credits", ResourceManager::CreateAsset<Texture2D>("textures/UI/Credits.png"));
			textures_2D.emplace("Exit", ResourceManager::CreateAsset<Texture2D>("textures/UI/ExitGame.png"));
			textures_2D.emplace("R", ResourceManager::CreateAsset<Texture2D>("textures/UI/Resume.png"));
			textures_2D.emplace("Re", ResourceManager::CreateAsset<Texture2D>("textures/UI/Restart.png"));
			textures_2D.emplace("Return", ResourceManager::CreateAsset<Texture2D>("textures/UI/ReturntoMainMenu.png"));
			textures_2D.emplace("TitleT", ResourceManager::CreateAsset<Texture2D>("textures/UI/Title.png"));
			textures_2D.emplace("Pause", ResourceManager::CreateAsset<Texture2D>("textures/UI/Paused.png"));
			textures_2D.emplace("LS", ResourceManager::CreateAsset<Texture2D>("textures/UI/LogoScreen.png"));
			textures_2D.emplace("Lose", ResourceManager::CreateAsset<Texture2D>("textures/UI/LoseScreen.png"));
			textures_2D.emplace("CharacterH", ResourceManager::CreateAsset<Texture2D>("textures/UI/CharacterHealth.png"));
			textures_2D.emplace("ShadowH", ResourceManager::CreateAsset<Texture2D>("textures/UI/AntiformHealth.png"));


	// WEEK 9
	textures_2D.emplace("Box Diffuse", ResourceManager::CreateAsset<Texture2D>("textures/box-diffuse.png"));
	textures_2D.emplace("Box Specular", ResourceManager::CreateAsset<Texture2D>("textures/box-specular.png"));
	textures_2D.emplace("Monkey", ResourceManager::CreateAsset<Texture2D>("textures/monkey-uvMap.png"));
	Texture2D::Sptr leafTex = ResourceManager::CreateAsset<Texture2D>("textures/leaves.png");
	{
		leafTex->SetMinFilter(MinFilter::Nearest);
		leafTex->SetMagFilter(MagFilter::Nearest);
		textures_2D.emplace("Leaf", leafTex);
	}
	textures_2D.emplace("Flashlight", ResourceManager::CreateAsset<Texture2D>("textures/flashlight.png"));
	textures_2D.emplace("Flashlight 2", ResourceManager::CreateAsset<Texture2D>("textures/flashlight-2.png"));
	textures_2D.emplace("Light Projection", ResourceManager::CreateAsset<Texture2D>("textures/light_projection.png"));

	Texture2DDescription singlePixelDescriptor;
	singlePixelDescriptor.Width = singlePixelDescriptor.Height = 1;
	singlePixelDescriptor.Format = InternalFormat::RGB8;

	float normalMapDefaultData[3] = { 0.5f, 0.5f, 1.0f };
	Texture2D::Sptr normalMapDefault = ResourceManager::CreateAsset<Texture2D>(singlePixelDescriptor);
	{
		normalMapDefault->LoadData(1, 1, PixelFormat::RGB, PixelType::Float, normalMapDefaultData);
		textures_2D.emplace("Normal Map Default", normalMapDefault);
	}


	float solidGrey[3] = { 0.5f, 0.5f, 0.5f };
	float solidBlack[3] = { 0.0f, 0.0f, 0.0f };
	float solidWhite[3] = { 1.0f, 1.0f, 1.0f };

	Texture2D::Sptr solidBlackTex = ResourceManager::CreateAsset<Texture2D>(singlePixelDescriptor);
	{
		solidBlackTex->LoadData(1, 1, PixelFormat::RGB, PixelType::Float, solidBlack);
		textures_2D.emplace("Solid Black", solidBlackTex);
	}

	Texture2D::Sptr solidGreyTex = ResourceManager::CreateAsset<Texture2D>(singlePixelDescriptor);
	{
		solidGreyTex->LoadData(1, 1, PixelFormat::RGB, PixelType::Float, solidGrey);
		textures_2D.emplace("Solid Grey", solidGreyTex);
	}

	Texture2D::Sptr solidWhiteTex = ResourceManager::CreateAsset<Texture2D>(singlePixelDescriptor);
	{
		solidWhiteTex->LoadData(1, 1, PixelFormat::RGB, PixelType::Float, solidWhite);
		textures_2D.emplace("Solid White", solidWhiteTex);
	}

	textures_2D.emplace("Normal Map", ResourceManager::CreateAsset<Texture2D>("textures/normal_map.png"));
	textures_2D.emplace("Brick Diffuse Map", ResourceManager::CreateAsset<Texture2D>("textures/bricks_diffuse.png"));
	textures_2D.emplace("Sand", ResourceManager::CreateAsset<Texture2D>("textures/terrain/sand.png"));
	textures_2D.emplace("Grass", ResourceManager::CreateAsset<Texture2D>("textures/terrain/grass.png"));
	textures_2D.emplace("Displacement Map", ResourceManager::CreateAsset<Texture2D>("textures/displacement_map.png"));
	textures_2D.emplace("Diffuse Map", ResourceManager::CreateAsset<Texture2D>("textures/bricks_diffuse.png"));

#pragma endregion

#pragma region 3D Textures

	textures_3D.emplace("Cool Lut", ResourceManager::CreateAsset<Texture3D>("luts/cool.CUBE"));

#pragma endregion

#pragma region Cube Maps

	
	textures_Cube.emplace("Ocean", ResourceManager::CreateAsset<TextureCube>("cubemaps/ocean/ocean.jpg"));


#pragma endregion

#pragma region Materials

	// This will be our box material, with no environment reflections
	Material::Sptr boxMaterial = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		boxMaterial->Name = "Box";
		boxMaterial->Set("u_Material.AlbedoMap", GetTexture2D("Box Diffuse"));
		boxMaterial->Set("u_Material.Shininess", 0.1f);
		boxMaterial->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		
		materials.emplace("Box", boxMaterial);
	}

	// This will be the reflective material, we'll make the whole thing 90% reflective
	Material::Sptr monkeyMaterial = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		monkeyMaterial->Name = "Monkey";
		monkeyMaterial->Set("u_Material.AlbedoMap", GetTexture2D("Monkey"));
		monkeyMaterial->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		monkeyMaterial->Set("u_Material.Shininess", 0.5f);
		
		materials.emplace("Monkey", monkeyMaterial);
	}

	// This will be the reflective material, we'll make the whole thing 50% reflective
	Material::Sptr testMaterial = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		testMaterial->Name = "Box-Specular";
		testMaterial->Set("u_Material.AlbedoMap", GetTexture2D("Box Diffuse"));
		testMaterial->Set("u_Material.Specular", GetTexture2D("Box Specular"));
		testMaterial->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		
		materials.emplace("Test", testMaterial);
	}

	// Our foliage vertex shader material 
	Material::Sptr foliageMaterial = ResourceManager::CreateAsset<Material>(foliageShader);
	{
		foliageMaterial->Name = "Foliage Shader";
		foliageMaterial->Set("u_Material.AlbedoMap", GetTexture2D("Leaf"));
		foliageMaterial->Set("u_Material.Shininess", 0.1f);
		foliageMaterial->Set("u_Material.DiscardThreshold", 0.1f);
		foliageMaterial->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));

		foliageMaterial->Set("u_WindDirection", glm::vec3(1.0f, 1.0f, 0.0f));
		foliageMaterial->Set("u_WindStrength", 0.5f);
		foliageMaterial->Set("u_VerticalScale", 1.0f);
		foliageMaterial->Set("u_WindSpeed", 1.0f);
		
		materials.emplace("Foliage", foliageMaterial);
	}

	// Our toon shader material
	Material::Sptr characterMaterial = ResourceManager::CreateAsset<Material>(celShader);
	{
		characterMaterial->Name = "Character";
		characterMaterial->Set("u_Material.AlbedoMap", GetTexture2D("CharacterTex"));
		characterMaterial->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		characterMaterial->Set("s_ToonTerm", GetTexture1D("Toon Lut"));
		characterMaterial->Set("u_Material.Shininess", 0.1f);
		characterMaterial->Set("u_Material.Steps", 8);
		materials.emplace("Character", characterMaterial);
	}

	Material::Sptr shadowMat = ResourceManager::CreateAsset<Material>(celShader);
	{
		shadowMat->Name = "Shadow";
		shadowMat->Set("u_Material.AlbedoMap", GetTexture2D("ShadowTex"));
		shadowMat->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		shadowMat->Set("s_ToonTerm", GetTexture1D("Toon Lut"));
		shadowMat->Set("u_Material.Shininess", 0.1f);
		shadowMat->Set("u_Material.Steps", 8);
		materials.emplace("Shadow", shadowMat);
	}


	Material::Sptr displacementTest = ResourceManager::CreateAsset<Material>(displacementShader);
	{
		displacementTest->Name = "Displacement Map";
		displacementTest->Set("u_Material.AlbedoMap", GetTexture2D("Diffuse Map"));
		displacementTest->Set("u_Material.NormalMap", GetTexture2D("Normal Map"));
		displacementTest->Set("s_Heightmap", GetTexture2D("Displacement Map"));
		displacementTest->Set("u_Material.Shininess", 0.5f);
		displacementTest->Set("u_Scale", 0.1f);
		materials.emplace("Displacement", displacementTest);
	}

	Material::Sptr grey = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		grey->Name = "Grey";
		grey->Set("u_Material.AlbedoMap", GetTexture2D("Solid Grey"));
		grey->Set("u_Material.Specular", GetTexture2D("Solid Black"));
		grey->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));

		materials.emplace("Grey", grey);
	}

	Material::Sptr whiteBrick = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		whiteBrick->Name = "White Bricks";
		whiteBrick->Set("u_Material.AlbedoMap", GetTexture2D("Displacement Map"));
		whiteBrick->Set("u_Material.Specular", GetTexture2D("Solid Grey"));
		whiteBrick->Set("u_Material.NormalMap", GetTexture2D("Normal Map"));

		materials.emplace("White Brick", whiteBrick);
	}

	Material::Sptr normalmapMat = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		normalmapMat->Name = "Tangent Space Normal Map";
		normalmapMat->Set("u_Material.AlbedoMap", GetTexture2D("Brick Diffuse Map"));
		normalmapMat->Set("u_Material.NormalMap", GetTexture2D("Normal Map"));
		normalmapMat->Set("u_Material.Shininess", 0.5f);
		normalmapMat->Set("u_Scale", 0.1f);

		materials.emplace("Normal Map", normalmapMat);
	}

	Material::Sptr multiTextureMat = ResourceManager::CreateAsset<Material>(multiTextureShader);
	{
		multiTextureMat->Name = "Multitexturing";
		multiTextureMat->Set("u_Material.DiffuseA", GetTexture2D("Sand"));
		multiTextureMat->Set("u_Material.DiffuseB", GetTexture2D("Grass"));
		multiTextureMat->Set("u_Material.NormalMapA", GetTexture2D("Normal Map Default"));
		multiTextureMat->Set("u_Material.NormalMapB", GetTexture2D("Normal Map Default"));
		multiTextureMat->Set("u_Material.Shininess", 0.5f);
		multiTextureMat->Set("u_Scale", 0.1f);

		materials.emplace("Multi-Texture", std::move(multiTextureMat));
	}

	Material::Sptr stoneWallMat = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		stoneWallMat->Name = "Stone Wall";
		stoneWallMat->Set("u_Material.AlbedoMap", GetTexture2D("Stone Wall"));
		stoneWallMat->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		stoneWallMat->Set("u_Material.Shininess", 0.1f);

		materials.emplace("Stone Wall", std::move(stoneWallMat));
	}

	Material::Sptr pressurePlateMaterial = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		pressurePlateMaterial->Name = "Pressure Plate";
		pressurePlateMaterial->Set("u_Material.AlbedoMap", GetTexture2D("Pressure Plate"));
		pressurePlateMaterial->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		pressurePlateMaterial->Set("u_Material.Shininess", 0.1f);

		materials.emplace("Pressure Plate", std::move(pressurePlateMaterial));
	}

	Material::Sptr sCageMaterial = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		sCageMaterial->Name = "Small Cage";
		sCageMaterial->Set("u_Material.AlbedoMap", GetTexture2D("Small Cage"));
		sCageMaterial->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		sCageMaterial->Set("u_Material.Shininess", 0.1f);

		materials.emplace("Small Cage", std::move(sCageMaterial));
	}

	Material::Sptr spikeTrapMat = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		spikeTrapMat->Name = "Spike Trap";
		spikeTrapMat->Set("u_Material.AlbedoMap", GetTexture2D("Spike Trap"));
		spikeTrapMat->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		spikeTrapMat->Set("u_Material.Shininess", 0.1f);

		materials.emplace("Spike Trap", std::move(spikeTrapMat));
	}


	Material::Sptr graveStoneMat = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		graveStoneMat->Name = "Grave Stone";
		graveStoneMat->Set("u_Material.AlbedoMap", GetTexture2D("Grave Stone"));
		graveStoneMat->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		graveStoneMat->Set("u_Material.Shininess", 0.1f);

		materials.emplace("Grave Stone", std::move(graveStoneMat));
	}

	Material::Sptr dagger = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		dagger->Name = "Character Dagger";
		dagger->Set("u_Material.AlbedoMap", GetTexture2D("Character Dagger"));
		dagger->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		dagger->Set("u_Material.Shininess", 0.1f);

		materials.emplace("Character Dagger", std::move(dagger));
	}

	Material::Sptr WGrate = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		WGrate->Name = "WallGrate";
		WGrate->Set("u_Material.AlbedoMap", GetTexture2D("WallGrate"));
		WGrate->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		WGrate->Set("u_Material.Shininess", 0.1f);

		materials.emplace("WallGrate", std::move(WGrate));
	}


	Material::Sptr FGrate = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		FGrate->Name = "Floor Grate";
		FGrate->Set("u_Material.AlbedoMap", GetTexture2D("Floor Grate"));
		FGrate->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		FGrate->Set("u_Material.Shininess", 0.1f);

		materials.emplace("Floor Grate", std::move(FGrate));
	}


	Material::Sptr keyMat = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		keyMat->Name = "Key";
		keyMat->Set("u_Material.AlbedoMap", GetTexture2D("Key"));
		keyMat->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		keyMat->Set("u_Material.Shininess", 0.1f);

		materials.emplace("Key", std::move(keyMat));
	}

	Material::Sptr whiteMat = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		whiteMat->Name = "White";
		whiteMat->Set("u_Material.AlbedoMap", GetTexture2D("White"));
		whiteMat->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		whiteMat->Set("u_Material.Shininess", 0.1f);

		materials.emplace("White", std::move(whiteMat));
	}

	Material::Sptr grayMat = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		grayMat->Name = "Gray";
		grayMat->Set("u_Material.AlbedoMap", GetTexture2D("Gray"));
		grayMat->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		grayMat->Set("u_Material.Shininess", 0.1f);

		materials.emplace("Gray", std::move(grayMat));
	}

	Material::Sptr brownMat = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		brownMat->Name = "Brown";
		brownMat->Set("u_Material.AlbedoMap", GetTexture2D("Brown"));
		brownMat->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		brownMat->Set("u_Material.Shininess", 0.1f);

		materials.emplace("Brown", std::move(brownMat));
	}

	Material::Sptr stone1Mat = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		stone1Mat->Name = "StoneTex";
		stone1Mat->Set("u_Material.AlbedoMap", GetTexture2D("StoneTex"));
		stone1Mat->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		stone1Mat->Set("u_Material.Shininess", 0.1f);

		materials.emplace("StoneTex", std::move(stone1Mat));
	}


	Material::Sptr shieldMat = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		shieldMat->Name = "ShieldTex";
		shieldMat->Set("u_Material.AlbedoMap", GetTexture2D("ShieldTex"));
		shieldMat->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		shieldMat->Set("u_Material.Shininess", 0.1f);

		materials.emplace("ShieldTex", std::move(shieldMat));
	}

	Material::Sptr staffMat = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		staffMat->Name = "StaffTex";
		staffMat->Set("u_Material.AlbedoMap", GetTexture2D("StaffTex"));
		staffMat->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		staffMat->Set("u_Material.Shininess", 0.1f);

		materials.emplace("StaffTex", std::move(staffMat));
	}

	Material::Sptr swordMat = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		swordMat->Name = "SwordTex";
		swordMat->Set("u_Material.AlbedoMap", GetTexture2D("SwordTex"));
		swordMat->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		swordMat->Set("u_Material.Shininess", 0.1f);

		materials.emplace("SwordTex", std::move(swordMat));
	}

	Material::Sptr leverMat = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		leverMat->Name = "LeverTex";
		leverMat->Set("u_Material.AlbedoMap", GetTexture2D("LeverTex"));
		leverMat->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		leverMat->Set("u_Material.Shininess", 0.1f);

		materials.emplace("LeverTex", std::move(leverMat));
	}

	Material::Sptr intactPillarMat = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		intactPillarMat->Name = "Intact Pillar";
		intactPillarMat->Set("u_Material.AlbedoMap", GetTexture2D("Intact Pillar"));
		intactPillarMat->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		intactPillarMat->Set("u_Material.Shininess", 0.1f);

		materials.emplace("Intact Pillar", std::move(intactPillarMat));
	}

	Material::Sptr damagedPillarMat = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		damagedPillarMat->Name = "Damaged Pillar";
		damagedPillarMat->Set("u_Material.AlbedoMap", GetTexture2D("Damaged Pillar"));
		damagedPillarMat->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		damagedPillarMat->Set("u_Material.Shininess", 0.1f);

		materials.emplace("Damaged Pillar", std::move(damagedPillarMat));
	}

	Material::Sptr destroyedPillarMat = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		destroyedPillarMat->Name = "Destroyed Pillar";
		destroyedPillarMat->Set("u_Material.AlbedoMap", GetTexture2D("Destroyed Pillar"));
		destroyedPillarMat->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		destroyedPillarMat->Set("u_Material.Shininess", 0.1f);

		materials.emplace("Destroyed Pillar", std::move(destroyedPillarMat));
	}

	Material::Sptr rockPileMat = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		rockPileMat->Name = "Rock Pile";
		rockPileMat->Set("u_Material.AlbedoMap", GetTexture2D("RockPileTex"));
		rockPileMat->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		rockPileMat->Set("u_Material.Shininess", 0.1f);

		materials.emplace("Rock Pile", std::move(rockPileMat));
	}

	Material::Sptr cobwebMat = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		cobwebMat->Name = "CobWeb";
		cobwebMat->Set("u_Material.AlbedoMap", GetTexture2D("cTex"));
		cobwebMat->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		cobwebMat->Set("u_Material.Shininess", 0.1f);

		materials.emplace("CobWeb", std::move(cobwebMat));
	}

	Material::Sptr singleMushroomMat = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		singleMushroomMat->Name = "SS";
		singleMushroomMat->Set("u_Material.AlbedoMap", GetTexture2D("Shroom"));
		singleMushroomMat->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		singleMushroomMat->Set("u_Material.Shininess", 0.1f);

		materials.emplace("SS", std::move(singleMushroomMat));
	}

	Material::Sptr multiMushroomMat = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		multiMushroomMat->Name = "MS";
		multiMushroomMat->Set("u_Material.AlbedoMap", GetTexture2D("Shroom"));
		multiMushroomMat->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		multiMushroomMat->Set("u_Material.Shininess", 0.1f);

		materials.emplace("MS", std::move(multiMushroomMat));
	}

	Material::Sptr CrystalMat = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		CrystalMat->Name = "Crystal";
		CrystalMat->Set("u_Material.AlbedoMap", GetTexture2D("Crystal"));
		CrystalMat->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		CrystalMat->Set("u_Material.Shininess", 0.1f);

		materials.emplace("Crystal", std::move(CrystalMat));
	}

	Material::Sptr heal = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		heal->Name = "HealingWell";
		heal->Set("u_Material.AlbedoMap", GetTexture2D("HealingWell"));
		heal->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		heal->Set("u_Material.Shininess", 0.1f);

		materials.emplace("HealingWell", std::move(heal));
	}

	for (auto& mat : materials)
	{
		if (mat.second->GetShader()->GetDebugName() == "Deferred - GBuffer Generation") 
		{
			mat.second->Set("u_Lights.Color", glm::vec3(1, 1, 1));
			mat.second->Set("u_Lights.ToggleAmbience", true);
			mat.second->Set("u_Lights.AmbienceStrength", 1.0f);
			mat.second->Set("u_Lights.ToggleDiffuse", true);
			mat.second->Set("u_Lights.ToggleSpecular", true);
			mat.second->Set("u_Lights.SpecularStrength", 1.0f);
			mat.second->Set("u_Lights.ToggleInversion", false); 
		}
	}

#pragma endregion

#pragma region Meshes

	        meshes.emplace("Character", ResourceManager::CreateAsset<MeshResource>("meshes/Character.obj"));
			meshes.emplace("HealingWell", ResourceManager::CreateAsset<MeshResource>("meshes/HealingWell.obj"));
			meshes.emplace("Crystal", ResourceManager::CreateAsset<MeshResource>("meshes/Crystal.obj"));
			meshes.emplace("MS", ResourceManager::CreateAsset<MeshResource>("meshes/Mushrooms.obj"));
			meshes.emplace("SS", ResourceManager::CreateAsset<MeshResource>("meshes/SingleMushroom.obj"));
			meshes.emplace("Grave Stone", ResourceManager::CreateAsset<MeshResource>("meshes/GraveStone.obj"));
			meshes.emplace("Key", ResourceManager::CreateAsset<MeshResource>("meshes/key.obj"));
			meshes.emplace("CobWeb", ResourceManager::CreateAsset<MeshResource>("meshes/CobWeb.obj"));
			meshes.emplace("Stone Wall", ResourceManager::CreateAsset<MeshResource>("meshes/StoneWall.obj"));
			meshes.emplace("Small Cage", ResourceManager::CreateAsset<MeshResource>("meshes/SCage.obj"));
			meshes.emplace("Pressure Plate", ResourceManager::CreateAsset<MeshResource>("meshes/PressurePlate.obj"));
			meshes.emplace("Spike Trap", ResourceManager::CreateAsset<MeshResource>("meshes/SpikedTrap.obj"));
			meshes.emplace("Character Dagger", ResourceManager::CreateAsset<MeshResource>("meshes/CharacterDagger.obj"));
			meshes.emplace("WallGrate", ResourceManager::CreateAsset<MeshResource>("meshes/WallGrate.obj"));
			meshes.emplace("Floor Grate", ResourceManager::CreateAsset<MeshResource>("meshes/FloorGrate.obj"));
			meshes.emplace("Cube", ResourceManager::CreateAsset<MeshResource>("meshes/Cube.obj"));
			meshes.emplace("Damaged Pillar", ResourceManager::CreateAsset<MeshResource>("meshes/DamagedPillar.obj"));
			meshes.emplace("Destroyed Pillar", ResourceManager::CreateAsset<MeshResource>("meshes/DestroyedPillar.obj"));
			meshes.emplace("Intact Pillar", ResourceManager::CreateAsset<MeshResource>("meshes/IntactPillar.obj"));
			meshes.emplace("Rock Pile", ResourceManager::CreateAsset<MeshResource>("meshes/RockPile3.obj"));
			meshes.emplace("Wall2", ResourceManager::CreateAsset<MeshResource>("meshes/Wall2.obj"));
			meshes.emplace("Shield", ResourceManager::CreateAsset<MeshResource>("meshes/GoblinShield.obj"));
			meshes.emplace("Staff", ResourceManager::CreateAsset<MeshResource>("meshes/GoblinStaff.obj"));
			meshes.emplace("Sword", ResourceManager::CreateAsset<MeshResource>("meshes/GoblinSword.obj"));
			meshes.emplace("Lever", ResourceManager::CreateAsset<MeshResource>("meshes/Lever.obj"));
	

	MeshResource::Sptr planeMesh = ResourceManager::CreateAsset<MeshResource>();
	{
		planeMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(1.0f)));
		planeMesh->GenerateMesh();
		meshes.emplace("Plane", planeMesh);
	}
	

	MeshResource::Sptr sphere = ResourceManager::CreateAsset<MeshResource>();
	{
		sphere->AddParam(MeshBuilderParam::CreateIcoSphere(ZERO, ONE, 5));
		sphere->GenerateMesh();
		meshes.emplace("Sphere", sphere);
	}
	

#pragma endregion

#pragma region Animations

	std::vector<MeshResource::Sptr> WalkAnimation;
	{
		for (int i = 0; i < 6; ++i)
		{
			std::string file;
			file.append("animations/character_walk/Walk");
			file.append(std::to_string((i + 1)));
			file.append(".obj");
			WalkAnimation.push_back(ResourceManager::CreateAsset<MeshResource>(file));
		}

		animations.emplace("Character Walk", std::move(WalkAnimation));
	}

	std::vector<MeshResource::Sptr> IdleAnimation;
	{
		for (int i = 0; i < 5; ++i)
		{
			std::string file;
			file.append("animations/character_idle/Idle");
			file.append(std::to_string((i + 1)));
			file.append(".obj");
			IdleAnimation.push_back(ResourceManager::CreateAsset<MeshResource>(file));
		}

		animations.emplace("Character Idle", std::move(IdleAnimation));
	}

	std::vector<MeshResource::Sptr> JumpAnimation;
	{
		for (int i = 0; i < 6; ++i)
		{
			std::string file;
			file.append("animations/character_jump/Jump");
			file.append(std::to_string((i + 1)));
			file.append(".obj");
			IdleAnimation.push_back(ResourceManager::CreateAsset<MeshResource>(file));
		}

		animations.emplace("Character Jump", std::move(JumpAnimation));
	}

	std::vector<MeshResource::Sptr> DoorAnimation;
	{
		for (int i = 0; i < 6; ++i)
		{
			std::string file;
			file.append("animations/wallGrate/WallGrateOpen");
			file.append(std::to_string((i + 1)));
			file.append(".obj");
			DoorAnimation.push_back(ResourceManager::CreateAsset<MeshResource>(file));
		}

		animations.emplace("Door", std::move(DoorAnimation));
	}


	std::vector<MeshResource::Sptr> SpikeAnimation;
	{
		for (int i = 0; i < 5; ++i)
		{
			std::string file;
			file.append("animations/spikedTrap/SpikedTrap");
			file.append(std::to_string((i + 1)));
			file.append(".obj");
			SpikeAnimation.push_back(ResourceManager::CreateAsset<MeshResource>(file));
		}

		animations.emplace("Spikes", std::move(SpikeAnimation));
	}

#pragma endregion

#pragma region Audio 

	// Initialize the Audio Engine Resources
	AudioEngine& engine = AudioEngine::Instance();
	engine.Init();
	engine.LoadBankWithString("Master");

	engine.LoadBank("SFX");
	engine.LoadBus("Character", "bus:/SFX/Character");
	engine.CreateSoundEvent("Jump", "event:/Character/Jump");
	engine.CreateSoundEvent("Walk", "event:/Character/Player Footsteps");
	engine.CreateSoundEvent("Interact", "event:/Character/Interact");
	engine.CreateSoundEvent("Swap", "event:/Character/Shadow Swap");

	engine.LoadBus("Interactables", "bus:/SFX/Objects");
	engine.CreateSoundEvent("Key", "event:/Interactables/Key");
	engine.CreateSoundEvent("Spikes", "event:/Interactables/Spikes");
	engine.CreateSoundEvent("Pressure Plate", "event:/Interactables/Pressure Plate");
	engine.CreateSoundEvent("Lever", "event:/Interactables/Lever");
	engine.CreateSoundEvent("Door", "event:/Interactables/Door Open");

	engine.LoadBank("Music");
	engine.LoadBus("Music", "bus:/Music");
	engine.CreateSoundEvent("Test", "event:/Music/Level 02");
	engine.CreateSoundEvent("Mohit", "event:/Music/Mohit");

#pragma endregion
}

ShaderProgram::Sptr Resources::GetShader(std::string name)
{
	std::map<std::string, ShaderProgram::Sptr>::iterator it;
	it = Instance().shaders.find(name);

	if (it != Instance().shaders.end())
	{
		return it->second;
	}

	else {
		std::cout << "ERROR <nullptr>: " << name << " not found in Shaders.";
		return nullptr;
	}
}

MeshResource::Sptr Resources::GetMesh(std::string name)
{
	std::map<std::string, MeshResource::Sptr>::iterator it;
	it = Instance().meshes.find(name);

	if (it != Instance().meshes.end())
	{
		return it->second;
	}

	else {
		std::cout << "ERROR <nullptr>: " << name << " not found in Meshes.";
		return nullptr;
	}
}

Texture1D::Sptr Resources::GetTexture1D(std::string name)
{
	std::map<std::string, Texture1D::Sptr>::iterator it;
	it = Instance().textures_1D.find(name);

	if (it != Instance().textures_1D.end())
	{
		return it->second;
	}

	else {
		std::cout << "ERROR <nullptr>: " << name << " not found in Textures_1D.";
		return nullptr;
	}
}

Texture2D::Sptr Resources::GetTexture2D(std::string name)
{
	std::map<std::string, Texture2D::Sptr>::iterator it;
	it = Instance().textures_2D.find(name);

	if (it != Instance().textures_2D.end())
	{
		return it->second;
	}

	else {
		std::cout << "ERROR <nullptr>: " << name << " not found in Textures_2D.";
		return nullptr;
	}
}

Texture3D::Sptr Resources::GetTexture3D(std::string name)
{
	std::map<std::string, Texture3D::Sptr>::iterator it;
	it = Instance().textures_3D.find(name);

	if (it != Instance().textures_3D.end())
	{
		return it->second;
	}

	else {
		std::cout << "ERROR <nullptr>: " << name << " not found in Textures_2D.";
		return nullptr;
	}
}

TextureCube::Sptr Resources::GetCubeMap(std::string name)
{
	std::map<std::string, TextureCube::Sptr>::iterator it;
	it = Instance().textures_Cube.find(name);

	if (it != Instance().textures_Cube.end())
	{
		return it->second;
	}

	else {
		std::cout << "ERROR <nullptr>: " << name << " not found in CubeMaps.";
		return nullptr;
	}
}

Material::Sptr Resources::GetMaterial(std::string name)
{
	std::map<std::string, Material::Sptr>::iterator it;
	it = Instance().materials.find(name);

	if (it != Instance().materials.end())
	{
		return it->second;
	}

	else {
		std::cout << "ERROR <nullptr>: " << name << " not found in Materials.";
		return nullptr;
	}
}

std::vector<MeshResource::Sptr> Resources::GetAnimation(std::string name)
{
	std::map<std::string, std::vector<MeshResource::Sptr>>::iterator it;
	it = Instance().animations.find(name);

	if (it != Instance().animations.end())
	{
		return it->second;
	}

	else {
		std::cout << "ERROR <nullptr>: " << name << " not found in Animations";
		return std::vector<MeshResource::Sptr>();
	}
}

