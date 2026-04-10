#pragma once


//-----------------------------------------------------------------------------------------------
struct FloatRange
{
public:
	float m_min = 0.f;
	float m_max = 0.f;

public:
	FloatRange();
	FloatRange( float min, float max );
	~FloatRange();

	void operator=( const FloatRange& copyFrom );
	bool operator==( const FloatRange& compare ) const;
	bool operator!=( const FloatRange& compare ) const;

	bool IsOnRange( float value ) const;
	bool IsOverlappingWith( const FloatRange& other ) const;

	void SetFromText( const char* text );

	static const FloatRange ZERO;         // [0, 0]
	static const FloatRange ONE;          // [1, 1]
	static const FloatRange ZERO_TO_ONE;  // [0, 1]
	static const FloatRange ZERO_TO_ZERO; // [0, 0]
};