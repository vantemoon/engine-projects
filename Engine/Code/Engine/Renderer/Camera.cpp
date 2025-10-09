#include "Engine/Renderer/Camera.hpp"


//-----------------------------------------------------------------------------------------------
Camera::Camera()
	: m_orthoBottomLeft( Vec2( 0.f, 0.f ) )
	, m_orthoTopRight( Vec2( 10.f, 10.f ) )
{
	// Do nothing
}


//-----------------------------------------------------------------------------------------------
Camera::~Camera() = default;


//-----------------------------------------------------------------------------------------------
void Camera::SetOrthoView( Vec2 const& bottomLeft, Vec2 const& topRight )
{
	m_orthoBottomLeft = bottomLeft;
	m_orthoTopRight = topRight;
}


//-----------------------------------------------------------------------------------------------
Vec2 Camera::GetOrthoBottomLeft() const
{
	return m_orthoBottomLeft;
}


//-----------------------------------------------------------------------------------------------
Vec2 Camera::GetOrthoTopRight() const
{
	return m_orthoTopRight;
}


//-----------------------------------------------------------------------------------------------
void Camera::Translate2D( Vec2 const& translation )
{
	m_orthoBottomLeft += translation;
	m_orthoTopRight += translation;
}