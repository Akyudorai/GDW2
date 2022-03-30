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
	textures_2D.emplace("Stone Slab 2", ResourceManager::CreateAsset<Texture2D>("textures/2D/StoneSlabsTexture.png"));
	textures_2D.emplace("Dark Pine Tree", ResourceManager::CreateAsset<Texture2D>("textures/2D/DarkPineTreeUVS.png"));
	textures_2D.emplace("Light Pine Tree", ResourceManager::CreateAsset<Texture2D>("textures/2D/LightTreeTextureUVS.png"));
	textures_2D.emplace("Turret Projectile", ResourceManager::CreateAsset<Texture2D>("textures/2D/TurretProjectileUVS.png"));
	textures_2D.emplace("Key", ResourceManager::CreateAsset<Texture2D>("textures/2D/KeyTextureUVS.png"));
	textures_2D.emplace("Standing Torch", ResourceManager::CreateAsset<Texture2D>("textures/2D/TorchUVS.png"));
	textures_2D.emplace("Grave Stone", ResourceManager::CreateAsset<Texture2D>("textures/2D/GraveStoneUVS.png"));
	textures_2D.emplace("Character", ResourceManager::CreateAsset<Texture2D>("textures/2D/CharacterUVS.png"));
	textures_2D.emplace("Shadow", ResourceManager::CreateAsset<Texture2D>("textures/2D/ShadowUVS.png"));
	textures_2D.emplace("Circle Cage", ResourceManager::CreateAsset<Texture2D>("textures/2D/CircleCageUVS.png"));
	textures_2D.emplace("Box Cage", ResourceManager::CreateAsset<Texture2D>("textures/2D/BoxcageUVS.png"));
	textures_2D.emplace("Long Cage", ResourceManager::CreateAsset<Texture2D>("textures/2D/LongCageUVS.png"));
	textures_2D.emplace("Small Cage", ResourceManager::CreateAsset<Texture2D>("textures/2D/SmallCageUVS.png"));
	textures_2D.emplace("U Cage", ResourceManager::CreateAsset<Texture2D>("textures/2D/UCageUVS.png"));
	textures_2D.emplace("Pressure Plate", ResourceManager::CreateAsset<Texture2D>("textures/2D/PressurePlateUVs.png"));
	textures_2D.emplace("Turret", ResourceManager::CreateAsset<Texture2D>("textures/2D/TurretUVS.png"));
	textures_2D.emplace("Door", ResourceManager::CreateAsset<Texture2D>("textures/2D/DoorUVs.png"));
	textures_2D.emplace("Spike Trap", ResourceManager::CreateAsset<Texture2D>("textures/2D/SpikedTrapUVS.png"));
	textures_2D.emplace("Relic 1", ResourceManager::CreateAsset<Texture2D>("textures/2D/StoneTablet1UVS.png"));
	textures_2D.emplace("Relic 2", ResourceManager::CreateAsset<Texture2D>("textures/2D/StoneTablet2UVS.png"));
	textures_2D.emplace("Relic 3", ResourceManager::CreateAsset<Texture2D>("textures/2D/StoneTablet3UVS.png"));
	textures_2D.emplace("Relic 4", ResourceManager::CreateAsset<Texture2D>("textures/2D/StoneTablet4UVS.png"));
	textures_2D.emplace("Relic 5", ResourceManager::CreateAsset<Texture2D>("textures/2D/StoneTablet5UVS.png"));
	textures_2D.emplace("Character Dagger", ResourceManager::CreateAsset<Texture2D>("textures/2D/DaggerTexture.png"));
	textures_2D.emplace("Health Orb", ResourceManager::CreateAsset<Texture2D>("textures/2D/HealthTexture.png"));
	textures_2D.emplace("Wall Grate", ResourceManager::CreateAsset<Texture2D>("textures/2D/WallGrateUVS.png"));
	textures_2D.emplace("Floor Grate", ResourceManager::CreateAsset<Texture2D>("textures/2D/FloorGrateUVS.png"));
	textures_2D.emplace("Large Candle", ResourceManager::CreateAsset<Texture2D>("textures/2D/LargeCandleTexture.png"));
	textures_2D.emplace("Wall2", ResourceManager::CreateAsset<Texture2D>("textures/2D/Wall_Tex2.png"));
	textures_2D.emplace("Wall_Tex3", ResourceManager::CreateAsset<Texture2D>("textures/2D/Wall_Tex3.png"));
	textures_2D.emplace("Wall_Tex4", ResourceManager::CreateAsset<Texture2D>("textures/2D/Wall_Tex4.png"));
	textures_2D.emplace("Wall_Tex5", ResourceManager::CreateAsset<Texture2D>("textures/2D/Wall_Tex5.png"));
	textures_2D.emplace("ShieldTex", ResourceManager::CreateAsset<Texture2D>("textures/2D/ShieldTexture.png"));
	textures_2D.emplace("StaffTex", ResourceManager::CreateAsset<Texture2D>("textures/2D/StaffTexture.png"));
	textures_2D.emplace("SwordTex", ResourceManager::CreateAsset<Texture2D>("textures/2D/GoblinSwordUV.png"));
	textures_2D.emplace("RockPileTex", ResourceManager::CreateAsset<Texture2D>("textures/2D/Rock3Textures.png"));
	textures_2D.emplace("LeverTex", ResourceManager::CreateAsset<Texture2D>("textures/2D/LeverTextures.png"));
	textures_2D.emplace("Intact Pillar", ResourceManager::CreateAsset<Texture2D>("textures/2D/PillarUV.png"));
	textures_2D.emplace("Damaged Pillar", ResourceManager::CreateAsset<Texture2D>("textures/2D/DamagedPillarUV.png"));
	textures_2D.emplace("Destroyed Pillar", ResourceManager::CreateAsset<Texture2D>("textures/2D/DestroyedPillarUV.png"));
	textures_2D.emplace("Brown", ResourceManager::CreateAsset<Texture2D>("textures/2D/Brown.png"));
	textures_2D.emplace("Gray", ResourceManager::CreateAsset<Texture2D>("textures/2D/Gray.png"));
	textures_2D.emplace("White", ResourceManager::CreateAsset<Texture2D>("textures/2D/White.png"));
	textures_2D.emplace("GrassTex", ResourceManager::CreateAsset<Texture2D>("textures/2D/Grass.png"));
	textures_2D.emplace("StoneTex", ResourceManager::CreateAsset<Texture2D>("textures/2D/Stone.png"));
	textures_2D.emplace("AbyssTex", ResourceManager::CreateAsset<Texture2D>("textures/2D/Abyss.png"));
	textures_2D.emplace("BarrelTex", ResourceManager::CreateAsset<Texture2D>("textures/2D/BarrelTexture.png"));
	textures_2D.emplace("BucketTex", ResourceManager::CreateAsset<Texture2D>("textures/2D/Bucket_Tex.png"));
	textures_2D.emplace("KegTex", ResourceManager::CreateAsset<Texture2D>("textures/2D/Keg_Tex.png"));

	//USER INTERFACE
	textures_2D.emplace("Menu Gloss", ResourceManager::CreateAsset<Texture2D>("textures/UI/MenuGloss.png"));
	textures_2D.emplace("Menu Gloss Reverse", ResourceManager::CreateAsset<Texture2D>("textures/UI/MenuGlossReverse.png"));
	textures_2D.emplace("Upper", ResourceManager::CreateAsset<Texture2D>("textures/UI/UpperGraphic.png"));
	textures_2D.emplace("Lower", ResourceManager::CreateAsset<Texture2D>("textures/UI/LowerGraphic.png"));
	textures_2D.emplace("Slider", ResourceManager::CreateAsset<Texture2D>("textures/UI/Slider.png"));
	textures_2D.emplace("SliderBar", ResourceManager::CreateAsset<Texture2D>("textures/UI/SliderBar.png"));
	textures_2D.emplace("Selector", ResourceManager::CreateAsset<Texture2D>("textures/UI/Selector.png"));
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
	textures_2D.emplace("L1", ResourceManager::CreateAsset<Texture2D>("textures/UI/Level1.png"));
	textures_2D.emplace("L2", ResourceManager::CreateAsset<Texture2D>("textures/UI/Level2.png"));
	textures_2D.emplace("L3", ResourceManager::CreateAsset<Texture2D>("textures/UI/Level3.png"));
	textures_2D.emplace("BL", ResourceManager::CreateAsset<Texture2D>("textures/UI/BossLevel.png"));
	textures_2D.emplace("New", ResourceManager::CreateAsset<Texture2D>("textures/UI/Brightness.png"));
	textures_2D.emplace("Volume", ResourceManager::CreateAsset<Texture2D>("textures/UI/Volume.png"));
	textures_2D.emplace("U.I", ResourceManager::CreateAsset<Texture2D>("textures/UI/U.ISize.png"));
	textures_2D.emplace("CP", ResourceManager::CreateAsset<Texture2D>("textures/UI/CreditsPage.png"));
	textures_2D.emplace("Options", ResourceManager::CreateAsset<Texture2D>("textures/UI/GameTitleIntro.png"));
	textures_2D.emplace("Controls", ResourceManager::CreateAsset<Texture2D>("textures/UI/Controls.png"));
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

	Material::Sptr characterMaterial = ResourceManager::CreateAsset<Material>(deferredForward);
	{
		characterMaterial->Name = "Character";
		characterMaterial->Set("u_Material.AlbedoMap", GetTexture2D("Character"));
		characterMaterial->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		characterMaterial->Set("u_Material.Shininess", 0.1f);
		materials.emplace("Character", characterMaterial);
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
	Material::Sptr toonMaterial = ResourceManager::CreateAsset<Material>(celShader);
	{
		toonMaterial->Name = "Toon";
		toonMaterial->Set("u_Material.AlbedoMap", GetTexture2D("Box Diffuse"));
		toonMaterial->Set("u_Material.NormalMap", GetTexture2D("Normal Map Default"));
		toonMaterial->Set("s_ToonTerm", GetTexture1D("Toon Lut"));
		toonMaterial->Set("u_Material.Shininess", 0.1f);
		toonMaterial->Set("u_Material.Steps", 8);
		materials.emplace("Toon", toonMaterial);
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

#pragma endregion

#pragma region Meshes

	meshes.emplace("Character", ResourceManager::CreateAsset<MeshResource>("meshes/Character.obj"));
	meshes.emplace("Dark Pine Tree", ResourceManager::CreateAsset<MeshResource>("meshes/DarkPineTree.obj"));
	meshes.emplace("Light Pine Tree", ResourceManager::CreateAsset<MeshResource>("meshes/LightPineTree.obj"));
	meshes.emplace("Grave Stone", ResourceManager::CreateAsset<MeshResource>("meshes/GraveStone.obj"));
	meshes.emplace("Key", ResourceManager::CreateAsset<MeshResource>("meshes/key.obj"));
	meshes.emplace("Stone Wall", ResourceManager::CreateAsset<MeshResource>("meshes/StoneWall.obj"));
	meshes.emplace("Turret Projectile", ResourceManager::CreateAsset<MeshResource>("meshes/TurretProjectile.obj"));
	meshes.emplace("Standing Torch", ResourceManager::CreateAsset<MeshResource>("meshes/StandingTorch.obj"));
	meshes.emplace("Enemy", ResourceManager::CreateAsset<MeshResource>("meshes/Enemy.obj"));
	meshes.emplace("Circle Cage", ResourceManager::CreateAsset<MeshResource>("meshes/CircleCage.obj"));
	meshes.emplace("Box Cage", ResourceManager::CreateAsset<MeshResource>("meshes/BoxCage.obj"));
	meshes.emplace("Long Cage", ResourceManager::CreateAsset<MeshResource>("meshes/CageLong.obj"));
	meshes.emplace("Small Cage", ResourceManager::CreateAsset<MeshResource>("meshes/SCage.obj"));
	meshes.emplace("U Cage", ResourceManager::CreateAsset<MeshResource>("meshes/CageU.obj"));
	meshes.emplace("Relic 1", ResourceManager::CreateAsset<MeshResource>("meshes/Symbol1.obj"));
	meshes.emplace("Relic 2", ResourceManager::CreateAsset<MeshResource>("meshes/Symbol2.obj"));
	meshes.emplace("Relic 3", ResourceManager::CreateAsset<MeshResource>("meshes/Symbol3.obj"));
	meshes.emplace("Relic 4", ResourceManager::CreateAsset<MeshResource>("meshes/Symbol4.obj"));
	meshes.emplace("Relic 5", ResourceManager::CreateAsset<MeshResource>("meshes/Symbol5.obj"));
	meshes.emplace("Pressure Plate", ResourceManager::CreateAsset<MeshResource>("meshes/PressurePlate.obj"));
	meshes.emplace("Turret", ResourceManager::CreateAsset<MeshResource>("meshes/Turret.obj"));
	meshes.emplace("Door", ResourceManager::CreateAsset<MeshResource>("meshes/door.obj"));
	meshes.emplace("Spike Trap", ResourceManager::CreateAsset<MeshResource>("meshes/SpikedTrap.obj"));
	meshes.emplace("Character Dagger", ResourceManager::CreateAsset<MeshResource>("meshes/CharacterDagger.obj"));
	meshes.emplace("Health Orb", ResourceManager::CreateAsset<MeshResource>("meshes/HealthOrb.obj"));
	meshes.emplace("Wall Grate", ResourceManager::CreateAsset<MeshResource>("meshes/WallGrate.obj"));
	meshes.emplace("Floor Grate", ResourceManager::CreateAsset<MeshResource>("meshes/FloorGrate.obj"));
	meshes.emplace("Large Candle", ResourceManager::CreateAsset<MeshResource>("meshes/LargeCandle.obj"));
	meshes.emplace("Cube", ResourceManager::CreateAsset<MeshResource>("meshes/cube.obj"));
	meshes.emplace("Damaged Pillar", ResourceManager::CreateAsset<MeshResource>("meshes/DamagedPillar.obj"));
	meshes.emplace("Destroyed Pillar", ResourceManager::CreateAsset<MeshResource>("meshes/DestroyedPillar.obj"));
	meshes.emplace("Intact Pillar", ResourceManager::CreateAsset<MeshResource>("meshes/IntactPillar.obj"));
	meshes.emplace("Rock Pile", ResourceManager::CreateAsset<MeshResource>("meshes/RockPile3.obj"));
	meshes.emplace("Sphere", ResourceManager::CreateAsset<MeshResource>("meshes/Sphere.obj"));
	meshes.emplace("Wall2", ResourceManager::CreateAsset<MeshResource>("meshes/Wall2.obj"));
	meshes.emplace("Shield", ResourceManager::CreateAsset<MeshResource>("meshes/GoblinShield.obj"));
	meshes.emplace("Staff", ResourceManager::CreateAsset<MeshResource>("meshes/GoblinStaff.obj"));
	meshes.emplace("Sword", ResourceManager::CreateAsset<MeshResource>("meshes/GoblinSword.obj"));
	meshes.emplace("Lever", ResourceManager::CreateAsset<MeshResource>("meshes/Lever.obj"));
	meshes.emplace("Barrel", ResourceManager::CreateAsset<MeshResource>("meshes/Barrel.obj"));
	meshes.emplace("Open Barrel", ResourceManager::CreateAsset<MeshResource>("meshes/OpenBarrel.obj"));
	meshes.emplace("Bucket", ResourceManager::CreateAsset<MeshResource>("meshes/Bucket.obj"));
	meshes.emplace("Keg", ResourceManager::CreateAsset<MeshResource>("meshes/KegWithStand.obj"));

	// Week 9
	meshes.emplace("Monkey", ResourceManager::CreateAsset<MeshResource>("Monkey.obj"));
	meshes.emplace("Fenrir", ResourceManager::CreateAsset<MeshResource>("fenrir.obj"));

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
		for (int i = 0; i < 8; ++i)
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
		for (int i = 0; i < 6; ++i)
		{
			std::string file;
			file.append("animations/character_idle/Idle");
			file.append(std::to_string((i + 1)));
			file.append(".obj");
			IdleAnimation.push_back(ResourceManager::CreateAsset<MeshResource>(file));
		}

		animations.emplace("Character Idle", std::move(IdleAnimation));
	}

	std::vector<MeshResource::Sptr> DoorAnimation;
	{
		for (int i = 0; i < 4; ++i)
		{
			std::string file;
			file.append("animations/door/door");
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

