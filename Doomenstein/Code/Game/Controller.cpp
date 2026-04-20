#include "Game/Controller.hpp"
#include "Game/Actor.hpp"
#include "Game/Map.hpp"


//-----------------------------------------------------------------------------------------------
void Controller::Possess( ActorHandle actorHandle )
{
	Actor* oldActor = GetActor();

	m_possessedActor = actorHandle;

	Actor* newActor = GetActor();

	if ( oldActor == newActor )
	{
		if ( newActor != nullptr )
		{
			newActor->OnPossessed( this );
		}
		return;
	}

	if ( oldActor != nullptr )
	{
		oldActor->OnUnpossessed();
	}

	if ( newActor != nullptr )
	{
		newActor->OnPossessed( this );
	}
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