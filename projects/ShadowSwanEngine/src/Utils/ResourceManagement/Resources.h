#pragma once

#include "ResourceManager.h"

// Gameplay
#include "Gameplay/Material.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Gameplay/PlayerController.h"

// Graphics
#include "Graphics/IndexBuffer.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/VertexArrayObject.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture2D.h"
#include "Graphics/TextureCube.h"
#include "Graphics/VertexTypes.h"
#include "Graphics/Font.h"
#include "Graphics/GuiBatcher.h"

// Components
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/Components/Camera.h"
#include "Gameplay/Components/RotatingBehaviour.h"
#include "Gameplay/Components/JumpBehaviour.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/MaterialSwapBehaviour.h"
#include "Gameplay/Components/HealthComponent.h"
#include "Gameplay/Components/InteractableComponent.h"
#include "Gameplay/Components/AnimatorComponent.h"
#include "Gameplay/Components/Enemy.h"
#include "Gameplay/Components/MovingPlatformBehavior.h"
#include "Gameplay/Components/SpikeTrapBehavior.h"
#include "Gameplay/Components/TurretBehavior.h"

// Physics
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Physics/Colliders/PlaneCollider.h"
#include "Gameplay/Physics/Colliders/SphereCollider.h"
#include "Gameplay/Physics/Colliders/ConvexMeshCollider.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Graphics/DebugDraw.h"
#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Components/SimpleCameraControl.h"
#include "Gameplay/Physics/Colliders/CylinderCollider.h"

// GUI
#include "Gameplay/Components/GUI/RectTransform.h"
#include "Gameplay/Components/GUI/GuiPanel.h"
#include "Gameplay/Components/GUI/GuiText.h"

using namespace Gameplay;

class Resources {
public:
	static Resources& GetInstance() { static Resources resources; return resources; }
	
#pragma region Unused/Unimplemented Method for Searching Assets
	
	/* /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		The idea is that we can use enum classes to store a reference to our assets and use them in drop-down lists to find
		what assets are available when making a call to an asset.  This would make it so we don't have to memorize every 
		asset that is available to us, and instead can just do a search from our dropdown list.
	*/ /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/*
		static enum class Shaders {
			Basic, Reflective, Specular, Foliage, Toon, Displacement, TangentSpace, MultiTexture
		};

		static enum class Textures {
			StoneWall, StoneSlab, StoneSlab2, DarkPineTree, LightPineTree, TurretProjectile, Key, Rock, StandingTorch, GraveStone, Enemy,
			Character, Shadow, CircleCage, BoxCage, LongCage, SmallCage, UCage, PressurePlate, Turret, Door, SpikeTrap, Relic1, Relic2,
			Relic3, Relic4, Relic5, MenuGloss, MenuGlossReverse
		};

		static enum class Materials {
			StoneWall, PressurePlate, StoneSlab, SmallCage, TurretProjectile, Turret, Door, Character, SpikeTrap, Relic1, Relic2, Relic3,
			Relic4, Relic5, Shadow, DarkPineTree, LightPineTree, StandingTorch, Rock, CircleCage, BoxCage, LongCage, UCage, GraveStone,
			Enemy
		};

		static enum class Meshes {
			Character, DarkPineTree, LightPineTree, GraveStone, Key, StoneWall, TurretProjectile, StandingTorch, StoneSlab, Enemy, CircleCage,
			BoxCage, LongCage, SmallCage, UCage, Relic1, Relic2, Relic3, Relic4, Relic5, PressurePlate, Turret, Door, SpikeTrap
		};

		static enum class Animations {
			CharacterWalk, CharacterIdle, Turret
		};
		*/

#pragma endregion	

protected:
	Resources() {};

protected:
	std::map<std::string, Shader::Sptr>								shaders;
	std::map<std::string, MeshResource::Sptr>						meshes;
	std::map<std::string, Texture2D::Sptr>							textures;
	std::map<std::string, Material::Sptr>							materials;
	std::map<std::string, std::vector<MeshResource::Sptr>>			animations;

public:
	
