#include "Game/Wasp.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerShip.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Wasp::Wasp( Game* game, Vec2 const& startingPosition )
	: Entity( game, startingPosition )
{
	Vec2 toPlayer = m_game->m_playerShip->m_position - m_position;
	m_orientationDegrees = toPlayer.GetOrientationDegrees();
	m_velocity = WASP_SPEED * toPlayer.GetNormalized();

	m_physicsRadius = WASP_PHYSICS_RADIUS;
	m_cosmeticRadius = WASP_COSMETIC_RADIUS;
	m_health = 2;
	m_isWasp = true;

	InitializeVertexArray();
}


//-----------------------------------------------------------------------------------------------
Wasp::~Wasp()
{
	delete[] m_vertexArray;
	m_vertexArray = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Wasp::Update()
{
	Entity::Update();

	Vec2 toPlayer = m_game->m_playerShip->m_position - m_position;
	m_orientationDegrees = toPlayer.GetOrientationDegrees();

	Accelerate();

	this->CheckCollisionWithBullets();
	this->CheckCollisionWithPlayerShip();
}


//-----------------------------------------------------------------------------------------------
void Wasp::Render() const
{
	/// Create a copy of the beetle's vertex array to transform
	Vertex tempWaspWorldVerts[NUM_WASP_VERTS];
	for ( int vertIndex = 0; vertIndex < NUM_WASP_VERTS; ++ vertIndex )
	{
		tempWaspWorldVerts[vertIndex] = m_vertexArray[vertIndex];
	}
	// Transform the copy to world space and render the vertexes
	TransformVertexArrayXY3D( NUM_WASP_VERTS, tempWaspWorldVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_renderer->DrawVertexArray( NUM_WASP_VERTS, tempWaspWorldVerts );
}


//-----------------------------------------------------------------------------------------------
void Wasp::Die()
{
	Entity::Die();

	m_game->SpawnDebrisCluster( 12, m_position, m_velocity, WASP_COLOR, m_cosmeticRadius * 0.1f, m_cosmeticRadius * 0.5f );

	m_game->SpawnImpactWave( m_position );

	m_game->m_isScreenShaking = true;
	if ( m_game->m_screenShakeIntensity < 0.5f )
		m_game->m_screenShakeIntensity = 0.5f;
	m_game->m_screenShakeDuration = 1.f;
	m_game->m_screenShakeStartTime = m_game->m_screenShakeStartTime = ( float ) Clock::GetSystemClock().GetTotalSeconds();

	SoundID entityDeathSound = g_engine->m_audioSystem->CreateOrGetSound( "Data/Audio/BeetlesWaspsDie.wav" );
	g_engine->m_audioSystem->StartSound( entityDeathSound, false, 0.6f, 0.f, 0.5f );
}


//-----------------------------------------------------------------------------------------------
void Wasp::InitializeVertexArray()
{
	m_vertexArray = new Vertex[NUM_WASP_VERTS];

	m_vertexArray[0].m_position = Vec3( -1.f, 1.2f, 0.f );
	m_vertexArray[1].m_position = Vec3( -0.5f, 0.f, 0.f );
	m_vertexArray[2].m_position = Vec3( 1.5f, 0.f, 0.f );
	m_vertexArray[0].m_color = WASP_COLOR;
	m_vertexArray[1].m_color = WASP_COLOR;
	m_vertexArray[2].m_color = WASP_COLOR;

	m_vertexArray[3].m_position = Vec3( -1.f, -1.2f, 0.f );
	m_vertexArray[4].m_position = Vec3( -0.5f, 0.f, 0.f );
	m_vertexArray[5].m_position = Vec3( 1.5f, 0.f, 0.f );
	m_vertexArray[3].m_color = WASP_COLOR;
	m_vertexArray[4].m_color = WASP_COLOR;
	m_vertexArray[5].m_color = WASP_COLOR;
}


//-----------------------------------------------------------------------------------------------
void Wasp::Accelerate()
{
	float deltaSeconds = ( float ) m_game->m_gameClock->GetDeltaSeconds();
	Vec2 forwardNormal = this->GetForwardNormal();
	m_velocity += forwardNormal * WASP_ACCELERATION * deltaSeconds;
	float speed = m_velocity.GetLength();
	if ( speed > WASP_MAX_SPEED )
	{
		m_velocity = m_velocity.GetNormalized() * WASP_MAX_SPEED;
	}
}


//-----------------------------------------------------------------------------------------------
void Wasp::WrapAroundScreen()
{
	if ( m_position.x < 0.f )
		m_position.x = ( float ) WORLD_SIZE_X;
	else if ( m_position.x > ( float ) WORLD_SIZE_X )
		m_position.x = 0.f;
	if ( m_position.y < 0.f )
		m_position.y = ( float ) WORLD_SIZE_Y;
	else if ( m_position.y > ( float ) WORLD_SIZE_Y )
		m_position.y = 0.f;
}