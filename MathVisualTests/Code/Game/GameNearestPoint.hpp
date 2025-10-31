#pragma once
#include "Game/Game.hpp"


//-----------------------------------------------------------------------------------------------
class GameNearestPoint : public Game
{
public:
	

public:
	GameNearestPoint();
	~GameNearestPoint();

	void Update( float deltaSeconds ) override;
	void Render() const override;
};