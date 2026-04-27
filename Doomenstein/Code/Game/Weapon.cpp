#include "Game/Weapon.hpp"
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include "Game/WeaponDefinition.hpp"
#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"


//-----------------------------------------------------------------------------------------------
Weapon::Weapon( WeaponDefinition const* definition )
	: m_definition( definition )
	, m_refireTimer( 0.0 )
{
	if ( m_definition != nullptr )
	{
		m_refireTimer.m_period = m_definition->m_refireTime;
	}
}


//-----------------------------------------------------------------------------------------------
Weapon::~Weapon()
{
}


//-----------------------------------------------------------------------------------------------
Vec3 Weapon::GetRandomDirectionInCone( Vec3 const& baseForward, float coneDegrees ) const
{
	RandomNumberGenerator rng;

	if ( coneDegrees <= 0.f )
	{
		return baseForward;
	}

	float halfCone = coneDegrees * 0.5f;

	float baseYaw = Atan2Degrees( baseForward.y, baseForward.x );
	float basePitch = Atan2Degrees( baseForward.z, Vec2( baseForward.x, baseForward.y ).GetLength() );

	float yaw = baseYaw + rng.RollRandomFloatInRange( -halfCone, halfCone );
	float pitch = basePitch + rng.RollRandomFloatInRange( -halfCone, halfCone );

	float cosPitch = CosDegrees( pitch );
	Vec3 dir = Vec3(
		cosPitch * CosDegrees( yaw ),
		cosPitch * SinDegrees( yaw ),
		SinDegrees( pitch ) );

	if ( dir == Vec3::ZERO )
	{
		return baseForward;
	}

	return dir.GetNormalized();
}


//-----------------------------------------------------------------------------------------------
bool Weapon::CanFire( Actor const* owner ) const
{
	if ( m_definition == nullptr || owner == nullptr || owner->m_definition == nullptr || owner->m_map == nullptr )
	{
		return false;
	}

	if ( m_refireTimer.m_period > 0.0 && !m_refireTimer.IsStopped() && !m_refireTimer.HasPeriodElapsed() )
	{
		return false;
	}

	return true;
}


