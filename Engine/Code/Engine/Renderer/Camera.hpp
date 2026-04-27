#pragma once
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"


//-----------------------------------------------------------------------------------------------
class Camera
{
public:
    enum Mode
    {
		eMode_Orthographic,
        eMode_Perspective,
		eMode_Count
	};

	void SetOrthoView( Vec2 const& bottomLeft, Vec2 const& topRight, float near = 0.f, float far = 1.f );
	void SetPerspectiveView( float aspect, float fov, float near, float far );

	void SetPositionAndOrientation( Vec3 const& position, EulerAngles const& orientation );
	void SetPosition( Vec3 const& position );
	Vec3 GetPosition() const;
	void SetOrientation( EulerAngles const& orientation );
	EulerAngles GetOrientation() const;
	Vec3 GetForwardDir() const;

	Mat44 GetCameraToWorldTransform() const;
	Mat44 GetWorldToCameraTransform() const;

	void SetCameraToRenderTransform( Mat44 const& mat );
	Mat44 GetCameraToRenderTransform() const;

	Mat44 GetRenderToClipTransform() const;

    Vec2 GetOrthoBottomLeft() const;
    Vec2 GetOrthoTopRight() const;
	void Translate2D( Vec2 const& translation );

	Mat44 GetOrthoMatrix() const;
	Mat44 GetPerspectiveMatrix() const;
	Mat44 GetProjectionMatrix() const;

	Mode GetMode() const;

private:
	Mode m_mode = eMode_Orthographic;

	Vec3 m_position;
	EulerAngles m_orientation;

    Vec2 m_orthoBottomLeft;
    Vec2 m_orthoTopRight;
	float m_orthoNear;
	float m_orthoFar;

	float m_perspectiveAspect;
	float m_perspectiveFOV;
	float m_perspectiveNear;
	float m_perspectiveFar;

	Mat44 m_cameraToRenderTransform;
};
