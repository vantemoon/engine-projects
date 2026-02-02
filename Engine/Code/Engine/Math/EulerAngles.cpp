#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec3.hpp"


//-----------------------------------------------------------------------------------------------
EulerAngles::EulerAngles( float yawDegrees, float pitchDegrees, float rollDegrees )
	: m_yawDegrees( yawDegrees )
	, m_pitchDegrees( pitchDegrees )
	, m_rollDegrees( rollDegrees )
{
}


//-----------------------------------------------------------------------------------------------
Vec3 EulerAngles::GetForwardDir_IFwd_JLeft_KUp() const
{
	float cy = CosDegrees( m_yawDegrees );
	float sy = SinDegrees( m_yawDegrees );
	float cp = CosDegrees( m_pitchDegrees );
	float sp = SinDegrees( m_pitchDegrees );
	return Vec3( cy * cp, sy * cp, -sp );
}


//-----------------------------------------------------------------------------------------------
Vec3 EulerAngles::GetAsVectors_IFwd_JLeft_KUp( Vec3& out_forwardIBasis, Vec3& out_leftJBasis, Vec3& out_upKBasis ) const
{
	float cy = CosDegrees( m_yawDegrees );
	float sy = SinDegrees( m_yawDegrees );
	float cp = CosDegrees( m_pitchDegrees );
	float sp = SinDegrees( m_pitchDegrees );
	float cr = CosDegrees( m_rollDegrees );
	float sr = SinDegrees( m_rollDegrees );
	out_forwardIBasis = Vec3( cy * cp, sy * cp, -sp );
	out_leftJBasis = Vec3( -cr * sy + sr * sp * cy, cr * cy + sr * sp * sy, sr * cp );
	out_upKBasis = Vec3( sr * sy + cr * sp * cy, -sr * cy + cr * sp * sy, cr * cp );
	return out_forwardIBasis;
}


//-----------------------------------------------------------------------------------------------
Mat44 EulerAngles::GetAsMatrix_IFwd_JLeft_KUp() const
{
	float cy = CosDegrees( m_yawDegrees );
	float sy = SinDegrees( m_yawDegrees );
	float cp = CosDegrees( m_pitchDegrees );
	float sp = SinDegrees( m_pitchDegrees );
	float cr = CosDegrees( m_rollDegrees );
	float sr = SinDegrees( m_rollDegrees );
	Mat44 mat;
	mat.m_values[Mat44::Ix] = cy * cp;
	mat.m_values[Mat44::Iy] = sy * cp;
	mat.m_values[Mat44::Iz] = -sp;
	mat.m_values[Mat44::Jx] = -cr * sy + sr * sp * cy;
	mat.m_values[Mat44::Jy] = cr * cy + sr * sp * sy;
	mat.m_values[Mat44::Jz] = sr * cp;
	mat.m_values[Mat44::Kx] = sr * sy + cr * sp * cy;
	mat.m_values[Mat44::Ky] = -sr * cy + cr * sp * sy;
	mat.m_values[Mat44::Kz] = cr * cp;
	mat.m_values[Mat44::Tx] = 0.f;
	mat.m_values[Mat44::Ty] = 0.f;
	mat.m_values[Mat44::Tz] = 0.f;
	mat.m_values[Mat44::Tw] = 1.f;
	return mat;
}


//-----------------------------------------------------------------------------------------------
void EulerAngles::operator+( EulerAngles const& anglesToAdd )
{
	m_yawDegrees += anglesToAdd.m_yawDegrees;
	m_pitchDegrees += anglesToAdd.m_pitchDegrees;
	m_rollDegrees += anglesToAdd.m_rollDegrees;
}


//-----------------------------------------------------------------------------------------------
EulerAngles const Interpolate( EulerAngles const& from, EulerAngles const& to, float lerpFraction )
{
	EulerAngles result;
	float yawDelta = GetShortestAngularDispDegrees( from.m_yawDegrees, to.m_yawDegrees );
	// float pitchDelta = GetShortestAngularDispDegrees( from.m_pitchDegrees, to.m_pitchDegrees );
	// float rollDelta = GetShortestAngularDispDegrees( from.m_rollDegrees, to.m_rollDegrees );

	result.m_yawDegrees = from.m_yawDegrees + ( yawDelta * lerpFraction );
	result.m_pitchDegrees = Interpolate( from.m_pitchDegrees, to.m_pitchDegrees, lerpFraction );
	result.m_rollDegrees = Interpolate( from.m_rollDegrees, to.m_rollDegrees, lerpFraction );
	return result;
}