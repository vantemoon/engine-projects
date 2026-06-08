#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include <algorithm>
#include <math.h>


//-----------------------------------------------------------------------------------------------
DevConsole::DevConsole( DevConsoleConfig const& config )
	: m_config( config )
	, m_linesOnScreen( config.m_linesOnScreen )
	, m_maxCommandHistory( config.m_maxCommandHistory )
{
}


//-----------------------------------------------------------------------------------------------
DevConsole::~DevConsole()
{
	delete m_insertionPointBlinkTimer;
	m_insertionPointBlinkTimer = nullptr;
}


//-----------------------------------------------------------------------------------------------
void DevConsole::Startup()
{
	if ( m_insertionPointBlinkTimer == nullptr )
	{
		m_insertionPointBlinkTimer = new Timer( 0.5 );
	}
	m_insertionPointBlinkTimer->Start();
	m_isInsertionPointVisible = true;

	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "Help", Command_Help );
	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "Clear", Command_Clear );
	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "Test", Command_Test );
	g_engine->m_eventSystem->SetEventRequiredArgs( "Test", { "a=<int>", "b=<int>" } );

	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "KeyDown", Command_KeyPressed );
	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "KeyUp", Command_KeyReleased );
	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "CharacterInput", Command_CharacterInput );
	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "KeyDown", InputSystem::Event_KeyPressed );
	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "KeyUp", InputSystem::Event_KeyReleased );
	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "CharacterInput", InputSystem::Event_CharacterInput );
}


//-----------------------------------------------------------------------------------------------
void DevConsole::Shutdown()
{
	delete m_insertionPointBlinkTimer;
	m_insertionPointBlinkTimer = nullptr;
}


//-----------------------------------------------------------------------------------------------
void DevConsole::BeginFrame()
{
	m_frameNumber ++;

	if ( m_mode == DevConsoleMode::HIDDEN || m_insertionPointBlinkTimer == nullptr )
	{
		return;
	}

	double elapsedSeconds = m_insertionPointBlinkTimer->GetElapsedSeconds();
	if ( elapsedSeconds >= m_insertionPointBlinkTimer->m_period )
	{
		int periodsElapsed = static_cast<int>( floor( elapsedSeconds / m_insertionPointBlinkTimer->m_period ) );
		if ( ( periodsElapsed % 2 ) != 0 )
		{
			m_isInsertionPointVisible = !m_isInsertionPointVisible;
		}
		m_insertionPointBlinkTimer->m_startTime += periodsElapsed * m_insertionPointBlinkTimer->m_period;
	}
}


//-----------------------------------------------------------------------------------------------
void DevConsole::EndFrame()
{
}


//-----------------------------------------------------------------------------------------------
void DevConsole::Execute( std::string const& consoleCommandText )
{
	m_commandHistory.push_back( consoleCommandText );
	m_commandHistorySearchOffset = 0;

	if ( m_maxCommandHistory > 0 && ( int ) m_commandHistory.size() > m_maxCommandHistory )
	{
		int excessCount = ( int ) m_commandHistory.size() - m_maxCommandHistory;
		m_commandHistory.erase( m_commandHistory.begin(), m_commandHistory.begin() + excessCount );
	}

	Strings splitText = SplitStringOnDelimiter( consoleCommandText, ' ' );
	if ( splitText.size() == 0 )
	{
		AddLine( Rgba8::INFO_MINOR, "" );
	}
	else
	{
		std::string command = splitText[0];
		std::string commandLower = ToLower( command );
		EventArgs args;
		std::vector<std::string> registeredEventNames = g_engine->m_eventSystem->GetEventNames();
		bool isRegistered = false;
		for ( std::string const& eventName : registeredEventNames )
		{
			if ( ToLower( eventName ) == commandLower )
			{
				isRegistered = true;
				break;
			}
		}
		if ( !isRegistered )
		{
			AddLine( Rgba8::ERROR, consoleCommandText );
			AddLine( Rgba8::ERROR, "Unknown command: " + command );
			return;
		}
		if ( splitText.size() == 1 )
		{
			EventRequiredArgsList requiredArgs = g_engine->m_eventSystem->GetEventRequiredArgs( command );
			if ( !requiredArgs.empty() )
			{
				std::string requiredArgsString;
				for ( std::string const& requiredArg : requiredArgs )
				{
					requiredArgsString += requiredArg + " ";
				}
				AddLine( Rgba8::WARNING, consoleCommandText );
				AddLine( Rgba8::WARNING, "Command requires arguments: " + requiredArgsString );
				return;
			}
		}
		for ( int argIndex = 1; argIndex < ( int ) splitText.size(); ++ argIndex )
		{
			std::string arg = splitText[argIndex];
			Strings argSplit = SplitStringOnDelimiter( arg, '=' );
			if ( argSplit.size() == 2 )
			{
				std::string key = argSplit[0];
				std::string value = argSplit[1];
				args.SetValue( key, value );
			}
			else
			{
				AddLine( Rgba8::WARNING, consoleCommandText );
				AddLine( Rgba8::WARNING, "Invalid argument: " + arg );
				EventRequiredArgsList requiredArgs = g_engine->m_eventSystem->GetEventRequiredArgs( command );
				if ( !requiredArgs.empty() )
				{
					std::string requiredArgsString;
					for ( std::string const& requiredArg : requiredArgs )
					{
						requiredArgsString += requiredArg + " ";
					}
					AddLine( Rgba8::WARNING, "Command requires arguments: " + requiredArgsString );
				}
				return;
			}
		}
		AddLine( Rgba8::SUCCESS, consoleCommandText );
		AddLine( Rgba8::SUCCESS, "Executed command: " + consoleCommandText );
		g_engine->m_eventSystem->FireEvent( command, args );
	}
}


