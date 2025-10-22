#include "Game/Entity.hpp"
#include "Game/Bullet.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerShip.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"


//-----------------------------------------------------------------------------------------------
Entity::Entity( Game* game, Vec2 startingPosition )
	: m_game( game )
	, m_position( startingPosition )
{
	// Default values, should be overridden by derived classes
	m_velocity = Vec2( 0.f, 0.f );
	m_orientationDegrees = 0.f;
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


//
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


//-----------------------------------------------------------------------------------------------
void Entity::CheckCollisionWithBullets()
{
	if ( m_game == nullptr )
		return;

	for ( int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex )
	{
		Bullet* bullet = m_game->m_bullets[bulletIndex];
		if ( bullet == nullptr || bullet->m_isDead )
			continue;
		float distanceSquared = GetDistanceSquared2D( m_position, bullet->m_position );
		float combinedRadii = m_physicsRadius + bullet->m_physicsRadius;
		if ( distanceSquared < ( combinedRadii * combinedRadii ) )
		{
			bullet->Die( true );
			this->TakeDamage( 1 );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Entity::CheckCollisionWithPlayerShip()
{
	if ( m_game == nullptr || m_game->m_playerShip == nullptr || m_game->m_playerShip->m_isDead || m_game->m_playerShip->m_isInvincible || m_isDead )
		return;

	PlayerShip* playerShip = m_game->m_playerShip;
	float distanceSquared = GetDistanceSquared2D( m_position, playerShip->m_position );
	float combinedRadii = m_physicsRadius + playerShip->m_physicsRadius;
	if ( distanceSquared < ( combinedRadii * combinedRadii ) )
	{
		playerShip->Die();
	}
}


//-----------------------------------------------------------------------------------------------
void Entity::GetEnemyTypeAndAction( char** out_type, char** out_action ) const
{
	*out_type = ( char* ) "Enemy";
	*out_action = ( char* ) "None";

	if ( m_isAsteroid )
	{
		*out_type = ( char* ) "Asteroid";
		*out_action = ( char* ) "Detonate (40)";
		return;
	}

	if ( m_isBeetle )
	{
		*out_type = ( char* ) "Beetle";
		*out_action = ( char* ) "Telefrag (55)";
		return;
	}

	if ( m_isWasp )
	{
		*out_type = ( char* ) "Wasp";
		*out_action = ( char* ) "Telefrag (55)";
		return;
	}
}
