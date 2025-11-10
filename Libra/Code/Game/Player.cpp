#include "Game/Player.hpp"
#include "Game/Bullet.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Player::Player( Vec2 startingPosition, float orientationDegrees )
	: Entity( startingPosition, orientationDegrees )
{
	m_physicsRadius = PLAYER_TANK_PHYSICS_RADIUS;
	m_cosmeticRadius = PLAYER_TANK_COSMETIC_RADIUS;

	m_targetMovementDirection = m_orientationDegrees;
	m_prevOrientationDegrees = m_orientationDegrees;

	m_turretOrientationDegrees = m_orientationDegrees;
	m_turretTargetOrientationDegrees = m_orientationDegrees;
	m_turretRelativeOrientationDegrees = 0.f;

	m_faction = ENTITY_FACTION_GOOD;

	m_isPushedByWalls = true;
	m_isPushedByEntities = true;
	m_doesPushEntities = true;
	m_isHitByBullets = true;

	m_health = PLAYER_TANK_HEALTH;

	InitializeVertexArray();
}


//-----------------------------------------------------------------------------------------------
Player::~Player() = default;


//-----------------------------------------------------------------------------------------------
void Player::Update( float deltaSeconds )
{
	m_timeSinceLastFire += deltaSeconds;

	UpdateFromKeyboard( deltaSeconds );
	UpdateFromController( deltaSeconds );
	TurnTowardMovementDirection( deltaSeconds );

	if ( !m_isTurretAiming )
	{
		float orientationDelta = m_orientationDegrees - m_prevOrientationDegrees;
		m_turretTargetOrientationDegrees += orientationDelta;
		m_turretOrientationDegrees = m_orientationDegrees + m_turretRelativeOrientationDegrees;
	}

	TurnTurretTowardAimDirection( deltaSeconds );

	m_velocity = m_thrustFraction * PLAYER_TANK_MOVE_SPEED_TILES_PER_SECOND * GetForwardNormal();
	Entity::Update( deltaSeconds );

	m_prevOrientationDegrees = m_orientationDegrees;
}


