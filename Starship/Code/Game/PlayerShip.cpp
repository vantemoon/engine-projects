#include "Game/PlayerShip.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
PlayerShip::PlayerShip( Game* game, Vec2 const& startingPosition, Vec2 const& startingVelocity )
	: Entity( game, startingPosition )
{
	m_velocity = startingVelocity;
	m_physicsRadius = PLAYER_SHIP_PHYSICS_RADIUS;
	m_cosmeticRadius = PLAYER_SHIP_COSMETIC_RADIUS;
	
	InitializeVertexArray();
}


//--------------------------------------------------------------------------------
void PlayerShip::InitializeVertexArray()
{
	m_vertexArray = new Vertex[NUM_SHIP_VERTS];

	// Triangle A
	m_vertexArray[0] = Vertex( Vec3( 2, 1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	m_vertexArray[1] = Vertex( Vec3( 0, 2, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	m_vertexArray[2] = Vertex( Vec3( -2, 1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );

	// Triangle B
	m_vertexArray[3] = Vertex( Vec3( 0, 1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	m_vertexArray[4] = Vertex( Vec3( -2, 1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	m_vertexArray[5] = Vertex( Vec3( -2, -1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );

	// Triangle C
	m_vertexArray[6] = Vertex( Vec3( 0, 1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	m_vertexArray[7] = Vertex( Vec3( -2, -1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	m_vertexArray[8] = Vertex( Vec3( 0, -1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );

	// Triangle D
	m_vertexArray[9] = Vertex( Vec3( 1, 0, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	m_vertexArray[10] = Vertex( Vec3( 0, 1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	m_vertexArray[11] = Vertex( Vec3( 0, -1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );

	// Triangle E
	m_vertexArray[12] = Vertex( Vec3( 2, -1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	m_vertexArray[13] = Vertex( Vec3( -2, -1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	m_vertexArray[14] = Vertex( Vec3( 0, -2, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
};


//--------------------------------------------------------------------------------
void PlayerShip::GetVertexArrayCopy( Vertex* out_vertexArray ) const
{
	// Triangle A
	out_vertexArray[0] = Vertex( Vec3( 2, 1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	out_vertexArray[1] = Vertex( Vec3( 0, 2, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	out_vertexArray[2] = Vertex( Vec3( -2, 1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );

	// Triangle B
	out_vertexArray[3] = Vertex( Vec3( 0, 1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	out_vertexArray[4] = Vertex( Vec3( -2, 1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	out_vertexArray[5] = Vertex( Vec3( -2, -1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );

	// Triangle C
	out_vertexArray[6] = Vertex( Vec3( 0, 1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	out_vertexArray[7] = Vertex( Vec3( -2, -1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	out_vertexArray[8] = Vertex( Vec3( 0, -1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );

	// Triangle D
	out_vertexArray[9] = Vertex( Vec3( 1, 0, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	out_vertexArray[10] = Vertex( Vec3( 0, 1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	out_vertexArray[11] = Vertex( Vec3( 0, -1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );

	// Triangle E
	out_vertexArray[12] = Vertex( Vec3( 2, -1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	out_vertexArray[13] = Vertex( Vec3( -2, -1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	out_vertexArray[14] = Vertex( Vec3( 0, -2, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
}


//--------------------------------------------------------------------------------
PlayerShip::~PlayerShip() = default;


//-----------------------------------------------------------------------------------------------
void PlayerShip::Update( float deltaSeconds )
{
	
	Entity::Update( deltaSeconds );

	UpdateFromKeyboard();
	UpdateFromController();

	if ( m_isDead )
		return;

	if ( !m_isAccelerating )
	{
		m_thrustFraction = 0.f;
	}
	Accelerate( deltaSeconds * m_thrustFraction );
	

	BounceOffWorldEdges();

	if( m_isTurningLeft )
	{
		TurnLeft();
	}
	else if( m_isTurningRight )
	{
		TurnRight();
	}
	else
	{
		m_angularVelocityDegreesPerSecond = 0.f;
	}
}


//-----------------------------------------------------------------------------------------------
void PlayerShip::UpdateFromKeyboard()
{
	// Attacking
	if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_SPACE ) )
	{
		m_game->SpawnBulletFromPlayerShip();
	}

	// Movement
	if ( g_engine->m_inputSystem->IsKeyDown( 'A' ) && !g_engine->m_inputSystem->IsKeyDown( 'D' ) )
	{
		m_game->m_playerShip->m_isTurningLeft = true;
	}
	else if ( g_engine->m_inputSystem->IsKeyDown( 'D' ) && !g_engine->m_inputSystem->IsKeyDown( 'A' ) )
	{
		m_game->m_playerShip->m_isTurningRight = true;
	}
	else
	{
		m_game->m_playerShip->m_isTurningLeft = false;
		m_game->m_playerShip->m_isTurningRight = false;
	}

	if ( g_engine->m_inputSystem->IsKeyDown( 'W' ) )
	{
		m_game->m_playerShip->m_isAccelerating = true;
		m_thrustFraction = 1.f;
	}
	else
	{
		m_game->m_playerShip->m_isAccelerating = false;
	}

	// Respawn
	if ( g_engine->m_inputSystem->WasKeyJustPressed( 'N' ) && m_isDead )
	{
		if ( m_game->m_playerSpareLives > 0 )
		{
			Respawn();
			--g_app->m_game->m_playerSpareLives;
		}
	}

	// Scanning
	if ( g_engine->m_inputSystem->IsKeyDown( 'T' ) )
	{
		m_game->m_isScanModeOn = true;
	}
	else if ( g_engine->m_inputSystem->WasKeyJustReleased( 'T' ) )
	{
		m_game->m_isScanModeOn = false;
	}
}


//-----------------------------------------------------------------------------------------------
void PlayerShip::UpdateFromController()
{
	XboxController const& controller = g_engine->m_inputSystem->GetController( 0 );

	// Movement
	float leftJoystickMagnitude = controller.GetLeftJoystick().GetMagnitude();
	float leftJoystickOrientationDegrees = controller.GetLeftJoystick().GetOrientationDegrees();
	if ( leftJoystickMagnitude > 0.f )
	{
		m_isAccelerating = true;
		m_thrustFraction = leftJoystickMagnitude;
		m_orientationDegrees = leftJoystickOrientationDegrees;
	}

	// Attacking
	static bool wasRightTriggerPressedLastFrame = false;
	float rightTriggerValue = controller.GetRightTrigger();
	bool isRightTriggerPressed = rightTriggerValue > 0.f;
	if ( controller.WasButtonJustPressed( XBOX_BUTTON_A ) || ( isRightTriggerPressed && !wasRightTriggerPressedLastFrame) )
	{
		m_game->SpawnBulletFromPlayerShip();
	}
	wasRightTriggerPressedLastFrame = isRightTriggerPressed;

	// Respawn
	if ( controller.WasButtonJustPressed( XBOX_BUTTON_START ) && m_isDead )
	{
		if ( m_game->m_playerSpareLives > 0 )
		{
			Respawn();
			--g_app->m_game->m_playerSpareLives;
		}
	}

	// Scanning with left trigger
	float leftTriggerValue = controller.GetLeftTrigger();
	if ( leftTriggerValue > 0.f )
	{
		m_game->m_isScanModeOn = true;
	}
	else
	{
		m_game->m_isScanModeOn = false;
	}
}


//-----------------------------------------------------------------------------------------------
void PlayerShip::Render() const
{
	if ( m_isDead )
		return;

	Vertex tempShipWorldVerts[NUM_SHIP_VERTS];
	for( int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; ++ vertIndex )
	{
		tempShipWorldVerts[vertIndex] = m_vertexArray[vertIndex];
	}

	TransformVertexArrayXY3D( NUM_SHIP_VERTS, tempShipWorldVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_renderer->DrawVertexArray( NUM_SHIP_VERTS, tempShipWorldVerts );

	if ( m_isAccelerating )
	{
		RenderThrust();
	}
}


//-----------------------------------------------------------------------------------------------
void PlayerShip::RenderThrust() const
{
	RandomNumberGenerator rng;

	float minLength = 1.0f;
	float maxLength = 1.0f + m_thrustFraction * 4.0f;
	float flickerLength = minLength + ( maxLength - minLength ) * rng.RollRandomFloatZeroToOne();

	unsigned char baseR = 255;
	unsigned char baseG = static_cast<unsigned char>(128 + rng.RollRandomIntInRange(0, 127));
	unsigned char baseB = 0;
	Rgba8 flickerColor( baseR, baseG, baseB, 255 );

	Vertex thrustVerts[3];
	thrustVerts[0] = Vertex( Vec3( -2.f, -1.f, 0.f ), flickerColor, Vec2() );
	thrustVerts[1] = Vertex( Vec3( -2.f - flickerLength, 0.f, 0.f ), Rgba8( 255, 255, 0, 0 ), Vec2() );
	thrustVerts[2] = Vertex( Vec3( -2.f, 1.f, 0.f ), flickerColor, Vec2() );

	TransformVertexArrayXY3D( 3, thrustVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_renderer->DrawVertexArray( 3, thrustVerts );
}

//-----------------------------------------------------------------------------------------------
void PlayerShip::Die()
{
	m_isDead = true;
	m_velocity = Vec2( 0.f, 0.f );
	m_angularVelocityDegreesPerSecond = 0.f;

	m_game->SpawnDebrisCluster( 30, m_position, m_velocity, Rgba8( 102, 153, 204 ), m_cosmeticRadius * 0.1f, m_cosmeticRadius * 0.8f );

	SoundID shipExplosionSound = g_engine->m_audioSystem->CreateOrGetSound( "Data/PlayershipDie.wav" );
	g_engine->m_audioSystem->StartSound( shipExplosionSound, false, 0.5f, 0.f, 0.5f );

	g_app->m_game->m_isScreenShaking = true;
	g_app->m_game->m_screenShakeIntensity = 0.5f;
	g_app->m_game->m_screenShakeDuration = 2.f;
	g_app->m_game->m_screenShakeStartTime = ( float ) GetCurrentTimeSeconds();

	g_app->m_game->m_isScreenShaking = true;
	g_app->m_game->m_screenShakeIntensity = 0.8f;
	g_app->m_game->m_screenShakeDuration = 2.f;
}


//-----------------------------------------------------------------------------------------------
void PlayerShip::BounceOffWorldEdges()
{
	float screenLeft = 0.f + m_physicsRadius;
	float screenRight = WORLD_SIZE_X - m_physicsRadius;
	float screenBottom = 0.f + m_physicsRadius;
	float screenTop = WORLD_SIZE_Y - m_physicsRadius;

	if ( m_position.x < screenLeft )
	{
		m_position.x = screenLeft;
		m_velocity.x = -m_velocity.x;
	}
	else if ( m_position.x > screenRight )
	{
		m_position.x = screenRight;
		m_velocity.x = -m_velocity.x;
	}
	if ( m_position.y < screenBottom )
	{
		m_position.y = screenBottom;
		m_velocity.y = -m_velocity.y;
	}
	else if ( m_position.y > screenTop )
	{
		m_position.y = screenTop;
		m_velocity.y = -m_velocity.y;
	}
}


//-----------------------------------------------------------------------------------------------
void PlayerShip::Accelerate( float deltaSeconds )
{
	Vec2 forwardNormal = GetForwardNormal();
	m_velocity += forwardNormal * PLAYER_SHIP_ACCELERATION * deltaSeconds;
}


//-----------------------------------------------------------------------------------------------
void PlayerShip::TurnLeft()
{
	m_angularVelocityDegreesPerSecond = PLAYER_SHIP_TURN_SPEED;
}


//-----------------------------------------------------------------------------------------------
void PlayerShip::TurnRight()
{
	m_angularVelocityDegreesPerSecond = -PLAYER_SHIP_TURN_SPEED;
}


//-----------------------------------------------------------------------------------------------
void PlayerShip::Respawn()
{
	m_isDead = false;
	m_health = 1;
	m_position = Vec2( WORLD_CENTER_X, WORLD_CENTER_Y );
	m_velocity = Vec2( 0.f, 0.f );
	m_orientationDegrees = 0.f;
	m_thrustFraction = 0.f;

	SoundID shipRespawnSound = g_engine->m_audioSystem->CreateOrGetSound( "Data/PlayershipRespawn.wav" );
	g_engine->m_audioSystem->StartSound( shipRespawnSound, false, 0.5f, 0.f, 1.f );
}