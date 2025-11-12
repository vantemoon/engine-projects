#include "Game/Entity.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"


//-----------------------------------------------------------------------------------------------
Entity::Entity( Vec2 startingPosition, float orientationDegrees )
	: m_position( startingPosition )
	, m_orientationDegrees( orientationDegrees )
{
	// Default values, should be overridden by derived classes
	m_velocity = Vec2( 0.f, 0.f );
	m_angularVelocityDegreesPerSecond = 0.f;
	m_physicsRadius = 1.f;
	m_cosmeticRadius = 1.f;
	m_health = 1;
	m_isDead = false;
	m_isGarbage = false;

	InitializeVertexArray();
}


//-----------------------------------------------------------------------------------------------
Entity::~Entity() = default;


//-----------------------------------------------------------------------------------------------
void Entity::InitializeVertexArray()
{
	// Do nothing
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
void Entity::TakeDamage( int damage )
{
	m_health -= damage;
	if ( m_health <= 0 )
	{
		Die();
	}
}


//-----------------------------------------------------------------------------------------------
void Entity::Die()
{
	m_isDead = true;
	m_isGarbage = true;
}


//-----------------------------------------------------------------------------------------------
bool Entity::IsOffScreen() const
{
	float screenLeft = 0.f;
	float screenRight = WORLD_SIZE_X;
	float screenBottom = 0.f;
	float screenTop = WORLD_SIZE_Y;

	if ( m_position.x + m_cosmeticRadius < screenLeft || m_position.x - m_cosmeticRadius > screenRight ||
		m_position.y + m_cosmeticRadius < screenBottom || m_position.y - m_cosmeticRadius > screenTop )
	{
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
Vec2 Entity::GetForwardNormal() const
{
	Vec2 forwardNormal = Vec2::MakeFromPolarDegrees( m_orientationDegrees, 1.f );
	return forwardNormal;
}


//-----------------------------------------------------------------------------------------------
bool Entity::IsAlive() const
{
	return !m_isDead;
}