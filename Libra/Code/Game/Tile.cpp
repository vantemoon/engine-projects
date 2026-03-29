#include "Game/Tile.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


//-----------------------------------------------------------------------------------------------
Tile::Tile( IntVec2 tileCoords, TileDefinition const* definition )
	: m_tileCoords( tileCoords )
	, m_definition( definition )
{
	if ( m_definition != nullptr )
	{
		m_health = m_definition->m_maxHealth;
	}
}


//-----------------------------------------------------------------------------------------------
Tile::~Tile()
{
	// DO NOTHING
}


//-----------------------------------------------------------------------------------------------
TileDefinition const& Tile::GetDefinition() const
{
	return *m_definition;
}


//-----------------------------------------------------------------------------------------------
bool Tile::IsSolid() const
{
	const TileDefinition& tileDef = GetDefinition();
	return tileDef.m_isSolid;
}


//-----------------------------------------------------------------------------------------------
bool Tile::IsDestructible() const
{
	return m_definition != nullptr && m_definition->m_isDestructible;
}


//-----------------------------------------------------------------------------------------------
void Tile::SetType( TileDefinition const* newDefinition )
{
	if ( newDefinition == nullptr )
	{
		return;
	}

	m_definition = newDefinition;
	m_health = m_definition->m_maxHealth;
}


//-----------------------------------------------------------------------------------------------
void Tile::TakeDamage( int damage )
{
	if ( damage <= 0 ) return;
	if ( !IsDestructible() ) return;

	m_health -= damage;

	if ( m_health <= 0 )
	{
		const std::string& destroyedTypeName = m_definition->m_altTileType;
		if ( !destroyedTypeName.empty() )
		{
			auto it = TileDefinition::s_definitions.find( destroyedTypeName );
			if ( it != TileDefinition::s_definitions.end() && it->second != nullptr )
			{
				SetType( it->second );
			}
			else
			{
				m_health = 0;
			}
		}
		else
		{
			m_health = 0;
		}
	}
}