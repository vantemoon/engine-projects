#pragma once
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/Vec2.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class Game; // Forward declaration
struct Vertex; // Forward declaration


//-----------------------------------------------------------------------------------------------
enum EntityType
{
	ENTITY_TYPE_INVALID = -1,
	ENTITY_TYPE_GOOD_PLAYER,
	ENTITY_TYPE_EVIL_SCORPIO,
	ENTITY_TYPE_EVIL_LEO,
	ENTITY_TYPE_EVIL_ARIES,
	ENTITY_TYPE_GOOD_BULLET,
	ENTITY_TYPE_GOOD_BOLT,
	ENTITY_TYPE_EVIL_BULLET,
	ENTITY_TYPE_EVIL_BOLT,
	NUM_ENTITY_TYPES
};


//-----------------------------------------------------------------------------------------------
enum EntityFaction
{
	ENTITY_FACTION_INVALID = -1,
	ENTITY_FACTION_GOOD,
	ENTITY_FACTION_NEUTRAL,
	ENTITY_FACTION_EVIL,
	NUM_ENTITY_FACTIONS
};


//-----------------------------------------------------------------------------------------------
class Entity
{
public:
	Vec2				 m_position;
	Vec2				 m_velocity;
	std::vector<Vertex>	 m_vertexArray;
	float				 m_orientationDegrees;
	float				 m_angularVelocityDegreesPerSecond;
	float				 m_physicsRadius;
	float				 m_cosmeticRadius;

	int					 m_health;
	bool				 m_isDead = false;
	bool				 m_isGarbage = false;

	bool                 m_isPushedByWalls;
	bool                 m_isPushedByEntities;
	bool                 m_doesPushEntities;
	bool                 m_isHitByBullets;

	EntityFaction		 m_faction = ENTITY_FACTION_INVALID;

public:
	Entity( Vec2 startingPosition, float orientationDegrees );
	virtual ~Entity();
	virtual void Update( float deltaSeconds );
	virtual void Render()													 const;
	virtual void TakeDamage( int damage );
	virtual void Die();

	// Accessors
	virtual bool IsOffScreen()												 const;
	virtual Vec2 GetForwardNormal()											 const;
	bool         IsAlive()													 const;

protected:
	virtual void InitializeVertexArray();
};


//-----------------------------------------------------------------------------------------------
typedef std::vector<Entity*> EntityList;