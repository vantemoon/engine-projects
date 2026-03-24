#pragma once
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec3.hpp"


//-----------------------------------------------------------------------------------------------
class Camera; // Forward declaration
class Game; // Forward declaration
struct Vertex; // Forward declaration


//-----------------------------------------------------------------------------------------------
class Player
{
public:
	Game* m_game = nullptr;
	Vec3 m_position;
	Vec3 m_velocity;
	EulerAngles m_orientation;
	EulerAngles m_angularVelocity;
	Rgba8 m_color = Rgba8::WHITE;
	Camera* m_playerCamera = nullptr;

public:
	Player( Game* owner );
	~Player();

	void Update( float deltaSeconds );
	void UpdateCamera();
	void UpdateFromMouse();
	void UpdateFromKeyboard( float deltaSeconds );
	void UpdateFromController( float deltaSeconds );
	void Render() const;
	Mat44 GetModelToWorldTransform() const;
};