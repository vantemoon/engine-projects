#include "Game/Actor.hpp"
#include "Game/AI.hpp"
#include "Game/App.hpp"
#include "Game/Controller.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include "Game/WeaponDefinition.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
Actor::Actor()
	: m_position( Vec3::ZERO )
	, m_orientation( EulerAngles::ZERO )
	, m_color( Rgba8::RED )
	, m_deadTimer( 0.0 )
	, m_animationClock( Clock::GetSystemClock() )
{
}


//-----------------------------------------------------------------------------------------------
Actor::Actor( ActorHandle handle, ActorDefinition const* definition, Map* map )
	: m_handle( handle )
	, m_definition( definition )
	, m_map( map )
	, m_position( Vec3::ZERO )
	, m_orientation( EulerAngles::ZERO )
	, m_color( Rgba8::RED )
	, m_deadTimer( 0.0 )
	, m_animationClock( Clock::GetSystemClock() )
{
	if ( m_definition == nullptr )
	{
		return;
	}

	m_isProjectile = m_definition->m_isProjectile;
	m_deadTimer.m_period = m_definition->corpseLifetime;

	m_maxHealth = m_definition->m_health;
	m_currentHealth = m_maxHealth;

	if ( m_definition->m_aiEnabled )
	{
		m_aiController = new AI();
		m_aiController->m_map = m_map;
		m_aiController->m_possessedActor = m_handle;
		m_controller = m_aiController;
	}

	m_inventory.reserve( m_definition->m_weaponDefNames.size() );
	for ( std::string const& weaponDefName : m_definition->m_weaponDefNames )
	{
		WeaponDefinition const* weaponDef = WeaponDefinition::GetWeaponDefinitionByName( weaponDefName );
		Weapon newWeapon( weaponDef );
		m_inventory.push_back( newWeapon );
	}
	m_currentWeapon = m_inventory.empty() ? nullptr : &m_inventory[0];

	if ( g_engine != nullptr && g_engine->m_renderer != nullptr )
	{
		if ( !m_definition->m_visualSpriteSheet.empty() )
		{
			m_visualTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( m_definition->m_visualSpriteSheet.c_str() );
		}
		if ( !m_definition->m_visualShader.empty() )
		{
			m_visualShader = g_engine->m_renderer->CreateOrGetShader( m_definition->m_visualShader.c_str(), VertexType::VERTEX_PCU );
		}
	}

	if ( g_engine != nullptr && g_engine->m_audioSystem != nullptr )
	{
		int soundTypeCount = ( int ) m_definition->m_soundTypes.size();
		int soundNameCount = ( int ) m_definition->m_soundNames.size();
		int soundCount = soundTypeCount;
		if ( soundNameCount < soundCount )
		{
			soundCount = soundNameCount;
		}

		m_soundTypes.reserve( soundCount );
		m_soundIDs.reserve( soundCount );

		for ( int soundIndex = 0; soundIndex < soundCount; ++soundIndex )
		{
			std::string const& soundType = m_definition->m_soundTypes[soundIndex];
			std::string const& soundName = m_definition->m_soundNames[soundIndex];
			if ( soundType.empty() || soundName.empty() )
			{
				continue;
			}

			SoundID soundID = g_engine->m_audioSystem->CreateOrGetSound( soundName );
			if ( soundID != MISSING_SOUND_ID )
			{
				m_soundTypes.push_back( soundType );
				m_soundIDs.push_back( soundID );
			}
		}
	}

	if ( !m_definition->m_animationGroupNames.empty() )
	{
		PlayAnimationByName( m_definition->m_animationGroupNames[0] );
	}
}


