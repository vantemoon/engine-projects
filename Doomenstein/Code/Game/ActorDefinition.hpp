#pragma once
#include "Engine/Math/FloatRange.hpp"
#include <string>


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
	float m_turnSpeed = 0.f;
	float m_drag = 0.f;
};