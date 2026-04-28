#pragma once
#include "Game/Controller.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec3.hpp"


//-----------------------------------------------------------------------------------------------
class Actor;
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
enum class ControlMode
{
	KEYBOARD,
	CONTROLLER,
	COUNT
};


//-----------------------------------------------------------------------------------------------
class Player : public Controller
{
public:
	Game* m_game = nullptr;

	Vec3 m_position;
	EulerAngles m_orientation;

	Camera* m_playerWorldCamera = nullptr;
	Camera* m_playerScreenCamera = nullptr;
	CameraMode m_cameraMode = CameraMode::FIRST_PERSON;

	ControlMode m_controlMode = ControlMode::KEYBOARD;

public:
	Player( Game* owner, ControlMode controlMode = ControlMode::KEYBOARD );
	~Player();

	void Update( float deltaSeconds );
	void UpdateInput();
	void UpdateCamera();
	void UpdateFreeFlyCameraControls( float deltaSeconds );
	void UpdateFirstPersonControls( Actor* actor, float deltaSeconds );

	void SelectPreviousWeapon( Actor* actor );
	void SelectNextWeapon( Actor* actor );
	void SelectWeaponBySlot( Actor* actor, int slotIndex );

	void Render() const;
	void RenderHUD() const;
	Mat44 GetModelToWorldTransform() const;

	ControlMode GetControlMode() const;

private:
	void UpdateFreeFlyCameraFromMouse();
	void UpdateFreeFlyCameraFromKeyboard( float deltaSeconds );
	void UpdateFreeFlyCameraFromController( float deltaSeconds );
	void UpdateFirstPersonFromMouse( Actor* actor );
	void UpdateFirstPersonFromKeyboard( Actor* actor, float deltaSeconds );
	void UpdateFirstPersonFromController( Actor* actor, float deltaSeconds );
};