//-----------------------------------------------------------------------------------------------
void DevConsole::AddLine( Rgba8 const& color, std::string const& text )
{
	double t = Clock::GetSystemClock().GetTotalSeconds();
	int totalSeconds = ( int ) t;
	int hours = ( totalSeconds / 3600 ) % 24;
	int minutes = ( totalSeconds / 60 ) % 60;
	int seconds = totalSeconds % 60;
	int millis = ( int )( ( t - ( double ) totalSeconds ) * 1000.0 );
	if ( millis < 0 ) millis = 0;
	if ( millis > 999 ) millis = 999;

	auto pad = []( int v, int width )
	{
		std::string s = std::to_string( v );
		while ( ( int ) s.size() < width )
		{
			s = "0" + s;
		}
		return s;
	};

	std::string line = "["
		+ pad( hours, 2 ) + ":"
		+ pad( minutes, 2 ) + ":"
		+ pad( seconds, 2 ) + "."
		+ pad( millis, 3 ) + "]"
		+ "[#" + std::to_string( m_frameNumber ) + "] "
		+ text;

	m_lines.emplace_back( color, line );
}


//-----------------------------------------------------------------------------------------------
void DevConsole::AddLineWithoutTimestamp( Rgba8 const& color, std::string const& text )
{
	std::string line = text;
	m_lines.emplace_back( color, line );
}


//-----------------------------------------------------------------------------------------------
void DevConsole::Render( AABB2 const& bound ) const
{
	if ( m_mode == DevConsoleMode::HIDDEN )
	{
		return;
	}
	if ( m_mode == DevConsoleMode::OPEN_FULL )
	{
		Shader* shader = g_engine->m_renderer->CreateOrGetShader( "EngineDefault" );
		g_engine->m_renderer->BindTexture( nullptr );
		g_engine->m_renderer->BindShader( shader );
		g_engine->m_renderer->SetBlendMode( BlendMode::ALPHA );
		g_engine->m_renderer->SetSamplerMode( SamplerMode::POINT_CLAMP );
		g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
		g_engine->m_renderer->BeginCamera( *m_config.m_camera );
		BitmapFont* font = g_engine->m_renderer->CreateOrGetBitmapFontFromFile( m_config.m_consoleFont.c_str() );
		Render_OpenFull( bound, *font, 1.f );
		g_engine->m_renderer->EndCamera( *m_config.m_camera );
		m_config.m_camera->SetOrthoView( Vec2( 0, 0 ), Vec2( 1600, 800 ) );
	}
}


//-----------------------------------------------------------------------------------------------
DevConsoleMode DevConsole::GetMode() const
{
	return m_mode;
}


//-----------------------------------------------------------------------------------------------
void DevConsole::SetMode( DevConsoleMode mode )
{
	m_mode = mode;
}


//-----------------------------------------------------------------------------------------------
void DevConsole::ToggleMode( DevConsoleMode mode )
{
	if ( m_mode == mode )
	{
		SetMode( DevConsoleMode::HIDDEN );
	}
	else
	{
		SetMode( mode );
	}
}


