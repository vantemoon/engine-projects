#pragma once
#include "Engine/Math/FloatRange.hpp"
#include <map>
#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
class WeaponDefinition
{
public:
	std::string m_name;
	float m_refireTime = 0.f;

	int m_rayCount = 0;
	float m_rayCone = 0.f;
	float m_rayRange = 0.f;
	FloatRange m_rayDamage = FloatRange::ZERO_TO_ZERO;
	float m_rayImpulse = 0.f;

	int m_projectileCount = 0;
	float m_projectileCone = 0.f;
	float m_projectileSpeed = 0.f;
	std::string m_projectileActorDefName = "";

	int m_meleeCount = 0;
	float m_meleeRange = 0.f;
	float m_meleeArc = 0.f;
	FloatRange m_meleeDamage = FloatRange::ZERO_TO_ZERO;
	float m_meleeImpulse = 0.f;

	static std::map<std::string, WeaponDefinition*> s_definitions;

public:
	static void InitializeDefinitions();
	static void ClearDefinitions();
	static WeaponDefinition const* GetWeaponDefinitionByName( std::string const& name );
};