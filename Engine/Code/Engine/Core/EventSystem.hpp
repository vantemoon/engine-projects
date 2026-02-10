#pragma once
#include "Engine/Core/NamedStrings.hpp"
#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
struct EventSystemConfig
{
	bool m_isEnabled = true;
};


//-----------------------------------------------------------------------------------------------
typedef NamedStrings EventArgs;
typedef bool ( *EventCallbackFunction )( EventArgs& );
typedef std::vector<EventCallbackFunction> EventCallbackFunctionList;


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

	std::vector<std::string> GetEventNames() const;

protected:
	EventSystemConfig								 m_config;
	std::map<std::string, EventCallbackFunctionList> m_subscriptionListsByEventName;
};


//-----------------------------------------------------------------------------------------------
// Standalone global-namespace helper functions
void SubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunction funtionPtr );
void UnsubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunction funtionPtr );
void FireEvent( std::string const& eventName, EventArgs& args );
void FireEvent( std::string const& eventName );