#pragma once


//-----------------------------------------------------------------------------------------------
struct WindowConfig
{
	bool m_isEnabled = true;
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

	WindowConfig m_config;
};