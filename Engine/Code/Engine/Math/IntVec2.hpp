#pragma once


//-----------------------------------------------------------------------------------------------
struct IntVec2
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	int x = 0;
	int y = 0;

	static const IntVec2 ZERO;

public:
	// Construction/Destruction
	IntVec2() = default;                              // default constructor (do nothing)
	~IntVec2() = default;                             // destructor (do nothing)
	IntVec2( const IntVec2& copyFrom );               // copy constructor (from another vec2)
	explicit IntVec2( int initialX, int initialY );   // explicit constructor (from x, y)

	// Accessors (const methods)
	float GetLength()							  const;
	int GetLengthSquared()						  const;
	int GetTaxicabLength()						  const;
	float GetOrientationRadians()				  const;
	float GetOrientationDegrees()				  const;
	IntVec2 const GetRotatedBy90Degrees()		  const;
	IntVec2 const GetRotatedByMinus90Degrees()	  const;

	// Mutators (non-const methods)
	void Rotate90Degrees();
	void RotateMinus90Degrees();

	// Operators
	void operator=( const IntVec2& copyFrom );        // IntVec2 = IntVec2
};