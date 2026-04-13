#include "Game/Weapon.hpp"
#include "Game/WeaponDefinition.hpp"


//-----------------------------------------------------------------------------------------------
Weapon::Weapon( WeaponDefinition const* definition )
	: m_definition( definition )
{
}


//-----------------------------------------------------------------------------------------------
Weapon::~Weapon()
{
}


//-----------------------------------------------------------------------------------------------
void Weapon::Fire()
{
}