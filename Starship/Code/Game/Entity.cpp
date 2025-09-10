#include "Game/Entity.hpp"


//-----------------------------------------------------------------------------------------------
Entity::Entity( Game* game, Vec2 startingPosition )
	: m_game( game )
	, m_position( startingPosition )
{
}


//-----------------------------------------------------------------------------------------------
Entity::~Entity()
{
}


//-----------------------------------------------------------------------------------------------
void Entity::Update( float deltaSeconds )
{
	m_position += m_velocity * deltaSeconds;
	m_orientationDegrees += m_angularVelocityDegreesPerSecond * deltaSeconds;
}


//-----------------------------------------------------------------------------------------------
void Entity::Render() const
{
	// Do nothing; base entity does not render anything.
}


//-----------------------------------------------------------------------------------------------
void Entity::Die()
{
	m_isDead = true;
}


//
bool Entity::IsOffScreen() const
{
	// TODO: Implement this function
	return false;
}


//-----------------------------------------------------------------------------------------------
Vec2 Entity::GetForwardNormal() const
{
	// TODO: Implement this function
	return Vec2( 1.f, 0.f );
}


//-----------------------------------------------------------------------------------------------
bool Entity::IsAlive() const
{
	return !m_isDead;
}