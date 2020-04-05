#pragma once

#include "pch.h"
#include <math.h>

//-------------------------------------------
typedef struct {
	float x, y, z;
} tVec3;

typedef struct {
	float x, y, z, w;
} tVec4;

union tMatrix4x4 {
	struct {
		tVec4 row0;
		tVec4 row1;
		tVec4 row2;
		tVec4 row3;
	};
	float m16[16];
	float m[4][4];
};

typedef struct {
	tVec3 normal;
	float distance;
} tPlane;

typedef struct {
	tVec3 point;
	tVec3 direction;
} tLine;

typedef struct {
	tVec3 point[3];
} tTriangle;

//-------------------------------------------
tVec3 vectorNull();
void vectorCopy(tVec3 c, tVec3 v);
bool vectorSame(tVec3 a, tVec3 b, float epsilon = 0.0001f);
tVec3 vectorNeg(tVec3 a);
tVec3 vectorAdd(tVec3 a, tVec3 b);
tVec3 vectorSub(tVec3 a, tVec3 b);
tVec3 vectorScale(tVec3 a, float s);
tVec4 vector4Scale(tVec4 a, float s);
float vectorDot(tVec3 a, tVec3 b);
tVec3 vectorCross(tVec3 a, tVec3 b);
float vectorLength(tVec3 a);
tVec3 vectorNormalizeLen(tVec3 a, float len);
tVec3 vectorNormalize(tVec3 a);
//-------------------------------------------
tMatrix4x4 matrixIdentity(void);
tMatrix4x4 matrixScale(tMatrix4x4 m, float s);
float matrixDetInternal(float a1, float a2, float a3, float b1, float b2, float b3, float c1, float c2, float c3);
float matrixDeterminant(tMatrix4x4 m);
tMatrix4x4 matrixAdjoint(tMatrix4x4 m);
tMatrix4x4 matrixInverse(tMatrix4x4 m);
//-------------------------------------------
tVec3 planeNormal(tVec3 a, tVec3 b, tVec3 c);
tVec3 planeOrigin(tVec3 normal, float distanceToOrigin);
float planeDistance(tVec3 normal, tVec3 pointOnPlane);
tPlane planeFromPoints(tVec3 a, tVec3 b, tVec3 c);
float planeHalfSpace(tPlane p, tVec3 point);
bool planePointBack(tPlane p, tVec3 point);
bool planePointFront(tPlane p, tVec3 point);
bool planePointOn(tPlane p, tVec3 point);
bool planesIntersectionPoint(tPlane p1, tPlane p2, tPlane p3, tVec3 *s);
//-------------------------------------------
bool isClockwise(tTriangle);