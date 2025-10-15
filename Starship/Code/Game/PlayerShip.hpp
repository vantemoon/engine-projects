#pragma once
#include "Game/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Entity; // Forward declaration
class Game;   // Forward declaration
struct Vec2;  // Forward declaration


//-----------------------------------------------------------------------------------------------
class PlayerShip : public Entity
{
public:
	static constexpr int NUM_SHIP_VERTS = 15;
	bool  m_isAccelerating = false;
	bool  m_isTurningLeft = false;
	bool  m_isTurningRight = false;
	float m_thrustFraction = 0.f;

	float m_maxEnergy = 100.f;
	float m_currentEnergy = 100.f;
	float m_energyRechargeRate = 10.f;
	float m_costDetonate = 40.0f;
	float m_costTelefrag = 55.0f;

	bool  m_isInvincible = false;
	float m_invincibleTimer = 0.0f;
	float m_invincibleDuration = 2.5f;

	bool  m_flashOn = true;
	float m_flashTimer = 0.0f;
	float m_flashInterval = 0.2f;
	unsigned char m_flashAlpha = 120;

public:
	PlayerShip( Game* game, Vec2 const& startingPosition, Vec2 const& startingVelocity );
	~PlayerShip() override;

	void  Update( float deltaSeconds ) override;
	void  UpdateFromKeyboard();
	void  UpdateFromController( float deltaseconds );
	void  UpdateEnergy( float deltaSeconds );
	bool  HasEnoughEnergy( float cost ) const;
	float GetEnergyFraction() const;
	bool  TrySpendEnergy( float cost );
	void  Render() const override;
	void  Die() override;
	void  Respawn();
	void  GetVertexArrayCopy( Vertex* out_vertexArray ) const;
	void  StartRespawnInvincibility( float durationSeconds );
	void  UpdateInvincibility( float deltaSeconds );

private:
	void  InitializeVertexArray() override;
	void  RenderThrust() const;
	void  BounceOffWorldEdges();
	void  Accelerate( float deltaSeconds );
	void  TurnLeft();
	void  TurnRight();
};