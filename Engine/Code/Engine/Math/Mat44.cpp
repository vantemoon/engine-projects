#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
Mat44::Mat44()
{
	// Initialize to identity matrix
	m_values[Ix] = 1.0f; m_values[Iy] = 0.0f; m_values[Iz] = 0.0f; m_values[Iw] = 0.0f;
	m_values[Jx] = 0.0f; m_values[Jy] = 1.0f; m_values[Jz] = 0.0f; m_values[Jw] = 0.0f;
	m_values[Kx] = 0.0f; m_values[Ky] = 0.0f; m_values[Kz] = 1.0f; m_values[Kw] = 0.0f;
	m_values[Tx] = 0.0f; m_values[Ty] = 0.0f; m_values[Tz] = 0.0f; m_values[Tw] = 1.0f;
}


//-----------------------------------------------------------------------------------------------
Mat44::Mat44( Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translationXY )
{
	m_values[Ix] = iBasis2D.x; m_values[Iy] = iBasis2D.y; m_values[Iz] = 0.0f;      m_values[Iw] = 0.0f;
	m_values[Jx] = jBasis2D.x; m_values[Jy] = jBasis2D.y; m_values[Jz] = 0.0f;      m_values[Jw] = 0.0f;
	m_values[Kx] = 0.0f;       m_values[Ky] = 0.0f;       m_values[Kz] = 1.0f;      m_values[Kw] = 0.0f;
	m_values[Tx] = translationXY.x; m_values[Ty] = translationXY.y; m_values[Tz] = 0.0f; m_values[Tw] = 1.0f;
}


//-----------------------------------------------------------------------------------------------
Mat44::Mat44( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translation3D )
{
	m_values[Ix] = iBasis3D.x; m_values[Iy] = iBasis3D.y; m_values[Iz] = iBasis3D.z; m_values[Iw] = 0.0f;
	m_values[Jx] = jBasis3D.x; m_values[Jy] = jBasis3D.y; m_values[Jz] = jBasis3D.z; m_values[Jw] = 0.0f;
	m_values[Kx] = kBasis3D.x; m_values[Ky] = kBasis3D.y; m_values[Kz] = kBasis3D.z; m_values[Kw] = 0.0f;
	m_values[Tx] = translation3D.x; m_values[Ty] = translation3D.y; m_values[Tz] = translation3D.z; m_values[Tw] = 1.0f;
}


//-----------------------------------------------------------------------------------------------
Mat44::Mat44( Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D )
{
	m_values[Ix] = iBasis4D.x; m_values[Iy] = iBasis4D.y; m_values[Iz] = iBasis4D.z; m_values[Iw] = iBasis4D.w;
	m_values[Jx] = jBasis4D.x; m_values[Jy] = jBasis4D.y; m_values[Jz] = jBasis4D.z; m_values[Jw] = jBasis4D.w;
	m_values[Kx] = kBasis4D.x; m_values[Ky] = kBasis4D.y; m_values[Kz] = kBasis4D.z; m_values[Kw] = kBasis4D.w;
	m_values[Tx] = translation4D.x; m_values[Ty] = translation4D.y; m_values[Tz] = translation4D.z; m_values[Tw] = translation4D.w;
}