//-----------------------------------------------------------------------------------------------
void Player::UpdateFromKeyboard( [[maybe_unused]] float deltaSeconds )
{
	// Movement
	bool isWKeyDown = g_engine->m_inputSystem->IsKeyDown( 'W' );
	bool isAKeyDown = g_engine->m_inputSystem->IsKeyDown( 'A' );
	bool isSKeyDown = g_engine->m_inputSystem->IsKeyDown( 'S' );
	bool isDKeyDown = g_engine->m_inputSystem->IsKeyDown( 'D' );

	bool isAnyMovementKeyDown = isWKeyDown || isAKeyDown || isSKeyDown || isDKeyDown;

	if ( isAnyMovementKeyDown )
	{
		m_isMovingForward = true;
		m_thrustFraction = 1.f;

		float totalDeltaOrientation = 0.f;
		int numKeysPressed = 0;
		if ( isWKeyDown ) {
			totalDeltaOrientation += 90.f;   
			numKeysPressed++;
		}
		if ( isAKeyDown ) {
			totalDeltaOrientation += 180.f;  
			numKeysPressed++;
		}
		if ( isSKeyDown ) {
			totalDeltaOrientation += 270.f;  
			numKeysPressed++;
		}
		if ( isDKeyDown )
		{
			if ( isWKeyDown )
			{
				totalDeltaOrientation += 0.f;
			}
			else if ( isSKeyDown )
			{
				totalDeltaOrientation += 360.f;
			}
			else
			{
				totalDeltaOrientation += 0.f;
			}
			numKeysPressed++;
		}
		float deltaOrientation = totalDeltaOrientation / ( float ) numKeysPressed;
		m_targetMovementDirection = deltaOrientation;
	}
	else
	{
		m_isMovingForward = false;
		m_thrustFraction = 0.f;
	}

	// Turret Aiming
	bool isIKeyDown = g_engine->m_inputSystem->IsKeyDown( 'I' );
	bool isJKeyDown = g_engine->m_inputSystem->IsKeyDown( 'J' );
	bool isKKeyDown = g_engine->m_inputSystem->IsKeyDown( 'K' );
	bool isLKeyDown = g_engine->m_inputSystem->IsKeyDown( 'L' );

	bool isAiming = isIKeyDown || isJKeyDown || isKKeyDown || isLKeyDown;

	if ( isAiming ) {
		float totalDeltaOrientation = 0.f;
		int numKeysPressed = 0;

		if ( isIKeyDown ) {
			totalDeltaOrientation += 90.f;   
			numKeysPressed++;
		}
		if ( isJKeyDown ) {
			totalDeltaOrientation += 180.f;  
			numKeysPressed++;
		}
		if ( isKKeyDown ) {
			totalDeltaOrientation += 270.f;  
			numKeysPressed++;
		}
		if ( isLKeyDown )
		{
			if ( isIKeyDown )
			{
				totalDeltaOrientation += 0.f;
			}
			else if ( isKKeyDown )
			{
				totalDeltaOrientation += 360.f;
			}
			else
			{
				totalDeltaOrientation += 0.f;
			}
			numKeysPressed++;
		}
		float deltaOrientation = totalDeltaOrientation / ( float ) numKeysPressed;
		m_turretTargetOrientationDegrees = deltaOrientation;

		m_isTurretAiming = true;
	} 
	else 
	{
		m_isTurretAiming = false;
	}

	// Fire
	if ( g_engine->m_inputSystem->IsKeyDown( KEYCODE_SPACE ) )
	{
		if ( m_timeSinceLastFire >= PLAYER_TANK_FIRE_COOLDOWN_SECONDS )
		{
			FireProjectile();
			m_timeSinceLastFire = 0.f;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Player::UpdateFromController( [[maybe_unused]] float deltaSeconds )
{
	XboxController const& controller = g_engine->m_inputSystem->GetController( 0 );

	// Movement
	float leftJoystickMagnitude = controller.GetLeftJoystick().GetMagnitude();
	float leftJoystickOrientationDegrees = controller.GetLeftJoystick().GetOrientationDegrees();
	if ( leftJoystickMagnitude > 0.f )
	{
		m_isMovingForward = true;
		m_thrustFraction = leftJoystickMagnitude;
		m_targetMovementDirection = leftJoystickOrientationDegrees;
	}

	// Turret Aiming
	float rightJoystickMagnitude = controller.GetRightJoystick().GetMagnitude();
	float rightJoystickOrientationDegrees = controller.GetRightJoystick().GetOrientationDegrees();
	if ( rightJoystickMagnitude > 0.f )
	{
		m_turretTargetOrientationDegrees = rightJoystickOrientationDegrees;
		m_isTurretAiming = true;
	}

	// Fire
	float rightTrigger = controller.GetRightTrigger();
	if ( rightTrigger > 0.f )
	{
		if ( m_timeSinceLastFire >= PLAYER_TANK_FIRE_COOLDOWN_SECONDS )
		{
			FireProjectile();
			m_timeSinceLastFire = 0.f;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Player::Render() const
{
	if ( m_isDead )
	{
		return;
	}

	std::vector<Vertex> tankBodyVerts;
	tankBodyVerts = m_tankBodyVertexArray;
	Texture* tankBodyTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/PlayerTankBase.png" );
	g_engine->m_renderer->BindTexture( tankBodyTexture );
	TransformVertexArrayXY3D( (int) tankBodyVerts.size(), tankBodyVerts.data(), 1.f, m_orientationDegrees, m_position );
	g_engine->m_renderer->DrawVertexArray( tankBodyVerts );

	std::vector<Vertex> turretVerts;
	turretVerts = m_turretVertexArray;
	Texture* turretTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/PlayerTankTop.png" );
	g_engine->m_renderer->BindTexture( turretTexture );
	TransformVertexArrayXY3D( (int) turretVerts.size(), turretVerts.data(), 1.f, m_turretOrientationDegrees, m_position );
	g_engine->m_renderer->DrawVertexArray( turretVerts );

	g_engine->m_renderer->BindTexture( nullptr );
}


//-----------------------------------------------------------------------------------------------
void Player::TakeDamage( int damage )
{
	if ( m_isInvincible )
		return;

	Entity::TakeDamage( damage );
}


//-----------------------------------------------------------------------------------------------
void Player::Die()
{
	// #ToDo: Handle player death (lives, respawn, game over, etc.)

	Entity::Die();
}


//-----------------------------------------------------------------------------------------------
void Player::InitializeVertexArray()
{
	// Body
	AABB2 bodyAABB2 = AABB2( -m_cosmeticRadius, -m_cosmeticRadius, m_cosmeticRadius, m_cosmeticRadius );
	AddVertsForAABB2D( m_tankBodyVertexArray, bodyAABB2, Rgba8::WHITE );

	// Turret
	AABB2 turretAABB2 = AABB2( -m_cosmeticRadius, -m_cosmeticRadius, m_cosmeticRadius, m_cosmeticRadius );
	AddVertsForAABB2D( m_turretVertexArray, turretAABB2, Rgba8::WHITE );
}


//-----------------------------------------------------------------------------------------------
void Player::TurnTowardMovementDirection( float deltaSeconds )
{
	if ( !m_isMovingForward )
		return;

	if ( m_targetMovementDirection == m_orientationDegrees )
		return;
	
	float currentOrientationDegrees = m_orientationDegrees;
	float deltaOrientationDegrees = m_targetMovementDirection - currentOrientationDegrees;
	deltaOrientationDegrees = GetShortestAngularDispDegrees( currentOrientationDegrees, m_targetMovementDirection );
	
	float maxDeltaThisFrame = PLAYER_TANK_TURN_SPEED_DEGREES_PER_SECOND * deltaSeconds;
	if ( fabsf( deltaOrientationDegrees ) <= maxDeltaThisFrame )
	{
		m_orientationDegrees = m_targetMovementDirection;
	}
	else
	{
		float deltaToApply;
		if ( deltaOrientationDegrees > 0.f )
		{
			deltaToApply = maxDeltaThisFrame;
		}
		else
		{
			deltaToApply = -maxDeltaThisFrame;
		}
		m_orientationDegrees += deltaToApply;
	}
}


//-----------------------------------------------------------------------------------------------
void Player::TurnTurretTowardAimDirection( float deltaSeconds )
{
	if ( !m_isTurretAiming )
	{
		return;
	}

    if ( m_turretOrientationDegrees == m_turretTargetOrientationDegrees )
        return;

    float currentTurretDegrees = m_turretOrientationDegrees;
    float deltaTurretDegrees = GetShortestAngularDispDegrees( currentTurretDegrees, m_turretTargetOrientationDegrees );

    float maxDeltaThisFrame = PLAYER_TANK_TURRET_TURN_SPEED_DEGREES_PER_SECOND * deltaSeconds;
    if ( fabsf( deltaTurretDegrees ) <= maxDeltaThisFrame )
    {
        m_turretOrientationDegrees = m_turretTargetOrientationDegrees;
    }
    else
    {
        m_turretOrientationDegrees += (deltaTurretDegrees > 0.f ? maxDeltaThisFrame : -maxDeltaThisFrame);
    }

    m_turretRelativeOrientationDegrees = m_turretOrientationDegrees - m_orientationDegrees;
}


//-----------------------------------------------------------------------------------------------
void Player::FireProjectile()
{
	Vec2 muzzleOffset = Vec2::MakeFromPolarDegrees( m_turretOrientationDegrees, m_physicsRadius + 0.5f );
	Vec2 projectileSpawnPosition = m_position + muzzleOffset;
	Entity* newBullet = g_game->m_currentMap->SpawnNewEntity( ENTITY_TYPE_GOOD_BOLT, projectileSpawnPosition, m_turretOrientationDegrees );
	if ( newBullet != nullptr )
		g_game->m_currentMap->AddEntityToMap( *newBullet, ENTITY_TYPE_GOOD_BOLT, ENTITY_FACTION_GOOD );
}