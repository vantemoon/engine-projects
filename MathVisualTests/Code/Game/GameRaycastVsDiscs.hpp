#pragma once
#include "Game/Game.hpp"
#include "Game/TestShapeDisc.hpp"


//-----------------------------------------------------------------------------------------------
class GameRaycastVsDiscs : public Game
{
public:
	const static int MAX_DISCS = 10;
	TestShapeDisc* m_testDiscs[MAX_DISCS] = {};

	bool                    m_isSlowMo = false;

	GameRaycastVsDiscs();
	~GameRaycastVsDiscs();

	void Update( float deltaSeconds ) override;
	void UpdateFromKeyboard();
	void Render() const override;

private:
	void GenerateRandomDiscs();
};