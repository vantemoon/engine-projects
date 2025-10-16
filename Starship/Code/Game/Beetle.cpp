#include "Game/Beetle.hpp"
#include "Game/App.hpp"
#include "Game/Bullet.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerShip.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Beetle::Beetle( Game* game, Vec2 const& startingPosition )
	: Entity( game, startingPosition )
{
	Vec2 toPlayer = m_game->m_playerShip->m_position - m_position;
	m_orientationDegrees = toPlayer.GetOrientationDegrees();
	m_velocity = BEETLE_SPEED * toPlayer.GetNormalized();
	m_physicsRadius = BEETLE_PHYSICS_RADIUS;
	m_cosmeticRadius = BEETLE_COSMETIC_RADIUS;
	m_health = 3;
	m_isBeetle = true;

	InitializeVertexArray();
}


//-----------------------------------------------------------------------------------------------
Beetle::~Beetle()
{
	delete[] m_vertexArray;
	m_vertexArray = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Beetle::Update( float deltaSeconds )
{
	Entity::Update( deltaSeconds );

	if ( m_game->m_playerShip->m_isDead || m_game->m_playerShip->m_isInvincible )
	{
		return;
	}
	Vec2 toPlayer = m_game->m_playerShip->m_position - m_position;
	m_orientationDegrees = toPlayer.GetOrientationDegrees();
	m_velocity = BEETLE_SPEED * toPlayer.GetNormalized();

	this->CheckCollisionWithBullets();
	this->CheckCollisionWithPlayerShip();
}


//-----------------------------------------------------------------------------------------------
void Beetle::Render() const
{
	// Create a copy of the beetle's vertex array to transform
	Vertex tempBeetleWorldVerts[NUM_BEETLE_VERTS];
	for ( int vertIndex = 0; vertIndex < NUM_BEETLE_VERTS; ++ vertIndex )
	{
		tempBeetleWorldVerts[vertIndex] = m_vertexArray[vertIndex];
	}
	// Transform the copy to world space and render the vertexes
	TransformVertexArrayXY3D( NUM_BEETLE_VERTS, tempBeetleWorldVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_renderer->DrawVertexArray( NUM_BEETLE_VERTS, tempBeetleWorldVerts );
}


//-----------------------------------------------------------------------------------------------
void Beetle::Die()
{
	Entity::Die();

	m_game->SpawnDebrisCluster( 12, m_position, m_velocity, BEETLE_COLOR, m_cosmeticRadius * 0.1f, m_cosmeticRadius * 0.6f );

	m_game->SpawnImpactWave( m_position );

	m_game->m_isScreenShaking = true;
	if ( m_game->m_screenShakeIntensity < 0.5f )
		m_game->m_screenShakeIntensity = 0.5f;
	m_game->m_screenShakeDuration = 1.f;
	m_game->m_screenShakeStartTime = ( float ) GetCurrentTimeSeconds();

	SoundID entityDeathSound = g_engine->m_audioSystem->CreateOrGetSound( "Data/BeetlesWaspsDie.wav" );
	g_engine->m_audioSystem->StartSound( entityDeathSound, false, 0.6f, 0.f, 0.5f );
}


//-----------------------------------------------------------------------------------------------
void Beetle::InitializeVertexArray()
{
	m_vertexArray = new Vertex[NUM_BEETLE_VERTS];
	
	m_vertexArray[0].m_position = Vec3( -2.f, 0.f, 0.f );
	m_vertexArray[1].m_position = Vec3( -0.5f, -0.5f, 0.f );
	m_vertexArray[2].m_position = Vec3( -0.5f, 0.5f, 0.f );
	m_vertexArray[0].m_color = BEETLE_COLOR;
	m_vertexArray[1].m_color = BEETLE_COLOR;
	m_vertexArray[2].m_color = BEETLE_COLOR;

	m_vertexArray[3].m_position = Vec3( -0.5f, 0.5f, 0.f );
	m_vertexArray[4].m_position = Vec3( 0.5f, 0.5f, 0.f );
	m_vertexArray[5].m_position = Vec3( 0.f, 2.f, 0.f );
	m_vertexArray[3].m_color = BEETLE_COLOR;
	m_vertexArray[4].m_color = BEETLE_COLOR;
	m_vertexArray[5].m_color = BEETLE_COLOR;

	m_vertexArray[6].m_position = Vec3( 0.5f, 0.5f, 0.f );
	m_vertexArray[7].m_position = Vec3( 0.5f, -0.5f, 0.f );
	m_vertexArray[8].m_position = Vec3( 2.f, 0.f, 0.f );
	m_vertexArray[6].m_color = BEETLE_COLOR;
	m_vertexArray[7].m_color = BEETLE_COLOR;
	m_vertexArray[8].m_color = BEETLE_COLOR;

	m_vertexArray[9].m_position = Vec3( 0.5f, -0.5f, 0.f );
	m_vertexArray[10].m_position = Vec3( -0.5f, -0.5f, 0.f );
	m_vertexArray[11].m_position = Vec3( 0.f, -2.f, 0.f );
	m_vertexArray[9].m_color = BEETLE_COLOR;
	m_vertexArray[10].m_color = BEETLE_COLOR;
	m_vertexArray[11].m_color = BEETLE_COLOR;

	m_vertexArray[12].m_position = Vec3( -0.5f, -0.5f, 0.f );
	m_vertexArray[13].m_position = Vec3( 0.5f, -0.5f, 0.f );
	m_vertexArray[14].m_position = Vec3( -0.5f, 0.5f, 0.f );
	m_vertexArray[12].m_color = BEETLE_COLOR;
	m_vertexArray[13].m_color = BEETLE_COLOR;
	m_vertexArray[14].m_color = BEETLE_COLOR;

	m_vertexArray[15].m_position = Vec3( 0.5f, -0.5f, 0.f );
	m_vertexArray[16].m_position = Vec3( 0.5f, 0.5f, 0.f );
	m_vertexArray[17].m_position = Vec3( -0.5f, 0.5f, 0.f );
	m_vertexArray[15].m_color = BEETLE_COLOR;
	m_vertexArray[16].m_color = BEETLE_COLOR;
	m_vertexArray[17].m_color = BEETLE_COLOR;
};