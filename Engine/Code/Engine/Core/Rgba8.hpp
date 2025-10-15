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
};