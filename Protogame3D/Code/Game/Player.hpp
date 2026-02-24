#pragma once
#include "Game/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Player : public Entity
{
public:
	Player( Game* owner );
	~Player();

	void Update( float deltaSeconds ) override;
	void UpdateFromMouse();
	void UpdateFromKeyboard( float deltaSeconds );
	void UpdateFromController( float deltaSeconds );
	void Render() const override;
};