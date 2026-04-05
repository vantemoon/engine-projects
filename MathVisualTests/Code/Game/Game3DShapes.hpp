#pragma once
#include "Game/Game.hpp"


//-----------------------------------------------------------------------------------------------
class Game3DShapes : public Game
{
public:
	Game3DShapes();
	~Game3DShapes();

	void Update( float deltaSeconds ) override;
	void Render() const override;
};