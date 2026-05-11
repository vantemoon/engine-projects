#pragma once
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include <map>
#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
class ActorDefinition
{
public:
	// Base
	std::string m_name;
	bool m_isVisible = false;
	bool m_isProjectile = false;
	int m_health = 1;
	float corpseLifetime = 0.f;
	std::string m_faction = "NEUTRAL";
	bool m_canBePossessed = false;
	bool m_dieOnSpawn = false;
	bool m_isVirtualPet = false;
	bool m_isPickup = false;
	float m_pickupRespawnSeconds = 10.f;

	// Collision
	float m_physicsRadius = 0.f;
	float m_physicsHeight = 0.f;
	bool m_collideWithWorld = false;
	bool m_collideWithActors = false;
	bool m_dieOnCollide = false;
	FloatRange m_damageOnCollide = FloatRange::ZERO_TO_ZERO;
	float m_impulseOnCollide = 0.f;

	// Physics
	bool m_isSimulated = false;
	bool m_isFlying = false;
	float m_walkSpeed = 0.f;
	float m_runSpeed = 0.f;
	float m_drag = 0.f;
	float m_turnSpeed = 0.f;

	// Camera
	float m_eyeHeight = 0.f;
	float m_cameraFOVDegrees = 60.f;

	// AI
	bool m_aiEnabled = false;
	float m_sightRadius = 0.f;
	float m_sightAngle = 0.f;

	// Weapons
	std::vector<std::string> m_weaponDefNames;

	// Visuals
	Vec2 m_visualSize = Vec2::ONE;
	Vec2 m_visualPivot = Vec2( 0.5f, 0.5f );
	BillboardType m_visualBillboardType = BillboardType::NONE;
	bool m_visualRenderLit = false;
	bool m_visualRenderRounded = false;
	std::string m_visualShader = "Default";
	std::string m_visualSpriteSheet = "Default";
	IntVec2 m_visualCellCount = IntVec2::ONE;

	// Animation
	std::vector<std::string> m_animationGroupNames;
	std::vector<bool> m_animationGroupScaleBySpeed;
	std::vector<float> m_animationGroupSecondsPerFrame;
	std::vector<SpriteAnimPlaybackType> m_animationGroupPlaybackModes;
	std::vector<int> m_animationDirectionGroupIndex;
	std::vector<Vec3> m_animationDirectionVectors;
	std::vector<int> m_animationStartFrames;
	std::vector<int> m_animationEndFrames;

	// Sounds
	std::vector<std::string> m_soundTypes;
	std::vector<std::string> m_soundNames;

	static std::map<std::string, ActorDefinition*> s_definitions;

public:
	static void InitializeDefinitions();
	static void ClearDefinitions();
	static ActorDefinition const* GetActorDefinitionByName( std::string const& name );

private:
	static void LoadDefinitionsFromFile( char const* filepath, bool isProjectile );

private:
	static BillboardType GetBillboardTypeFromText( char const* text );
	static SpriteAnimPlaybackType GetPlaybackTypeFromText( char const* text );
};