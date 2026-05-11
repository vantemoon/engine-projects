#pragma once
#include "Game/Controller.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
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

	int m_playerIndex = -1;
	int m_controllerIndex = -1;

	int m_killCount = 0;
	int m_deathCount = 0;

	int m_foodAmmo = 10;
	int m_cleaningCharges = 5;

	AABB2 m_viewport = AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) );

	Camera* m_playerWorldCamera = nullptr;
	Camera* m_playerScreenCamera = nullptr;
	CameraMode m_cameraMode = CameraMode::FIRST_PERSON;

	ControlMode m_controlMode = ControlMode::KEYBOARD;

public:
	Player( Game* owner, ControlMode controlMode = ControlMode::KEYBOARD );
	~Player();

	void Update( float deltaSeconds ) override;
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
	void SetPlayerIndex( int playerIndex );
	void SetControllerIndex( int controllerIndex );
	void SetViewport( AABB2 const& viewport );

	bool IsDead() const;
	Vec3 GetForwardVector() const;
	Vec3 GetUpVector() const;

private:
	void UpdateFreeFlyCameraFromMouse();
	void UpdateFreeFlyCameraFromKeyboard( float deltaSeconds );
	void UpdateFreeFlyCameraFromController( float deltaSeconds );
	void UpdateFirstPersonFromMouse( Actor* actor );
	void UpdateFirstPersonFromKeyboard( Actor* actor, float deltaSeconds );
	void UpdateFirstPersonFromController( Actor* actor, float deltaSeconds );
	Actor* GetLookedAtVirtualPetActor() const;
};