	void Initialize()
	{
		// Initialize ResourceManager
		ResourceManager::Init();

		// Register all our resource types so we can load them from manifest files
		ResourceManager::RegisterType<Texture2D>();
		ResourceManager::RegisterType<TextureCube>();
		ResourceManager::RegisterType<Shader>();
		ResourceManager::RegisterType<Material>();
		ResourceManager::RegisterType<MeshResource>();

		// Register all of our component types so we can load them from files
		ComponentManager::RegisterType<Camera>();
		ComponentManager::RegisterType<RenderComponent>();
		ComponentManager::RegisterType<Physics::RigidBody>();
		ComponentManager::RegisterType<Physics::TriggerVolume>();
		ComponentManager::RegisterType<RotatingBehaviour>();
		ComponentManager::RegisterType<JumpBehaviour>();
		ComponentManager::RegisterType<MaterialSwapBehaviour>();
		ComponentManager::RegisterType<TriggerVolumeEnterBehaviour>();
		ComponentManager::RegisterType<SimpleCameraControl>();
		ComponentManager::RegisterType<HealthComponent>();
		ComponentManager::RegisterType<InteractableComponent>();
		ComponentManager::RegisterType<AnimatorComponent>();
		ComponentManager::RegisterType<Enemy>();
		ComponentManager::RegisterType<SpikeTrapBehavior>();
		ComponentManager::RegisterType<MovingPlatformBehavior>();
		ComponentManager::RegisterType<TurretBehavior>();
		ComponentManager::RegisterType<RectTransform>();
		ComponentManager::RegisterType<GuiPanel>();
		ComponentManager::RegisterType<GuiText>();


		// SHADERS
		//////////////////////////////////////
		{
			shaders.emplace("Reflective", ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string> {
				{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
				{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_environment_reflective.glsl" }
			}));
			shaders.emplace("Basic", ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string> {
				{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
				{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_blinn_phong_textured.glsl" }
			}));
			shaders.emplace("Specular", ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string> {
				{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
				{ ShaderPartType::Fragment, "shaders/fragment_shaders/textured_specular.glsl" }
			}));
			shaders.emplace("Foliage", ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string> {
				{ ShaderPartType::Vertex, "shaders/vertex_shaders/foliage.glsl" },
				{ ShaderPartType::Fragment, "shaders/fragment_shaders/screendoor_transparency.glsl" }
			}));
			shaders.emplace("Toon", ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string> {
				{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
				{ ShaderPartType::Fragment, "shaders/fragment_shaders/toon_shading.glsl" }
			}));
			shaders.emplace("Displacement", ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string> {
				{ ShaderPartType::Vertex, "shaders/vertex_shaders/displacement_mapping.glsl" },
				{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_tangentspace_normal_maps.glsl" }
			}));
			shaders.emplace("Tangent Space", ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string> {
				{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
				{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_tangentspace_normal_maps.glsl" }
			}));
			shaders.emplace("Multi-Texture", ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string> {
				{ ShaderPartType::Vertex, "shaders/vertex_shaders/vert_multitextured.glsl" },
				{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_multitextured.glsl" }
			}));
		}

		// TEXTURES
		//////////////////////////////////////
		{
			textures.emplace("Stone Wall", ResourceManager::CreateAsset<Texture2D>("textures/StoneWallUVs.png"));
			textures.emplace("Stone Slab", ResourceManager::CreateAsset<Texture2D>("textures/StoneSlabUVs.png"));
			textures.emplace("Stone Slab 2", ResourceManager::CreateAsset<Texture2D>("textures/StoneSlabsTexture.png"));
			textures.emplace("Dark Pine Tree", ResourceManager::CreateAsset<Texture2D>("textures/DarkPineTreeUVS.png"));
			textures.emplace("Light Pine Tree", ResourceManager::CreateAsset<Texture2D>("textures/LightTreeTextureUVS.png"));
			textures.emplace("Turret Projectile", ResourceManager::CreateAsset<Texture2D>("textures/TurretProjectileUVS.png"));
			textures.emplace("Key", ResourceManager::CreateAsset<Texture2D>("textures/KeyTextureUVS.png"));
			textures.emplace("Rock", ResourceManager::CreateAsset<Texture2D>("textures/RockUVS.png"));
			textures.emplace("Standing Torch", ResourceManager::CreateAsset<Texture2D>("textures/TorchUVS.png"));
			textures.emplace("Grave Stone", ResourceManager::CreateAsset<Texture2D>("textures/GraveStoneUVS.png"));
			textures.emplace("Enemy", ResourceManager::CreateAsset<Texture2D>("textures/EnemyUVS.png"));
			textures.emplace("Character", ResourceManager::CreateAsset<Texture2D>("textures/CharacterUVS.png"));
			textures.emplace("Shadow", ResourceManager::CreateAsset<Texture2D>("textures/ShadowUVS.png"));
			textures.emplace("Circle Cage", ResourceManager::CreateAsset<Texture2D>("textures/CircleCageUVS.png"));
			textures.emplace("Box Cage", ResourceManager::CreateAsset<Texture2D>("textures/BoxcageUVS.png"));
			textures.emplace("Long Cage", ResourceManager::CreateAsset<Texture2D>("textures/LongCageUVS.png"));
			textures.emplace("Small Cage", ResourceManager::CreateAsset<Texture2D>("textures/SmallCageUVS.png"));
			textures.emplace("U Cage", ResourceManager::CreateAsset<Texture2D>("textures/UCageUVS.png"));
			textures.emplace("Pressure Plate", ResourceManager::CreateAsset<Texture2D>("textures/PressurePlateUVs.png"));
			textures.emplace("Turret", ResourceManager::CreateAsset<Texture2D>("textures/TurretUVS.png"));
			textures.emplace("Door", ResourceManager::CreateAsset<Texture2D>("textures/DoorUVs.png"));
			textures.emplace("Spike Trap", ResourceManager::CreateAsset<Texture2D>("textures/SpikedTrapUVS.png"));
			textures.emplace("Relic 1", ResourceManager::CreateAsset<Texture2D>("textures/StoneTablet1UVS.png"));
			textures.emplace("Relic 2", ResourceManager::CreateAsset<Texture2D>("textures/StoneTablet2UVS.png"));
			textures.emplace("Relic 3", ResourceManager::CreateAsset<Texture2D>("textures/StoneTablet3UVS.png"));
			textures.emplace("Relic 4", ResourceManager::CreateAsset<Texture2D>("textures/StoneTablet4UVS.png"));
			textures.emplace("Relic 5", ResourceManager::CreateAsset<Texture2D>("textures/StoneTablet5UVS.png"));
			textures.emplace("Menu Gloss", ResourceManager::CreateAsset<Texture2D>("textures/MenuGloss.png"));
			textures.emplace("Menu Gloss Reverse", ResourceManager::CreateAsset<Texture2D>("textures/MenuGlossReverse.png"));
			textures.emplace("Character Dagger", ResourceManager::CreateAsset<Texture2D>("textures/DaggerTexture.png"));
			textures.emplace("Health Orb", ResourceManager::CreateAsset<Texture2D>("textures/HealthTexture.png"));
			textures.emplace("Wall Grate", ResourceManager::CreateAsset<Texture2D>("textures/WallGrateUVS.png"));
			textures.emplace("Floor Grate", ResourceManager::CreateAsset<Texture2D>("textures/FloorGrateUVS.png"));
			textures.emplace("Small Candle", ResourceManager::CreateAsset<Texture2D>("textures/SmallCandleTexture.png"));
			textures.emplace("Large Candle", ResourceManager::CreateAsset<Texture2D>("textures/LargeCandleTexture.png"));
			textures.emplace("Brown", ResourceManager::CreateAsset<Texture2D>("textures/Brown.png"));
			textures.emplace("Gray", ResourceManager::CreateAsset<Texture2D>("textures/Gray.png"));
			textures.emplace("White", ResourceManager::CreateAsset<Texture2D>("textures/White.png"));
			textures.emplace("Wall 2", ResourceManager::CreateAsset<Texture2D>("textures/Wall_Tex2.png"));
		}
		
		// MATERIALS
		//////////////////////////////////////	
		{
			Material::Sptr stoneWallMat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				stoneWallMat->Name = "Stone Wall";
				stoneWallMat->Set("u_Material.Diffuse", GetTexture("Stone Wall"));
				stoneWallMat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Stone Wall", std::move(stoneWallMat));
			}

			Material::Sptr pressurePlateMaterial = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				pressurePlateMaterial->Name = "Pressure Plate";
				pressurePlateMaterial->Set("u_Material.Diffuse", GetTexture("Pressure Plate"));
				pressurePlateMaterial->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Pressure Plate", std::move(pressurePlateMaterial));
			}

			Material::Sptr stoneSlabMaterial = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				stoneSlabMaterial->Name = "Stone Slab";
				stoneSlabMaterial->Set("u_Material.Diffuse", GetTexture("Stone Slab 2"));
				stoneSlabMaterial->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Stone Slab", std::move(stoneSlabMaterial));
			}

			Material::Sptr sCageMaterial = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				sCageMaterial->Name = "Small Cage";
				sCageMaterial->Set("u_Material.Diffuse", GetTexture("Small Cage"));
				sCageMaterial->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Small Cage", std::move(sCageMaterial));
			}

