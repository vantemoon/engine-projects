#pragma once
#include "Game/Controller.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec3.hpp"


//-----------------------------------------------------------------------------------------------
class Camera;
class Game;
struct Vertex;


//-----------------------------------------------------------------------------------------------
enum class CameraMode
{ 
	FREE_FLY,
	FIRST_PERSON,
	COUNT
};


//-----------------------------------------------------------------------------------------------
class Player : public Controller
{
public:
	Game* m_game = nullptr;

	Vec3 m_position;
	EulerAngles m_orientation;

	Camera* m_playerCamera = nullptr;
	CameraMode m_cameraMode = CameraMode::FREE_FLY;

public:
	Player( Game* owner );
	~Player();

	void Update( float deltaSeconds );
	void UpdateInput();
	void UpdateCamera();
	void UpdateFreeFlyCameraFromMouse();
	void UpdateFreeFlyCameraFromKeyboard( float deltaSeconds );
	void UpdateFreeFlyCameraFromController( float deltaSeconds );

	void Render() const;
	Mat44 GetModelToWorldTransform() const;
};