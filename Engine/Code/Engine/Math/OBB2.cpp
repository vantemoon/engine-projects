#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
OBB2::OBB2( Vec2 const& center, Vec2 const& iBasisNormal, Vec2 const& halfDimensions )
	: m_center( center )
	, m_iBasisNormal( iBasisNormal.GetNormalized() )
	, m_halfDimensions( halfDimensions )
{
}


//-----------------------------------------------------------------------------------------------
void OBB2::GetCornerPoints( Vec2* out_fourCornerWorldPositions ) const
{
	Vec2 jBasisNormal = m_iBasisNormal.GetRotatedBy90Degrees();
	out_fourCornerWorldPositions[0] = m_center + ( m_iBasisNormal * m_halfDimensions.x ) + ( jBasisNormal * m_halfDimensions.y ); // Top Right
	out_fourCornerWorldPositions[1] = m_center - ( m_iBasisNormal * m_halfDimensions.x ) + ( jBasisNormal * m_halfDimensions.y ); // Top Left
	out_fourCornerWorldPositions[2] = m_center - ( m_iBasisNormal * m_halfDimensions.x ) - ( jBasisNormal * m_halfDimensions.y ); // Bottom Left
	out_fourCornerWorldPositions[3] = m_center + ( m_iBasisNormal * m_halfDimensions.x ) - ( jBasisNormal * m_halfDimensions.y ); // Bottom Right
}


//-----------------------------------------------------------------------------------------------
Vec2 const OBB2::GetLocalPosForWorldPos( Vec2 const& worldPos ) const
{
	Vec2 displacementFromCenter = worldPos - m_center;
	float localX = DotProduct2D( displacementFromCenter, m_iBasisNormal );

	Vec2 jBasisNormal = m_iBasisNormal.GetRotatedBy90Degrees();
	float localY = DotProduct2D( displacementFromCenter, jBasisNormal );

	return Vec2( localX, localY );
}


//-----------------------------------------------------------------------------------------------
Vec2 const OBB2::GetWorldPosForLocalPos( Vec2 const& localPos ) const
{
	Vec2 jBasisNormal = m_iBasisNormal.GetRotatedBy90Degrees();
	Vec2 worldPos = m_center + ( m_iBasisNormal * localPos.x ) + ( jBasisNormal * localPos.y );

	return worldPos;
}


//-----------------------------------------------------------------------------------------------
void OBB2::RotateAboutCenter( float rotationDeltaDegrees )
{
	m_iBasisNormal.RotateDegrees( rotationDeltaDegrees );
}