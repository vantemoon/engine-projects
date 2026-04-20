#include "Game/Weapon.hpp"
#include "Game/Actor.hpp"
#include "Game/Map.hpp"
#include "Game/WeaponDefinition.hpp"
#include "Engine/Core/DebugRender.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


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

	for ( int rayIndex = 0; rayIndex < m_definition->m_rayCount; ++rayIndex )
	{
		Vec3 rayDir = GetRandomDirectionInCone( baseForward, m_definition->m_rayCone );

		Actor* rayHitActor = nullptr;
		RaycastResult3D rayResult = owner->m_map->RaycastAll( fireStartPos, rayDir, m_definition->m_rayRange, owner, &rayHitActor );

		if ( rayResult.m_didImpact )
		{
			Vec3 debugOffset( 0.f, 0.f, -0.08f );
			Vec3 debugStart = fireStartPos + debugOffset;
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
		spawnInfo.m_position = fireStartPos;

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
		Actor* meleeTarget = owner->m_map->GetClosestActorInSector( fireStartPos, baseForward, m_definition->m_meleeRange, m_definition->m_meleeArc, owner );
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