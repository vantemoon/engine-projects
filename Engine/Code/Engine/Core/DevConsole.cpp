#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include <algorithm>


//-----------------------------------------------------------------------------------------------
const Rgba8 DevConsole::ERROR = Rgba8( 255, 0, 0 );
const Rgba8 DevConsole::WARNING = Rgba8( 255, 255, 0 );
const Rgba8 DevConsole::INFO_MAJOR = Rgba8( 0, 255, 0 );
const Rgba8 DevConsole::INFO_MINOR = Rgba8( 0, 0, 255 );
const Rgba8 DevConsole::DEFAULT_TEXT_COLOR = Rgba8::WHITE;


//-----------------------------------------------------------------------------------------------
DevConsole::DevConsole( DevConsoleConfig const& config )
	: m_config( config )
{
}


//-----------------------------------------------------------------------------------------------
DevConsole::~DevConsole()
{
}


//-----------------------------------------------------------------------------------------------
void DevConsole::Startup()
{
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
}


//-----------------------------------------------------------------------------------------------
void DevConsole::BeginFrame()
{
	m_frameNumber ++;
}


//-----------------------------------------------------------------------------------------------
void DevConsole::EndFrame()
{
}


//-----------------------------------------------------------------------------------------------
void DevConsole::Execute( std::string const& consoleCommandText )
{
	Strings splitText = SplitStringOnDelimiter( consoleCommandText, ' ' );
	if ( splitText.size() == 0 )
	{
		AddLine( INFO_MINOR, "" );
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
			AddLine( WARNING, "Unknown command: " + command );
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
				AddLine( WARNING, "Command requires arguments: " + requiredArgsString );
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
				AddLine( WARNING, "Invalid argument: " + arg );
				EventRequiredArgsList requiredArgs = g_engine->m_eventSystem->GetEventRequiredArgs( command );
				if ( !requiredArgs.empty() )
				{
					std::string requiredArgsString;
					for ( std::string const& requiredArg : requiredArgs )
					{
						requiredArgsString += requiredArg + " ";
					}
					AddLine( WARNING, "Command requires arguments: " + requiredArgsString );
				}
				return;
			}
		}
		AddLine( INFO_MINOR, "Executed command: " + consoleCommandText );
		g_engine->m_eventSystem->FireEvent( command, args );
	}
}


//-----------------------------------------------------------------------------------------------
void DevConsole::AddLine( Rgba8 const& color, std::string const& text )
{
	double t = GetCurrentTimeSeconds();
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
void DevConsole::Render( AABB2 const& bound ) const
{
	if ( m_mode == DevConsoleMode::HIDDEN )
	{
		return;
	}
	if ( m_mode == DevConsoleMode::OPEN_FULL )
	{
		BitmapFont* font = g_engine->m_renderer->CreateOrGetBitmapFontFromFile( m_config.m_consoleFont.c_str() );
		Render_OpenFull( bound, *font, 1.f );
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

	AddVertsForAABB2D( backgroundVerts, bound, Rgba8( 0, 0, 0, 200 ) );

	float cellHeight = 20.f;
	float yOffset = bound.m_mins.y + 2.f + cellHeight;
	for ( int lineIndex = static_cast< int >( m_lines.size() ) - 1; lineIndex >= 0; -- lineIndex )
	{
		if ( yOffset + cellHeight > bound.m_maxs.y )
		{
			break;
		}

		DevConsoleLine const& line = m_lines[lineIndex];
		Vec2 textMins = Vec2( bound.m_mins.x + 5.f, yOffset );
		font.AddVertsForText2D( textVerts, textMins, cellHeight, line.second, line.first, fontAspect );
		yOffset += cellHeight;
	}

	std::string currentCommand = g_gameConfigBlackboard.GetValue( "currentCommand", "" );
	Vec2 textMins = Vec2( bound.m_mins.x + 5.f, 2.f );
	font.AddVertsForText2D( textVerts, textMins, cellHeight, currentCommand, DEFAULT_TEXT_COLOR, fontAspect );

	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( static_cast< int >( backgroundVerts.size() ), backgroundVerts.data() );

	g_engine->m_renderer->BindTexture( &font.GetTexture() );
	g_engine->m_renderer->DrawVertexArray( static_cast< int >( textVerts.size() ), textVerts.data() );
}


//------------------------------------------------------------------------------------------------
bool DevConsole::Command_KeyPressed( EventArgs& args )
{
	if ( g_engine && g_engine->m_devConsole )
	{
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
					g_engine->m_devConsole->AddLine( DEFAULT_TEXT_COLOR, g_gameConfigBlackboard.GetValue( "currentCommand", "" ) );
					std::string currentCommand = g_gameConfigBlackboard.GetValue( "currentCommand", "" );
					g_engine->m_devConsole->Execute( currentCommand );
					g_gameConfigBlackboard.SetValue( "currentCommand", "" );
				}
				return true;
			}

			if ( key == KEYCODE_ESCAPE )
			{
				if ( !g_gameConfigBlackboard.GetValue( "currentCommand", "" ).empty() )
				{
					g_gameConfigBlackboard.SetValue( "currentCommand", "" );
				}
				else
				{
					g_engine->m_devConsole->SetMode( DevConsoleMode::HIDDEN );
				}
				return true;
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
	UNUSED( args );
	if ( g_engine && g_engine->m_devConsole )
	{
		if ( g_engine->m_devConsole->GetMode() != DevConsoleMode::HIDDEN )
		{
			std::string characterText = args.GetValue( "Character", "" );
			int character = static_cast<int>( characterText[0] );
			if ( character >= 32 && character <= 126 && characterText != "`" && characterText != "~" )
			{
				std::string currentCommand = g_gameConfigBlackboard.GetValue( "currentCommand", "" );
				currentCommand += characterText;
				g_gameConfigBlackboard.SetValue( "currentCommand", currentCommand );
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
		g_engine->m_devConsole->AddLine( INFO_MAJOR, "Registered commands:" );
		for ( std::string const& eventName : eventNames )
		{
			g_engine->m_devConsole->AddLine( DEFAULT_TEXT_COLOR, eventName );
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
		g_engine->m_devConsole->AddLine( INFO_MAJOR, "Result of AddTwoInts( " + std::to_string( a ) + ", " + std::to_string( b ) + " ) = " + std::to_string( result ) );
	}
	return false;
}


//------------------------------------------------------------------------------------------------
int DevConsole::AddTwoInts( int a, int b ) const
{
	return a + b;
}