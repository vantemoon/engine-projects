#pragma once


//-----------------------------------------------------------------------------------------------
struct ID3D11Buffer;


//-----------------------------------------------------------------------------------------------
class ConstantBuffer
{
	friend class Renderer;

public:
	ConstantBuffer( size_t size );
	ConstantBuffer( ConstantBuffer const& copy ) = delete;
	virtual ~ConstantBuffer();

private:
	size_t m_size = 0;
	ID3D11Buffer* m_buffer = nullptr;
};