#include "Game/ActorHandler.hpp"


//----------------------------------------------------------------------------------------------
ActorHandler const ActorHandler::INVALID = ActorHandler();


//-----------------------------------------------------------------------------------------------
ActorHandler::ActorHandler()
	: m_data( 0u )
{
}


//-----------------------------------------------------------------------------------------------
ActorHandler::ActorHandler( unsigned int uid, unsigned int index )
{
	m_data = ( ( uid & MAX_ACTOR_UID ) << 16 ) | ( index & MAX_ACTOR_INDEX );
}


//-----------------------------------------------------------------------------------------------
bool ActorHandler::IsValid() const
{
	return ( m_data != ActorHandler::INVALID.m_data );
}


//-----------------------------------------------------------------------------------------------
unsigned int ActorHandler::GetIndex() const
{
	return ( m_data & MAX_ACTOR_INDEX );
}


//-----------------------------------------------------------------------------------------------
bool ActorHandler::operator==( ActorHandler const& other ) const
{
	return ( m_data == other.m_data );
}


//-----------------------------------------------------------------------------------------------
bool ActorHandler::operator!=( ActorHandler const& other ) const
{
	return ( m_data != other.m_data );
}