#pragma once
#include "Engine/Math/Vec2.hpp"
#include <string>


//-----------------------------------------------------------------------------------------------
struct WindowConfig
{
	bool			m_isEnabled = true;
	float			m_clientAspect = (16.0f / 9.0f);
	std::string		m_windowTitle = "Math Visual Tests";
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

	WindowConfig	m_config;
	void*			m_displayDeviceContext = 0;
	void*			m_windowHandle = 0;

private:
	void CreateOSWindow();
	void RunMessagePump();
};