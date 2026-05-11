#pragma once
#include "Game/ActorHandle.hpp"
#include "Game/Controller.hpp"


//-----------------------------------------------------------------------------------------------
class AI : public Controller
{
public:
	ActorHandle m_targetActorHandle = ActorHandle::INVALID;

public:
	AI();
	~AI();

	void Update( float deltaSeconds );
	void UpdateVirtualPetAI( Actor* self, float deltaSeconds );
	void DamagedBy( Actor* attacker );
};