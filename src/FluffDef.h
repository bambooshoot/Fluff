#pragma once

#include <io.h>
#include <array>
#include <vector>
#include <map>
#include <random>
#include <memory>
#include <optional>
#include <numeric>
#include <regex>
#include <type_traits>
#include <openEXR/ImathBox.h>
#include <openEXR/ImathVec.h>
#include <openEXR/ImathQuat.h>

#include <tbb/tbb.h>

#define ONE_THIRD ((1.0f)/(3.0f))
#define ONE_SIXTH ((1.0f)/(6.0f))
#define PI 3.1415926f
#define PI2 (PI*2)

typedef Imath::V3f      Vec;
typedef const Vec       CVec;

typedef Imath::V2f      Vec2;
typedef const Vec2      CVec2;

typedef Imath::Box2f    Box2;

typedef Imath::Box3f    Box;

typedef Imath::Quatf    Quat;

typedef Imath::M44f     Matrix;
typedef const Matrix    CMatrix;

typedef unsigned int	uint;
typedef const uint		cuint;
typedef const int		cint;
typedef const float		cfloat;

typedef std::string     Str;
typedef const Str       CStr;

#define TYPEDEF_ARRAY_AND_CONST(TYPE,NUM,NAME,CNAME) \
typedef std::array<TYPE, NUM>	NAME; \
typedef const NAME				CNAME;

TYPEDEF_ARRAY_AND_CONST(int, 2, UVIdx, CUVIdx)
TYPEDEF_ARRAY_AND_CONST(uint, 3, TriangleId, CTriangleId)

#define TYPEDEF_VECTOR_AND_CONST(TYPE,NEWNAME,CNEWNAME) \
typedef std::vector<TYPE>	NEWNAME; \
typedef const NEWNAME		CNEWNAME;

TYPEDEF_VECTOR_AND_CONST(TriangleId, TriangleIdList, CTriangleIdList)
TYPEDEF_VECTOR_AND_CONST(uint, UIntList, CUIntList)
TYPEDEF_VECTOR_AND_CONST(float, FloatList, CFloatList)
TYPEDEF_VECTOR_AND_CONST(Vec, VecList, CVecList)
TYPEDEF_VECTOR_AND_CONST(Vec2, Vec2List, CVec2List)
TYPEDEF_VECTOR_AND_CONST(Vec, VectorList, CVectorList)
TYPEDEF_VECTOR_AND_CONST(Vec2, Vector2List, CVector2List)

using NodeId = uint;

using RandGen = std::mt19937;
using RealDistrib = std::uniform_real_distribution<float>;

struct KeyIds
{
	uint mtxKeyId, pKeyId;
};

using CKeyIds = const KeyIds;

template<class T>
inline T clamp(const T v, const T l, const T h)
{
	return std::max<T>(std::min<T>(v, h), l);
}

template<typename T>
inline T lerp(const T v, const T l, const T h)
{
	return clamp<float>((v - l) / (h - l), 0, 1);
}

inline bool fileExists(CStr & fileName)
{
	return _access(fileName.c_str(),0) != -1;
}
