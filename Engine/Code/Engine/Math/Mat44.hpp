#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"


//-----------------------------------------------------------------------------------------------
struct Mat44
{
	enum { Ix, Iy, Iz, Iw, Jx, Jy, Jz, Jw, Kx, Ky, Kz, Kw, Tx, Ty, Tz, Tw };
	float m_values[16];

	Mat44();
	explicit Mat44( Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translation2D );
	explicit Mat44( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translation3D );
	explicit Mat44( Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D );
	explicit Mat44( float const* sixteenValuesBasisMajor );
};