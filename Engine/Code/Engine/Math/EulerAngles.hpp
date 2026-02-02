#pragma once


//-----------------------------------------------------------------------------------------------
struct Mat44;
struct Vec3;


//-----------------------------------------------------------------------------------------------
struct EulerAngles
{
public:
	EulerAngles() = default;
	EulerAngles( float yawDegrees, float pitchDegrees, float rollDegrees );
	Vec3 GetForwardDir_IFwd_JLeft_KUp() const;
	Vec3 GetAsVectors_IFwd_JLeft_KUp( Vec3& out_forwardIBasis, Vec3& out_leftJBasis, Vec3& out_upKBasis ) const;
	Mat44 GetAsMatrix_IFwd_JLeft_KUp() const;

	void operator+( EulerAngles const& anglesToAdd );

	friend EulerAngles const Interpolate( EulerAngles const& from, EulerAngles const& to, float lerpFraction );

public:
	float m_yawDegrees = 0.f;   // rotation about the up (Y) axis
	float m_pitchDegrees = 0.f; // rotation about the right (X) axis
	float m_rollDegrees = 0.f;  // rotation about the forward (Z) axis
};