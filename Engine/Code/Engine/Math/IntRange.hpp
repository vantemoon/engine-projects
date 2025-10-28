#pragma once


//-----------------------------------------------------------------------------------------------
struct IntRange
{
public:
	int m_min = 0;
	int m_max = 0;

public:
	IntRange();
	IntRange( int min, int max );
	~IntRange();

	void operator=( const IntRange& copyFrom );
	bool operator==( const IntRange& compare ) const;
	bool operator!=( const IntRange& compare ) const;

	bool IsOnRange( int value ) const;
	bool IsOverlappingWith( const IntRange& other ) const;

	static const IntRange ZERO;         // [0, 0]
	static const IntRange ONE;          // [1, 1]
	static const IntRange ZERO_TO_ONE;  // [0, 1]
};