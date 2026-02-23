#include "Engine/Renderer/Camera.hpp"


//-----------------------------------------------------------------------------------------------
void Camera::SetOrthoView( Vec2 const& bottomLeft, Vec2 const& topRight, float near, float far )
{
	m_mode = eMode_Orthographic;
	m_orthoBottomLeft = bottomLeft;
	m_orthoTopRight = topRight;
	m_orthoNear = near;
	m_orthoFar = far;
}


//-----------------------------------------------------------------------------------------------
void Camera::SetPerspectiveView( float aspect, float fov, float near, float far )
{
	m_mode = eMode_Perspective;
	m_perspectiveAspect = aspect;
	m_perspectiveFOV = fov;
	m_perspectiveNear = near;
	m_perspectiveFar = far;
}


//-----------------------------------------------------------------------------------------------
void Camera::SetPositionAndOrientation( Vec3 const& position, EulerAngles const& orientation )
{
	SetPosition( position );
	SetOrientation( orientation );
}


//-----------------------------------------------------------------------------------------------
void Camera::SetPosition( Vec3 const& position )
{
	m_position = position;
}


//-----------------------------------------------------------------------------------------------
Vec3 Camera::GetPosition() const
{
	return m_position;
}


//-----------------------------------------------------------------------------------------------
void Camera::SetOrientation( EulerAngles const& orientation )
{
	m_orientation = orientation;
}


//-----------------------------------------------------------------------------------------------
EulerAngles Camera::GetOrientation() const
{
	return m_orientation;
}


//-----------------------------------------------------------------------------------------------
Mat44 Camera::GetCameraToWorldTransform() const
{
	Mat44 cameraToWorld = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
	cameraToWorld.AppendTranslation3D( m_position );
	return cameraToWorld;
}


//-----------------------------------------------------------------------------------------------
Mat44 Camera::GetWorldToCameraTransform() const
{
	return GetCameraToWorldTransform().GetOrthonormalInverse();
}


//-----------------------------------------------------------------------------------------------
void Camera::SetCameraToRenderTransform( Mat44 const& mat )
{
	m_cameraToRenderTransform = mat;
}


//-----------------------------------------------------------------------------------------------
Mat44 Camera::GetCameraToRenderTransform() const
{
	return m_cameraToRenderTransform;
}


//-----------------------------------------------------------------------------------------------
Mat44 Camera::GetRenderToClipTransform() const
{
	if ( m_mode == eMode_Orthographic )
	{
		return GetOrthoMatrix();
	}
	else // if ( m_mode == eMode_Perspective )
	{
		return GetPerspectiveMatrix();
	}
}


//-----------------------------------------------------------------------------------------------
Mat44 Camera::GetOrthoMatrix() const
{
	return Mat44::MakeOrthoProjection( m_orthoBottomLeft.x, m_orthoTopRight.x, m_orthoBottomLeft.y, m_orthoTopRight.y, m_orthoNear, m_orthoFar );
}


//-----------------------------------------------------------------------------------------------
Mat44 Camera::GetPerspectiveMatrix() const
{
	return Mat44::MakePerspectiveProjection( m_perspectiveFOV, m_perspectiveAspect, m_perspectiveNear, m_perspectiveFar );
}


//-----------------------------------------------------------------------------------------------
Mat44 Camera::GetProjectionMatrix() const
{
	if ( m_mode == eMode_Orthographic )
	{
		return GetOrthoMatrix();
	}
	else // if ( m_mode == eMode_Perspective )
	{
		return GetPerspectiveMatrix();
	}
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