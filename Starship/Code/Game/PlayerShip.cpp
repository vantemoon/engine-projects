#include "Game/PlayerShip.hpp"


//-----------------------------------------------------------------------------------------------
PlayerShip::PlayerShip( Vec2 const& startingPosition, Vec2 const& startingVelocity )
	: m_position( startingPosition )
	, m_velocity( startingVelocity )
{
}


//-----------------------------------------------------------------------------------------------
void PlayerShip::Update( float deltaSeconds )
{
	m_position += m_velocity * deltaSeconds;
}