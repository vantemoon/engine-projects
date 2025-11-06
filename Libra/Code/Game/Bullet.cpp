#include "Game/Bullet.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Texture.hpp"


//-----------------------------------------------------------------------------------------------
Bullet::Bullet( Vec2 startingPosition, float orientationDegrees, EntityType type )
	: Entity( startingPosition, orientationDegrees )
{
	m_physicsRadius = BULLET_PHYSICS_RADIUS;
	m_cosmeticRadius = BULLET_COSMETIC_RADIUS;
	switch ( type )
	{
		case ENTITY_TYPE_GOOD_BULLET:
			m_faction = ENTITY_FACTION_GOOD;
			break;

		case ENTITY_TYPE_GOOD_BOLT:
			m_faction = ENTITY_FACTION_GOOD;
			break;

		case ENTITY_TYPE_EVIL_BULLET:
			m_faction = ENTITY_FACTION_EVIL;
			break;

		case ENTITY_TYPE_EVIL_BOLT:
			m_faction = ENTITY_FACTION_EVIL;
			break;

		default:
			m_faction = ENTITY_FACTION_NEUTRAL;
			break;
	}
	m_isPushedByWalls = false;
	m_isPushedByEntities = false;
	m_doesPushEntities = false;
	m_isHitByBullets = false;
	m_velocity = BULLET_SPEED_TILES_PER_SECOND * GetForwardNormal();
	m_health = 3;
	InitializeVertexArray();
}


//-----------------------------------------------------------------------------------------------
Bullet::~Bullet() = default;


//-----------------------------------------------------------------------------------------------
void Bullet::Update( float deltaSeconds )
{
	Entity::Update( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void Bullet::Render() const
{
	if ( m_isDead )
	{
		return;
	}

	std::vector<Vertex> verts = m_vertexArray;
	Texture* bulletTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/FriendlyBolt.png" );
	g_engine->m_renderer->BindTexture( bulletTexture );
	TransformVertexArrayXY3D( ( int ) verts.size(), verts.data(), 1.f, m_orientationDegrees, m_position );
	g_engine->m_renderer->DrawVertexArray( verts );

	g_engine->m_renderer->BindTexture( nullptr );
}


//-----------------------------------------------------------------------------------------------
void Bullet::TakeDamage( int damage )
{
	Entity::TakeDamage( damage );
}


//-----------------------------------------------------------------------------------------------
void Bullet::Die()
{
	Entity::Die();
}


//-----------------------------------------------------------------------------------------------
void Bullet::InitializeVertexArray()
{
	float halfWidth = BULLET_LENGTH * 0.5f;
	float halfHeight = BULLET_WIDTH * 0.5f;
	AABB2 bulletAABB2 = AABB2( -halfWidth, -halfHeight, halfWidth, halfHeight );
	AddVertsForAABB2D( m_vertexArray, bulletAABB2, Rgba8::WHITE );
}