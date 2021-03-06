#pragma once

#include "vector2d.h"
#include "vector3d.h"
#include "plane3d.h"
#include "aabbox3d.h"

//row major matrix
template <class T>
class CMatrix4
{
public:
	//
	CMatrix4() { makeIdentity(); }
	explicit CMatrix4(bool Identity) { if (Identity) makeIdentity(); else makeZero(); }
	CMatrix4(const CMatrix4& other){ (*this) = other; }

	const T* pointer() const { return M; }
	T* pointer() { return M; }

	CMatrix4<T>& operator=(const CMatrix4<T> &other);

	bool operator==(const CMatrix4<T> &other) const;
	bool operator!=(const CMatrix4<T> &other) const { return !(*this == other); }

	CMatrix4<T> operator+(const CMatrix4<T>& other) const;
	CMatrix4<T>& operator+=(const CMatrix4<T>& other);
	CMatrix4<T> operator-(const CMatrix4<T>& other) const;
	CMatrix4<T>& operator-=(const CMatrix4<T>& other);

	CMatrix4<T> operator*(const CMatrix4<T>& other) const;
	CMatrix4<T>& operator*=(const CMatrix4<T>& other) { return setbyproduct(*this, other); }
	CMatrix4<T> operator*(const T& scalar) const;
	CMatrix4<T>& operator*=(const T& scalar);

	//
	CMatrix4<T>& makeIdentity()
	{
		M[1] = M[2] = M[3] = M[4] =
			M[6] = M[7] = M[8] = M[9] =
			M[11] = M[12] = M[13] = M[14] = 0;
		M[0] = M[5] = M[10] = M[15] = (T)1;
		return *this;
	}
	CMatrix4<T>& makeZero(){ memset(M, 0, 16 * sizeof(T)); return *this; }
	CMatrix4<T> getInverse() const;
	float getDeterminant() const;
	void transpose();
	bool equals(const CMatrix4<T>& other, const T tolerance = (T)ROUNDING_ERROR_f32) const;

	bool isIdentity() const;
	bool isZero() const;
	//
	vector3df transformVector(const vector3df& vect, float& z) const;
	vector3df transformVector(const vector3df& vect) const;
	plane3df transformPlane(const plane3df& plane) const;
	aabbox3df transformBox(const aabbox3df& box) const;
	vector3df rotateVector(const vector3df& vect) const;
	vector3df inverseRotateVector(const vector3df& vect) const;

	vector3df getRow(int i) const
	{
		return vector3df(M[i * 4 + 0], M[i * 4 + 1], M[i * 4 + 2]);
	}
	vector3df getCol(int i) const 
	{ 
		return vector3df(M[0 * 4 + i], M[1 * 4 + i], M[2 * 4 + i]);
	}
	//	Set row and column
	void setRow(int i, const vector3df& v) 
	{ 
		M[i * 4 + 0] = v.x; M[i * 4 + 1] = v.y; M[i * 4 + 2] = v.z;
	}
	void setCol(int i, const vector3df& v) 
	{ 
		M[0 * 4 + i] = v.x; M[1 * 4 + i] = v.y; M[2 * 4 + i] = v.z;
	}

	CMatrix4<T>& setTranslation(const vector3d<T>& translation);
	vector3d<T> getTranslation() const { return vector3d<T>(_14, _24, _34); }

	CMatrix4<T>& setRotationRadians(const vector3d<T>& rotation);
	CMatrix4<T>& setRotationDegrees(const vector3d<T>& rotation){ return setRotationRadians(rotation * DEGTORAD); }
	vector3d<T> getRotationRadians() const;
	vector3d<T> getRotationDegrees() const { return getRotationRadians() * RADTODEG; }

	CMatrix4<T>& scale(const vector3d<T>& scale);
	CMatrix4<T>& scale(const T scale) { return scale(vector3d<T>(scale, scale, scale)); }
	CMatrix4<T>& setScale(const vector3d<T>& scale);
	vector3d<T> getScale() const;

	vector3d<T> getDir() const { return f3d::normalize(getRow(2)); }
	vector3d<T> getUp() const { return f3d::normalize(getRow(1)); }
	vector3d<T> getRight() const { return f3d::normalize(getRow(0)); }


