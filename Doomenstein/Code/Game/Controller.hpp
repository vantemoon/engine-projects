#pragma once
#include "Game/ActorHandle.hpp"


//-----------------------------------------------------------------------------------------------
class Actor;
class Map;


//-----------------------------------------------------------------------------------------------
class Controller
{
public:
	virtual ~Controller() = default;

	virtual void Update( float deltaSeconds ) = 0;
	void Possess( ActorHandle actorHandle );
	Actor* GetActor() const;

public:
	ActorHandle m_possessedActor = ActorHandle::INVALID;
	Map* m_map = nullptr;
};