//-----------------------------------------------------------------------------------------------
void DevConsole::Render_OpenFull( AABB2 const& bound, BitmapFont& font, float fontAspect ) const
{
	std::vector<Vertex> backgroundVerts;
	std::vector<Vertex> textVerts;

	AddVertsForAABB2D( backgroundVerts, bound, Rgba8( 0, 0, 0, 150 ) );

	float cellHeight = 20.f;
	float cellWidth = cellHeight * fontAspect;
	float availableWidth = bound.m_maxs.x - bound.m_mins.x - 10.f;
	int maxVisibleChars = static_cast<int>( availableWidth / cellWidth );
	if ( maxVisibleChars < 0 )
	{
		maxVisibleChars = 0;
	}

	float yOffset = bound.m_mins.y + 2.f + cellHeight;
	int linesRendered = 0;
	for ( int lineIndex = static_cast< int >( m_lines.size() ) - 1; lineIndex >= 0; -- lineIndex )
	{
		if ( m_linesOnScreen > 0 && linesRendered >= m_linesOnScreen )
		{
			break;
		}
		if ( yOffset + cellHeight > bound.m_maxs.y )
		{
			break;
		}

		DevConsoleLine const& line = m_lines[lineIndex];
		Vec2 textMins = Vec2( bound.m_mins.x + 5.f, yOffset );
		font.AddVertsForText2D( textVerts, textMins, cellHeight, line.second, line.first, fontAspect );
		yOffset += cellHeight;
		++ linesRendered;
	}

	std::string currentCommand = g_gameConfigBlackboard.GetValue( "currentCommand", "" );
	int commandLength = static_cast<int>( currentCommand.size() );
	int insertionPointPosition = m_insertionPointPosition;
	if ( insertionPointPosition < 0 )
	{
		insertionPointPosition = 0;
	}
	if ( insertionPointPosition > commandLength )
	{
		insertionPointPosition = commandLength;
	}

	int startIndex = 0;
	if ( maxVisibleChars > 0 && insertionPointPosition > maxVisibleChars )
	{
		startIndex = insertionPointPosition - maxVisibleChars;
	}
	std::string visibleCommand = currentCommand.substr( startIndex, maxVisibleChars );
	int visibleInsertionPoint = insertionPointPosition - startIndex;

	Vec2 textMins = Vec2( bound.m_mins.x + 5.f, 2.f );
	font.AddVertsForText2D( textVerts, textMins, cellHeight, visibleCommand, Rgba8::WHITE, fontAspect );

	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( static_cast< int >( backgroundVerts.size() ), backgroundVerts.data() );

	g_engine->m_renderer->BindTexture( &font.GetTexture() );
	g_engine->m_renderer->DrawVertexArray( static_cast< int >( textVerts.size() ), textVerts.data() );

	if ( m_isInsertionPointVisible )
	{
		RenderInsertionPoint( cellWidth, cellHeight, visibleInsertionPoint );
	}
}


//-----------------------------------------------------------------------------------------------
void DevConsole::RenderInsertionPoint( float cellWidth, float cellHeight, int insertionPointPosition ) const
{
	if ( !m_isInsertionPointVisible )
	{
		return;
	}

	float offsetFromStart = insertionPointPosition * cellWidth + 5.f;
	Vec2 mins = Vec2( offsetFromStart, 2.f );
	Vec2 maxs = Vec2( offsetFromStart + 2.f, cellHeight + 2.f );
	std::vector<Vertex> verts;
	AddVertsForAABB2D( verts, AABB2( mins, maxs ), Rgba8::DEFAULT_TEXT_COLOR );
	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( static_cast< int >( verts.size() ), verts.data() );
}


