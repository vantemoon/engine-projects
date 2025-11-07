#include "Game/Bullet.hpp"
#include "Game/Game.hpp"
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
	m_velocity = GOOD_BOLT_SPEED_TILES_PER_SECOND * GetForwardNormal();
	m_health = 3;
	InitializeVertexArray();
}


//-----------------------------------------------------------------------------------------------
Bullet::~Bullet() = default;


//-----------------------------------------------------------------------------------------------
void Bullet::Update( float deltaSeconds )
{
	UpdatePhysics( deltaSeconds );
	CheckForCollisions();
}


//-----------------------------------------------------------------------------------------------
void Bullet::UpdatePhysics( float deltaSeconds )
{
	Vec2 nextPosition = m_position + m_velocity * deltaSeconds;
	if ( g_game->m_currentMap->IsPointInSolid( nextPosition ) )
	{
		TakeDamage( 1 );
		IntVec2 currentTileCoords = g_game->m_currentMap->GetTileCoordsForWorldPosition( m_position );
		IntVec2 nextTileCoords = g_game->m_currentMap->GetTileCoordsForWorldPosition( nextPosition );
		Vec2 normal = Vec2::MakeFromIntVec2( currentTileCoords - nextTileCoords );
		m_velocity.Reflect( normal );
		m_orientationDegrees = m_velocity.GetOrientationDegrees();
	}

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


//-----------------------------------------------------------------------------------------------
void Bullet::CheckForCollisions()
{
	// If overlapping with an entity of different faction, deal damage and die
	for ( Entity* otherEntity : g_game->m_currentMap->m_allEntities )
	{
		if ( otherEntity == this )
		{
			continue;
		}
		
		if ( otherEntity->m_faction != m_faction && !otherEntity->m_isDead )
		{
			float distSquared = ( otherEntity->m_position - m_position ).GetLengthSquared();
			float combinedRadii = otherEntity->m_physicsRadius + m_physicsRadius;
			if ( distSquared <= ( combinedRadii * combinedRadii ) )
			{
				otherEntity->TakeDamage( 1 );
				Die();
				return;
			}
		}
	}
}