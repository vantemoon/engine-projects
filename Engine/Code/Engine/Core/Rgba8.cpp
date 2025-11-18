#include "Engine/Core/Rgba8.hpp"


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