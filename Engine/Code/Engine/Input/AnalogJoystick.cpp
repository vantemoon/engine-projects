#include "Engine/Math/MathUtils.hpp"
#include "Engine/Input/AnalogJoystick.hpp"


//-----------------------------------------------------------------------------------------------
Vec2 AnalogJoystick::GetPosition() const
{
	return m_correctedPosition;
}


//-----------------------------------------------------------------------------------------------
float AnalogJoystick::GetMagnitude() const
{
	return m_correctedPosition.GetLength();
}


//-----------------------------------------------------------------------------------------------
float AnalogJoystick::GetOrientationDegrees() const
{
	return m_correctedPosition.GetOrientationDegrees();
}


//-----------------------------------------------------------------------------------------------
Vec2 AnalogJoystick::GetRawUncorrectedPosition() const
{
	return m_rawPosition;
}


//-----------------------------------------------------------------------------------------------
float AnalogJoystick::GetInnerDeadZoneFraction() const
{
	return m_innerDeadZoneFraction;
}


//-----------------------------------------------------------------------------------------------
float AnalogJoystick::GetOuterDeadZoneFraction() const
{
	return m_outerDeadZoneFraction;
}


//-----------------------------------------------------------------------------------------------
void AnalogJoystick::Reset()
{
	m_innerDeadZoneFraction = 0.3f;
	m_outerDeadZoneFraction = 0.95f;

	m_rawPosition = Vec2( 0.f, 0.f );
	m_correctedPosition = Vec2( 0.f, 0.f );
}


//-----------------------------------------------------------------------------------------------
void AnalogJoystick::SetDeadZoneThresholds( float normalizedInnerDeadZoneThreshold, float normalizedOuterDeadZoneThreshold )
{
	m_innerDeadZoneFraction = normalizedInnerDeadZoneThreshold;
	m_outerDeadZoneFraction = normalizedOuterDeadZoneThreshold;
}


//-----------------------------------------------------------------------------------------------
void AnalogJoystick::SetPosition( float rawNormalizedX, float rawNormalizedY )
{
	m_rawPosition.x = rawNormalizedX;
	m_rawPosition.y = rawNormalizedY;
	float magnitude = m_rawPosition.GetLength();
	if ( magnitude < m_innerDeadZoneFraction )
	{
		m_correctedPosition = Vec2( 0.f, 0.f );
		return;
	}
	if ( magnitude > m_outerDeadZoneFraction )
	{
		m_correctedPosition = m_rawPosition.GetNormalized();
		return;
	}
	float scaledMagnitude = RangeMapClamped( magnitude, m_innerDeadZoneFraction, m_outerDeadZoneFraction, 0.f, 1.f );
	m_correctedPosition = m_rawPosition.GetNormalized() * scaledMagnitude;
}