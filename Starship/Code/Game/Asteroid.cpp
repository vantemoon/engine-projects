#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Asteroid.hpp"
#include "Game/Bullet.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"
#include "App.hpp"


//-----------------------------------------------------------------------------------------------
Asteroid::Asteroid( Game* game, Vec2 const& startingPosition, float orientationDegrees, Vec2 const& startingVelocity, float startingAngularVelocity )
	: Entity( game, startingPosition )
{
	m_velocity = startingVelocity;
	m_orientationDegrees = orientationDegrees;
	m_angularVelocityDegreesPerSecond = startingAngularVelocity;
	m_physicsRadius = 1.6f;
	m_cosmeticRadius = 2.0f;
	m_health = 3;

	RandomNumberGenerator rng;
	m_vertexArray = new Vertex[NUM_ASTEROID_VERTS];

	// Generate 16 random radii for the outer vertexes
	float radii[NUM_ASTEROID_VERTS];
	for ( int vertIndex = 0; vertIndex < NUM_ASTEROID_VERTS; ++vertIndex )
	{
		radii[vertIndex] = rng.RollRandomFloatInRange( m_physicsRadius, m_cosmeticRadius );
	}

	// Transform the random vertexes
	float rotationDegrees = 360.f / 16.f;
	Vec3 vertexPos1 = Vec3( radii[0], 0.f, 0 );
	Vec3 vertexPos2 = Vec3( radii[1], 0.f, 0 );
	TransformPositionXY3D( vertexPos2, 1.f, rotationDegrees, Vec2( 0.f, 0.f ) );
	Vec3 vertexPos3 = Vec3( radii[2], 0.f, 0 );
	TransformPositionXY3D( vertexPos3, 1.f, rotationDegrees * 2.f, Vec2( 0.f, 0.f ) );
	Vec3 vertexPos4 = Vec3( radii[3], 0.f, 0 );
	TransformPositionXY3D( vertexPos4, 1.f, rotationDegrees * 3.f, Vec2( 0.f, 0.f ) );
	Vec3 vertexPos5 = Vec3( radii[4], 0.f, 0 );
	TransformPositionXY3D( vertexPos5, 1.f, rotationDegrees * 4.f, Vec2( 0.f, 0.f ) );
	Vec3 vertexPos6 = Vec3( radii[5], 0.f, 0 );
	TransformPositionXY3D( vertexPos6, 1.f, rotationDegrees * 5.f, Vec2( 0.f, 0.f ) );
	Vec3 vertexPos7 = Vec3( radii[6], 0.f, 0 );
	TransformPositionXY3D( vertexPos7, 1.f, rotationDegrees * 6.f, Vec2( 0.f, 0.f ) );
	Vec3 vertexPos8 = Vec3( radii[7], 0.f, 0 );
	TransformPositionXY3D( vertexPos8, 1.f, rotationDegrees * 7.f, Vec2( 0.f, 0.f ) );
	Vec3 vertexPos9 = Vec3( radii[8], 0.f, 0 );
	TransformPositionXY3D( vertexPos9, 1.f, rotationDegrees * 8.f, Vec2( 0.f, 0.f ) );
	Vec3 vertexPos10 = Vec3( radii[9], 0.f, 0 );
	TransformPositionXY3D( vertexPos10, 1.f, rotationDegrees * 9.f, Vec2( 0.f, 0.f ) );
	Vec3 vertexPos11 = Vec3( radii[10], 0.f, 0 );
	TransformPositionXY3D( vertexPos11, 1.f, rotationDegrees * 10.f, Vec2( 0.f, 0.f ) );
	Vec3 vertexPos12 = Vec3( radii[11], 0.f, 0 );
	TransformPositionXY3D( vertexPos12, 1.f, rotationDegrees * 11.f, Vec2( 0.f, 0.f ) );
	Vec3 vertexPos13 = Vec3( radii[12], 0.f, 0 );
	TransformPositionXY3D( vertexPos13, 1.f, rotationDegrees * 12.f, Vec2( 0.f, 0.f ) );
	Vec3 vertexPos14 = Vec3( radii[13], 0.f, 0 );
	TransformPositionXY3D( vertexPos14, 1.f, rotationDegrees * 13.f, Vec2( 0.f, 0.f ) );
	Vec3 vertexPos15 = Vec3( radii[14], 0.f, 0 );
	TransformPositionXY3D( vertexPos15, 1.f, rotationDegrees * 14.f, Vec2( 0.f, 0.f ) );
	Vec3 vertexPos16 = Vec3( radii[15], 0.f, 0 );
	TransformPositionXY3D( vertexPos16, 1.f, rotationDegrees * 15.f, Vec2( 0.f, 0.f ) );

	// Triangle A
	m_vertexArray[0] = Vertex( Vec3( 0.f, 0.f, 0 ), Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[1] = Vertex( vertexPos1, Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[2] = Vertex( vertexPos2, Rgba8( 100, 100, 100, 255 ), Vec2() );

	// Triangle B
	m_vertexArray[3] = Vertex( Vec3( 0.f, 0.f, 0 ), Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[4] = Vertex( vertexPos2, Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[5] = Vertex( vertexPos3, Rgba8( 100, 100, 100, 255 ), Vec2() );

	// Triangle C
	m_vertexArray[6] = Vertex( Vec3( 0.f, 0.f, 0 ), Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[7] = Vertex( vertexPos3, Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[8] = Vertex( vertexPos4, Rgba8( 100, 100, 100, 255 ), Vec2() );

	// Triangle D
	m_vertexArray[9] = Vertex( Vec3( 0.f, 0.f, 0 ), Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[10] = Vertex( vertexPos4, Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[11] = Vertex( vertexPos5, Rgba8( 100, 100, 100, 255 ), Vec2() );

	// Triangle E
	m_vertexArray[12] = Vertex( Vec3( 0.f, 0.f, 0 ), Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[13] = Vertex( vertexPos5, Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[14] = Vertex( vertexPos6, Rgba8( 100, 100, 100, 255 ), Vec2() );

	// Triangle F
	m_vertexArray[15] = Vertex( Vec3( 0.f, 0.f, 0 ), Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[16] = Vertex( vertexPos6, Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[17] = Vertex( vertexPos7, Rgba8( 100, 100, 100, 255 ), Vec2() );

	// Triangle G
	m_vertexArray[18] = Vertex( Vec3( 0.f, 0.f, 0 ), Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[19] = Vertex( vertexPos7, Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[20] = Vertex( vertexPos8, Rgba8( 100, 100, 100, 255 ), Vec2() );

	// Triangle H
	m_vertexArray[21] = Vertex( Vec3( 0.f, 0.f, 0 ), Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[22] = Vertex( vertexPos8, Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[23] = Vertex( vertexPos9, Rgba8( 100, 100, 100, 255 ), Vec2() );

	// Triangle I
	m_vertexArray[24] = Vertex( Vec3( 0.f, 0.f, 0 ), Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[25] = Vertex( vertexPos9, Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[26] = Vertex( vertexPos10, Rgba8( 100, 100, 100, 255 ), Vec2() );

	// Triangle J
	m_vertexArray[27] = Vertex( Vec3( 0.f, 0.f, 0 ), Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[28] = Vertex( vertexPos10, Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[29] = Vertex( vertexPos11, Rgba8( 100, 100, 100, 255 ), Vec2() );

	// Triangle K
	m_vertexArray[30] = Vertex( Vec3( 0.f, 0.f, 0 ), Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[31] = Vertex( vertexPos11, Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[32] = Vertex( vertexPos12, Rgba8( 100, 100, 100, 255 ), Vec2() );

	// Triangle L
	m_vertexArray[33] = Vertex( Vec3( 0.f, 0.f, 0 ), Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[34] = Vertex( vertexPos12, Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[35] = Vertex( vertexPos13, Rgba8( 100, 100, 100, 255 ), Vec2() );

	// Triangle M
	m_vertexArray[36] = Vertex( Vec3( 0.f, 0.f, 0 ), Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[37] = Vertex( vertexPos13, Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[38] = Vertex( vertexPos14, Rgba8( 100, 100, 100, 255 ), Vec2() );

	// Triangle N
	m_vertexArray[39] = Vertex( Vec3( 0.f, 0.f, 0 ), Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[40] = Vertex( vertexPos14, Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[41] = Vertex( vertexPos15, Rgba8( 100, 100, 100, 255 ), Vec2() );

	// Triangle O
	m_vertexArray[42] = Vertex( Vec3( 0.f, 0.f, 0 ), Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[43] = Vertex( vertexPos15, Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[44] = Vertex( vertexPos16, Rgba8( 100, 100, 100, 255 ), Vec2() );

	// Triangle P
	m_vertexArray[45] = Vertex( Vec3( 0.f, 0.f, 0 ), Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[46] = Vertex( vertexPos16, Rgba8( 100, 100, 100, 255 ), Vec2() );
	m_vertexArray[47] = Vertex( vertexPos1, Rgba8( 100, 100, 100, 255 ), Vec2() );
};


//--------------------------------------------------------------------------------
Asteroid::~Asteroid() = default;


//--------------------------------------------------------------------------------
void Asteroid::CheckCollisionWithBullets()
{
	if(g_app->m_game == nullptr)
		return;

	for(int bulletIndex = 0; bulletIndex < Game::MAX_BULLETS; ++bulletIndex)
	{
		Bullet* bullet = g_app->m_game->m_bullets[bulletIndex];
		if(bullet == nullptr || bullet->m_isDead)
			continue;
		float distanceSquared = GetDistanceSquared2D( m_position, bullet->m_position );
		float combinedRadii = m_physicsRadius + bullet->m_physicsRadius;
		if(distanceSquared < (combinedRadii * combinedRadii))
		{
			bullet->Die();
			m_health -= 1;
		}
	}
}


//--------------------------------------------------------------------------------
void Asteroid::CheckCollisionWithPlayerShip()
{
	if(g_app->m_game == nullptr || g_app->m_game->m_playerShip == nullptr)
		return;

	PlayerShip* playerShip = g_app->m_game->m_playerShip;
	float distanceSquared = GetDistanceSquared2D( m_position, playerShip->m_position );
	float combinedRadii = m_physicsRadius + playerShip->m_physicsRadius;
	if (distanceSquared < ( combinedRadii * combinedRadii ) )
	{
		playerShip->Die();
		m_health -= 1;
	}
}


//-----------------------------------------------------------------------------------------------
void Asteroid::Update( float deltaSeconds )
{
	Entity::Update( deltaSeconds );

	if( m_health <= 0 )
	{
		Die();
	}

	if( IsOffScreen() )
	{
		Die();
	}

	this->CheckCollisionWithBullets();
	this->CheckCollisionWithPlayerShip();
}


//-----------------------------------------------------------------------------------------------
void Asteroid::Render() const
{
	// Create a copy of the asteroid's vertex array to transform
	Vertex tempAsteroidWorldVerts[NUM_ASTEROID_VERTS];
	for ( int vertIndex = 0; vertIndex < NUM_ASTEROID_VERTS; ++vertIndex )
	{
		tempAsteroidWorldVerts[vertIndex] = m_vertexArray[vertIndex];
	}
	// Transform the copy to world space and render the vertexes
	TransformVertexArrayXY3D( NUM_ASTEROID_VERTS, tempAsteroidWorldVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_renderer->DrawVertexArray( NUM_ASTEROID_VERTS, tempAsteroidWorldVerts );
}


//-----------------------------------------------------------------------------------------------
void Asteroid::Die()
{
	Entity::Die(); // m_isDead = true;
	m_isGarbage = true;

}

//-----------------------------------------------------------------------------------------------
bool Asteroid::IsOffScreen() const
{
	float screenLeft = 0.f;
	float screenRight = 200.f;
	float screenBottom = 0.f;
	float screenTop = 100.f;
	if ( m_position.x < screenLeft - m_cosmeticRadius || m_position.x > screenRight + m_cosmeticRadius ||
		 m_position.y < screenBottom - m_cosmeticRadius || m_position.y > screenTop + m_cosmeticRadius )
	{
		return true;
	}
	return false;
}