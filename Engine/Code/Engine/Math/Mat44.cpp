#include "Engine/Math/Mat44.hpp"


//-----------------------------------------------------------------------------------------------
Mat44::Mat44()
{
	// Initialize to identity matrix
	m_values[Ix] = 1.0f; m_values[Iy] = 0.0f; m_values[Iz] = 0.0f; m_values[Iw] = 0.0f;
	m_values[Jx] = 0.0f; m_values[Jy] = 1.0f; m_values[Jz] = 0.0f; m_values[Jw] = 0.0f;
	m_values[Kx] = 0.0f; m_values[Ky] = 0.0f; m_values[Kz] = 1.0f; m_values[Kw] = 0.0f;
	m_values[Tx] = 0.0f; m_values[Ty] = 0.0f; m_values[Tz] = 0.0f; m_values[Tw] = 1.0f;
}


//-----------------------------------------------------------------------------------------------
Mat44::Mat44( Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translation2D )
{
	m_values[Ix] = iBasis2D.x; m_values[Iy] = iBasis2D.y; m_values[Iz] = 0.0f;      m_values[Iw] = 0.0f;
	m_values[Jx] = jBasis2D.x; m_values[Jy] = jBasis2D.y; m_values[Jz] = 0.0f;      m_values[Jw] = 0.0f;
	m_values[Kx] = 0.0f;       m_values[Ky] = 0.0f;       m_values[Kz] = 1.0f;      m_values[Kw] = 0.0f;
	m_values[Tx] = translation2D.x; m_values[Ty] = translation2D.y; m_values[Tz] = 0.0f; m_values[Tw] = 1.0f;
}


//-----------------------------------------------------------------------------------------------
Mat44::Mat44( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translation3D )
{
	m_values[Ix] = iBasis3D.x; m_values[Iy] = iBasis3D.y; m_values[Iz] = iBasis3D.z; m_values[Iw] = 0.0f;
	m_values[Jx] = jBasis3D.x; m_values[Jy] = jBasis3D.y; m_values[Jz] = jBasis3D.z; m_values[Jw] = 0.0f;
	m_values[Kx] = kBasis3D.x; m_values[Ky] = kBasis3D.y; m_values[Kz] = kBasis3D.z; m_values[Kw] = 0.0f;
	m_values[Tx] = translation3D.x; m_values[Ty] = translation3D.y; m_values[Tz] = translation3D.z; m_values[Tw] = 1.0f;
}


//-----------------------------------------------------------------------------------------------
Mat44::Mat44( Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D )
{
	m_values[Ix] = iBasis4D.x; m_values[Iy] = iBasis4D.y; m_values[Iz] = iBasis4D.z; m_values[Iw] = iBasis4D.w;
	m_values[Jx] = jBasis4D.x; m_values[Jy] = jBasis4D.y; m_values[Jz] = jBasis4D.z; m_values[Jw] = jBasis4D.w;
	m_values[Kx] = kBasis4D.x; m_values[Ky] = kBasis4D.y; m_values[Kz] = kBasis4D.z; m_values[Kw] = kBasis4D.w;
	m_values[Tx] = translation4D.x; m_values[Ty] = translation4D.y; m_values[Tz] = translation4D.z; m_values[Tw] = translation4D.w;
}


//-----------------------------------------------------------------------------------------------
Mat44::Mat44( float const* sixteenValuesBasisMajor )
{
	for ( int index = 0; index < 16; ++index )
	{
		m_values[index] = sixteenValuesBasisMajor[index];
	}
}