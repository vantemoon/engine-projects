#include "Game/ActorHandle.hpp"


//----------------------------------------------------------------------------------------------
ActorHandle const ActorHandle::INVALID = ActorHandle();


//-----------------------------------------------------------------------------------------------
ActorHandle::ActorHandle()
	: m_data( 0u )
{
}


//-----------------------------------------------------------------------------------------------
ActorHandle::ActorHandle( unsigned int uid, unsigned int index )
{
	m_data = ( ( uid & MAX_ACTOR_UID ) << 16 ) | ( index & MAX_ACTOR_INDEX );
}


//-----------------------------------------------------------------------------------------------
bool ActorHandle::IsValid() const
{
	return ( m_data != ActorHandle::INVALID.m_data );
}


//-----------------------------------------------------------------------------------------------
unsigned int ActorHandle::GetIndex() const
{
	return ( m_data & MAX_ACTOR_INDEX );
}


//-----------------------------------------------------------------------------------------------
bool ActorHandle::operator==( ActorHandle const& other ) const
{
	return ( m_data == other.m_data );
}


//-----------------------------------------------------------------------------------------------
bool ActorHandle::operator!=( ActorHandle const& other ) const
{
	return ( m_data != other.m_data );
}