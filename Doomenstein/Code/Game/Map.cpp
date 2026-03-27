#include "Game/Map.hpp"
#include "Game/Actor.hpp"


//-----------------------------------------------------------------------------------------------
Map::Map()
{
	InitializeActors();
}


//-----------------------------------------------------------------------------------------------
Map::~Map()
{
	for ( Actor* actor : m_actors )
	{
		delete actor;
	}
	m_actors.clear();
}


//-----------------------------------------------------------------------------------------------
void Map::Update()
{
	for ( Actor* actor : m_actors )
	{
		actor->Update();
	}
}


//-----------------------------------------------------------------------------------------------
void Map::Render() const
{
	for ( Actor* actor : m_actors )
	{
		actor->Render();
	}
}


//-----------------------------------------------------------------------------------------------
void Map::InitializeActors()
{
	Actor* enemyActorA = new Actor();
	enemyActorA->m_physicsRadius = 0.35f;
	enemyActorA->m_physicsHeight = 0.75f;
	enemyActorA->m_color = Rgba8::RED;
	enemyActorA->m_isStatic = true;
	enemyActorA->m_position = Vec3( 7.5f, 8.5f, 0.25f );
	m_actors.push_back( enemyActorA );

	Actor* enemyActorB = new Actor();
	enemyActorB->m_physicsRadius = 0.35f;
	enemyActorB->m_physicsHeight = 0.75f;
	enemyActorB->m_color = Rgba8::RED;
	enemyActorB->m_isStatic = true;
	enemyActorB->m_position = Vec3( 8.5f, 8.5f, 0.125f );
	m_actors.push_back( enemyActorB );

	Actor* enemyActorC = new Actor();
	enemyActorC->m_physicsRadius = 0.35f;
	enemyActorC->m_physicsHeight = 0.75f;
	enemyActorC->m_color = Rgba8::RED;
	enemyActorC->m_isStatic = true;
	enemyActorC->m_position = Vec3( 9.5f, 8.5f, 0.0f );
	m_actors.push_back( enemyActorC );

	Actor* projectileActor = new Actor();
	projectileActor->m_physicsRadius = 0.0625f;
	projectileActor->m_physicsHeight = 0.125f;
	projectileActor->m_color = Rgba8::BLUE;
	projectileActor->m_isStatic = false;
	projectileActor->m_position = Vec3( 5.5f, 8.5f, 0.0f );
	m_actors.push_back( projectileActor );

}