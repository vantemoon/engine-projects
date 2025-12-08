#include "Game/Bullet.hpp"
#include "Game/Aries.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/EngineCommon.hpp"
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
			m_health = g_gameConfigBlackboard.GetValue( "goodBulletMaxHealth", 3 );
			m_velocity = g_gameConfigBlackboard.GetValue( "goodBulletSpeed", 80.f ) * GetForwardNormal();
			m_physicsRadius = g_gameConfigBlackboard.GetValue( "bulletPhysicsRadius", 0.5f );
			m_cosmeticRadius = g_gameConfigBlackboard.GetValue( "bulletCosmeticRadius", 1.f );
			m_isBullet = true;
			m_isBolt = false;
			break;

		case ENTITY_TYPE_GOOD_BOLT:
			m_faction = ENTITY_FACTION_GOOD;
			m_health = g_gameConfigBlackboard.GetValue( "goodBoltMaxHealth", 3 );
			m_velocity = g_gameConfigBlackboard.GetValue( "goodBoltSpeed", 80.f ) * GetForwardNormal();
			m_physicsRadius = g_gameConfigBlackboard.GetValue( "boltPhysicsRadius", 0.5f );
			m_cosmeticRadius = g_gameConfigBlackboard.GetValue( "boltCosmeticRadius", 1.f );
			m_isBullet = false;
			m_isBolt = true;
			break;

		case ENTITY_TYPE_GOOD_FLAME_BULLET:
			m_faction = ENTITY_FACTION_GOOD;
			m_health = g_gameConfigBlackboard.GetValue( "goodBulletMaxHealth", 3 );
			m_velocity = g_gameConfigBlackboard.GetValue( "goodBulletSpeed", 80.f ) * GetForwardNormal();
			m_physicsRadius = g_gameConfigBlackboard.GetValue( "bulletPhysicsRadius", 0.5f );
			m_cosmeticRadius = g_gameConfigBlackboard.GetValue( "bulletCosmeticRadius", 1.f );
			m_isBullet = true;
			m_isBolt = false;
			break;

		case ENTITY_TYPE_EVIL_BULLET:
			m_faction = ENTITY_FACTION_EVIL;
			m_health = g_gameConfigBlackboard.GetValue( "evilBulletMaxHealth", 1 );
			m_velocity = g_gameConfigBlackboard.GetValue( "evilBulletSpeed", 60.f ) * GetForwardNormal();
			m_physicsRadius = g_gameConfigBlackboard.GetValue( "bulletPhysicsRadius", 0.5f );
			m_cosmeticRadius = g_gameConfigBlackboard.GetValue( "bulletCosmeticRadius", 1.f );
			m_isBullet = true;
			m_isBolt = false;
			break;

		case ENTITY_TYPE_EVIL_BOLT:
			m_faction = ENTITY_FACTION_EVIL;
			m_health = g_gameConfigBlackboard.GetValue( "evilBoltMaxHealth", 1 );
			m_velocity = g_gameConfigBlackboard.GetValue( "evilBoltSpeed", 60.f ) * GetForwardNormal();
			m_physicsRadius = g_gameConfigBlackboard.GetValue( "boltPhysicsRadius", 0.5f );
			m_cosmeticRadius = g_gameConfigBlackboard.GetValue( "boltCosmeticRadius", 1.f );
			m_isBullet = false;
			m_isBolt = true;
			break;

		default:
			m_faction = ENTITY_FACTION_NEUTRAL;
			break;
	}

	m_isPushedByWalls = g_gameConfigBlackboard.GetValue( "bulletsArePushedByWalls", false );
	m_isPushedByEntities = g_gameConfigBlackboard.GetValue( "bulletsArePushedByEntities", false );
	m_doesPushEntities = g_gameConfigBlackboard.GetValue( "bulletsDoPushEntities", false );
	m_isHitByBullets = g_gameConfigBlackboard.GetValue( "bulletsAreHitByBullets", false );
	m_canSwim = g_gameConfigBlackboard.GetValue( "bulletsCanSwim", true );

	InitializeVertexArray();
}


//-----------------------------------------------------------------------------------------------
Bullet::~Bullet() = default;


//-----------------------------------------------------------------------------------------------
void Bullet::Update( float deltaSeconds )
{
	if ( m_isDead || m_isGarbage )
	{
		return;
	}

	m_age += deltaSeconds;

	UpdatePhysics( deltaSeconds );
	ResolveCollision();
}


