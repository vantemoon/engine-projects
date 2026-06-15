#pragma once
#include "Game/ChessPieceDefinition.hpp"
#include "Game/ChessObject.hpp"
#include "Engine/Math/IntVec2.hpp"


//-----------------------------------------------------------------------------------------------
class ChessPiece : public ChessObject
{
public:
	ChessPiece( ChessPieceDefinition const& definition, bool isWhite = true, IntVec2 boardCoord = IntVec2( -1, -1 ) );
	~ChessPiece();

	void Update() override;
	void Render() const override;

	void StartMovementAnimation( IntVec2 const& from, IntVec2 const& to );
	bool IsMovementAnimationComplete() const;
	Vec3 GetCurrentPosition() const;

public:
	ChessPieceDefinition const* m_definition = nullptr;

	bool m_isWhite = true;
	bool m_isCaptured = false;
	bool m_hasMoved = false;

	IntVec2 m_boardCoords = IntVec2( -1, -1 );

	bool m_hasPendingPromotion = false;
	ChessPieceDefinition const* m_pendingPromotionDefinition = nullptr;

private:
	bool m_isMoving = false;
	float m_moveAgeSeconds = 0.f;
	float m_moveDurationSeconds = 0.35f;

	IntVec2 m_moveStartCoords = IntVec2( -1, -1 );
	IntVec2 m_moveEndCoords = IntVec2( -1, -1 );
};