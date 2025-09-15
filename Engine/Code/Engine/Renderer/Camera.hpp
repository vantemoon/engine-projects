#pragma once
#include "Engine/Math/Vec2.hpp"


//-----------------------------------------------------------------------------------------------
class Camera
{
private:
    Vec2 m_orthoBottomLeft;
    Vec2 m_orthoTopRight;

public:
	Camera();
	~Camera();
    void SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight );
    Vec2 GetOrthoBottomLeft() const;
    Vec2 GetOrthoTopRight() const;
};
