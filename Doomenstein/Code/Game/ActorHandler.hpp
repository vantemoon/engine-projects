#pragma once


//-----------------------------------------------------------------------------------------------
struct ActorHandler
{
public:
	ActorHandler();
	ActorHandler( unsigned int uid, unsigned int index );
	
	bool IsValid() const;
	unsigned int GetIndex() const;
	bool operator==( ActorHandler const& other ) const;
	bool operator!=( ActorHandler const& other ) const;

	static const ActorHandler INVALID;
	static const unsigned int MAX_ACTOR_UID = 0x0000fffeu;
	static const unsigned int MAX_ACTOR_INDEX = 0x0000ffffu;

private:
	unsigned int m_data;
};