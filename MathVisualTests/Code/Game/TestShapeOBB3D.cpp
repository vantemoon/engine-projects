#include "Game/TestShapeOBB3D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>


//-----------------------------------------------------------------------------------------------
TestShapeOBB3D::TestShapeOBB3D( Vec3 const& center, Vec3 const& iBasisNormal, Vec3 const& jBasisNormal, Vec3 const& kBasisNormal, Vec3 const& halfDimensions )
	: m_orientedBox( center, iBasisNormal, jBasisNormal, kBasisNormal, halfDimensions )
{
	float sp = GetClamped( -iBasisNormal.z, -1.f, 1.f );
	float pitchDegrees = ConvertRadiansToDegrees( asinf( sp ) );
	float yawDegrees = Atan2Degrees( iBasisNormal.y, iBasisNormal.x );

	float cp = sqrtf( ( iBasisNormal.x * iBasisNormal.x ) + ( iBasisNormal.y * iBasisNormal.y ) );
	float rollDegrees = 0.f;

	if ( cp > 0.0001f )
	{
		rollDegrees = Atan2Degrees( jBasisNormal.z, kBasisNormal.z );
	}
	else
	{
		yawDegrees = Atan2Degrees( -jBasisNormal.x, jBasisNormal.y );
	}

	m_orientation = EulerAngles( yawDegrees, pitchDegrees, rollDegrees );
}


//-----------------------------------------------------------------------------------------------
TestShapeOBB3D::~TestShapeOBB3D()
{
}