	// camera view, perspective
	//camera
	CMatrix4<T>& buildCameraLookAtMatrixLH(
		const vector3df& position,
		const vector3df& target,
		const vector3df& upVector);

	CMatrix4<T>& buildCameraLookAtMatrixRH(
		const vector3df& position,
		const vector3df& target,
		const vector3df& upVector);

	//projection
	CMatrix4<T>& buildProjectionMatrixPerspectiveFovRH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar);

	CMatrix4<T>& buildProjectionMatrixPerspectiveFovLH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar);

	CMatrix4<T>& buildProjectionMatrixPerspectiveRH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar);

	CMatrix4<T>& buildProjectionMatrixPerspectiveLH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar);

	CMatrix4<T>& buildProjectionMatrixOrthoFovLH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar);

	CMatrix4<T>& buildProjectionMatrixOrthoFovRH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar);

	CMatrix4<T>& buildProjectionMatrixOrthoLH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar);

	CMatrix4<T>& buildProjectionMatrixOrthoRH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar);

	//
	CMatrix4<T>& buildTextureTransform(float rotateRad,
		const vector2df &rotatecenter,
		const vector2df &translate,
		const vector2df &scale);

	CMatrix4<T>& setTextureRotationCenter(float rotateRad);

	CMatrix4<T>& setTextureTranslate(float x, float y);

	CMatrix4<T>& setTextureScale(float sx, float sy);

	CMatrix4<T>& setTextureScaleCenter(float sx, float sy);

	CMatrix4<T>& setM(const T* data);

	static const CMatrix4<T>& Identity() { static CMatrix4<T> m(true); return m; }
	static const CMatrix4<T>& Zero() { static CMatrix4<T> m(false); return m; }

	CMatrix4<T>& buildMatrix(const vector3d<T>& vDir, const vector3d<T>& vUp, const vector3d<T>& vPos);

public:
	union
	{
		struct
		{
			T M[16];
		};
		struct
		{
			T _11, _21, _31, _41;
			T _12, _22, _32, _42;
			T _13, _23, _33, _43;
			T _14, _24, _34, _44;
		};
	};

	private:
		float Det3x3(float a11, float a12, float a13, float a21, float a22, float a23,
			float a31, float a32, float a33) const
		{
			return a11 * a22 * a33 + a21 * a32 * a13 + a31 * a12 * a23 -
				a13 * a22 * a31 - a23 * a32 * a11 - a33 * a12 * a21;
		}
};

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildMatrix(const vector3d<T>& vDir, const vector3d<T>& vUp, const vector3d<T>& vPos)
{
	vector3d<T>	vX, vY, vZ;

	vZ = vDir;
	vZ.normalize();
	vY = vUp;
	vY.normalize();
	vX = vY.crossProduct(vZ);
	vX.normalize();

	_11 = vX.x;
	_12 = vX.y;
	_13 = vX.z;
	_14 = (T)0.0f;

	_21 = vY.x;
	_22 = vY.y;
	_23 = vY.z;
	_24 = (T)0.0f;

	_31 = vZ.x;
	_32 = vZ.y;
	_33 = vZ.z;
	_34 = (T)0.0f;

	_41 = vPos.x;
	_42 = vPos.y;
	_43 = vPos.z;
	_44 = (T)1.0f;

	return *this;
}

