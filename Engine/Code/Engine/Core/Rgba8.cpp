#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
const Rgba8 Rgba8::WHITE = Rgba8( 255, 255, 255, 255 );
const Rgba8 Rgba8::BLACK = Rgba8( 0, 0, 0, 255 );
const Rgba8 Rgba8::RED = Rgba8( 255, 0, 0, 255 );
const Rgba8 Rgba8::GREEN = Rgba8( 0, 255, 0, 255 );
const Rgba8 Rgba8::BLUE = Rgba8( 0, 0, 255, 255 );
const Rgba8 Rgba8::YELLOW = Rgba8( 255, 255, 0, 255 );
const Rgba8 Rgba8::CYAN = Rgba8( 0, 255, 255, 255 );
const Rgba8 Rgba8::MAGENTA = Rgba8( 255, 0, 255, 255 );

//-----------------------------------------------------------------------------------------------
Rgba8::Rgba8( unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte )
	: r( redByte )
	, g( greenByte )
	, b( blueByte )
	, a( alphaByte )
{
}


//-----------------------------------------------------------------------------------------------
void Rgba8::SetFromText( const char* text )
{
	Strings splitStrings = SplitStringOnDelimiter( text, ',' );
	if ( splitStrings.size() >= 3 )
	{
		r = static_cast<unsigned char>( atoi( splitStrings[0].c_str() ) );
		g = static_cast<unsigned char>( atoi( splitStrings[1].c_str() ) );
		b = static_cast<unsigned char>( atoi( splitStrings[2].c_str() ) );
		if ( splitStrings.size() >= 4 )
		{
			a = static_cast<unsigned char>( atoi( splitStrings[3].c_str() ) );
		}
		else
		{
			a = 255; // default alpha to 255 if not provided
		}
	}
}


//-----------------------------------------------------------------------------------------------
Rgba8 Rgba8::Interpolate( Rgba8 const& start, Rgba8 const& end, float fractionOfEnd ) const
{
	float newR = ::Interpolate( NormalizeByte( start.r ), NormalizeByte( end.r ), fractionOfEnd );
	float newG = ::Interpolate( NormalizeByte( start.g ), NormalizeByte( end.g ), fractionOfEnd );
	float newB = ::Interpolate( NormalizeByte( start.b ), NormalizeByte( end.b ), fractionOfEnd );
	float newA = ::Interpolate( NormalizeByte( start.a ), NormalizeByte( end.a ), fractionOfEnd );
	return Rgba8( DenormalizeByte( newR ), DenormalizeByte( newG ), DenormalizeByte( newB ), DenormalizeByte( newA ) );
}