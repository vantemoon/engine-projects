#pragma once


//-----------------------------------------------------------------------------------------------
class WeaponDefinition;


//-----------------------------------------------------------------------------------------------
class Weapon
{
public:
	Weapon( WeaponDefinition const* definition );
	~Weapon();

	void Fire();

public:
	WeaponDefinition const* m_definition = nullptr;
};