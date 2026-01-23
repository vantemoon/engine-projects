#pragma once


//-----------------------------------------------------------------------------------------------
struct Rgba8
{
public:
	unsigned char r = 255;
	unsigned char g = 255;
	unsigned char b = 255;
	unsigned char a = 255;

public:
	// Construction/Destruction
	~Rgba8() = default;											               // destructor (do nothing)
	Rgba8() = default;											               // default constructor (do nothing)
	explicit Rgba8( unsigned char redByte, unsigned char greenByte,            // explicit constructor (from r, g, b, a)
					unsigned char blueByte, unsigned char alphaByte = 255 );

	Rgba8 Interpolate( Rgba8 const& start, Rgba8 const& end, float fractionOfEnd ) const;
	void  SetFromText( const char* text );
	void  GetAsFloats( float* colorAsFloats ) const;

	static const Rgba8 WHITE;
	static const Rgba8 BLACK;
	static const Rgba8 RED;
	static const Rgba8 GREEN;
	static const Rgba8 BLUE;
	static const Rgba8 YELLOW;
	static const Rgba8 CYAN;
	static const Rgba8 MAGENTA;
};