//------------------------------------------------------------------------------------------------
bool DevConsole::Command_KeyPressed( EventArgs& args )
{
	if ( g_engine && g_engine->m_devConsole )
	{
		if ( g_engine->m_devConsole->m_insertionPointBlinkTimer != nullptr )
		{
			g_engine->m_devConsole->m_insertionPointBlinkTimer->Start();
			g_engine->m_devConsole->m_isInsertionPointVisible = true;
		}

		unsigned char key = static_cast<unsigned char>( args.GetValue( "KeyCode", -1 ) );
		if ( key == KEYCODE_TILDE )
		{
			g_engine->m_devConsole->ToggleMode( DevConsoleMode::OPEN_FULL );
			return true;
		}

		if ( g_engine->m_devConsole->GetMode() != DevConsoleMode::HIDDEN )
		{
			// Console is open
			if ( key == KEYCODE_ENTER )
			{
				if ( g_gameConfigBlackboard.GetValue( "currentCommand", "" ).empty() )
				{
					g_engine->m_devConsole->SetMode( DevConsoleMode::HIDDEN );
				}
				else
				{
					std::string currentCommand = g_gameConfigBlackboard.GetValue( "currentCommand", "" );
					g_engine->m_devConsole->Execute( currentCommand );
					g_gameConfigBlackboard.SetValue( "currentCommand", "" );
					g_engine->m_devConsole->m_insertionPointPosition = 0;
				}
				return true;
			}

			if ( key == KEYCODE_ESCAPE )
			{
				if ( !g_gameConfigBlackboard.GetValue( "currentCommand", "" ).empty() )
				{
					g_gameConfigBlackboard.SetValue( "currentCommand", "" );
					g_engine->m_devConsole->m_insertionPointPosition = 0;
				}
				else
				{
					g_engine->m_devConsole->SetMode( DevConsoleMode::HIDDEN );
				}
				return true;
			}

			if ( key == KEYCODE_LEFTARROW )
			{
				if ( g_engine->m_devConsole->m_insertionPointPosition > 0 )
				{
					g_engine->m_devConsole->m_insertionPointPosition --;
				}
				return true;
			}

			if ( key == KEYCODE_RIGHTARROW )
			{
				std::string currentCommand = g_gameConfigBlackboard.GetValue( "currentCommand", "" );
				if ( g_engine->m_devConsole->m_insertionPointPosition < ( int ) currentCommand.size() )
				{
					g_engine->m_devConsole->m_insertionPointPosition ++;
				}
				return true;
			}

			if ( key == KEYCODE_HOME )
			{
				g_engine->m_devConsole->m_insertionPointPosition = 0;
				return true;
			}

			if ( key == KEYCODE_END )
			{
				std::string currentCommand = g_gameConfigBlackboard.GetValue( "currentCommand", "" );
				g_engine->m_devConsole->m_insertionPointPosition = ( int ) currentCommand.size();
				return true;
			}

			if ( key == KEYCODE_DELETE )
			{
				std::string currentCommand = g_gameConfigBlackboard.GetValue( "currentCommand", "" );
				if ( g_engine->m_devConsole->m_insertionPointPosition < ( int ) currentCommand.size() )
				{
					currentCommand.erase( g_engine->m_devConsole->m_insertionPointPosition, 1 );
					g_gameConfigBlackboard.SetValue( "currentCommand", currentCommand );
				}
			}

			if ( key == KEYCODE_BACKSPACE )
			{
				std::string currentCommand = g_gameConfigBlackboard.GetValue( "currentCommand", "" );
				if ( g_engine->m_devConsole->m_insertionPointPosition > 0 && !currentCommand.empty() )
				{
					currentCommand.erase( g_engine->m_devConsole->m_insertionPointPosition - 1, 1 );
					g_gameConfigBlackboard.SetValue( "currentCommand", currentCommand );
					g_engine->m_devConsole->m_insertionPointPosition --;
				}
			}

			if ( key == KEYCODE_UPARROW )
			{
				std::string currentCommand = g_engine->m_devConsole->GetPreviousCommand( g_engine->m_devConsole->m_commandHistorySearchOffset );
				g_gameConfigBlackboard.SetValue( "currentCommand", currentCommand );
				g_engine->m_devConsole->m_insertionPointPosition = ( int ) currentCommand.size();
				if ( g_engine->m_devConsole->m_commandHistorySearchOffset < ( int ) g_engine->m_devConsole->m_commandHistory.size() - 1 )
				{
					g_engine->m_devConsole->m_commandHistorySearchOffset ++;
				}
			}

			if ( key == KEYCODE_DOWNARROW )
			{
				if ( g_engine->m_devConsole->m_commandHistorySearchOffset == 0 )
				{
					g_gameConfigBlackboard.SetValue( "currentCommand", "" );
					g_engine->m_devConsole->m_insertionPointPosition = 0;
					return true;
				}

				if ( g_engine->m_devConsole->m_commandHistorySearchOffset > 0 )
				{
					g_engine->m_devConsole->m_commandHistorySearchOffset --;
				}
				std::string currentCommand = g_engine->m_devConsole->GetNextCommand( g_engine->m_devConsole->m_commandHistorySearchOffset );
				g_gameConfigBlackboard.SetValue( "currentCommand", currentCommand );
				g_engine->m_devConsole->m_insertionPointPosition = ( int ) currentCommand.size();
			}

			return true;
		}
		else // Console is closed
		{
			return false;
		}
	}
	return false;
}


