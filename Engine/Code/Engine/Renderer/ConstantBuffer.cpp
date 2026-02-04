#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <d3d11.h>


//-----------------------------------------------------------------------------------------------
ConstantBuffer::ConstantBuffer( size_t size )
	: m_size( size )
{
	// DO NOTHING
}

ConstantBuffer::~ConstantBuffer()
{
	DX_SAFE_RELEASE( m_buffer );
}