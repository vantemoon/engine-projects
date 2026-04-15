#include "Game/Controller.hpp"
#include "Game/Map.hpp"


//-----------------------------------------------------------------------------------------------
void Controller::Possess( ActorHandle actorHandle )
{
	m_possessedActor = actorHandle;
}


//-----------------------------------------------------------------------------------------------
Actor* Controller::GetActor() const
{
	if ( !m_possessedActor.IsValid() )
	{
		return nullptr;
	}

	if ( m_map == nullptr )
	{
		return nullptr;
	}

	return m_map->GetActorByHandle( m_possessedActor );
}