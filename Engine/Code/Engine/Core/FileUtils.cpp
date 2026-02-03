#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"


//-----------------------------------------------------------------------------------------------
int FileReadToBuffer( std::vector<uint8_t>& out_buffer, std::string const& filename )
{
	FILE* file = nullptr;
	if ( fopen_s( &file, filename.c_str(), "rb" ) != 0 )
	{
		ERROR_AND_DIE( Stringf( "File could not be opened: %s", filename.c_str() ) );
	}

	if ( fseek( file, 0, SEEK_END ) != 0 )
	{
		ERROR_AND_DIE( Stringf( "Failed to seek to end of file: %s", filename.c_str() ) );
	}

	long fileSize = ftell( file );
	if ( fileSize < 0 )
	{
		ERROR_AND_DIE( Stringf( "Failed to get file size: %s", filename.c_str() ) );
	}

	out_buffer.resize( static_cast<size_t>( fileSize ) );

	if ( fseek( file, 0, SEEK_SET ) != 0 )
	{
		ERROR_AND_DIE( Stringf( "Failed to seek to start of file: %s", filename.c_str() ) );
	}

	size_t bytesRead = fread( out_buffer.data(), 1, static_cast<size_t>( fileSize ), file );
	if ( bytesRead != static_cast<size_t>( fileSize ) )
	{
		ERROR_AND_DIE( Stringf( "Failed to read entire file: %s", filename.c_str() ) );
	}

	fclose( file );
	return 0;
}


//-----------------------------------------------------------------------------------------------
int FileReadToString( std::string& out_string, std::string const& filename )
{
	std::vector<uint8_t> buffer;
	FileReadToBuffer( buffer, filename );

	buffer.push_back( '\0' );
	out_string = std::string( reinterpret_cast<char*>( buffer.data() ) );

	return 0;
}