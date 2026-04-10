#pragma once
#include "Engine/Math/FloatRange.hpp"
#include <map>
#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
class ActorDefinition
{
public:
	// Base
	bool m_isVisible = false;
	int m_health = 1;
	float corpseLifetime = 0.f;
	std::string m_faction = "NEUTRAL";
	bool m_canBePossessed = false;

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

public:
	static std::map<std::string, ActorDefinition*> s_definitions;

	static void InitializeDefinitions();
	static void ClearDefinitions();
	static ActorDefinition const* GetActorDefinitionByName( std::string const& name );
};