//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
void Bullet::UpdatePhysics( float deltaSeconds )
{
	// If current position is in solid tile, push out and reflect
	if ( g_game->m_currentMap->IsPointInSolidTile( m_position ) )
	{
		// Push out
		Vec2 bulletCenter = m_position;
		float bulletRadius = m_physicsRadius;
		IntVec2 impactedTileCoords = g_game->m_currentMap->GetTileCoordsForWorldPosition( m_position );
		AABB2 tileBounds = g_game->m_currentMap->GetTileBounds( impactedTileCoords );
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

			if ( m_faction == ENTITY_FACTION_GOOD )
			{
				Tile* impactedTile = g_game->m_currentMap->GetTile( impactedTileCoords );
				if ( impactedTile != nullptr && impactedTile->IsDestructible() )
				{
					impactedTile->TakeDamage( 1 );
				}
			}

			m_velocity.Reflect( normal );
			m_orientationDegrees = m_velocity.GetOrientationDegrees();
			m_position = bulletCenter;
		}
	}

	Vec2 nextPosition = m_position + m_velocity * deltaSeconds;
	if ( g_game->m_currentMap->IsPointInSolidTile( nextPosition ) )
	{
		if ( m_faction == ENTITY_FACTION_GOOD )
		{
			IntVec2 nextTileCoords = g_game->m_currentMap->GetTileCoordsForWorldPosition( nextPosition );
			Tile* impactedTile = g_game->m_currentMap->GetTile( nextTileCoords );
			if ( impactedTile != nullptr && impactedTile->IsDestructible() )
			{
				impactedTile->TakeDamage( 1 );
			}
		}

		TakeDamage( 1 );

		IntVec2 currentTileCoords = g_game->m_currentMap->GetTileCoordsForWorldPosition( m_position );
		IntVec2 nextTileCoords = g_game->m_currentMap->GetTileCoordsForWorldPosition( nextPosition );
		Vec2 normal = Vec2::MakeFromIntVec2( currentTileCoords - nextTileCoords );
		normal.Normalize();
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
			bulletTexture = g_game->m_goodBulletTexture;
			break;

		case ENTITY_TYPE_GOOD_BOLT:
			bulletTexture = g_game->m_goodBoltTexture;
			break;

		case ENTITY_TYPE_EVIL_BULLET:
			bulletTexture = g_game->m_evilBulletTexture;
			break;

		case ENTITY_TYPE_EVIL_BOLT:
			bulletTexture = g_game->m_evilBoltTexture;
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
	g_game->m_currentMap->SpawnExplosionAtPosition( m_position, 0.3f, 2.f );
	Entity::Die();
}


//-----------------------------------------------------------------------------------------------
void Bullet::InitializeVertexArray()
{
	if ( m_isBolt )
	{
		float halfWidth = g_gameConfigBlackboard.GetValue( "boltLength", 3.f ) * 0.5f;
		float halfHeight = g_gameConfigBlackboard.GetValue( "boltWidth", 1.5f ) * 0.5f;
		AABB2 bulletAABB2 = AABB2( -halfWidth, -halfHeight, halfWidth, halfHeight );
		AddVertsForAABB2D( m_vertexArray, bulletAABB2, Rgba8::WHITE );
	}
	else if ( m_isBullet )
	{
		float halfWidth = g_gameConfigBlackboard.GetValue( "bulletLength", 2.f ) * 0.5f;
		float halfHeight = g_gameConfigBlackboard.GetValue( "bulletWidth", 1.f ) * 0.5f;
		AABB2 bulletAABB2 = AABB2( -halfWidth, -halfHeight, halfWidth, halfHeight );
		AddVertsForAABB2D( m_vertexArray, bulletAABB2, Rgba8::WHITE );
	}
}


//-----------------------------------------------------------------------------------------------
void Bullet::ResolveCollision()
{
	if ( m_isDead || m_isGarbage ) return;

	if ( m_age < 0.01f ) return;

	// If overlapping with an entity of different faction, deal damage and die
	EntityFaction oppositeFaction = GetOppositeFaction();
	for ( Entity* otherEntity : g_game->m_currentMap->m_entityListsByFaction[oppositeFaction] )
	{
		if ( otherEntity == nullptr || otherEntity->m_isDead || !otherEntity->m_isHitByBullets )
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