//-----------------------------------------------------------------------------------------------
void Weapon::Fire( Actor* owner )
{
	if ( m_definition == nullptr || owner == nullptr || owner->m_definition == nullptr || owner->m_map == nullptr )
	{
		return;
	}

	if ( m_refireTimer.m_period > 0.0 && !m_refireTimer.IsStopped() && !m_refireTimer.HasPeriodElapsed() )
	{
		return;
	}

	RandomNumberGenerator damageRng;

	Vec3 fireStartPos = owner->m_position;
	fireStartPos.z += owner->m_definition->m_eyeHeight;

	Vec3 baseForward = owner->m_orientation.GetForwardDir_IFwd_JLeft_KUp();
	if ( baseForward == Vec3::ZERO )
	{
		baseForward = Vec3( 1.f, 0.f, 0.f );
	}
	baseForward = baseForward.GetNormalized();

	Vec3 attackSpawnCenter = fireStartPos;
	Vec2 ownerForwardXY( baseForward.x, baseForward.y );
	if ( ownerForwardXY != Vec2::ZERO )
	{
		ownerForwardXY.Normalize();
		attackSpawnCenter.x += ownerForwardXY.x * owner->m_definition->m_physicsRadius;
		attackSpawnCenter.y += ownerForwardXY.y * owner->m_definition->m_physicsRadius;
	}
	else
	{
		attackSpawnCenter.x += owner->m_definition->m_physicsRadius;
	}

	for ( int rayIndex = 0; rayIndex < m_definition->m_rayCount; ++rayIndex )
	{
		Vec3 rayDir = GetRandomDirectionInCone( baseForward, m_definition->m_rayCone );

		Actor* rayHitActor = nullptr;
		RaycastResult3D rayResult = owner->m_map->RaycastAll( attackSpawnCenter, rayDir, m_definition->m_rayRange, owner, &rayHitActor );

		if ( rayResult.m_didImpact )
		{
			Vec3 debugOffset( 0.f, 0.f, -0.08f );
			Vec3 debugStart = attackSpawnCenter + debugOffset;
			Vec3 debugEnd = rayResult.m_impactPos + debugOffset;
			DebugAddWorldCylinder( debugStart, debugEnd, 0.01f, 1.0f, Rgba8::BLUE, Rgba8::BLUE );
		}

		if ( rayHitActor != nullptr )
		{
			int minDamage = ( int ) m_definition->m_rayDamage.m_min;
			int maxDamage = ( int ) m_definition->m_rayDamage.m_max;
			if ( minDamage > maxDamage )
			{
				int temp = minDamage;
				minDamage = maxDamage;
				maxDamage = temp;
			}

			if ( maxDamage > 0 )
			{
				int damageAmount = damageRng.RollRandomIntInRange( minDamage, maxDamage );
				if ( damageAmount > 0 )
				{
					rayHitActor->TakeDamage( damageAmount, owner );
				}
			}

			if ( m_definition->m_rayImpulse > 0.f )
			{
				rayHitActor->AddImpulse( rayDir * m_definition->m_rayImpulse );
			}
		}
	}

	for ( int projectileIndex = 0; projectileIndex < m_definition->m_projectileCount; ++projectileIndex )
	{
		if ( m_definition->m_projectileActorDefName.empty() )
		{
			break;
		}

		Vec3 projectileDir = GetRandomDirectionInCone( baseForward, m_definition->m_projectileCone );

		SpawnInfo spawnInfo;
		spawnInfo.m_actor = m_definition->m_projectileActorDefName;

		ActorDefinition const* projectileDef = ActorDefinition::GetActorDefinitionByName( m_definition->m_projectileActorDefName );
		float projectileHalfHeight = 0.f;
		if ( projectileDef != nullptr )
		{
			projectileHalfHeight = projectileDef->m_physicsHeight * 0.5f;
		}

		spawnInfo.m_position = attackSpawnCenter;
		spawnInfo.m_position.z -= projectileHalfHeight;

		float yaw = Atan2Degrees( projectileDir.y, projectileDir.x );
		float pitch = Atan2Degrees( projectileDir.z, Vec2( projectileDir.x, projectileDir.y ).GetLength() );
		spawnInfo.m_orientation = Vec3( yaw, pitch, 0.f );

		spawnInfo.m_velocity = projectileDir * m_definition->m_projectileSpeed;

		Actor* spawnedProjectile = owner->m_map->SpawnActor( spawnInfo );
		if ( spawnedProjectile != nullptr )
		{
			spawnedProjectile->m_ownerHandle = owner->m_handle;
		}
	}

	for ( int meleeIndex = 0; meleeIndex < m_definition->m_meleeCount; ++meleeIndex )
	{
		Actor* meleeTarget = owner->m_map->GetClosestActorInSector(
			owner->m_position,
			baseForward,
			m_definition->m_meleeRange,
			m_definition->m_meleeArc,
			owner );

		if ( meleeTarget == nullptr )
		{
			continue;
		}

		int minDamage = ( int ) m_definition->m_meleeDamage.m_min;
		int maxDamage = ( int ) m_definition->m_meleeDamage.m_max;
		if ( minDamage > maxDamage )
		{
			int temp = minDamage;
			minDamage = maxDamage;
			maxDamage = temp;
		}

		if ( maxDamage > 0 )
		{
			int damageAmount = damageRng.RollRandomIntInRange( minDamage, maxDamage );
			if ( damageAmount > 0 )
			{
				meleeTarget->TakeDamage( damageAmount, owner );
			}
		}

		if ( m_definition->m_meleeImpulse > 0.f )
		{
			Vec3 impulseDir = meleeTarget->m_position - owner->m_position;
			if ( impulseDir != Vec3::ZERO )
			{
				impulseDir = impulseDir.GetNormalized();
				meleeTarget->AddImpulse( impulseDir * m_definition->m_meleeImpulse );
			}
		}
	}

	if ( m_refireTimer.m_period > 0.0 )
	{
		m_refireTimer.Start();
	}
}


//-----------------------------------------------------------------------------------------------
void Weapon::Render( Actor const* owner ) const
{
	if ( owner == nullptr || g_engine == nullptr || g_engine->m_renderer == nullptr )
	{
		return;
	}

	if ( m_definition != nullptr && m_definition->m_name == "DemonMelee" )
	{
		return;
	}

	if ( g_app == nullptr || g_app->m_game == nullptr || g_app->m_game->m_player == nullptr || g_app->m_game->m_screenCamera == nullptr )
	{
		return;
	}

	if ( g_app->m_game->m_player->GetActor() != owner )
	{
		return;
	}

	Texture* hudBaseTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/Hud_Base.png" );
	if ( hudBaseTexture == nullptr )
	{
		return;
	}

	Camera screenCamera = *g_app->m_game->m_player->m_playerScreenCamera;
	g_engine->m_renderer->BeginCamera( screenCamera );
	g_engine->m_renderer->SetBlendMode( BlendMode::ALPHA );
	g_engine->m_renderer->SetDepthMode( DepthMode::DISABLED );
	g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_NONE );

	IntVec2 hudDimensions = hudBaseTexture->GetDimensions();
	float scaleFactor = SCREEN_SIZE_X / ( float ) hudDimensions.x;

	Vec2 hudMins( 0.f, 0.f );
	Vec2 hudMaxs( SCREEN_SIZE_X, ( float ) hudDimensions.y * scaleFactor );
	AABB2 hudBounds( hudMins, hudMaxs );

	std::vector<Vertex> hudVerts;
	hudVerts.reserve( 6 );
	AddVertsForAABB2D( hudVerts, hudBounds, Rgba8::WHITE );

	g_engine->m_renderer->BindTexture( hudBaseTexture );
	g_engine->m_renderer->SetModelConstants();
	g_engine->m_renderer->DrawVertexArray( hudVerts );
	g_engine->m_renderer->BindTexture( nullptr );

	g_engine->m_renderer->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
	g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
	g_engine->m_renderer->EndCamera( screenCamera );
}