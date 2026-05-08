#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
OBB3::OBB3( Vec3 const& center, Vec3 const& iBasisNormal, Vec3 const& jBasisNormal, Vec3 const& kBasisNormal, Vec3 const& halfDimensions )
	: m_center( center )
	, m_iBasisNormal( iBasisNormal )
	, m_jBasisNormal( jBasisNormal )
	, m_kBasisNormal( kBasisNormal )
	, m_halfDimensions( halfDimensions )
{
}


//-----------------------------------------------------------------------------------------------
void OBB3::GetCornerPoints( Vec3* out_eightCornerWorldPositions ) const
{
	Vec3 const& kBasisNormal = m_kBasisNormal;

	out_eightCornerWorldPositions[0] = m_center + ( m_iBasisNormal * m_halfDimensions.x ) + ( m_jBasisNormal * m_halfDimensions.y ) + ( kBasisNormal * m_halfDimensions.z );
	out_eightCornerWorldPositions[1] = m_center - ( m_iBasisNormal * m_halfDimensions.x ) + ( m_jBasisNormal * m_halfDimensions.y ) + ( kBasisNormal * m_halfDimensions.z );
	out_eightCornerWorldPositions[2] = m_center - ( m_iBasisNormal * m_halfDimensions.x ) - ( m_jBasisNormal * m_halfDimensions.y ) + ( kBasisNormal * m_halfDimensions.z );
	out_eightCornerWorldPositions[3] = m_center + ( m_iBasisNormal * m_halfDimensions.x ) - ( m_jBasisNormal * m_halfDimensions.y ) + ( kBasisNormal * m_halfDimensions.z );

	out_eightCornerWorldPositions[4] = m_center + ( m_iBasisNormal * m_halfDimensions.x ) + ( m_jBasisNormal * m_halfDimensions.y ) - ( kBasisNormal * m_halfDimensions.z );
	out_eightCornerWorldPositions[5] = m_center - ( m_iBasisNormal * m_halfDimensions.x ) + ( m_jBasisNormal * m_halfDimensions.y ) - ( kBasisNormal * m_halfDimensions.z );
	out_eightCornerWorldPositions[6] = m_center - ( m_iBasisNormal * m_halfDimensions.x ) - ( m_jBasisNormal * m_halfDimensions.y ) - ( kBasisNormal * m_halfDimensions.z );
	out_eightCornerWorldPositions[7] = m_center + ( m_iBasisNormal * m_halfDimensions.x ) - ( m_jBasisNormal * m_halfDimensions.y ) - ( kBasisNormal * m_halfDimensions.z );
}


//-----------------------------------------------------------------------------------------------
Vec3 const OBB3::GetLocalPosForWorldPos( Vec3 const& worldPos ) const
{
	Vec3 displacementFromCenter = worldPos - m_center;
	float localX = DotProduct3D( displacementFromCenter, m_iBasisNormal );
	float localY = DotProduct3D( displacementFromCenter, m_jBasisNormal );
	float localZ = DotProduct3D( displacementFromCenter, m_kBasisNormal );

	return Vec3( localX, localY, localZ );
}


//-----------------------------------------------------------------------------------------------
Vec3 const OBB3::GetWorldPosForLocalPos( Vec3 const& localPos ) const
{
	Vec3 worldPos = m_center + ( m_iBasisNormal * localPos.x ) + ( m_jBasisNormal * localPos.y ) + ( m_kBasisNormal * localPos.z );

	return worldPos;
}


//-----------------------------------------------------------------------------------------------
void OBB3::RotateAboutCenter( float rotationDeltaDegrees )
{
	m_iBasisNormal = m_iBasisNormal.GetRotatedAboutZDegrees( rotationDeltaDegrees );
	m_jBasisNormal = m_jBasisNormal.GetRotatedAboutZDegrees( rotationDeltaDegrees );
	m_kBasisNormal = m_kBasisNormal.GetRotatedAboutZDegrees( rotationDeltaDegrees );
}