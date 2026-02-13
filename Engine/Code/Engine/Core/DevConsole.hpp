#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/EventSystem.hpp"
#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
struct DevConsoleConfig
{
	bool m_isEnabled = true;
	std::string m_consoleFont = "Data/Fonts/SquirrelFixedFont";
	int m_linesOnScreen = 40;
	int m_maxCommandHistory = 128;
};


//-----------------------------------------------------------------------------------------------
enum DevConsoleMode
{
	HIDDEN,
	OPEN_FULL,
	NUM_MODES
};


//-----------------------------------------------------------------------------------------------
typedef std::pair<Rgba8, std::string> DevConsoleLine;

//-----------------------------------------------------------------------------------------------
class DevConsole
{
public:
	DevConsole( DevConsoleConfig const& config );
	~DevConsole();

	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void Execute( std::string const& consoleCommandText );
	void AddLine( Rgba8 const& color, std::string const& text );
	void Render( AABB2 const& bound ) const;

	DevConsoleMode GetMode() const;
	void SetMode( DevConsoleMode mode );
	void ToggleMode( DevConsoleMode mode );

	static bool Command_KeyPressed( EventArgs& args );
	static bool Command_KeyReleased( EventArgs& args );
	static bool Command_CharacterInput( EventArgs& args );
	static bool Command_Help( EventArgs& args );
	static bool Command_Clear( EventArgs& args );

	static bool Command_Test( EventArgs& args );
	int AddTwoInts( int a, int b ) const;

	static const Rgba8 ERROR;
	static const Rgba8 WARNING;
	static const Rgba8 INFO_MAJOR;
	static const Rgba8 INFO_MINOR;
	static const Rgba8 DEFAULT_TEXT_COLOR;

protected:
	void Render_OpenFull( AABB2 const& bound, BitmapFont& font, float fontAspect = 1.f ) const;
	void RenderInsertionPoint( float cellWidth, float cellHeight, int insertionPointPosition ) const;
	void InsertCharacterAtInsertionPoint( char character );
	std::string GetPreviousCommand( int offsetFromCurrent ) const;
	std::string GetNextCommand( int offsetFromCurrent ) const;

protected:
	DevConsoleConfig			m_config;
	DevConsoleMode				m_mode = DevConsoleMode::HIDDEN;
	std::vector<DevConsoleLine> m_lines;
	int                         m_linesOnScreen = 40;
	int                         m_maxCommandHistory = 128;
	std::vector<std::string>	m_commandHistory;
	int							m_commandHistorySearchOffset = 0;
	int							m_frameNumber = 0;
	int                         m_insertionPointPosition = 0;
	bool						m_isInsertionPointVisible = true;
};