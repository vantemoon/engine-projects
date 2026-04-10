#include "Engine/Math/FloatRange.hpp"
#include <string>


//-----------------------------------------------------------------------------------------------
const FloatRange FloatRange::ZERO(0.f, 0.f);
const FloatRange FloatRange::ONE(1.f, 1.f);
const FloatRange FloatRange::ZERO_TO_ONE(0.f, 1.f);
const FloatRange FloatRange::ZERO_TO_ZERO( 0.f, 0.f );


//-----------------------------------------------------------------------------------------------
FloatRange::FloatRange()
{
	m_min = 0.f;
	m_max = 0.f;
}


//-----------------------------------------------------------------------------------------------
FloatRange::FloatRange( float min, float max )
{
	m_min = min;
	m_max = max;
}


//-----------------------------------------------------------------------------------------------
FloatRange::~FloatRange() = default;


//-----------------------------------------------------------------------------------------------
void FloatRange::operator=( const FloatRange& copyFrom )
{
	m_min = copyFrom.m_min;
	m_max = copyFrom.m_max;
}


//-----------------------------------------------------------------------------------------------
bool FloatRange::operator==( const FloatRange& compare ) const
{
	return ( m_min == compare.m_min && m_max == compare.m_max );
}


//-----------------------------------------------------------------------------------------------
bool FloatRange::operator!=( const FloatRange& compare ) const
{
	return ! ( *this == compare );
}


//-----------------------------------------------------------------------------------------------
bool FloatRange::IsOnRange( float value ) const
{
	return ( value >= m_min && value <= m_max );
}


//-----------------------------------------------------------------------------------------------
bool FloatRange::IsOverlappingWith( const FloatRange& other ) const
{
	return ( m_min <= other.m_max && m_max >= other.m_min );
}


//-----------------------------------------------------------------------------------------------
void FloatRange::SetFromText( const char* text )
{
	if ( text == nullptr )
	{
		return;
	}

	std::string textStr( text );
	size_t commaIndex = textStr.find( ',' );
	if ( commaIndex == std::string::npos )
	{
		return;
	}

	std::string minStr = textStr.substr( 0, commaIndex );
	std::string maxStr = textStr.substr( commaIndex + 1 );

	m_min = std::stof( minStr );
	m_max = std::stof( maxStr );
}