template <class T>
inline void CMatrix4<T>::transpose()
{
	T tmp;
	tmp = _12;
	_12 = _21;
	_21 = tmp;

	tmp = _13;
	_13 = _31;
	_31 = tmp;

	tmp = _14;
	_14 = _41;
	_41 = tmp;

	tmp = _23;
	_23 = _32;
	_32 = tmp;

	tmp = _24;
	_24 = _42;
	_42 = tmp;

	tmp = _34;
	_34 = _43;
	_43 = tmp;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setM(const T* data)
{
	memcpy(M, data, 16 * sizeof(T));
	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setTextureScaleCenter(float sx, float sy)
{
	M[0] = (T)sx;
	M[5] = (T)sy;
	M[8] = (T)(0.5f - 0.5f * sx);
	M[9] = (T)(0.5f - 0.5f * sy);

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setTextureScale(float sx, float sy)
{
	M[0] = (T)sx;
	M[5] = (T)sy;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setTextureTranslate(float x, float y)
{
	M[8] = (T)x;
	M[9] = (T)y;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setTextureRotationCenter(float rotateRad)
{
	const float c = cosf(rotateRad);
	const float s = sinf(rotateRad);
	M[0] = (T)c;
	M[1] = (T)s;

	M[4] = (T)-s;
	M[5] = (T)c;

	M[8] = (T)(0.5f * (s - c) + 0.5f);
	M[9] = (T)(-0.5f * (s + c) + 0.5f);

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildTextureTransform(float rotateRad, const vector2df &rotatecenter, const vector2df &translate, const vector2df &scale)
{
	const float c = cosf(rotateRad);
	const float s = sinf(rotateRad);

	M[0] = (T)(c * scale.x);
	M[1] = (T)(s * scale.y);
	M[2] = 0;
	M[3] = 0;

	M[4] = (T)(-s * scale.x);
	M[5] = (T)(c * scale.y);
	M[6] = 0;
	M[7] = 0;

	M[8] = (T)(c * scale.x * rotatecenter.x + -s * rotatecenter.y + translate.x);
	M[9] = (T)(s * scale.y * rotatecenter.x + c * rotatecenter.y + translate.y);
	M[10] = 1;
	M[11] = 0;

	M[12] = 0;
	M[13] = 0;
	M[14] = 0;
	M[15] = 1;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::operator=(const CMatrix4<T> &other)
{
	ASSERT(this != &other);
	memcpy(M, other.M, 16 * sizeof(T));
	return *this;
}

template <class T>
inline bool CMatrix4<T>::operator==(const CMatrix4<T>& other) const
{
	if (this == &other) return true;
	return memcmp(this, &other, sizeof(CMatrix4<T>)) == 0;
}

template <class T>
inline CMatrix4<T> CMatrix4<T>::operator+(const CMatrix4<T>& other) const
{
	CMatrix4<T> temp;

	temp[0] = M[0] + other[0];
	temp[1] = M[1] + other[1];
	temp[2] = M[2] + other[2];
	temp[3] = M[3] + other[3];
	temp[4] = M[4] + other[4];
	temp[5] = M[5] + other[5];
	temp[6] = M[6] + other[6];
	temp[7] = M[7] + other[7];
	temp[8] = M[8] + other[8];
	temp[9] = M[9] + other[9];
	temp[10] = M[10] + other[10];
	temp[11] = M[11] + other[11];
	temp[12] = M[12] + other[12];
	temp[13] = M[13] + other[13];
	temp[14] = M[14] + other[14];
	temp[15] = M[15] + other[15];

	return temp;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::operator+=(const CMatrix4<T>& other)
{
	M[0] += other[0];
	M[1] += other[1];
	M[2] += other[2];
	M[3] += other[3];
	M[4] += other[4];
	M[5] += other[5];
	M[6] += other[6];
	M[7] += other[7];
	M[8] += other[8];
	M[9] += other[9];
	M[10] += other[10];
	M[11] += other[11];
	M[12] += other[12];
	M[13] += other[13];
	M[14] += other[14];
	M[15] += other[15];

	return *this;
}

template <class T>
inline CMatrix4<T> CMatrix4<T>::operator-(const CMatrix4<T>& other) const
{
	CMatrix4<T> temp;

	temp[0] = M[0] - other[0];
	temp[1] = M[1] - other[1];
	temp[2] = M[2] - other[2];
	temp[3] = M[3] - other[3];
	temp[4] = M[4] - other[4];
	temp[5] = M[5] - other[5];
	temp[6] = M[6] - other[6];
	temp[7] = M[7] - other[7];
	temp[8] = M[8] - other[8];
	temp[9] = M[9] - other[9];
	temp[10] = M[10] - other[10];
	temp[11] = M[11] - other[11];
	temp[12] = M[12] - other[12];
	temp[13] = M[13] - other[13];
	temp[14] = M[14] - other[14];
	temp[15] = M[15] - other[15];

	return temp;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::operator-=(const CMatrix4<T>& other)
{
	M[0] -= other[0];
	M[1] -= other[1];
	M[2] -= other[2];
	M[3] -= other[3];
	M[4] -= other[4];
	M[5] -= other[5];
	M[6] -= other[6];
	M[7] -= other[7];
	M[8] -= other[8];
	M[9] -= other[9];
	M[10] -= other[10];
	M[11] -= other[11];
	M[12] -= other[12];
	M[13] -= other[13];
	M[14] -= other[14];
	M[15] -= other[15];

	return *this;
}

template <class T>
inline CMatrix4<T> CMatrix4<T>::operator*(const CMatrix4<T>& other) const
{
	CMatrix4<T> res;
	const CMatrix4<T>& lhs = *this;
	const CMatrix4<T>& rhs = other;

	res._11 = lhs._11 * rhs._11 + lhs._12 * rhs._21 + lhs._13 * rhs._31 + lhs._14 * rhs._41;
	res._12 = lhs._11 * rhs._12 + lhs._12 * rhs._22 + lhs._13 * rhs._32 + lhs._14 * rhs._42;
	res._13 = lhs._11 * rhs._13 + lhs._12 * rhs._23 + lhs._13 * rhs._33 + lhs._14 * rhs._43;
	res._14 = lhs._11 * rhs._14 + lhs._12 * rhs._24 + lhs._13 * rhs._34 + lhs._14 * rhs._44;

	res._21 = lhs._21 * rhs._11 + lhs._22 * rhs._21 + lhs._23 * rhs._31 + lhs._24 * rhs._41;
	res._22 = lhs._21 * rhs._12 + lhs._22 * rhs._22 + lhs._23 * rhs._32 + lhs._24 * rhs._42;
	res._23 = lhs._21 * rhs._13 + lhs._22 * rhs._23 + lhs._23 * rhs._33 + lhs._24 * rhs._43;
	res._24 = lhs._21 * rhs._14 + lhs._22 * rhs._24 + lhs._23 * rhs._34 + lhs._24 * rhs._44;

	res._31 = lhs._31 * rhs._11 + lhs._32 * rhs._21 + lhs._33 * rhs._31 + lhs._34 * rhs._41;
	res._32 = lhs._31 * rhs._12 + lhs._32 * rhs._22 + lhs._33 * rhs._32 + lhs._34 * rhs._42;
	res._33 = lhs._31 * rhs._13 + lhs._32 * rhs._23 + lhs._33 * rhs._33 + lhs._34 * rhs._43;
	res._34 = lhs._31 * rhs._14 + lhs._32 * rhs._24 + lhs._33 * rhs._34 + lhs._34 * rhs._44;

	res._41 = lhs._41 * rhs._11 + lhs._42 * rhs._21 + lhs._43 * rhs._31 + lhs._44 * rhs._41;
	res._42 = lhs._41 * rhs._12 + lhs._42 * rhs._22 + lhs._43 * rhs._32 + lhs._44 * rhs._42;
	res._43 = lhs._41 * rhs._13 + lhs._42 * rhs._23 + lhs._43 * rhs._33 + lhs._44 * rhs._43;
	res._44 = lhs._41 * rhs._14 + lhs._42 * rhs._24 + lhs._43 * rhs._34 + lhs._44 * rhs._44;

	return res;
}

template <class T>
inline CMatrix4<T> CMatrix4<T>::operator*(const T& scalar) const
{
	CMatrix4<T> temp;

	temp[0] = M[0] * scalar;
	temp[1] = M[1] * scalar;
	temp[2] = M[2] * scalar;
	temp[3] = M[3] * scalar;
	temp[4] = M[4] * scalar;
	temp[5] = M[5] * scalar;
	temp[6] = M[6] * scalar;
	temp[7] = M[7] * scalar;
	temp[8] = M[8] * scalar;
	temp[9] = M[9] * scalar;
	temp[10] = M[10] * scalar;
	temp[11] = M[11] * scalar;
	temp[12] = M[12] * scalar;
	temp[13] = M[13] * scalar;
	temp[14] = M[14] * scalar;
	temp[15] = M[15] * scalar;

	return temp;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::operator*=(const T& scalar)
{
	M[0] *= scalar;
	M[1] *= scalar;
	M[2] *= scalar;
	M[3] *= scalar;
	M[4] *= scalar;
	M[5] *= scalar;
	M[6] *= scalar;
	M[7] *= scalar;
	M[8] *= scalar;
	M[9] *= scalar;
	M[10] *= scalar;
	M[11] *= scalar;
	M[12] *= scalar;
	M[13] *= scalar;
	M[14] *= scalar;
	M[15] *= scalar;

	return *this;
}

template <class T>
inline float CMatrix4<T>::getDeterminant() const
{
	float fDet;
	fDet = _11 * _22 * _33 * _44;
	fDet += _21 * _32 * _43 * _14;
	fDet += _31 * _42 * _13 * _24;
	fDet += _41 * _12 * _23 * _34;
	fDet -= _41 * _32 * _23 * _14;
	fDet -= _42 * _33 * _24 * _11;
	fDet -= _43 * _34 * _21 * _12;
	fDet -= _44 * _31 * _22 * _13;

	return fDet;
}

template <class T>
inline CMatrix4<T> CMatrix4<T>::getInverse() const
{
	CMatrix4<T> out;

	float d = getDeterminant();
	d = reciprocal_(d);

	out._11 = Det3x3(_22, _23, _24, _32, _33, _34, _42, _43, _44) * d;
	out._12 = -Det3x3(_12, _13, _14, _32, _33, _34, _42, _43, _44) * d;
	out._13 = Det3x3(_12, _13, _14, _22, _23, _24, _42, _43, _44) * d;
	out._14 = -Det3x3(_12, _13, _14, _22, _23, _24, _32, _33, _34) * d;

	out._21 = -Det3x3(_21, _23, _24, _31, _33, _34, _41, _43, _44) * d;
	out._22 = Det3x3(_11, _13, _14, _31, _33, _34, _41, _43, _44) * d;
	out._23 = -Det3x3(_11, _13, _14, _21, _23, _24, _41, _43, _44) * d;
	out._24 = Det3x3(_11, _13, _14, _21, _23, _24, _31, _33, _34) * d;

	out._31 = Det3x3(_21, _22, _24, _31, _32, _34, _41, _42, _44) * d;
	out._32 = -Det3x3(_11, _12, _14, _31, _32, _34, _41, _42, _44) * d;
	out._33 = Det3x3(_11, _12, _14, _21, _22, _24, _41, _42, _44) * d;
	out._34 = -Det3x3(_11, _12, _14, _21, _22, _24, _31, _32, _34) * d;

	out._41 = -Det3x3(_21, _22, _23, _31, _32, _33, _41, _42, _43) * d;
	out._42 = Det3x3(_11, _12, _13, _31, _32, _33, _41, _42, _43) * d;
	out._43 = -Det3x3(_11, _12, _13, _21, _22, _23, _41, _42, _43) * d;
	out._44 = Det3x3(_11, _12, _13, _21, _22, _23, _31, _32, _33) * d;

	return out;
}

template <class T>
inline bool CMatrix4<T>::equals(const CMatrix4<T>& other, const T tolerance) const
{
	for (int32_t i = 0; i < 16; ++i)
	if (!equals_(M[i], other.M[i], tolerance))
		return false;

	return true;
}

template <class T>
inline bool CMatrix4<T>::isIdentity() const
{
	return (*this) == Identity();
}

template <class T>
bool CMatrix4<T>::isZero() const
{
	return (*this) == Zero();
}

template <class T>
inline vector3df CMatrix4<T>::transformVector(const vector3df& vect, float& z) const
{
	vector3df vector;

	vector.x = vect.x*M[0] + vect.y*M[4] + vect.z*M[8] + M[12];
	vector.y = vect.x*M[1] + vect.y*M[5] + vect.z*M[9] + M[13];
	vector.z = vect.x*M[2] + vect.y*M[6] + vect.z*M[10] + M[14];
	z = vect.x*M[3] + vect.y*M[7] + vect.z*M[11] + M[15];

	return vector;
}

template <class T>
inline vector3df CMatrix4<T>::transformVector(const vector3df& vect) const
{
	vector3df vector;

	vector.x = vect.x*M[0] + vect.y*M[4] + vect.z*M[8] + M[12];
	vector.y = vect.x*M[1] + vect.y*M[5] + vect.z*M[9] + M[13];
	vector.z = vect.x*M[2] + vect.y*M[6] + vect.z*M[10] + M[14];

	return vector;
}

template <class T>
inline plane3df CMatrix4<T>::transformPlane(const plane3df& plane) const
{
	vector3df point = transformVector(plane.getMemberPoint());

	CMatrix4<T> m = getInverse();
	m.transpose();

	vector3df normal = m.transformVector(plane.Normal);

	return plane3df(point, normal);
}

template <class T>
inline aabbox3df CMatrix4<T>::transformBox(const aabbox3df& box) const
{
	vector3df vCenter = transformVector(box.getCenter());

	const vector3df& ext = box.getExtent();
	vector3df vAxisX = rotateVector(vector3df(ext.x, 0, 0));
	vector3df vAxisY = rotateVector(vector3df(0, ext.y, 0));
	vector3df vAxisZ = rotateVector(vector3df(0, 0, ext.z));

	vector3df vExtends;
	vExtends.x = fabs(vAxisX.x) + fabs(vAxisY.x) + fabs(vAxisZ.x);
	vExtends.y = fabs(vAxisX.y) + fabs(vAxisY.y) + fabs(vAxisZ.y);
	vExtends.z = fabs(vAxisX.z) + fabs(vAxisY.z) + fabs(vAxisZ.z);

	aabbox3df aabbNew;
	aabbNew.setByCenterExtent(vCenter, vExtends);

	return aabbNew;
}

template <class T>
inline vector3df CMatrix4<T>::rotateVector(const vector3df& vect) const
{
	vector3df tmp;
	tmp.x = vect.x*M[0] + vect.y*M[4] + vect.z*M[8];
	tmp.y = vect.x*M[1] + vect.y*M[5] + vect.z*M[9];
	tmp.z = vect.x*M[2] + vect.y*M[6] + vect.z*M[10];
	return tmp;
}

template <class T>
inline  vector3df CMatrix4<T>::inverseRotateVector(const vector3df& vect) const
{
	vector3df tmp;
	tmp.x = vect.x*M[0] + vect.y*M[1] + vect.z*M[2];
	tmp.y = vect.x*M[4] + vect.y*M[5] + vect.z*M[6];
	tmp.z = vect.x*M[8] + vect.y*M[9] + vect.z*M[10];
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setTranslation(const vector3d<T>& translation)
{
	_14 = translation.x;
	_24 = translation.y;
	_34 = translation.z;
	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setRotationRadians(const vector3d<T>& rotation)
{
	const float cr = cos(rotation.x);
	const float sr = sin(rotation.x);
	const float cp = cos(rotation.y);
	const float sp = sin(rotation.y);
	const float cy = cos(rotation.z);
	const float sy = sin(rotation.z);

	M[0] = (T)(cp*cy);
	M[1] = (T)(cp*sy);
	M[2] = (T)(-sp);

	const float srsp = sr*sp;
	const float crsp = cr*sp;

	M[4] = (T)(srsp*cy - cr*sy);
	M[5] = (T)(srsp*sy + cr*cy);
	M[6] = (T)(sr*cp);

	M[8] = (T)(crsp*cy + sr*sy);
	M[9] = (T)(crsp*sy - sr*cy);
	M[10] = (T)(cr*cp);

	return *this;
}

template <class T>
inline vector3d<T> CMatrix4<T>::getRotationRadians() const
{
	const CMatrix4<T> &mat = *this;
	const vector3d<T> scale = getScale();
	const vector3d<float> invScale(reciprocal_(scale.x), reciprocal_(scale.y), reciprocal_(scale.z));

	float Y = -asin(mat[2] * invScale.x);
	const float C = cos(Y);

	float rotx, roty, X, Z;

	if (!iszero_(C))
	{
		const float invC = reciprocal_(C);
		rotx = mat[10] * invC * invScale.z;
		roty = mat[6] * invC * invScale.y;
		X = atan2(roty, rotx);
		rotx = mat[0] * invC * invScale.x;
		roty = mat[1] * invC * invScale.x;
		Z = atan2(roty, rotx);
	}
	else
	{
		X = 0.0;
		rotx = mat[5] * invScale.y;
		roty = -mat[4] * invScale.y;
		Z = atan2(roty, rotx);
	}

	// fix values that get below zero
	// before it would set (!) values to 360
	// that were above 360:
	if (X < 0.0) X += 2 * PI;
	if (Y < 0.0) Y += 2 * PI;
	if (Z < 0.0) Z += 2 * PI;

	return vector3d<T>((T)X, (T)Y, (T)Z);
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::scale(const vector3d<T>& scale)
{
	T x = abs_(scale.x);			// 0
	T y = abs_(scale.y);			// 5
	T z = abs_(scale.z);			// 10

	M[0] *= x;
	M[1] *= y;
	M[2] *= z;

	M[4] *= x;
	M[5] *= y;
	M[6] *= z;

	M[8] *= x;
	M[9] *= y;
	M[10] *= z;

	M[12] *= x;
	M[13] *= y;
	M[14] *= z;

	return (*this);
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setScale(const vector3d<T>& scale)
{
	T x = abs_(scale.x);			// 0
	T y = abs_(scale.y);			// 5
	T z = abs_(scale.z);			// 10

	M[0] = x;
	M[1] = y;
	M[2] = z;

	M[4] = x;
	M[5] = y;
	M[6] = z;

	M[8] = x;
	M[9] = y;
	M[10] = z;

	M[12] = x;
	M[13] = y;
	M[14] = z;

	return (*this);
}

template <class T>
inline vector3d<T> CMatrix4<T>::getScale() const
{
	// We have to do the full calculation.
	return vector3d<T>(squareroot_(M[0] * M[0] + M[1] * M[1] + M[2] * M[2]),
		squareroot_(M[4] * M[4] + M[5] * M[5] + M[6] * M[6]),
		squareroot_(M[8] * M[8] + M[9] * M[9] + M[10] * M[10]));
}

template <class T>
inline  CMatrix4<T>& CMatrix4<T>::buildCameraLookAtMatrixLH(
	const vector3df& position,
	const vector3df& target,
	const vector3df& upVector)
{
	vector3df zaxis = target - position;
	zaxis.normalize();

	vector3df xaxis = upVector.crossProduct(zaxis);
	xaxis.normalize();

	vector3df yaxis = zaxis.crossProduct(xaxis);

	M[0] = (T)xaxis.x;
	M[1] = (T)yaxis.x;
	M[2] = (T)zaxis.x;
	M[3] = 0;

	M[4] = (T)xaxis.y;
	M[5] = (T)yaxis.y;
	M[6] = (T)zaxis.y;
	M[7] = 0;

	M[8] = (T)xaxis.z;
	M[9] = (T)yaxis.z;
	M[10] = (T)zaxis.z;
	M[11] = 0;

	M[12] = (T)-xaxis.dotProduct(position);
	M[13] = (T)-yaxis.dotProduct(position);
	M[14] = (T)-zaxis.dotProduct(position);
	M[15] = 1;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildCameraLookAtMatrixRH(
	const vector3df& position,
	const vector3df& target,
	const vector3df& upVector)
{
	vector3df zaxis = position - target;
	zaxis.normalize();

	vector3df xaxis = upVector.crossProduct(zaxis);
	xaxis.normalize();

	vector3df yaxis = zaxis.crossProduct(xaxis);

	M[0] = (T)xaxis.x;
	M[1] = (T)yaxis.x;
	M[2] = (T)zaxis.x;
	M[3] = 0;

	M[4] = (T)xaxis.y;
	M[5] = (T)yaxis.y;
	M[6] = (T)zaxis.y;
	M[7] = 0;

	M[8] = (T)xaxis.z;
	M[9] = (T)yaxis.z;
	M[10] = (T)zaxis.z;
	M[11] = 0;

	M[12] = (T)-xaxis.dotProduct(position);
	M[13] = (T)-yaxis.dotProduct(position);
	M[14] = (T)-zaxis.dotProduct(position);
	M[15] = 1;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixPerspectiveFovLH(
	float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
{
	const float h = reciprocal_((float)tan(fieldOfViewRadians*0.5f));
	ASSERT(aspectRatio != 0.f); //divide by zero
	const float w = (h / aspectRatio);

	ASSERT(zNear != zFar); //divide by zero
	M[0] = w;
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)h;
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (T)(zFar / (zFar - zNear));
	M[11] = 1;

	M[12] = 0;
	M[13] = 0;
	M[14] = (T)(-zNear*zFar / (zFar - zNear));
	M[15] = 0;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixPerspectiveFovRH(
	float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
{
	const float h = reciprocal_(tan(fieldOfViewRadians*0.5f));
	ASSERT(aspectRatio != 0.f); //divide by zero
	const T w = h / aspectRatio;

	ASSERT(zNear != zFar); //divide by zero
	M[0] = w;
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)h;
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (T)(zFar / (zNear - zFar)); // DirectX version
	//		M[10] = (T)(zFar+zNear/(zNear-zFar)); // OpenGL version
	M[11] = -1;

	M[12] = 0;
	M[13] = 0;
	M[14] = (T)(zNear*zFar / (zNear - zFar)); // DirectX version
	//		M[14] = (T)(2.0f*zNear*zFar/(zNear-zFar)); // OpenGL version
	M[15] = 0;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixPerspectiveLH(
	float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
{
	ASSERT(widthOfViewVolume != 0.f); //divide by zero
	ASSERT(heightOfViewVolume != 0.f); //divide by zero
	ASSERT(zNear != zFar); //divide by zero
	M[0] = (T)(2 * zNear / widthOfViewVolume);
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)(2 * zNear / heightOfViewVolume);
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (T)(zFar / (zFar - zNear));
	M[11] = 1;

	M[12] = 0;
	M[13] = 0;
	M[14] = (T)(zNear*zFar / (zNear - zFar));
	M[15] = 0;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixPerspectiveRH(
	float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
{
	ASSERT(widthOfViewVolume != 0.f); //divide by zero
	ASSERT(heightOfViewVolume != 0.f); //divide by zero
	ASSERT(zNear != zFar); //divide by zero
	M[0] = (T)(2 * zNear / widthOfViewVolume);
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)(2 * zNear / heightOfViewVolume);
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (T)(zFar / (zNear - zFar));
	M[11] = -1;

	M[12] = 0;
	M[13] = 0;
	M[14] = (T)(zNear*zFar / (zNear - zFar));
	M[15] = 0;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixOrthoFovLH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
{
	const float h = reciprocal_((float)tan(fieldOfViewRadians*0.5));
	ASSERT(aspectRatio != 0.f); //divide by zero
	const T w = (T)(h / aspectRatio);

	ASSERT(zNear != zFar); //divide by zero
	ASSERT(zNear != 0.0f);  //divide by zero

	M[0] = w / zNear;
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)h / zNear;
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (T)(zFar / (zFar - zNear));
	M[11] = 1;

	M[12] = 0;
	M[13] = 0;
	M[14] = (T)(-zNear*zFar / (zFar - zNear));
	M[15] = 0;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixOrthoFovRH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
{
	const float h = reciprocal_((float)tan(fieldOfViewRadians*0.5));
	ASSERT(aspectRatio != 0.f); //divide by zero
	const T w = (T)(h / aspectRatio);

	ASSERT(zNear != zFar); //divide by zero
	ASSERT(zNear != 0.0f);  //divide by zero

	M[0] = w / zNear;
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)h / zNear;
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (T)(1 / (zNear - zFar));
	M[11] = 0;

	M[12] = 0;
	M[13] = 0;
	M[14] = (T)(zNear / (zNear - zFar));
	M[15] = -1;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixOrthoLH(
	float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
{
	ASSERT(widthOfViewVolume != 0.f); //divide by zero
	ASSERT(heightOfViewVolume != 0.f); //divide by zero
	ASSERT(zNear != zFar); //divide by zero
	M[0] = (T)(2 / widthOfViewVolume);
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)(2 / heightOfViewVolume);
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (T)(1 / (zFar - zNear));
	M[11] = 0;

	M[12] = 0;
	M[13] = 0;
	M[14] = (T)(zNear / (zNear - zFar));
	M[15] = 1;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixOrthoRH(
	float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
{
	ASSERT(widthOfViewVolume != 0.f); //divide by zero
	ASSERT(heightOfViewVolume != 0.f); //divide by zero
	ASSERT(zNear != zFar); //divide by zero
	M[0] = (T)(2 / widthOfViewVolume);
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)(2 / heightOfViewVolume);
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (T)(1 / (zNear - zFar));
	M[11] = 0;

	M[12] = 0;
	M[13] = 0;
	M[14] = (T)(zNear / (zNear - zFar));
	M[15] = -1;

	return *this;
}

typedef CMatrix4<float> matrix4;
