#pragma once


//-----------------------------------------------------------------------------------------------
class ChessObject
{
public:
	ChessObject();
	virtual ~ChessObject();

	virtual void Update() = 0;
	virtual void Render() const = 0;
};