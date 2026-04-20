#pragma once
#include "Game/Game.hpp"


//-----------------------------------------------------------------------------------------------
class Game2DCurves : public Game
{
public:
	Game2DCurves();
	~Game2DCurves();

	void Update( float deltaSeconds) override;
	void UpdateFromKeyboard( float deltaSeconds );
	void Render() const override;
};