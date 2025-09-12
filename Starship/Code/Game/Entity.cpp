#include "Game/Entity.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"


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
	// Do nothing
}


//-----------------------------------------------------------------------------------------------
void Entity::Die()
{
	m_isDead = true;
}


//
bool Entity::IsOffScreen() const
{
	// Do nothing
	return false;
}


//-----------------------------------------------------------------------------------------------
Vec2 Entity::GetForwardNormal() const
{
	Vec2 forwardNormal = Vec2::MakeFromPolarDegrees( m_orientationDegrees );
	return forwardNormal;
}


//-----------------------------------------------------------------------------------------------
bool Entity::IsAlive() const
{
	return !m_isDead;
}