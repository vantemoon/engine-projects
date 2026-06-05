#include "Game/ChessPieceDefinition.hpp"


//-----------------------------------------------------------------------------------------------
std::map<std::string, ChessPieceDefinition*> ChessPieceDefinition::s_definitions;


//-----------------------------------------------------------------------------------------------
void ChessPieceDefinition::InitializeDefinitions()
{
	s_definitions["PAWN"] = new ChessPieceDefinition();
	s_definitions["PAWN"]->m_type = ChessPieceType::PAWN;
	s_definitions["PAWN"]->m_symbol = 'p';

	s_definitions["ROOK"] = new ChessPieceDefinition();
	s_definitions["ROOK"]->m_type = ChessPieceType::ROOK;
	s_definitions["ROOK"]->m_symbol = 'r';

	s_definitions["KNIGHT"] = new ChessPieceDefinition();
	s_definitions["KNIGHT"]->m_type = ChessPieceType::KNIGHT;
	s_definitions["KNIGHT"]->m_symbol = 'n';

	s_definitions["BISHOP"] = new ChessPieceDefinition();
	s_definitions["BISHOP"]->m_type = ChessPieceType::BISHOP;
	s_definitions["BISHOP"]->m_symbol = 'b';

	s_definitions["QUEEN"] = new ChessPieceDefinition();
	s_definitions["QUEEN"]->m_type = ChessPieceType::QUEEN;
	s_definitions["QUEEN"]->m_symbol = 'q';

	s_definitions["KING"] = new ChessPieceDefinition();
	s_definitions["KING"]->m_type = ChessPieceType::KING;
	s_definitions["KING"]->m_symbol = 'k';
}


//-----------------------------------------------------------------------------------------------
void ChessPieceDefinition::ClearDefinitions()
{
	for ( auto& pair : s_definitions )
	{
		delete pair.second;
	}
	s_definitions.clear();
}


//-----------------------------------------------------------------------------------------------
ChessPieceDefinition const* ChessPieceDefinition::GetDefinitionByType( ChessPieceType type )
{
	for ( auto& pair : s_definitions )
	{
		if ( pair.second->m_type == type )
		{
			return pair.second;
		}
	}
	return nullptr;
}