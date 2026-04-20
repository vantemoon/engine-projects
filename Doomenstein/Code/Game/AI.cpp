#include "Game/AI.hpp"
#include "Game/Actor.hpp"
#include "Game/Map.hpp"
#include "Game/WeaponDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
AI::AI()
{
}


//-----------------------------------------------------------------------------------------------
AI::~AI()
{
}


//-----------------------------------------------------------------------------------------------
void AI::Update( float deltaSeconds )
{
	Actor* self = GetActor();
	if ( self == nullptr || self->m_definition == nullptr || self->m_isDead || self->m_isDestroyed )
	{
		return;
	}

	Actor* target = nullptr;
	if ( m_targetActorHandle.IsValid() && m_map != nullptr )
	{
		target = m_map->GetActorByHandle( m_targetActorHandle );
		if ( target == nullptr
			|| target == self
			|| target->m_isDead
			|| target->m_isDestroyed
			|| target->m_definition == nullptr
			|| target->m_definition->m_faction == self->m_definition->m_faction
			|| target->m_definition->m_faction == "NEUTRAL"
			|| self->m_definition->m_faction == "NEUTRAL" )
		{
			m_targetActorHandle = ActorHandle::INVALID;
			target = nullptr;
		}
	}

	if ( target == nullptr && m_map != nullptr )
	{
		target = m_map->GetClosestVisibleEnemy( self );
		if ( target != nullptr )
		{
			m_targetActorHandle = target->m_handle;
		}
	}

	if ( target == nullptr )
	{
		return;
	}

	Vec3 toTarget = target->m_position - self->m_position;
	Vec3 toTargetXY = Vec3( toTarget.x, toTarget.y, 0.f );
	float distToTargetXY = toTargetXY.GetLength();

	float maxTurnDegrees = self->m_definition->m_turnSpeed * deltaSeconds;
	if ( maxTurnDegrees > 0.f && toTargetXY != Vec3::ZERO )
	{
		self->TurnInDirection( toTargetXY, maxTurnDegrees );
	}

	WeaponDefinition const* weaponDef = nullptr;
	if ( self->m_currentWeapon != nullptr )
	{
		weaponDef = self->m_currentWeapon->m_definition;
	}

	bool hasMeleeWeapon = ( weaponDef != nullptr && weaponDef->m_meleeCount > 0 && weaponDef->m_meleeRange > 0.f );

	float stopDistance = self->m_definition->m_physicsRadius + target->m_definition->m_physicsRadius + 0.05f;
	if ( hasMeleeWeapon )
	{
		float preferredMeleeDistance = weaponDef->m_meleeRange * 0.8f;
		if ( preferredMeleeDistance > stopDistance )
		{
			stopDistance = preferredMeleeDistance;
		}
	}

	if ( toTargetXY != Vec3::ZERO && distToTargetXY > stopDistance )
	{
		float minMoveSpeed = self->m_definition->m_walkSpeed;
		if ( minMoveSpeed <= 0.f )
		{
			minMoveSpeed = self->m_definition->m_runSpeed * 0.25f;
		}

		float slowDownDistance = stopDistance + 1.0f;
		float speedScale = 1.f;
		if ( distToTargetXY < slowDownDistance )
		{
			speedScale = GetClamped( ( distToTargetXY - stopDistance ) / ( slowDownDistance - stopDistance ), 0.f, 1.f );
		}

		float moveSpeed = minMoveSpeed + ( self->m_definition->m_runSpeed - minMoveSpeed ) * speedScale;
		self->MoveInDirection( toTargetXY, moveSpeed );
	}

	bool shouldAttack = false;
	if ( weaponDef != nullptr )
	{
		bool canDoMelee = ( weaponDef->m_meleeCount > 0 && weaponDef->m_meleeRange > 0.f );
		bool canDoRay = ( weaponDef->m_rayCount > 0 && weaponDef->m_rayRange > 0.f );
		bool canDoProjectile = ( weaponDef->m_projectileCount > 0 && !weaponDef->m_projectileActorDefName.empty() );

		if ( canDoMelee && distToTargetXY <= weaponDef->m_meleeRange )
		{
			shouldAttack = true;
		}
		else if ( canDoRay || canDoProjectile )
		{
			shouldAttack = true;
		}
	}

	if ( shouldAttack )
	{
		self->Attack();
	}
}


//-----------------------------------------------------------------------------------------------
void AI::DamagedBy( Actor* attacker )
{
	Actor* self = GetActor();
	if ( self == nullptr || self->m_definition == nullptr || attacker == nullptr || attacker == self || attacker->m_isDead || attacker->m_isDestroyed || attacker->m_definition == nullptr )
	{
		return;
	}

	if ( attacker->m_definition->m_faction == self->m_definition->m_faction
		|| attacker->m_definition->m_faction == "NEUTRAL"
		|| self->m_definition->m_faction == "NEUTRAL" )
	{
		return;
	}

	m_targetActorHandle = attacker->m_handle;
}