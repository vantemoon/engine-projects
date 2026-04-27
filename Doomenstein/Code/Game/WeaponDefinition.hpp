#pragma once
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Vec2.hpp"
#include <map>
#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
class WeaponDefinition
{
public:
	std::string m_name;
	float m_refireTime = 0.f;

	// Ray
	int m_rayCount = 0;
	float m_rayCone = 0.f;
	float m_rayRange = 0.f;
	FloatRange m_rayDamage = FloatRange::ZERO_TO_ZERO;
	float m_rayImpulse = 0.f;

	// Projectile
	int m_projectileCount = 0;
	float m_projectileCone = 0.f;
	float m_projectileSpeed = 0.f;
	std::string m_projectileActorDefName = "";

	// Melee
	int m_meleeCount = 0;
	float m_meleeRange = 0.f;
	float m_meleeArc = 0.f;
	FloatRange m_meleeDamage = FloatRange::ZERO_TO_ZERO;
	float m_meleeImpulse = 0.f;

	// HUD
	std::string m_hudShader = "Default";
	std::string m_hudTexture = "";
	std::string m_rectileTexture = "";
	Vec2 m_rectileSize = Vec2::ONE;
	Vec2 m_spriteSize = Vec2::ONE;
	Vec2 m_spritePivot = Vec2( 0.5f, 0.f );

	// Animation
	std::vector<std::string> m_animationNames;
	std::vector<std::string> m_animationShaders;
	std::vector<std::string> m_animationSpriteSheets;
	std::vector<Vec2> m_animationCellCounts;
	std::vector<float> m_animationGroupSecondsPerFrame;
	std::vector<int> m_animationStartFrames;
	std::vector<int> m_animationEndFrames;

	// Sound
	std::vector<std::string> m_soundTypes;
	std::vector<std::string> m_soundNames;

	static std::map<std::string, WeaponDefinition*> s_definitions;

public:
	static void InitializeDefinitions();
	static void ClearDefinitions();
	static WeaponDefinition const* GetWeaponDefinitionByName( std::string const& name );
};