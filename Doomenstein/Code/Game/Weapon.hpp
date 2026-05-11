#pragma once
#include "Engine/Core/Timer.hpp"
#include <string>


//-----------------------------------------------------------------------------------------------
class WeaponDefinition;
class Actor;
struct Vec3;


//-----------------------------------------------------------------------------------------------
class Weapon
{
public:
	Weapon( WeaponDefinition const* definition );
	~Weapon();

	bool CanFire( Actor const* owner ) const;
	void Fire( Actor* owner );

	void Render( Actor const* owner ) const;
	void PlayAnimation( Actor const* owner ) const;

private:
	Vec3 GetRandomDirectionInCone( Vec3 const& baseForward, float coneDegrees ) const;
	void PlaySoundByType( Vec3 const& position, std::string const& soundType ) const;

public:
	WeaponDefinition const* m_definition = nullptr;
	Timer m_refireTimer = Timer( 0.0 );
	double m_attackStartTime = 0.0;
};