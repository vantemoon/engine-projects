#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class  Camera;     // Forward declaration
class  Map;        // Forward declaration
class  Player;     // Forward declaration
struct Vec3;       // Forward declaration
struct Vertex;     // Forward declaration


//-----------------------------------------------------------------------------------------------
class Game
{
public:
	std::vector<Map*>	m_maps;
	Map*				m_currentMap = nullptr;

	Player*				m_player = nullptr;
	Camera*				m_screenCamera = nullptr;

	bool				m_isScreenShaking = false;
	float				m_screenShakeIntensity = 0.f;
	float				m_screenShakeDuration = 0.f;
	float				m_screenShakeStartTime = 0.f;

	GameState			m_currentGameState;
	bool				m_isDebugFeaturesOn = false;
	Clock*				m_gameClock = nullptr;
	bool				m_hasControlsBeenShown = false;

public:
	Game();
	~Game();

	void Startup();
	void LoadGameConfigFromFile( char const* filepath );

	void Update();
	void UpdateAttractMode();
	void UpdateEntities();
	void UpdateFromKeyboard();
	void UpdateFromController();

	void Render()													  const;
	void RenderMap()												  const;
	void RenderEntities()											  const;
	void RenderHUD()												  const;
	void RenderAttractMode()										  const;
	
	void ScreenShake( float intensity );
	void Reset();

	static bool Command_Controls( EventArgs& args );

protected:
	void DeleteGarbageEntities();
	void DebugDraw()												   const;
	Vec3 TransformWorldToScreen( Vec3 const& worldPosition )		   const;
	bool IsOnScreen( Vec2 const& worldPosition, float cosmeticRadius ) const;
	void AddInstructionsToDevConsole()								   const;
	void AddVertsForCube( std::vector<Vertex>& verts, float size) const;
};