//-----------------------------------------------------------------------------------------------
Mat44::Mat44( float const* sixteenValuesBasisMajor )
{
	for ( int index = 0; index < 16; ++index )
	{
		m_values[index] = sixteenValuesBasisMajor[index];
	}
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeTranslation2D( Vec2 const& translationXY )
{
	Mat44 translationMat;
	translationMat.m_values[Tx] = translationXY.x;
	translationMat.m_values[Ty] = translationXY.y;
	return translationMat;
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeTranslation3D( Vec3 const& translation3D )
{
	Mat44 translationMat;
	translationMat.m_values[Tx] = translation3D.x;
	translationMat.m_values[Ty] = translation3D.y;
	translationMat.m_values[Tz] = translation3D.z;
	return translationMat;
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeUniformScale2D( float uniformScaleXY )
{
	Mat44 scaleMat;
	scaleMat.m_values[Ix] = uniformScaleXY;
	scaleMat.m_values[Jy] = uniformScaleXY;
	return scaleMat;
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeUniformScale3D( float uniformScaleXYZ )
{
	Mat44 scaleMat;
	scaleMat.m_values[Ix] = uniformScaleXYZ;
	scaleMat.m_values[Jy] = uniformScaleXYZ;
	scaleMat.m_values[Kz] = uniformScaleXYZ;
	return scaleMat;
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeNonUniformScale2D( Vec2 const& nonUniformScaleXY )
{
	Mat44 scaleMat;
	scaleMat.m_values[Ix] = nonUniformScaleXY.x;
	scaleMat.m_values[Jy] = nonUniformScaleXY.y;
	return scaleMat;
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeNonUniformScale3D( Vec3 const& nonUniformScaleXYZ )
{
	Mat44 scaleMat;
	scaleMat.m_values[Ix] = nonUniformScaleXYZ.x;
	scaleMat.m_values[Jy] = nonUniformScaleXYZ.y;
	scaleMat.m_values[Kz] = nonUniformScaleXYZ.z;
	return scaleMat;
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeZRotationDegrees( float rotationDegreesAboutZ )
{
	Mat44 rotationMat;
	float cosTheta = CosDegrees( rotationDegreesAboutZ );
	float sinTheta = SinDegrees( rotationDegreesAboutZ );
	rotationMat.m_values[Ix] = cosTheta;
	rotationMat.m_values[Iy] = sinTheta;
	rotationMat.m_values[Jx] = -sinTheta;
	rotationMat.m_values[Jy] = cosTheta;
	return rotationMat;
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeYRotationDegrees( float rotationDegreesAboutY )
{
	Mat44 rotationMat;
	float cosTheta = CosDegrees( rotationDegreesAboutY );
	float sinTheta = SinDegrees( rotationDegreesAboutY );
	rotationMat.m_values[Ix] = cosTheta;
	rotationMat.m_values[Iz] = -sinTheta;
	rotationMat.m_values[Kx] = sinTheta;
	rotationMat.m_values[Kz] = cosTheta;
	return rotationMat;
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::MakeXRotationDegrees( float rotationDegreesAboutX )
{
	Mat44 rotationMat;
	float cosTheta = CosDegrees( rotationDegreesAboutX );
	float sinTheta = SinDegrees( rotationDegreesAboutX );
	rotationMat.m_values[Jy] = cosTheta;
	rotationMat.m_values[Jz] = sinTheta;
	rotationMat.m_values[Ky] = -sinTheta;
	rotationMat.m_values[Kz] = cosTheta;
	return rotationMat;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::TransformVectorQuantity2D( Vec2 const& vectorQuantityXY ) const
{
	Vec2 result;
	result.x = m_values[Ix] * vectorQuantityXY.x + m_values[Jx] * vectorQuantityXY.y;
	result.y = m_values[Iy] * vectorQuantityXY.x + m_values[Jy] * vectorQuantityXY.y;
	return result;
}


//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::TransformVectorQuantity3D( Vec3 const& vectorQuantityXYZ ) const
{
	Vec3 result;
	result.x = m_values[Ix] * vectorQuantityXYZ.x + m_values[Jx] * vectorQuantityXYZ.y + m_values[Kx] * vectorQuantityXYZ.z;
	result.y = m_values[Iy] * vectorQuantityXYZ.x + m_values[Jy] * vectorQuantityXYZ.y + m_values[Ky] * vectorQuantityXYZ.z;
	result.z = m_values[Iz] * vectorQuantityXYZ.x + m_values[Jz] * vectorQuantityXYZ.y + m_values[Kz] * vectorQuantityXYZ.z;
	return result;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::TransformPosition2D( Vec2 const& positionXY ) const
{
	Vec2 result;
	result.x = m_values[Ix] * positionXY.x + m_values[Jx] * positionXY.y + m_values[Tx];
	result.y = m_values[Iy] * positionXY.x + m_values[Jy] * positionXY.y + m_values[Ty];
	return result;
}


//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::TransformPosition3D( Vec3 const& positionXYZ ) const
{
	Vec3 result;
	result.x = m_values[Ix] * positionXYZ.x + m_values[Jx] * positionXYZ.y + m_values[Kx] * positionXYZ.z + m_values[Tx];
	result.y = m_values[Iy] * positionXYZ.x + m_values[Jy] * positionXYZ.y + m_values[Ky] * positionXYZ.z + m_values[Ty];
	result.z = m_values[Iz] * positionXYZ.x + m_values[Jz] * positionXYZ.y + m_values[Kz] * positionXYZ.z + m_values[Tz];
	return result;
}


//-----------------------------------------------------------------------------------------------
Vec4 const Mat44::TransformHomogeneous3D( Vec4 const& homogeneousPoint3D ) const
{
	Vec4 result;
	result.x = m_values[Ix] * homogeneousPoint3D.x + m_values[Jx] * homogeneousPoint3D.y + m_values[Kx] * homogeneousPoint3D.z + m_values[Tx] * homogeneousPoint3D.w;
	result.y = m_values[Iy] * homogeneousPoint3D.x + m_values[Jy] * homogeneousPoint3D.y + m_values[Ky] * homogeneousPoint3D.z + m_values[Ty] * homogeneousPoint3D.w;
	result.z = m_values[Iz] * homogeneousPoint3D.x + m_values[Jz] * homogeneousPoint3D.y + m_values[Kz] * homogeneousPoint3D.z + m_values[Tz] * homogeneousPoint3D.w;
	result.w = m_values[Iw] * homogeneousPoint3D.x + m_values[Jw] * homogeneousPoint3D.y + m_values[Kw] * homogeneousPoint3D.z + m_values[Tw] * homogeneousPoint3D.w;
	return result;
}


//-----------------------------------------------------------------------------------------------
float* Mat44::GetAsFloatArray()
{
	return m_values;
}


//-----------------------------------------------------------------------------------------------
float const* Mat44::GetAsFloatArray() const
{
	return m_values;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::GetIBasis2D() const
{
	return Vec2( m_values[Ix], m_values[Iy] );
}


//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::GetJBasis2D() const
{
	return Vec2( m_values[Jx], m_values[Jy] );
}


//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::GetTranslation2D() const
{
	return Vec2( m_values[Tx], m_values[Ty] );
}


//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::GetIBasis3D() const
{
	return Vec3( m_values[Ix], m_values[Iy], m_values[Iz] );
}


//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::GetJBasis3D() const
{
	return Vec3( m_values[Jx], m_values[Jy], m_values[Jz] );
}


//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::GetKBasis3D() const
{
	return Vec3( m_values[Kx], m_values[Ky], m_values[Kz] );
}


//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::GetTranslation3D() const
{
	return Vec3( m_values[Tx], m_values[Ty], m_values[Tz] );
}


//-----------------------------------------------------------------------------------------------
Vec4 const Mat44::GetIBasis4D() const
{
	return Vec4( m_values[Ix], m_values[Iy], m_values[Iz], m_values[Iw] );
}


//-----------------------------------------------------------------------------------------------
Vec4 const Mat44::GetJBasis4D() const
{
	return Vec4( m_values[Jx], m_values[Jy], m_values[Jz], m_values[Jw] );
}


//-----------------------------------------------------------------------------------------------
Vec4 const Mat44::GetKBasis4D() const
{
	return Vec4( m_values[Kx], m_values[Ky], m_values[Kz], m_values[Kw] );
}


//-----------------------------------------------------------------------------------------------
Vec4 const Mat44::GetTranslation4D() const
{
	return Vec4( m_values[Tx], m_values[Ty], m_values[Tz], m_values[Tw] );
}


//-----------------------------------------------------------------------------------------------
void Mat44::SetTranslation2D( Vec2 const& translationXY )
{
	m_values[Tx] = translationXY.x;
	m_values[Ty] = translationXY.y;
	m_values[Tz] = 0.0f;
	m_values[Tw] = 1.0f;
}


//-----------------------------------------------------------------------------------------------
void Mat44::SetTranslation3D( Vec3 const& translation3D )
{
	m_values[Tx] = translation3D.x;
	m_values[Ty] = translation3D.y;
	m_values[Tz] = translation3D.z;
	m_values[Tw] = 1.0f;
}


//-----------------------------------------------------------------------------------------------
void Mat44::SetIJ2D( Vec2 const& iBasis2D, Vec2 const& jBasis2D )
{
	m_values[Ix] = iBasis2D.x;
	m_values[Iy] = iBasis2D.y;
	m_values[Iz] = 0.0f;
	m_values[Iw] = 0.0f;
	m_values[Jx] = jBasis2D.x;
	m_values[Jy] = jBasis2D.y;
	m_values[Jz] = 0.0f;
	m_values[Jw] = 0.0f;
}


//-----------------------------------------------------------------------------------------------
void Mat44::SetIJT2D( Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translationXY )
{
	m_values[Ix] = iBasis2D.x;
	m_values[Iy] = iBasis2D.y;
	m_values[Iz] = 0.0f;
	m_values[Iw] = 0.0f;
	m_values[Jx] = jBasis2D.x;
	m_values[Jy] = jBasis2D.y;
	m_values[Jz] = 0.0f;
	m_values[Jw] = 0.0f;
	m_values[Tx] = translationXY.x;
	m_values[Ty] = translationXY.y;
	m_values[Tz] = 0.0f;
	m_values[Tw] = 1.0f;
}


//-----------------------------------------------------------------------------------------------
void Mat44::SetIJK3D( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D )
{
	m_values[Ix] = iBasis3D.x;
	m_values[Iy] = iBasis3D.y;
	m_values[Iz] = iBasis3D.z;
	m_values[Iw] = 0.0f;
	m_values[Jx] = jBasis3D.x;
	m_values[Jy] = jBasis3D.y;
	m_values[Jz] = jBasis3D.z;
	m_values[Jw] = 0.0f;
	m_values[Kx] = kBasis3D.x;
	m_values[Ky] = kBasis3D.y;
	m_values[Kz] = kBasis3D.z;
	m_values[Kw] = 0.0f;
}


//-----------------------------------------------------------------------------------------------
void Mat44::SetIJKT3D( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translation3D )
{
	m_values[Ix] = iBasis3D.x;
	m_values[Iy] = iBasis3D.y;
	m_values[Iz] = iBasis3D.z;
	m_values[Iw] = 0.0f;
	m_values[Jx] = jBasis3D.x;
	m_values[Jy] = jBasis3D.y;
	m_values[Jz] = jBasis3D.z;
	m_values[Jw] = 0.0f;
	m_values[Kx] = kBasis3D.x;
	m_values[Ky] = kBasis3D.y;
	m_values[Kz] = kBasis3D.z;
	m_values[Kw] = 0.0f;
	m_values[Tx] = translation3D.x;
	m_values[Ty] = translation3D.y;
	m_values[Tz] = translation3D.z;
	m_values[Tw] = 1.0f;
}


//-----------------------------------------------------------------------------------------------
void Mat44::SetIJKT4D( Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D )
{
	m_values[Ix] = iBasis4D.x;
	m_values[Iy] = iBasis4D.y;
	m_values[Iz] = iBasis4D.z;
	m_values[Iw] = iBasis4D.w;
	m_values[Jx] = jBasis4D.x;
	m_values[Jy] = jBasis4D.y;
	m_values[Jz] = jBasis4D.z;
	m_values[Jw] = jBasis4D.w;
	m_values[Kx] = kBasis4D.x;
	m_values[Ky] = kBasis4D.y;
	m_values[Kz] = kBasis4D.z;
	m_values[Kw] = kBasis4D.w;
	m_values[Tx] = translation4D.x;
	m_values[Ty] = translation4D.y;
	m_values[Tz] = translation4D.z;
	m_values[Tw] = translation4D.w;
}


//-----------------------------------------------------------------------------------------------
void Mat44::Append( Mat44 const& appendThis )
{
	Mat44 original = *this;
	m_values[Ix] = original.m_values[Ix] * appendThis.m_values[Ix] + original.m_values[Jx] * appendThis.m_values[Iy] + original.m_values[Kx] * appendThis.m_values[Iz] + original.m_values[Tx] * appendThis.m_values[Iw];
	m_values[Iy] = original.m_values[Iy] * appendThis.m_values[Ix] + original.m_values[Jy] * appendThis.m_values[Iy] + original.m_values[Ky] * appendThis.m_values[Iz] + original.m_values[Ty] * appendThis.m_values[Iw];
	m_values[Iz] = original.m_values[Iz] * appendThis.m_values[Ix] + original.m_values[Jz] * appendThis.m_values[Iy] + original.m_values[Kz] * appendThis.m_values[Iz] + original.m_values[Tz] * appendThis.m_values[Iw];
	m_values[Iw] = original.m_values[Iw] * appendThis.m_values[Ix] + original.m_values[Jw] * appendThis.m_values[Iy] + original.m_values[Kw] * appendThis.m_values[Iz] + original.m_values[Tw] * appendThis.m_values[Iw];
	m_values[Jx] = original.m_values[Ix] * appendThis.m_values[Jx] + original.m_values[Jx] * appendThis.m_values[Jy] + original.m_values[Kx] * appendThis.m_values[Jz] + original.m_values[Tx] * appendThis.m_values[Jw];
	m_values[Jy] = original.m_values[Iy] * appendThis.m_values[Jx] + original.m_values[Jy] * appendThis.m_values[Jy] + original.m_values[Ky] * appendThis.m_values[Jz] + original.m_values[Ty] * appendThis.m_values[Jw];
	m_values[Jz] = original.m_values[Iz] * appendThis.m_values[Jx] + original.m_values[Jz] * appendThis.m_values[Jy] + original.m_values[Kz] * appendThis.m_values[Jz] + original.m_values[Tz] * appendThis.m_values[Jw];
	m_values[Jw] = original.m_values[Iw] * appendThis.m_values[Jx] + original.m_values[Jw] * appendThis.m_values[Jy] + original.m_values[Kw] * appendThis.m_values[Jz] + original.m_values[Tw] * appendThis.m_values[Jw];
	m_values[Kx] = original.m_values[Ix] * appendThis.m_values[Kx] + original.m_values[Jx] * appendThis.m_values[Ky] + original.m_values[Kx] * appendThis.m_values[Kz] + original.m_values[Tx] * appendThis.m_values[Kw];
	m_values[Ky] = original.m_values[Iy] * appendThis.m_values[Kx] + original.m_values[Jy] * appendThis.m_values[Ky] + original.m_values[Ky] * appendThis.m_values[Kz] + original.m_values[Ty] * appendThis.m_values[Kw];
	m_values[Kz] = original.m_values[Iz] * appendThis.m_values[Kx] + original.m_values[Jz] * appendThis.m_values[Ky] + original.m_values[Kz] * appendThis.m_values[Kz] + original.m_values[Tz] * appendThis.m_values[Kw];
	m_values[Kw] = original.m_values[Iw] * appendThis.m_values[Kx] + original.m_values[Jw] * appendThis.m_values[Ky] + original.m_values[Kw] * appendThis.m_values[Kz] + original.m_values[Tw] * appendThis.m_values[Kw];
	m_values[Tx] = original.m_values[Ix] * appendThis.m_values[Tx] + original.m_values[Jx] * appendThis.m_values[Ty] + original.m_values[Kx] * appendThis.m_values[Tz] + original.m_values[Tx] * appendThis.m_values[Tw];
	m_values[Ty] = original.m_values[Iy] * appendThis.m_values[Tx] + original.m_values[Jy] * appendThis.m_values[Ty] + original.m_values[Ky] * appendThis.m_values[Tz] + original.m_values[Ty] * appendThis.m_values[Tw];
	m_values[Tz] = original.m_values[Iz] * appendThis.m_values[Tx] + original.m_values[Jz] * appendThis.m_values[Ty] + original.m_values[Kz] * appendThis.m_values[Tz] + original.m_values[Tz] * appendThis.m_values[Tw];
	m_values[Tw] = original.m_values[Iw] * appendThis.m_values[Tx] + original.m_values[Jw] * appendThis.m_values[Ty] + original.m_values[Kw] * appendThis.m_values[Tz] + original.m_values[Tw] * appendThis.m_values[Tw];
}


//-----------------------------------------------------------------------------------------------
void Mat44::AppendZRotation( float rotationDegreesAboutZ )
{
	Append( MakeZRotationDegrees( rotationDegreesAboutZ ) );
}


//-----------------------------------------------------------------------------------------------
void Mat44::AppendYRotation( float rotationDegreesAboutY )
{
	Append( MakeYRotationDegrees( rotationDegreesAboutY ) );
}


//-----------------------------------------------------------------------------------------------
void Mat44::AppendXRotation( float rotationDegreesAboutX )
{
	Append( MakeXRotationDegrees( rotationDegreesAboutX ) );
}


//-----------------------------------------------------------------------------------------------
void Mat44::AppendTranslation2D( Vec2 const& translationXY )
{
	Mat44 translationMat = MakeTranslation2D( translationXY );
	Append( translationMat );
}


//-----------------------------------------------------------------------------------------------
void Mat44::AppendTranslation3D( Vec3 const& translation3D )
{
	Mat44 translationMat = MakeTranslation3D( translation3D );
	Append( translationMat );
}


//-----------------------------------------------------------------------------------------------
void Mat44::AppendScaleUniform2D( float uniformScaleXY )
{
	Mat44 scaleMat = MakeUniformScale2D( uniformScaleXY );
	Append( scaleMat );
}


//-----------------------------------------------------------------------------------------------
void Mat44::AppendScaleUniform3D( float uniformScaleXYZ )
{
	Mat44 scaleMat = MakeUniformScale3D( uniformScaleXYZ );
	Append( scaleMat );
}


//-----------------------------------------------------------------------------------------------
void Mat44::AppendScaleNonUniform2D( Vec2 const& nonUniformScaleXY )
{
	Mat44 scaleMat = MakeNonUniformScale2D( nonUniformScaleXY );
	Append( scaleMat );
}


//-----------------------------------------------------------------------------------------------
void Mat44::AppendScaleNonUniform3D( Vec3 const& nonUniformScaleXYZ )
{
	Mat44 scaleMat = MakeNonUniformScale3D( nonUniformScaleXYZ );
	Append( scaleMat );
}