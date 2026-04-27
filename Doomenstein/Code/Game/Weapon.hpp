#pragma once
#include "Engine/Core/Timer.hpp"


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

private:
	Vec3 GetRandomDirectionInCone( Vec3 const& baseForward, float coneDegrees ) const;

public:
	WeaponDefinition const* m_definition = nullptr;
	Timer m_refireTimer = Timer( 0.0 );
};