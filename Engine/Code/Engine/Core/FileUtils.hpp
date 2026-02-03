#pragma once
#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
int FileReadToBuffer( std::vector<uint8_t>& out_buffer, std::string const& filename );
int FileReadToString( std::string& out_string, std::string const& filename );