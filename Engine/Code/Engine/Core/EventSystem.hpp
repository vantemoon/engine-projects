#pragma once
#include "Engine/Core/NamedStrings.hpp"
#include <algorithm>
#include <string>
#include <vector>
#include <map>


//-----------------------------------------------------------------------------------------------
struct EventSystemConfig
{
	bool m_isEnabled = true;
};


//-----------------------------------------------------------------------------------------------
typedef NamedStrings EventArgs;
typedef bool ( *EventCallbackFunction )( EventArgs& );
typedef std::vector<EventCallbackFunction> EventCallbackFunctionList;
typedef std::vector<std::string> EventRequiredArgsList;


//-----------------------------------------------------------------------------------------------
struct cmpCaseInsensitive
{
	bool operator() ( std::string const& a, std::string const& b ) const
	{
		std::string lowerA = a;
		std::string lowerB = b;
		std::transform( lowerA.begin(), lowerA.end(), lowerA.begin(),
			[]( unsigned char c ) { return static_cast<char>( std::tolower( c ) ); } );
		std::transform( lowerB.begin(), lowerB.end(), lowerB.begin(),
			[]( unsigned char c ) { return static_cast<char>( std::tolower( c ) ); } );
		return lowerA < lowerB;
	}
};


//-----------------------------------------------------------------------------------------------
class EventSystem
{
public:
	EventSystem( EventSystemConfig const& config );
	~EventSystem() = default;

	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void SubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunction funtionPtr );
	void UnsubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunction funtionPtr );
	void FireEvent( std::string const& eventName, EventArgs& args );
	void FireEvent( std::string const& eventName );

	std::vector<std::string> GetEventNames( bool includeRequiredArgs = false ) const;
	void SetEventRequiredArgs( std::string const& eventName, EventRequiredArgsList const& requiredArgs );
	EventRequiredArgsList GetEventRequiredArgs( std::string const& eventName ) const;

protected:
	EventSystemConfig														m_config;
	std::map<std::string, EventCallbackFunctionList, cmpCaseInsensitive>	m_subscriptionListsByEventName;
	std::map<std::string, EventRequiredArgsList, cmpCaseInsensitive>		m_requiredArgsByEventName;
};


//-----------------------------------------------------------------------------------------------
// Standalone global-namespace helper functions
void SubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunction funtionPtr );
void UnsubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunction funtionPtr );
void FireEvent( std::string const& eventName, EventArgs& args );
void FireEvent( std::string const& eventName );