//-----------------------------------------------------------------------------------------------
Actor::~Actor()
{
	if ( m_controller == m_aiController )
	{
		m_controller = nullptr;
	}

	delete m_aiController;
	m_aiController = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Actor::PlayAnimationByName( std::string const& animationGroupName )
{
	if ( m_definition == nullptr || animationGroupName.empty() )
	{
		return;
	}

	int const groupIndex = GetAnimationGroupIndexByName( animationGroupName );
	if ( groupIndex < 0 )
	{
		return;
	}

	if ( m_currentAnimationGroupIndex == groupIndex )
	{
		return;
	}

	m_currentAnimationGroupIndex = groupIndex;
	m_currentAnimationGroupName = animationGroupName;
	m_animationClock.Reset();
	m_animationClock.SetTimeScale( 1.0 );
}


//-----------------------------------------------------------------------------------------------
void Actor::PlaySoundByType( std::string const& soundType, float volume ) const
{
	if ( soundType.empty() || g_engine == nullptr || g_engine->m_audioSystem == nullptr )
	{
		return;
	}

	float const clampedVolume = GetClamped( volume, 0.f, 1.f );
	for ( int soundIndex = 0; soundIndex < ( int ) m_soundTypes.size() && soundIndex < ( int ) m_soundIDs.size(); ++soundIndex )
	{
		if ( m_soundTypes[soundIndex] == soundType )
		{
			g_engine->m_audioSystem->StartSound( m_soundIDs[soundIndex], false, clampedVolume );
			return;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Actor::Update()
{
	if ( m_definition == nullptr || m_isDestroyed )
	{
		return;
	}

	if ( m_definition->m_dieOnSpawn && !m_isDead )
	{
		m_isDead = true;
		m_currentHealth = 0;

		m_velocity = Vec3::ZERO;
		m_acceleration = Vec3::ZERO;

		m_deadTimer.Stop();
		m_deadTimer.m_period = m_definition->corpseLifetime;

		if ( m_deadTimer.m_period <= 0.0 )
		{
			m_isDestroyed = true;
		}
		else
		{
			m_deadTimer.Start();
		}

		if ( GetAnimationGroupIndexByName( "Death" ) != -1 )
		{
			PlayAnimationByName( "Death" );
		}
	}

	if ( !m_definition->m_animationGroupNames.empty() && m_currentAnimationGroupIndex < 0 )
	{
		PlayAnimationByName( m_definition->m_animationGroupNames[0] );
	}

	if ( m_currentAnimationGroupIndex >= 0 && m_currentAnimationGroupIndex < ( int ) m_definition->m_animationGroupScaleBySpeed.size() )
	{
		if ( m_definition->m_animationGroupScaleBySpeed[m_currentAnimationGroupIndex] )
		{
			double timeScale = 1.0;
			if ( m_definition->m_runSpeed > 0.f )
			{
				float const speed = m_velocity.GetLength();
				timeScale = ( double ) ( speed / m_definition->m_runSpeed );
			}
			m_animationClock.SetTimeScale( timeScale );
		}
		else
		{
			m_animationClock.SetTimeScale( 1.0 );
		}
	}

	if ( !m_isDead && m_currentAnimationGroupName != "Death" && IsCurrentAnimationGroupComplete() )
	{
		if ( GetAnimationGroupIndexByName( "Walk" ) != -1 )
		{
			PlayAnimationByName( "Walk" );
		}
		else if ( !m_definition->m_animationGroupNames.empty() )
		{
			PlayAnimationByName( m_definition->m_animationGroupNames[0] );
		}
	}

	if ( m_isDead )
	{
		m_velocity = Vec3::ZERO;
		m_acceleration = Vec3::ZERO;

		if ( m_deadTimer.m_period <= 0.0 || m_deadTimer.HasPeriodElapsed() )
		{
			m_isDestroyed = true;
		}
		return;
	}

	UpdatePhysics();
}


//-----------------------------------------------------------------------------------------------
void Actor::UpdatePhysics()
{
	if ( m_definition == nullptr || !m_definition->m_isSimulated || m_isDead || m_isDestroyed )
	{
		return;
	}

	float deltaSeconds = static_cast<float>( Clock::GetSystemClock().GetDeltaSeconds() );

	if ( !m_definition->m_isFlying )
	{
		m_velocity.z = 0.f;
		m_acceleration.z = 0.f;
	}

	Vec3 dragForce = m_definition->m_drag * -m_velocity;
	AddForce( dragForce );

	m_velocity += m_acceleration * deltaSeconds;
	m_position += m_velocity * deltaSeconds;

	if ( !m_definition->m_isFlying )
	{
		m_velocity.z = 0.f;
		m_position.z = 0.f;
	}

	m_acceleration = Vec3::ZERO;
}


//-----------------------------------------------------------------------------------------------
void Actor::TakeDamage( int damageAmount, Actor* attacker )
{
	if ( m_definition == nullptr || damageAmount <= 0 || m_isDead || m_isDestroyed )
	{
		return;
	}

	if ( attacker != nullptr && m_aiController != nullptr && m_controller == m_aiController )
	{
		m_aiController->DamagedBy( attacker );
	}

	m_currentHealth -= damageAmount;

	if ( m_currentHealth <= 0 )
	{
		m_isDead = true;
		m_currentHealth = 0;

		Player* player = g_app->m_game->GetPlayerFromActor( attacker );
		if ( player != nullptr )
		{
			player->m_killCount++;
		}

		player = g_app->m_game->GetPlayerFromActor( this );
		if ( player != nullptr )
		{
			player->m_deathCount++;
		}

		m_velocity = Vec3::ZERO;
		m_acceleration = Vec3::ZERO;

		m_deadTimer.Stop();
		m_deadTimer.m_period = m_definition->corpseLifetime;

		PlaySoundByType( "Death" );
		if ( GetAnimationGroupIndexByName( "Death" ) != -1 )
		{
			PlayAnimationByName( "Death" );
		}

		if ( m_deadTimer.m_period <= 0.0 )
		{
			m_isDestroyed = true;
		}
		else
		{
			m_deadTimer.Start();
		}
	}
	else
	{
		PlaySoundByType( "Hurt" );
		if ( GetAnimationGroupIndexByName( "Hurt" ) != -1 )
		{
			PlayAnimationByName( "Hurt" );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Actor::AddForce( Vec3 const& force )
{
	if ( m_definition == nullptr || !m_definition->m_isSimulated || m_isDead || m_isDestroyed )
	{
		return;
	}

	m_acceleration += force;
}


//-----------------------------------------------------------------------------------------------
void Actor::AddImpulse( Vec3 const& impulse )
{
	if ( m_definition == nullptr || !m_definition->m_isSimulated || m_isDead || m_isDestroyed )
	{
		return;
	}

	m_velocity += impulse;
}


//-----------------------------------------------------------------------------------------------
void Actor::OnCollide( Actor* otherActor, Vec3 const& collisionNormal )
{
	if ( m_definition == nullptr || m_isDead || m_isDestroyed )
	{
		return;
	}

	if ( otherActor != nullptr )
	{
		if ( !m_definition->m_collideWithActors )
		{
			return;
		}
	}
	else
	{
		if ( !m_definition->m_collideWithWorld )
		{
			return;
		}
	}

	Vec3 impulseDirection = collisionNormal;
	if ( impulseDirection == Vec3::ZERO )
	{
		if ( otherActor != nullptr )
		{
			impulseDirection = otherActor->m_position - m_position;
		}
		else
		{
			impulseDirection = -m_velocity;
		}
	}

	if ( impulseDirection != Vec3::ZERO )
	{
		impulseDirection = impulseDirection.GetNormalized();
	}

	if ( otherActor != nullptr )
	{
		int minDamage = ( int ) m_definition->m_damageOnCollide.m_min;
		int maxDamage = ( int ) m_definition->m_damageOnCollide.m_max;
		if ( minDamage > maxDamage )
		{
			int temp = minDamage;
			minDamage = maxDamage;
			maxDamage = temp;
		}

		if ( maxDamage > 0 )
		{
			static RandomNumberGenerator s_rng;
			int const damageAmount = s_rng.RollRandomIntInRange( minDamage, maxDamage );
			if ( damageAmount > 0 )
			{
				Actor* damageInstigator = this;
				if ( m_isProjectile && m_map != nullptr && m_ownerHandle.IsValid() )
				{
					Actor* ownerActor = m_map->GetActorByHandle( m_ownerHandle );
					if ( ownerActor != nullptr && !ownerActor->m_isDead && !ownerActor->m_isDestroyed )
					{
						damageInstigator = ownerActor;
					}
				}

				otherActor->TakeDamage( damageAmount, damageInstigator );
			}
		}

		if ( m_definition->m_impulseOnCollide > 0.f && impulseDirection != Vec3::ZERO )
		{
			otherActor->AddImpulse( impulseDirection * m_definition->m_impulseOnCollide );
		}
	}

	PlaySoundByType( "Collide", 0.65f );

	if ( m_definition->m_dieOnCollide )
	{
		TakeDamage( m_currentHealth );
	}
}


//-----------------------------------------------------------------------------------------------
void Actor::OnPossessed( Controller* newController )
{
	if ( newController == nullptr || m_controller == newController )
	{
		return;
	}

	if ( m_controller != nullptr )
	{
		m_controller->Possess( ActorHandle::INVALID );
	}

	m_controller = newController;
}


//-----------------------------------------------------------------------------------------------
void Actor::OnUnpossessed()
{
	Controller* previousController = m_controller;
	m_controller = nullptr;

	if ( previousController != nullptr && previousController != m_aiController && m_aiController != nullptr )
	{
		m_aiController->Possess( m_handle );
	}
}


//-----------------------------------------------------------------------------------------------
void Actor::MoveInDirection( Vec3 const& moveDirection, float speed )
{
	if ( m_definition == nullptr || moveDirection == Vec3::ZERO || speed <= 0.f )
	{
		return;
	}

	bool const isAttackPlaying = ( m_currentAnimationGroupName == "Attack" ) && !IsCurrentAnimationGroupComplete();
	bool const isHurtPlaying = ( m_currentAnimationGroupName == "Hurt" ) && !IsCurrentAnimationGroupComplete();

	if ( !isAttackPlaying && !isHurtPlaying )
	{
		if ( GetAnimationGroupIndexByName( "Walk" ) != -1 )
		{
			PlayAnimationByName( "Walk" );
		}
	}

	Vec3 moveDirectionNormalized = moveDirection.GetNormalized();
	Vec3 movementForce = moveDirectionNormalized * ( speed * m_definition->m_drag );
	AddForce( movementForce );
}


//-----------------------------------------------------------------------------------------------
void Actor::TurnInDirection( Vec3 const& turnDirection, float maxTurnDegrees )
{
	if ( m_definition == nullptr || turnDirection == Vec3::ZERO || maxTurnDegrees <= 0.f )
	{
		return;
	}

	float targetYaw = turnDirection.GetOrientationAboutZDegrees();
	float yawDifference = GetShortestAngularDispDegrees( m_orientation.m_yawDegrees, targetYaw );
	float clampedYawChange = GetClamped( yawDifference, -maxTurnDegrees, maxTurnDegrees );

	m_orientation.m_yawDegrees += clampedYawChange;
}


//-----------------------------------------------------------------------------------------------
void Actor::Attack()
{
	if ( m_definition == nullptr || m_currentWeapon == nullptr || m_isDead || m_isDestroyed )
	{
		return;
	}

	if ( !m_currentWeapon->CanFire( this ) )
	{
		return;
	}

	PlaySoundByType( "Attack" );
	if ( GetAnimationGroupIndexByName( "Attack" ) != -1 )
	{
		if ( m_currentAnimationGroupName != "Attack" || IsCurrentAnimationGroupComplete() )
		{
			PlayAnimationByName( "Attack" );
		}
	}

	m_currentWeapon->Fire( this );
}


//-----------------------------------------------------------------------------------------------
void Actor::EquipWeapon( Weapon* weapon )
{
	if ( m_definition == nullptr || weapon == nullptr )
	{
		return;
	}

	for ( int i = 0; i < static_cast<int>( m_inventory.size() ); i++ )
	{
		if ( &m_inventory[i] == weapon )
		{
			m_currentWeapon = &m_inventory[i];
			return;
		}
	}
}


//-----------------------------------------------------------------------------------------------
int Actor::GetAnimationGroupIndexByName( std::string const& animationGroupName ) const
{
	if ( m_definition == nullptr )
	{
		return -1;
	}

	for ( int groupIndex = 0; groupIndex < ( int ) m_definition->m_animationGroupNames.size(); ++groupIndex )
	{
		if ( m_definition->m_animationGroupNames[groupIndex] == animationGroupName )
		{
			return groupIndex;
		}
	}
	return -1;
}


//-----------------------------------------------------------------------------------------------
int Actor::GetBestDirectionAnimationIndex() const
{
	if ( m_definition == nullptr || m_currentAnimationGroupIndex < 0 )
	{
		return -1;
	}

	Vec3 toViewer = Vec3( 1.f, 0.f, 0.f );
	Player* renderingPlayer = nullptr;
	if ( g_app != nullptr && g_app->m_game != nullptr )
	{
		renderingPlayer = g_app->m_game->m_currentRenderingPlayer;
	}
	if ( renderingPlayer != nullptr && renderingPlayer->GetActor() != nullptr )
	{
		toViewer = renderingPlayer->GetActor()->m_position - m_position;
	}
	if ( toViewer == Vec3::ZERO )
	{
		toViewer = Vec3( 1.f, 0.f, 0.f );
	}
	toViewer = ( -toViewer ).GetNormalized();

	float const cosYaw = CosDegrees( m_orientation.m_yawDegrees );
	float const sinYaw = SinDegrees( m_orientation.m_yawDegrees );

	float bestDot = -9999.f;
	int bestIndex = -1;

	for ( int directionIndex = 0; directionIndex < ( int ) m_definition->m_animationDirectionGroupIndex.size(); ++directionIndex )
	{
		if ( m_definition->m_animationDirectionGroupIndex[directionIndex] != m_currentAnimationGroupIndex )
		{
			continue;
		}

		Vec3 localDirection = m_definition->m_animationDirectionVectors[directionIndex];

		Vec3 worldDirection;
		worldDirection.x = ( localDirection.x * cosYaw ) - ( localDirection.y * sinYaw );
		worldDirection.y = ( localDirection.x * sinYaw ) + ( localDirection.y * cosYaw );
		worldDirection.z = localDirection.z;

		if ( worldDirection == Vec3::ZERO )
		{
			continue;
		}
		worldDirection = worldDirection.GetNormalized();

		float const dot = ( worldDirection.x * toViewer.x ) + ( worldDirection.y * toViewer.y ) + ( worldDirection.z * toViewer.z );
		if ( dot > bestDot )
		{
			bestDot = dot;
			bestIndex = directionIndex;
		}
	}

	return bestIndex;
}


//-----------------------------------------------------------------------------------------------
int Actor::GetCurrentAnimationFrameIndex() const
{
	if ( m_definition == nullptr || m_currentAnimationGroupIndex < 0 )
	{
		return 0;
	}

	int directionIndex = GetBestDirectionAnimationIndex();
	if ( directionIndex < 0
		|| directionIndex >= ( int ) m_definition->m_animationStartFrames.size()
		|| directionIndex >= ( int ) m_definition->m_animationEndFrames.size() )
	{
		return 0;
	}

	int startFrame = m_definition->m_animationStartFrames[directionIndex];
	int endFrame = m_definition->m_animationEndFrames[directionIndex];
	if ( endFrame < startFrame )
	{
		int temp = startFrame;
		startFrame = endFrame;
		endFrame = temp;
	}

	int const frameCount = ( endFrame - startFrame ) + 1;
	if ( frameCount <= 0 )
	{
		return startFrame;
	}

	float secondsPerFrame = 1.f;
	if ( m_currentAnimationGroupIndex < ( int ) m_definition->m_animationGroupSecondsPerFrame.size() )
	{
		secondsPerFrame = m_definition->m_animationGroupSecondsPerFrame[m_currentAnimationGroupIndex];
	}
	secondsPerFrame = ( secondsPerFrame <= 0.f ) ? 0.001f : secondsPerFrame;

	int frameOffset = 0;
	int rawFrame = ( int ) ( m_animationClock.GetTotalSeconds() / ( double ) secondsPerFrame );

	SpriteAnimPlaybackType playbackType = LOOP;
	if ( m_currentAnimationGroupIndex < ( int ) m_definition->m_animationGroupPlaybackModes.size() )
	{
		playbackType = m_definition->m_animationGroupPlaybackModes[m_currentAnimationGroupIndex];
	}

	if ( playbackType == ONCE )
	{
		frameOffset = rawFrame;
		if ( frameOffset > frameCount - 1 )
		{
			frameOffset = frameCount - 1;
		}
	}
	else if ( playbackType == PINGPONG )
	{
		if ( frameCount <= 1 )
		{
			frameOffset = 0;
		}
		else
		{
			int const pingPongLength = ( frameCount * 2 ) - 2;
			int const pingPongFrame = rawFrame % pingPongLength;
			frameOffset = ( pingPongFrame < frameCount ) ? pingPongFrame : ( pingPongLength - pingPongFrame );
		}
	}
	else
	{
		frameOffset = rawFrame % frameCount;
	}

	return startFrame + frameOffset;
}


//-----------------------------------------------------------------------------------------------
bool Actor::IsCurrentAnimationGroupComplete() const
{
	if ( m_definition == nullptr || m_currentAnimationGroupIndex < 0 )
	{
		return false;
	}

	if ( m_currentAnimationGroupIndex >= ( int ) m_definition->m_animationGroupPlaybackModes.size() )
	{
		return false;
	}

	SpriteAnimPlaybackType playbackType = m_definition->m_animationGroupPlaybackModes[m_currentAnimationGroupIndex];
	if ( playbackType != ONCE )
	{
		return false;
	}

	int directionIndex = GetBestDirectionAnimationIndex();
	if ( directionIndex < 0
		|| directionIndex >= ( int ) m_definition->m_animationStartFrames.size()
		|| directionIndex >= ( int ) m_definition->m_animationEndFrames.size() )
	{
		return true;
	}

	int startFrame = m_definition->m_animationStartFrames[directionIndex];
	int endFrame = m_definition->m_animationEndFrames[directionIndex];
	if ( endFrame < startFrame )
	{
		int temp = startFrame;
		startFrame = endFrame;
		endFrame = temp;
	}

	int const frameCount = ( endFrame - startFrame ) + 1;
	if ( frameCount <= 0 )
	{
		return true;
	}

	float secondsPerFrame = 1.f;
	if ( m_currentAnimationGroupIndex < ( int ) m_definition->m_animationGroupSecondsPerFrame.size() )
	{
		secondsPerFrame = m_definition->m_animationGroupSecondsPerFrame[m_currentAnimationGroupIndex];
	}
	secondsPerFrame = ( secondsPerFrame <= 0.f ) ? 0.001f : secondsPerFrame;

	double const totalSeconds = m_animationClock.GetTotalSeconds();
	double const totalDuration = ( double ) frameCount * ( double ) secondsPerFrame;
	return totalSeconds >= totalDuration;
}


//-----------------------------------------------------------------------------------------------
AABB2 Actor::GetUVsForFrameIndex( int frameIndex ) const
{
	if ( m_definition == nullptr || m_definition->m_visualCellCount.x <= 0 || m_definition->m_visualCellCount.y <= 0 )
	{
		return AABB2( Vec2::ZERO, Vec2::ONE );
	}

	int const cellsX = m_definition->m_visualCellCount.x;
	int const cellsY = m_definition->m_visualCellCount.y;
	int const maxFrame = ( cellsX * cellsY ) - 1;

	int clampedFrame = frameIndex;
	if ( clampedFrame < 0 )
	{
		clampedFrame = 0;
	}
	if ( clampedFrame > maxFrame )
	{
		clampedFrame = maxFrame;
	}

	int const frameX = clampedFrame % cellsX;
	int const frameY = clampedFrame / cellsX;

	float const uMin = ( float ) frameX / ( float ) cellsX;
	float const uMax = ( float ) ( frameX + 1 ) / ( float ) cellsX;

	float const vMax = 1.f - ( ( float ) frameY / ( float ) cellsY );
	float const vMin = 1.f - ( ( float ) ( frameY + 1 ) / ( float ) cellsY );

	return AABB2( Vec2( uMin, vMin ), Vec2( uMax, vMax ) );
}


//-----------------------------------------------------------------------------------------------
void Actor::AppendQuad( std::vector<Vertex>& outVerts, Vec3 const& bl, Vec3 const& br, Vec3 const& tr, Vec3 const& tl, AABB2 const& uvs, Vec3 const& normal ) const
{
	Vertex v0( bl, Rgba8::WHITE, Vec2( uvs.m_mins.x, uvs.m_mins.y ) );
	Vertex v1( br, Rgba8::WHITE, Vec2( uvs.m_maxs.x, uvs.m_mins.y ) );
	Vertex v2( tr, Rgba8::WHITE, Vec2( uvs.m_maxs.x, uvs.m_maxs.y ) );
	Vertex v3( tl, Rgba8::WHITE, Vec2( uvs.m_mins.x, uvs.m_maxs.y ) );

	v0.m_normal = normal;
	v1.m_normal = normal;
	v2.m_normal = normal;
	v3.m_normal = normal;

	outVerts.push_back( v0 );
	outVerts.push_back( v1 );
	outVerts.push_back( v2 );

	outVerts.push_back( v0 );
	outVerts.push_back( v2 );
	outVerts.push_back( v3 );
}


//-----------------------------------------------------------------------------------------------
void Actor::AppendQuadWithVertexNormals( std::vector<Vertex>& outVerts, Vec3 const& bl, Vec3 const& br, Vec3 const& tr, Vec3 const& tl, AABB2 const& uvs, Vec3 const& nbl, Vec3 const& nbr, Vec3 const& ntr, Vec3 const& ntl ) const
{
	Vertex v0( bl, Rgba8::WHITE, Vec2( uvs.m_mins.x, uvs.m_mins.y ) );
	Vertex v1( br, Rgba8::WHITE, Vec2( uvs.m_maxs.x, uvs.m_mins.y ) );
	Vertex v2( tr, Rgba8::WHITE, Vec2( uvs.m_maxs.x, uvs.m_maxs.y ) );
	Vertex v3( tl, Rgba8::WHITE, Vec2( uvs.m_mins.x, uvs.m_maxs.y ) );

	v0.m_normal = nbl;
	v1.m_normal = nbr;
	v2.m_normal = ntr;
	v3.m_normal = ntl;

	outVerts.push_back( v0 );
	outVerts.push_back( v1 );
	outVerts.push_back( v2 );

	outVerts.push_back( v0 );
	outVerts.push_back( v2 );
	outVerts.push_back( v3 );
}


//-----------------------------------------------------------------------------------------------
void Actor::AppendBillboardVerts( std::vector<Vertex>& outVerts, AABB2 const& uvs ) const
{
	Vec2 const size = m_definition->m_visualSize;
	Vec2 const pivot = m_definition->m_visualPivot;

	float const left = -pivot.x * size.x;
	float const right = ( 1.f - pivot.x ) * size.x;
	float const bottom = -pivot.y * size.y;
	float const top = ( 1.f - pivot.y ) * size.y;

	bool const lit = m_definition->m_visualRenderLit;
	bool const rounded = m_definition->m_visualRenderRounded;

	if ( lit && rounded )
	{
		float const middle = ( left + right ) * 0.5f;
		float const uMiddle = ( uvs.m_mins.x + uvs.m_maxs.x ) * 0.5f;

		AABB2 firstHalfUVs( Vec2( uvs.m_mins.x, uvs.m_mins.y ), Vec2( uMiddle, uvs.m_maxs.y ) );
		AABB2 secondHalfUVs( Vec2( uMiddle, uvs.m_mins.y ), Vec2( uvs.m_maxs.x, uvs.m_maxs.y ) );

		float const tilt = 0.45f;

		Vec3 nLeft = Vec3( 1.f, -tilt, 0.f ).GetNormalized();
		Vec3 nCenter = Vec3( 1.f, 0.f, 0.f ).GetNormalized();
		Vec3 nRight = Vec3( 1.f, tilt, 0.f ).GetNormalized();

		AppendQuadWithVertexNormals(
			outVerts,
			Vec3( 0.f, left, bottom ),
			Vec3( 0.f, middle, bottom ),
			Vec3( 0.f, middle, top ),
			Vec3( 0.f, left, top ),
			firstHalfUVs,
			nLeft, nCenter, nCenter, nLeft );

		AppendQuadWithVertexNormals(
			outVerts,
			Vec3( 0.f, middle, bottom ),
			Vec3( 0.f, right, bottom ),
			Vec3( 0.f, right, top ),
			Vec3( 0.f, middle, top ),
			secondHalfUVs,
			nCenter, nRight, nRight, nCenter );
	}
	else
	{
		Vec3 normal = lit ? Vec3( 1.f, 0.f, 0.f ) : Vec3::ZERO;
		AppendQuad(
			outVerts,
			Vec3( 0.f, left, bottom ),
			Vec3( 0.f, right, bottom ),
			Vec3( 0.f, right, top ),
			Vec3( 0.f, left, top ),
			uvs,
			normal );
	}
}


//-----------------------------------------------------------------------------------------------
void Actor::Render() const
{
	if ( g_engine == nullptr || g_engine->m_renderer == nullptr || m_definition == nullptr )
	{
		return;
	}

	if ( !m_definition->m_isVisible )
	{
		return;
	}

	Rgba8 modelTint = Rgba8::WHITE;
	if ( m_isDead )
	{
		modelTint = Rgba8( 80, 80, 80, 255 );
	}

	Mat44 const modelMatrix = GetModelMatrix();

	int const frameIndex = GetCurrentAnimationFrameIndex();
	AABB2 const uvs = GetUVsForFrameIndex( frameIndex );

	std::vector<Vertex> billboardVerts;
	billboardVerts.reserve( 12 );
	AppendBillboardVerts( billboardVerts, uvs );

	if ( m_visualShader != nullptr )
	{
		g_engine->m_renderer->BindShader( m_visualShader );
	}
	else
	{
		g_engine->m_renderer->BindShader( nullptr );
	}
	g_engine->m_renderer->BindTexture( m_visualTexture );

	g_engine->m_renderer->SetModelConstants( modelMatrix, modelTint );
	g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_NONE );
	g_engine->m_renderer->DrawVertexArray( billboardVerts );
	g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );

	g_engine->m_renderer->BindShader( nullptr );
	g_engine->m_renderer->BindTexture( nullptr );
}


//-----------------------------------------------------------------------------------------------
void Actor::SetSolidColor( Rgba8 const& color )
{
	m_color = color;

	int solidEndIndex = m_solidVertCount;
	if ( solidEndIndex > ( int ) m_verts.size() )
	{
		solidEndIndex = ( int ) m_verts.size();
	}

	Rgba8 lighterColor = Rgba8(
		static_cast<unsigned char>( GetClamped( static_cast<float>( color.r ) + 150.f, 0.f, 255.f ) ),
		static_cast<unsigned char>( GetClamped( static_cast<float>( color.g ) + 150.f, 0.f, 255.f ) ),
		static_cast<unsigned char>( GetClamped( static_cast<float>( color.b ) + 150.f, 0.f, 255.f ) ),
		color.a );

	for ( int index = 0; index < solidEndIndex; ++index )
	{
		m_verts[index].m_color = color;
	}

	for ( int index = solidEndIndex; index < ( int ) m_verts.size(); ++index )
	{
		m_verts[index].m_color = lighterColor;
	}
}


//-----------------------------------------------------------------------------------------------
Mat44 Actor::GetModelMatrix() const
{
	Mat44 modelToWorld;
	Mat44 translation = Mat44::MakeTranslation3D( m_position );

	EulerAngles billboardOrientation = m_orientation;

	if ( m_definition != nullptr && m_definition->m_visualBillboardType != BillboardType::NONE )
	{
		Vec3 viewVector = Vec3( 1.f, 0.f, 0.f );
		Player* renderingPlayer = nullptr;
		if ( g_app != nullptr && g_app->m_game != nullptr )
		{
			renderingPlayer = g_app->m_game->m_currentRenderingPlayer;
		}
		if ( renderingPlayer != nullptr && renderingPlayer->GetActor() != nullptr )
		{
			viewVector = renderingPlayer->GetActor()->m_position - m_position;
		}

		BillboardType const billboardType = m_definition->m_visualBillboardType;
		bool const isOpposing = ( billboardType == BillboardType::WORLD_UP_OPPOSING || billboardType == BillboardType::FULL_OPPOSING );
		bool const isWorldUp = ( billboardType == BillboardType::WORLD_UP_FACING || billboardType == BillboardType::WORLD_UP_OPPOSING );

		if ( isOpposing )
		{
			viewVector = -viewVector;
		}
		if ( isWorldUp )
		{
			viewVector.z = 0.f;
		}

		if ( viewVector == Vec3::ZERO )
		{
			viewVector = Vec3( 1.f, 0.f, 0.f );
		}
		viewVector = viewVector.GetNormalized();

		float const yaw = Atan2Degrees( viewVector.y, viewVector.x );
		float pitch = 0.f;
		if ( !isWorldUp )
		{
			pitch = Atan2Degrees( viewVector.z, Vec2( viewVector.x, viewVector.y ).GetLength() );
		}

		billboardOrientation = EulerAngles( yaw, pitch, 0.f );
	}
	else
	{
		billboardOrientation = EulerAngles( m_orientation.m_yawDegrees, 0.f, 0.f );
	}

	Mat44 rotation = billboardOrientation.GetAsMatrix_IFwd_JLeft_KUp();

	modelToWorld.Append( translation );
	modelToWorld.Append( rotation );

	return modelToWorld;
}