			Material::Sptr TurretProjMat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				TurretProjMat->Name = "Turret Projectile";
				TurretProjMat->Set("u_Material.Diffuse", GetTexture("Turret Projectile"));
				TurretProjMat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Turret Projectile", std::move(TurretProjMat));
			}

			Material::Sptr turretMat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				turretMat->Name = "Turret";
				turretMat->Set("u_Material.Diffuse", GetTexture("Turret"));
				turretMat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Turret", std::move(turretMat));
			}

			Material::Sptr doorMat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				doorMat->Name = "Door";
				doorMat->Set("u_Material.Diffuse", GetTexture("Door"));
				doorMat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Door", std::move(doorMat));
			}

			Material::Sptr characterMat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				characterMat->Name = "Character";
				characterMat->Set("u_Material.Diffuse", GetTexture("Character"));
				characterMat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Character", std::move(characterMat));
			}

			Material::Sptr spikeTrapMat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				spikeTrapMat->Name = "Spike Trap";
				spikeTrapMat->Set("u_Material.Diffuse", GetTexture("Spike Trap"));
				spikeTrapMat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Spike Trap", std::move(spikeTrapMat));
			}

			//Symbol Materials
			Material::Sptr symbol1Mat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				symbol1Mat->Name = "Relic 1";
				symbol1Mat->Set("u_Material.Diffuse", GetTexture("Relic 1"));
				symbol1Mat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Relic 1", std::move(symbol1Mat));
			}
			Material::Sptr symbol2Mat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				symbol2Mat->Name = "Relic 2";
				symbol2Mat->Set("u_Material.Diffuse", GetTexture("Relic 2"));
				symbol2Mat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Relic 2", std::move(symbol2Mat));
			}
			Material::Sptr symbol3Mat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				symbol3Mat->Name = "Relic 3";
				symbol3Mat->Set("u_Material.Diffuse", GetTexture("Relic 3"));
				symbol3Mat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Relic 3", std::move(symbol3Mat));
			}
			Material::Sptr symbol4Mat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				symbol4Mat->Name = "Relic 4";
				symbol4Mat->Set("u_Material.Diffuse", GetTexture("Relic 4"));
				symbol4Mat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Relic 4", std::move(symbol4Mat));
			}
			Material::Sptr symbol5Mat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				symbol5Mat->Name = "Relic 5";
				symbol5Mat->Set("u_Material.Diffuse", GetTexture("Relic 5"));
				symbol5Mat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Relic 5", std::move(symbol5Mat));
			}

			Material::Sptr shadowMat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				shadowMat->Name = "Shadow";
				shadowMat->Set("u_Material.Diffuse", GetTexture("Shadow"));
				shadowMat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Shadow", std::move(shadowMat));
			}
			//Tree Materials
			Material::Sptr darkTreeMat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				darkTreeMat->Name = "Dark Pine Tree";
				darkTreeMat->Set("u_Material.Diffuse", GetTexture("Dark Pine Tree"));
				darkTreeMat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Dark Pine Tree", std::move(darkTreeMat));
			}
			Material::Sptr lightTreeMat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				lightTreeMat->Name = "Light Pine Tree";
				lightTreeMat->Set("u_Material.Diffuse", GetTexture("Light Pine Tree"));
				lightTreeMat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Light Pine Tree", std::move(lightTreeMat));
			}

			Material::Sptr standingTorchMat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				standingTorchMat->Name = "Standing Torch";
				standingTorchMat->Set("u_Material.Diffuse", GetTexture("Standing Torch"));
				standingTorchMat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Standing Torch", std::move(standingTorchMat));
			}
			Material::Sptr rockMat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				rockMat->Name = "Rock";
				rockMat->Set("u_Material.Diffuse", GetTexture("Rock"));
				rockMat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Rock", std::move(rockMat));
			}

			Material::Sptr boxCageMat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				boxCageMat->Name = "Box Cage";
				boxCageMat->Set("u_Material.Diffuse", GetTexture("Box Cage"));
				boxCageMat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Box Cage", std::move(boxCageMat));
			}
			Material::Sptr circleCageMat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				circleCageMat->Name = "Circle Cage";
				circleCageMat->Set("u_Material.Diffuse", GetTexture("Circle Cage"));
				circleCageMat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Circle Cage", std::move(circleCageMat));
			}
			Material::Sptr longCageMat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				longCageMat->Name = "Long Cage";
				longCageMat->Set("u_Material.Diffuse", GetTexture("Long Cage"));
				longCageMat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Long Cage", std::move(longCageMat));
			}
			Material::Sptr uCageMat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				uCageMat->Name = "U Cage";
				uCageMat->Set("u_Material.Diffuse", GetTexture("U Cage"));
				uCageMat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("U Cage", std::move(uCageMat));
			}

			Material::Sptr graveStoneMat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				graveStoneMat->Name = "Grave Stone";
				graveStoneMat->Set("u_Material.Diffuse", GetTexture("Grave Stone"));
				graveStoneMat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Grave Stone", std::move(graveStoneMat));
			}

			Material::Sptr enemyMat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				enemyMat->Name = "Enemy";
				enemyMat->Set("u_Material.Diffuse", GetTexture("Enemy"));
				enemyMat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Enemy", std::move(enemyMat));
			}

			Material::Sptr dagger = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				dagger->Name = "Character Dagger";
				dagger->Set("u_Material.Diffuse", GetTexture("Character Dagger"));
				dagger->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Character Dagger", std::move(dagger));
			}

			Material::Sptr Orb = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				Orb->Name = "Health Orb";
				Orb->Set("u_Material.Diffuse", GetTexture("Health Orb"));
				Orb->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Health Orb", std::move(Orb));
			}

			Material::Sptr WGrate = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				WGrate->Name = "Wall Grate";
				WGrate->Set("u_Material.Diffuse", GetTexture("Wall Grate"));
				WGrate->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Wall Grate", std::move(WGrate));
			}


			Material::Sptr FGrate = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				FGrate->Name = "Floor Grate";
				FGrate->Set("u_Material.Diffuse", GetTexture("Floor Grate"));
				FGrate->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Floor Grate", std::move(FGrate));
			}

			Material::Sptr SCandle = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				SCandle->Name = "Small Candle";
				SCandle->Set("u_Material.Diffuse", GetTexture("Small Candle"));
				SCandle->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Small Candle", std::move(SCandle));
			}

			Material::Sptr LCandle = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				LCandle->Name = "Large Candle";
				LCandle->Set("u_Material.Diffuse", GetTexture("Large Candle"));
				LCandle->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Large Candle", std::move(LCandle));
			}

			Material::Sptr keyMat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				keyMat->Name = "Key";
				keyMat->Set("u_Material.Diffuse", GetTexture("Key"));
				keyMat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Key", std::move(keyMat));
			}

			Material::Sptr whiteMat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				whiteMat->Name = "White";
				whiteMat->Set("u_Material.Diffuse", GetTexture("White"));
				whiteMat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("White", std::move(whiteMat));
			}

			Material::Sptr grayMat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				grayMat->Name = "Gray";
				grayMat->Set("u_Material.Diffuse", GetTexture("Gray"));
				grayMat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Gray", std::move(grayMat));
			}

			Material::Sptr brownMat = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				brownMat->Name = "Brown";
				brownMat->Set("u_Material.Diffuse", GetTexture("Brown"));
				brownMat->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Brown", std::move(brownMat));
			}

			Material::Sptr Wall_Mat2 = ResourceManager::CreateAsset<Material>(GetShader("Basic"));
			{
				Wall_Mat2->Name = "Wall 2";
				Wall_Mat2->Set("u_Material.Diffuse", GetTexture("Wall 2"));
				Wall_Mat2->Set("u_Material.Shininess", 0.1f);
				materials.emplace("Wall 2", std::move(Wall_Mat2));
			}

		}


		// MESHES
		//////////////////////////////////////		
		{
			meshes.emplace("Character", ResourceManager::CreateAsset<MeshResource>("Character.obj"));
			meshes.emplace("Dark Pine Tree", ResourceManager::CreateAsset<MeshResource>("DarkPineTree.obj"));
			meshes.emplace("Light Pine Tree", ResourceManager::CreateAsset<MeshResource>("LightPineTree.obj"));
			meshes.emplace("Grave Stone", ResourceManager::CreateAsset<MeshResource>("GraveStone.obj"));
			meshes.emplace("Key", ResourceManager::CreateAsset<MeshResource>("key.obj"));
			meshes.emplace("Stone Wall", ResourceManager::CreateAsset<MeshResource>("StoneWall.obj"));
			meshes.emplace("Turret Projectile", ResourceManager::CreateAsset<MeshResource>("TurretProjectile.obj"));
			meshes.emplace("Standing Torch", ResourceManager::CreateAsset<MeshResource>("StandingTorch.obj"));
			meshes.emplace("Stone Slab", ResourceManager::CreateAsset<MeshResource>("stoneslabs.obj"));
			meshes.emplace("Enemy", ResourceManager::CreateAsset<MeshResource>("Enemy.obj"));
			meshes.emplace("Circle Cage", ResourceManager::CreateAsset<MeshResource>("CircleCage.obj"));
			meshes.emplace("Box Cage", ResourceManager::CreateAsset<MeshResource>("BoxCage.obj"));
			meshes.emplace("Long Cage", ResourceManager::CreateAsset<MeshResource>("CageLong.obj"));
			meshes.emplace("Small Cage", ResourceManager::CreateAsset<MeshResource>("SCage.obj"));
			meshes.emplace("U Cage", ResourceManager::CreateAsset<MeshResource>("CageU.obj"));
			meshes.emplace("Relic 1", ResourceManager::CreateAsset<MeshResource>("Symbol1.obj"));
			meshes.emplace("Relic 2", ResourceManager::CreateAsset<MeshResource>("Symbol2.obj"));
			meshes.emplace("Relic 3", ResourceManager::CreateAsset<MeshResource>("Symbol3.obj"));
			meshes.emplace("Relic 4", ResourceManager::CreateAsset<MeshResource>("Symbol4.obj"));
			meshes.emplace("Relic 5", ResourceManager::CreateAsset<MeshResource>("Symbol5.obj"));
			meshes.emplace("Pressure Plate", ResourceManager::CreateAsset<MeshResource>("PressurePlate.obj"));
			meshes.emplace("Turret", ResourceManager::CreateAsset<MeshResource>("Turret.obj"));
			meshes.emplace("Door", ResourceManager::CreateAsset<MeshResource>("door.obj"));
			meshes.emplace("Spike Trap", ResourceManager::CreateAsset<MeshResource>("SpikedTrap.obj"));
			meshes.emplace("Rock", ResourceManager::CreateAsset<MeshResource>("Rock.obj"));	
			meshes.emplace("Character Dagger", ResourceManager::CreateAsset<MeshResource>("CharacterDagger.obj"));
			meshes.emplace("Health Orb", ResourceManager::CreateAsset<MeshResource>("HealthOrb.obj"));
			meshes.emplace("Wall Grate", ResourceManager::CreateAsset<MeshResource>("WallGrate.obj"));
			meshes.emplace("Floor Grate", ResourceManager::CreateAsset<MeshResource>("FloorGrate.obj"));
			meshes.emplace("Small Candle", ResourceManager::CreateAsset<MeshResource>("SmallCandle.obj"));
			meshes.emplace("Large Candle", ResourceManager::CreateAsset<MeshResource>("LargeCandle.obj"));
			meshes.emplace("Cube", ResourceManager::CreateAsset<MeshResource>("cube.obj"));
			meshes.emplace("Bucket", ResourceManager::CreateAsset<MeshResource>("Bucket.obj"));
			meshes.emplace("Keg", ResourceManager::CreateAsset<MeshResource>("KegStand.obj"));
			meshes.emplace("Damaged Pillar", ResourceManager::CreateAsset<MeshResource>("DamagedPillar.obj"));
			meshes.emplace("Destroyed Pillar", ResourceManager::CreateAsset<MeshResource>("DestroyedPillar.obj"));
			meshes.emplace("Pillar", ResourceManager::CreateAsset<MeshResource>("IntactPillar.obj"));
			meshes.emplace("Rock Pile", ResourceManager::CreateAsset<MeshResource>("RockPile3.obj"));
			meshes.emplace("Sphere", ResourceManager::CreateAsset<MeshResource>("Sphere.obj"));
			meshes.emplace("Wall 2", ResourceManager::CreateAsset<MeshResource>("Wall2.obj"));
		}

		// ANIMATIONS
		//////////////////////////////////////
		{
			std::vector<MeshResource::Sptr> WalkAnimation;
			{
				for (int i = 0; i < 8; ++i)
				{
					std::string file;
					file.append("models/character_walk/Walk");
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
					file.append("models/character_idle/Idle");
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
					file.append("models/door/door");
					file.append(std::to_string((i + 1)));
					file.append(".obj");
					DoorAnimation.push_back(ResourceManager::CreateAsset<MeshResource>(file));
				}

				animations.emplace("Door", std::move(DoorAnimation));
			}

			std::vector<MeshResource::Sptr> SpikeAnimation;
			{
				for (int i = 0; i < 3; ++i)
				{
					std::string file;
					file.append("models/spikedTrap/SpikedTrap");
					file.append(std::to_string((i + 1)));
					file.append(".obj");
					DoorAnimation.push_back(ResourceManager::CreateAsset<MeshResource>(file));
				}

				animations.emplace("Spikes", std::move(DoorAnimation));
			}
		}
	
	}

	static Shader::Sptr GetShader(std::string name) {
		std::map<std::string, Shader::Sptr>::iterator it;
		it = GetInstance().shaders.find(name);

		if (it != GetInstance().shaders.end())
		{
			return it->second;
		}

		else {
			std::cout << "ERROR <nullptr>: " << name << " not found in Shaders.";
			return nullptr;
		}
	}
	static MeshResource::Sptr GetMesh(std::string name)
	{
		std::map<std::string, MeshResource::Sptr>::iterator it;
		it = GetInstance().meshes.find(name);

		if (it != GetInstance().meshes.end())
		{
			return it->second;
		}

		else {
			std::cout << "ERROR <nullptr>: " << name << " not found in Meshes.";
			return nullptr;
		}
	}

	static Texture2D::Sptr GetTexture(std::string name)
	{
		std::map<std::string, Texture2D::Sptr>::iterator it;
		it = GetInstance().textures.find(name);

		if (it != GetInstance().textures.end())
		{
			return it->second;
		}

		else {
			std::cout << "ERROR <nullptr>: " << name << " not found in Textures.";
			return nullptr;
		}
	}

	static Material::Sptr GetMaterial(std::string name)
	{
		std::map<std::string, Material::Sptr>::iterator it;
		it = GetInstance().materials.find(name);

		if (it != GetInstance().materials.end())
		{
			return it->second;
		}

		else {
			std::cout << "ERROR <nullptr>: " << name << " not found in Materials.";
			return nullptr;
		}
	}

	static std::vector<MeshResource::Sptr> GetAnimation(std::string name)
	{
		std::map<std::string, std::vector<MeshResource::Sptr>>::iterator it;
		it = GetInstance().animations.find(name);

		if (it != GetInstance().animations.end())
		{
			return it->second;
		}

		else {
			std::cout << "ERROR <nullptr>: " << name << " not found in Animations";
			return std::vector<MeshResource::Sptr>();
		}
	}

};