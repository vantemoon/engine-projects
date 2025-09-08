//-----------------------------------------------------------------------------------------------
// UnitTests_MP1A02.hpp
//
#include "Game/GameCommon.hpp"
#include "Game/UnitTests_MP1A01.hpp" // Uses any or all #defines and dependencies from MP1-A01


//-----------------------------------------------------------------------------------------------
void RunTests_MP1A02();


//-----------------------------------------------------------------------------------------------
// YOU MAY COMMENT THESE OUT TEMPORARILY to disable certain test sets while you work.
// For every assignment submission, all test sets must be enabled.
//
#define ENABLE_TestSet_MP1A02_MathUtils_Angles
#define ENABLE_TestSet_MP1A02_MathUtils_Queries
#define ENABLE_TestSet_MP1A02_Vec2_Methods
#define ENABLE_TestSet_MP1A02_Vec3_Basics
#define ENABLE_TestSet_MP1A02_Vec3_Methods
#define ENABLE_TestSet_MP1A02_RandomNumberGenerator_Ints
#define ENABLE_TestSet_MP1A02_RandomNumberGenerator_Floats
//#define ENABLE_TestSet_MP1A02_MathUtils_Transforms


//-----------------------------------------------------------------------------------------------
// YOU MAY CHANGE any of these #includes to match your engine filenames (e.g. Vector2D.hpp, etc.)
//
#include "Engine/Math/Vec3.hpp"						// #include for your Vec3 struct/class
#include "Engine/Math/MathUtils.hpp"				// #include for your math utilities header
#include "Engine/Math/RandomNumberGenerator.hpp"	// #include for your RNG class header


//-----------------------------------------------------------------------------------------------
// YOU MAY CHANGE the "Your Name" column of these #defines to match your own classes / functions
//
//		MathUnitTests name					Your name
//		~~~~~~~~~~~~~~~~~~~~~~				~~~~~~~~~~~~~~~~~~~~~~
#define Vector3Class						Vec3
#define Vertex3DPCUClass					Vertex
#define RngClass							RandomNumberGenerator

#define Function_ConvertDegreesToRadians	ConvertDegreesToRadians
#define Function_ConvertRadiansToDegrees	ConvertRadiansToDegrees
#define Function_CosDegrees					CosDegrees
#define Function_SinDegrees					SinDegrees
#define Function_Atan2Degrees				Atan2Degrees

#define Function_GetDistance2D				GetDistance2D
#define Function_GetDistanceSquared2D		GetDistanceSquared2D

#define Function_GetDistance3D				GetDistance3D
#define Function_GetDistanceXY3D			GetDistanceXY3D
#define Function_GetDistanceSquared3D		GetDistanceSquared3D
#define Function_GetDistanceXYSquared3D		GetDistanceXYSquared3D

#define Function_DoDiscsOverlap				DoDiscsOverlap
#define Function_DoSpheresOverlap			DoSpheresOverlap

#define Vec2_MakeFromPolarDegrees			MakeFromPolarDegrees
#define Vec2_MakeFromPolarRadians			MakeFromPolarRadians

#define Vec2_GetLength						GetLength
#define Vec2_GetLengthSquared				GetLengthSquared
#define Vec2_GetOrientationDegrees			GetOrientationDegrees
#define Vec2_GetOrientationRadians			GetOrientationRadians

#define Vec2_GetRotated90Degrees			GetRotatedBy90Degrees
#define Vec2_GetRotatedMinus90Degrees		GetRotatedByMinus90Degrees
#define Vec2_GetRotatedDegrees				GetRotatedByDegrees
#define Vec2_GetRotatedRadians				GetRotatedByRadians
#define Vec2_SetOrientationDegrees			SetOrientationDegrees
#define Vec2_SetOrientationRadians			SetOrientationRadians
#define Vec2_SetPolarDegrees				SetPolarDegrees
#define Vec2_SetPolarRadians				SetPolarRadians
#define Vec2_RotateDegrees					RotateDegrees
#define Vec2_RotateRadians					RotateRadians
#define Vec2_Rotate90Degrees				Rotate90Degrees
#define Vec2_RotateMinus90Degrees			RotateMinus90Degrees

#define Vec2_GetClamped						GetClamped
#define Vec2_GetNormalized					GetNormalized
#define Vec2_SetLength						SetLength
#define Vec2_ClampLength					ClampLength
#define Vec2_Normalize						Normalize
#define Vec2_NormalizeAndGetLength			NormalizeAndGetPreviousLength

#define Vec3_GetLength						GetLength
#define Vec3_GetLengthXY					GetLengthXY
#define Vec3_GetLengthSquared				GetLengthSquared
#define Vec3_GetLengthXYSquared				GetLengthXYSquared
#define Vec3_GetOrientationAboutZDegrees	GetOrientationAboutZDegrees
#define Vec3_GetOrientationAboutZRadians	GetOrientationAboutZRadians
#define Vec3_GetRotatedAboutZDegrees		GetRotatedAboutZDegrees
#define Vec3_GetRotatedAboutZRadians		GetRotatedAboutZRadians

#define Rng_RollRandomIntLessThan			RollRandomIntLessThan
#define Rng_RollRandomIntInRange			RollRandomIntInRange
#define Rng_RollRandomFloatZeroToOne		RollRandomFloatZeroToOne
#define Rng_RollRandomFloatInRange			RollRandomFloatInRange

#define Function_TransformPosition2D		TransformPosition2D
#define Function_TransformPositionXY3D		TransformPositionXY3D



