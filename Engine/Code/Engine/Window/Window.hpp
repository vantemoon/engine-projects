#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include <string>


//-----------------------------------------------------------------------------------------------
struct WindowConfig
{
	bool			m_isEnabled = true;
	float			m_clientAspect = 2.f;
	std::string		m_windowTitle = "Default Window"; // Default title
};


//-----------------------------------------------------------------------------------------------
class Window
{
public:
	Window( WindowConfig const& config);
	~Window();
	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	Vec2 GetNormalizedMouseUV() const;
	void* GetHwnd() const;
	IntVec2 GetClientDimensions() const;

	WindowConfig	m_config;
	void*			m_displayDeviceContext = 0;
	void*			m_windowHandle = 0;
	IntVec2         m_clientDimensions = IntVec2::ZERO;

private:
	void CreateOSWindow();
	void RunMessagePump();
};