//------------------------------------------------------------------------------------------------
bool DevConsole::Command_KeyReleased( EventArgs& args )
{
	UNUSED( args );
	if ( g_engine && g_engine->m_devConsole )
	{
		if ( g_engine->m_devConsole->GetMode() != DevConsoleMode::HIDDEN )
		{
			return true;
		}
	}
	return false;
}


//------------------------------------------------------------------------------------------------
bool DevConsole::Command_CharacterInput( EventArgs& args )
{
	if ( g_engine && g_engine->m_devConsole )
	{
		if ( g_engine->m_devConsole->GetMode() != DevConsoleMode::HIDDEN )
		{
			if ( g_engine->m_devConsole->m_insertionPointBlinkTimer != nullptr )
			{
				g_engine->m_devConsole->m_insertionPointBlinkTimer->Start();
				g_engine->m_devConsole->m_isInsertionPointVisible = true;
			}

			std::string characterText = args.GetValue( "Character", "" );
			int character = static_cast< int >( characterText[0] );
			if ( character >= 32 && character <= 126 && characterText != "`" && characterText != "~" )
			{
				g_engine->m_devConsole->InsertCharacterAtInsertionPoint( static_cast< char >( character ) );
			}
			return true;
		}
	}
	return false;
}


//------------------------------------------------------------------------------------------------
bool DevConsole::Command_Help( EventArgs& args )
{
	UNUSED( args );
	if ( g_engine && g_engine->m_devConsole )
	{
		std::vector<std::string> eventNames = g_engine->m_eventSystem->GetEventNames( true );
		g_engine->m_devConsole->AddLine( Rgba8::INFO_MAJOR, "Registered commands:" );
		for ( std::string const& eventName : eventNames )
		{
			g_engine->m_devConsole->AddLine( Rgba8::INFO_MINOR, eventName );
		}
		return true;
	}
	return false;
}


//------------------------------------------------------------------------------------------------
bool DevConsole::Command_Clear( EventArgs& args )
{
	UNUSED( args );
	if ( g_engine && g_engine->m_devConsole )
	{
		g_engine->m_devConsole->m_lines.clear();
		return true;
	}
	return false;
}


//------------------------------------------------------------------------------------------------
bool DevConsole::Command_Test( EventArgs& args )
{
	UNUSED( args );
	if ( g_engine && g_engine->m_devConsole )
	{
		int a = args.GetValue( "a", 0 );
		int b = args.GetValue( "b", 0 );
		int result = g_engine->m_devConsole->AddTwoInts( a, b );
		g_engine->m_devConsole->AddLine( Rgba8::INFO_MAJOR, "Result of AddTwoInts( " + std::to_string( a ) + ", " + std::to_string( b ) + " ) = " + std::to_string( result ) );
	}
	return false;
}


//------------------------------------------------------------------------------------------------
int DevConsole::AddTwoInts( int a, int b ) const
{
	return a + b;
}


//------------------------------------------------------------------------------------------------
void DevConsole::InsertCharacterAtInsertionPoint( char character )
{
	std::string currentCommand = g_gameConfigBlackboard.GetValue( "currentCommand", "" );
	if ( m_insertionPointPosition < 0 )
	{
		m_insertionPointPosition = 0;
	}
	if ( m_insertionPointPosition > ( int ) currentCommand.size() )
	{
		m_insertionPointPosition = ( int ) currentCommand.size();
	}

	currentCommand.insert( m_insertionPointPosition, 1, character );
	g_gameConfigBlackboard.SetValue( "currentCommand", currentCommand );
	m_insertionPointPosition ++;
}


//------------------------------------------------------------------------------------------------
std::string DevConsole::GetPreviousCommand( int offsetFromCurrent ) const
{
	int commandHistoryIndex = ( int ) m_commandHistory.size() - 1 - offsetFromCurrent;
	if ( commandHistoryIndex >= 0 && commandHistoryIndex < ( int ) m_commandHistory.size() )
	{
		return m_commandHistory[commandHistoryIndex];
	}
	else
	{
		return "";
	}
}


//------------------------------------------------------------------------------------------------
std::string DevConsole::GetNextCommand( int offsetFromCurrent ) const
{
	int commandHistoryIndex = ( int ) m_commandHistory.size() - 1 - offsetFromCurrent;
	if ( commandHistoryIndex >= 0 && commandHistoryIndex < ( int ) m_commandHistory.size() )
	{
		return m_commandHistory[commandHistoryIndex];
	}
	else
	{
		return "";
	}
}