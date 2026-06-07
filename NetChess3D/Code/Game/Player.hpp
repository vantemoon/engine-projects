#pragma once
#include "Game/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class  Camera;


//-----------------------------------------------------------------------------------------------
enum class CameraMode
{
	POV,
	OVERHEAD,
	FREE_FLY,
	NUM_CAMERA_MODES
};


//-----------------------------------------------------------------------------------------------
class Player : public Entity
{
public:
	Camera* m_playerCamera = nullptr;
	CameraMode m_cameraMode = CameraMode::POV;

	Vec3 m_freeFlyPosition = Vec3( 4.f, -4.f, 5.f );
	EulerAngles m_freeFlyOrientation = EulerAngles( 90.f, 32.f, 0.f );

public:
	Player( Game* owner );
	~Player();

	void Update( float deltaSeconds ) override;

	void UpdateCamera();
	void UpdatePOVCamera();
	void UpdateOverheadCamera();

	void UpdateFreeFlyCamera();
	void UpdateFreeFlyCameraFromMouse();
	void UpdateFreeFlyCameraFromKeyboard( float deltaSeconds );
	void UpdateFreeFlyCameraFromController( float deltaSeconds );

	void SetCameraLookAt( Vec3 const& position, Vec3 const& target );
	void Render() const override;
};