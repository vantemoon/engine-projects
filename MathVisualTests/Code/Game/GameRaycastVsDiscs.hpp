#pragma once
#include "Game/Game.hpp"


//-----------------------------------------------------------------------------------------------
class GameRaycastVsDiscs : public Game
{
public:
	GameRaycastVsDiscs();
	~GameRaycastVsDiscs();

	void Update( float deltaSeconds ) override;
	void Render() const override;
};