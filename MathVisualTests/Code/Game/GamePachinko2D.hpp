#pragma once
#include "Game/Game.hpp"
#include "Game/TestShapeCapsule.hpp"
#include "Game/TestShapeDisc.hpp"
#include "Game/TestShapeOBB.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class GamePachinko2D : public Game
{
public:
	std::vector<TestShapeCapsule*> m_capsuleBumpers;
	std::vector<TestShapeDisc*> m_discBumpers;
	std::vector<TestShapeOBB*> m_obbBumpers;
	std::vector<TestShapeDisc*> m_balls;

	Vec2 m_rayStartPos = Vec2( 500, 250 );
	Vec2 m_rayEndPos = Vec2( 1500, 750 );

	bool m_isSlowMo = false;

	GamePachinko2D();
	~GamePachinko2D();

	void Update( float deltaSeconds ) override;
	void UpdateFromKeyboard();
	void Render() const override;

private:
	void LoadGameConfigFromFile( char const* filepath );
	Vec2 GetMouseWorldPos() const;
	void GenerateRandomBumpers();
	void Reset();
	Rgba8 GetBumperColorFromElasticity( float elasticity ) const;
};