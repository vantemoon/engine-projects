#include "Game/Bullet.hpp"
#include "Game/Aries.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Texture.hpp"


//-----------------------------------------------------------------------------------------------
Bullet::Bullet( Vec2 startingPosition, float orientationDegrees, EntityType type )
	: Entity( startingPosition, orientationDegrees )
{
	m_type = type;

	switch ( m_type )
	{
		case ENTITY_TYPE_GOOD_BULLET:
			m_faction = ENTITY_FACTION_GOOD;
			// m_health = GOOD_BULLET_HEALTH;
			// m_velocity = GOOD_BULLET_SPEED_TILES_PER_SECOND * GetForwardNormal();
			m_physicsRadius = BULLET_PHYSICS_RADIUS;
			m_cosmeticRadius = BULLET_COSMETIC_RADIUS;
			m_isBullet = true;
			m_isBolt = false;
			break;

		case ENTITY_TYPE_GOOD_BOLT:
			m_faction = ENTITY_FACTION_GOOD;
			m_health = GOOD_BOLT_HEALTH;
			m_velocity = GOOD_BOLT_SPEED_TILES_PER_SECOND * GetForwardNormal();
			m_physicsRadius = BOLT_PHYSICS_RADIUS;
			m_cosmeticRadius = BOLT_COSMETIC_RADIUS;
			m_isBullet = false;
			m_isBolt = true;
			break;

		case ENTITY_TYPE_EVIL_BULLET:
			m_faction = ENTITY_FACTION_EVIL;
			m_health = EVIL_BULLET_HEALTH;
			m_velocity = EVIL_BULLET_SPEED_TILES_PER_SECOND * GetForwardNormal();
			m_physicsRadius = BULLET_PHYSICS_RADIUS;
			m_cosmeticRadius = BULLET_COSMETIC_RADIUS;
			m_isBullet = true;
			m_isBolt = false;
			break;

		case ENTITY_TYPE_EVIL_BOLT:
			m_faction = ENTITY_FACTION_EVIL;
			m_health = EVIL_BOLT_HEALTH;
			m_velocity = EVIL_BOLT_SPEED_TILES_PER_SECOND * GetForwardNormal();
			m_physicsRadius = BOLT_PHYSICS_RADIUS;
			m_cosmeticRadius = BOLT_COSMETIC_RADIUS;
			m_isBullet = false;
			m_isBolt = true;
			break;

		default:
			m_faction = ENTITY_FACTION_NEUTRAL;
			break;
	}

	m_isPushedByWalls = false;
	m_isPushedByEntities = false;
	m_doesPushEntities = false;
	m_isHitByBullets = false;

	InitializeVertexArray();
}


//-----------------------------------------------------------------------------------------------
Bullet::~Bullet() = default;


//-----------------------------------------------------------------------------------------------
void Bullet::Update( float deltaSeconds )
{
	UpdatePhysics( deltaSeconds );
	ResolveCollision();
}


//-----------------------------------------------------------------------------------------------
void Bullet::UpdatePhysics( float deltaSeconds )
{
	// If current position is in solid tile, push out and reflect
	if ( g_game->m_currentMap->IsPointInSolidTile( m_position ) )
	{
		// Push out
		Vec2 bulletCenter = m_position;
		float bulletRadius = m_physicsRadius;
		AABB2 tileBounds = g_game->m_currentMap->GetTileBounds( g_game->m_currentMap->GetTileCoordsForWorldPosition( m_position ) );
		if ( PushDiscOutOfFixedAABB2D( bulletCenter, bulletRadius, tileBounds ) ) {
			Vec2 tileCenter = tileBounds.GetCenter();
			Vec2 toTileCenter = bulletCenter - tileCenter;
			Vec2 normal;

			bool hitFromSide = fabsf( toTileCenter.x ) > fabsf( toTileCenter.y );
			if ( hitFromSide )
			{
				if ( toTileCenter.x > 0.f ) normal = Vec2( 1.f, 0.f );
				else normal = Vec2( -1.f, 0.f );
			}
			else
			{
				if ( toTileCenter.y > 0.f ) normal = Vec2( 0.f, 1.f );
				else normal = Vec2( 0.f, -1.f );
			}

			m_velocity.Reflect( normal );
			m_orientationDegrees = m_velocity.GetOrientationDegrees();
			m_position = bulletCenter;
		}
	}

	Vec2 nextPosition = m_position + m_velocity * deltaSeconds;
	if ( g_game->m_currentMap->IsPointInSolidTile( nextPosition ) )
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

	Texture* bulletTexture = nullptr;
	switch ( m_type )
	{
		default:
			break;

		case ENTITY_TYPE_GOOD_BULLET:
			bulletTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/FriendlyBullet.png" );
			break;

		case ENTITY_TYPE_GOOD_BOLT:
			bulletTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/FriendlyBolt.png" );
			break;

		case ENTITY_TYPE_EVIL_BULLET:
			bulletTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/EnemyBullet.png" );
			break;

		case ENTITY_TYPE_EVIL_BOLT:
			bulletTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/EnemyBolt.png" );
			break;
	}
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
	if ( m_isBolt )
	{
		float halfWidth = BOLT_LENGTH * 0.5f;
		float halfHeight = BOLT_WIDTH * 0.5f;
		AABB2 bulletAABB2 = AABB2( -halfWidth, -halfHeight, halfWidth, halfHeight );
		AddVertsForAABB2D( m_vertexArray, bulletAABB2, Rgba8::WHITE );
	}
	else if ( m_isBullet )
	{
		float halfWidth = BULLET_LENGTH * 0.5f;
		float halfHeight = BULLET_WIDTH * 0.5f;
		AABB2 bulletAABB2 = AABB2( -halfWidth, -halfHeight, halfWidth, halfHeight );
		AddVertsForAABB2D( m_vertexArray, bulletAABB2, Rgba8::WHITE );
	}
}


//-----------------------------------------------------------------------------------------------
void Bullet::ResolveCollision()
{
	// If overlapping with an entity of different faction, deal damage and die
	EntityFaction oppositeFaction = GetOppositeFaction();
	for ( Entity* otherEntity : g_game->m_currentMap->m_entityListsByFaction[oppositeFaction] )
	{
		if ( otherEntity->m_isDead || !otherEntity->m_isHitByBullets )
		{
			continue;
		}
		
		float distanceSquared = ( otherEntity->m_position - m_position ).GetLengthSquared();
		float combinedRadii = otherEntity->m_physicsRadius + m_physicsRadius;
		if ( distanceSquared <= ( combinedRadii * combinedRadii ) )
		{
			Aries* aries = dynamic_cast<Aries*>( otherEntity );
			if ( aries != nullptr )
			{
      				Vec2 oldVelocity = m_velocity;
				aries->ReflectBullet( *this );

				// Bullet was reflected
				if ( m_velocity != oldVelocity )
				{
					TakeDamage( 1 );
					return; 
				}
			}

			// Bullet was not reflected, deal damage and die
			otherEntity->TakeDamage( 1 );
			Die();
			return;
		}
	}
}


//-----------------------------------------------------------------------------------------------
EntityFaction Bullet::GetOppositeFaction() const
{
	if ( m_faction == ENTITY_FACTION_GOOD )
	{
		return ENTITY_FACTION_EVIL;
	}
	if ( m_faction == ENTITY_FACTION_EVIL )
	{
		return ENTITY_FACTION_GOOD;
	}
	else
	{
		return ENTITY_FACTION_NEUTRAL;
	}
}