#pragma once
#include <string>


//-----------------------------------------------------------------------------------------------
struct ShaderConfig
{
	std::string m_name;
	std::string m_vertexEntryPoint = "VertexMain";
	std::string m_pixelEntryPoint = "PixelMain";
};


//-----------------------------------------------------------------------------------------------
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;


//-----------------------------------------------------------------------------------------------
class Shader
{
	friend class Renderer;

public:
	Shader( ShaderConfig const& config );
	Shader( Shader const& copy ) = delete;
	~Shader();

	const std::string& GetName() const;

private:
	ShaderConfig m_config;
	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
	ID3D11InputLayout* m_inputLayout = nullptr;
};