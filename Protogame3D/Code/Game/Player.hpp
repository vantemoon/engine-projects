#pragma once
#include "Game/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Player : public Entity
{
public:
	Player( Game* owner );
	~Player();

	void Update( float deltaSeconds ) override;
	void Render() const override;
};