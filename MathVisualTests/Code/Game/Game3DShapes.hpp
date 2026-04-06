#pragma once
#include "Game/Game.hpp"
#include "Game/TestShape.hpp"
#include "Game/TestShapeAABB3D.hpp"


//-----------------------------------------------------------------------------------------------
class Game3DShapes : public Game
{
public:
	const static int	MAX_SHAPES = 12;
	const static int	NUM_SHAPE_PER_TYPE = 4;

	TestShapeAABB3D*	m_testAABBs[NUM_SHAPE_PER_TYPE] = {};

	Game3DShapes();
	~Game3DShapes();

	void Update( float deltaSeconds ) override;
	void UpdateFromKeyboard( float deltaSeconds );
	void Render() const override;

private:
	void GenerateRandomShapes();
	void GenerateRandomAABBs();
};