#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Mat44.hpp"


//-----------------------------------------------------------------------------------------------
class Actor
{
public:
	Actor();
	~Actor();

	void Update();
	void Render() const;
	Mat44 GetModelMatrix() const;

public:
	Vec3 m_position;
	EulerAngles m_orientation;
	Rgba8 m_color;

	float m_physicsHeight = 0.f;
	float m_physicsRadius = 0.f;

	bool m_isStatic = false;
};