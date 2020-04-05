#pragma once

#include "pch.h"
#include <math.h>
#include "mymath.h"

//-------------------------------------------
tVec3 vectorNull() {
	tVec3 v;
	v.x = 0.f;
	v.y = 0.f;
	v.z = 0.f;
	return v;
}

void vectorCopy(tVec3 c, tVec3 v) {
	c.x = v.x;
	c.y = v.y;
	c.z = v.z;
}

bool vectorSame(tVec3 a, tVec3 b, float epsilon) {
	return (fabsf(a.x - b.x) < epsilon && fabsf(a.y - b.y) < epsilon && fabsf(a.z - b.z) < epsilon);
}

tVec3 vectorNeg(tVec3 a) {
	tVec3 v;
	v.x = -a.x;
	v.y = -a.y;
	v.z = -a.z;
	return v;
}

tVec3 vectorAdd(tVec3 a, tVec3 b) {
	tVec3 v;
	v.x = a.x + b.x;
	v.y = a.y + b.y;
	v.z = a.z + b.z;
	return v;
}

tVec3 vectorSub(tVec3 a, tVec3 b) {
	tVec3 v;
	v.x = a.x - b.x;
	v.y = a.y - b.y;
	v.z = a.z - b.z;
	return v;
}

tVec3 vectorScale(tVec3 a, float s) {
	tVec3 v;
	v.x = a.x * s;
	v.y = a.y * s;
	v.z = a.z * s;
	return v;
}

tVec4 vector4Scale(tVec4 a, float s) {
	tVec4 v;
	v.x = a.x * s;
	v.y = a.y * s;
	v.z = a.z * s;
	v.w = a.w * s;
	return v;
}

float vectorDot(tVec3 a, tVec3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

tVec3 vectorCross(tVec3 a, tVec3 b) {
	tVec3 v;
	v.x = a.y * b.z - a.z * b.y;
	v.y = a.z * b.x - a.x * b.z;
	v.z = a.x * b.y - a.y * b.x;
	return v;
}

float vectorLength(tVec3 a) {
	return (float)sqrt(vectorDot(a, a));
}

tVec3 vectorNormalizeLen(tVec3 a, float len) {
	if (len == 0.f) {
		return vectorNull();
	}
	else {
		float rLen = 1.f / len;
		return vectorScale(a, rLen);
	}
}

tVec3 vectorNormalize(tVec3 a) {
	return vectorNormalizeLen(a, vectorLength(a));
}

//-------------------------------------------
tMatrix4x4 matrixIdentity(void) {
	tMatrix4x4 m;
	m.row0 = { 1.f, 0.f, 0.f, 0.f };
	m.row1 = { 0.f, 1.f, 0.f, 0.f };
	m.row2 = { 0.f, 0.f, 1.f, 0.f };
	m.row3 = { 0.f, 0.f, 0.f, 1.f };
	return m;
}

tMatrix4x4 matrixScale(tMatrix4x4 m, float s) {
	tMatrix4x4 ms;
	ms.row0 = vector4Scale(m.row0, s);
	ms.row1 = vector4Scale(m.row1, s);
	ms.row2 = vector4Scale(m.row2, s);
	ms.row3 = vector4Scale(m.row3, s);
	return ms;
}

float matrixDetInternal(float a1, float a2, float a3, float b1, float b2, float b3, float c1, float c2, float c3) {
	return (a1 * (b2 * c3 - b3 * c2) -
			b1 * (a2 * c3 - a3 * c2) +
			c1 * (a2 * b3 - a3 * b2));
}

float matrixDeterminant(tMatrix4x4 m) {
	return (m.m[0][0] * matrixDetInternal(m.m[1][1], m.m[2][1], m.m[3][1], m.m[1][2], m.m[2][2], m.m[3][2], m.m[1][3], m.m[2][3], m.m[3][3]) -
			m.m[0][1] * matrixDetInternal(m.m[1][0], m.m[2][0], m.m[3][0], m.m[1][2], m.m[2][2], m.m[3][2], m.m[1][3], m.m[2][3], m.m[3][3]) +
			m.m[0][2] * matrixDetInternal(m.m[1][0], m.m[2][0], m.m[3][0], m.m[1][1], m.m[2][1], m.m[3][1], m.m[1][3], m.m[2][3], m.m[3][3]) -
			m.m[0][3] * matrixDetInternal(m.m[1][0], m.m[2][0], m.m[3][0], m.m[1][1], m.m[2][1], m.m[3][1], m.m[1][2], m.m[2][2], m.m[3][2]));
}

tMatrix4x4 matrixAdjoint(tMatrix4x4 m) {
	tMatrix4x4 ma;
	ma.m[0][0] = matrixDetInternal(m.m[1][1], m.m[2][1], m.m[3][1], m.m[1][2], m.m[2][2], m.m[3][2], m.m[1][3], m.m[2][3], m.m[3][3]);
	ma.m[1][0] = -matrixDetInternal(m.m[1][0], m.m[2][0], m.m[3][0], m.m[1][2], m.m[2][2], m.m[3][2], m.m[1][3], m.m[2][3], m.m[3][3]);
	ma.m[2][0] = matrixDetInternal(m.m[1][0], m.m[2][0], m.m[3][0], m.m[1][1], m.m[2][1], m.m[3][1], m.m[1][3], m.m[2][3], m.m[3][3]);
	ma.m[3][0] = -matrixDetInternal(m.m[1][0], m.m[2][0], m.m[3][0], m.m[1][1], m.m[2][1], m.m[3][1], m.m[1][2], m.m[2][2], m.m[3][2]);

	ma.m[0][1] = -matrixDetInternal(m.m[0][1], m.m[2][1], m.m[3][1], m.m[0][2], m.m[2][2], m.m[3][2], m.m[0][3], m.m[2][3], m.m[3][3]);
	ma.m[1][1] = matrixDetInternal(m.m[0][0], m.m[2][0], m.m[3][0], m.m[0][2], m.m[2][2], m.m[3][2], m.m[0][3], m.m[2][3], m.m[3][3]);
	ma.m[2][1] = -matrixDetInternal(m.m[0][0], m.m[2][0], m.m[3][0], m.m[0][1], m.m[2][1], m.m[3][1], m.m[0][3], m.m[2][3], m.m[3][3]);
	ma.m[3][1] = matrixDetInternal(m.m[0][0], m.m[2][0], m.m[3][0], m.m[0][1], m.m[2][1], m.m[3][1], m.m[0][2], m.m[2][2], m.m[3][2]);

	ma.m[0][2] = matrixDetInternal(m.m[0][1], m.m[1][1], m.m[3][1], m.m[0][2], m.m[1][2], m.m[3][2], m.m[0][3], m.m[1][3], m.m[3][3]);
	ma.m[1][2] = -matrixDetInternal(m.m[0][0], m.m[1][0], m.m[3][0], m.m[0][2], m.m[1][2], m.m[3][2], m.m[0][3], m.m[1][3], m.m[3][3]);
	ma.m[2][2] = matrixDetInternal(m.m[0][0], m.m[1][0], m.m[3][0], m.m[0][1], m.m[1][1], m.m[3][1], m.m[0][3], m.m[1][3], m.m[3][3]);
	ma.m[3][2] = -matrixDetInternal(m.m[0][0], m.m[1][0], m.m[3][0], m.m[0][1], m.m[1][1], m.m[3][1], m.m[0][2], m.m[1][2], m.m[3][2]);

	ma.m[0][3] = -matrixDetInternal(m.m[0][1], m.m[1][1], m.m[2][1], m.m[0][2], m.m[1][2], m.m[2][2], m.m[0][3], m.m[1][3], m.m[2][3]);
	ma.m[1][3] = matrixDetInternal(m.m[0][0], m.m[1][0], m.m[2][0], m.m[0][2], m.m[1][2], m.m[2][2], m.m[0][3], m.m[1][3], m.m[2][3]);
	ma.m[2][3] = -matrixDetInternal(m.m[0][0], m.m[1][0], m.m[2][0], m.m[0][1], m.m[1][1], m.m[2][1], m.m[0][3], m.m[1][3], m.m[2][3]);
	ma.m[3][3] = matrixDetInternal(m.m[0][0], m.m[1][0], m.m[2][0], m.m[0][1], m.m[1][1], m.m[2][1], m.m[0][2], m.m[1][2], m.m[2][2]);
	return ma;
}

tMatrix4x4 matrixInverse(tMatrix4x4 m) {
	tMatrix4x4 mi;
	float det = matrixDeterminant(m);
	if (fabsf(det) < 0.0001f) {
		mi = matrixIdentity();
	}
	else {
		mi = matrixAdjoint(m);
		mi = matrixScale(mi, 1.f / det);
	}
	return mi;
}

//-------------------------------------------
tVec3 planeNormal(tVec3 a, tVec3 b, tVec3 c) {
	return vectorNormalize(vectorCross(vectorSub(c, b), vectorSub(a, b)));
}

tVec3 planeOrigin(tVec3 normal, float distanceToOrigin) {
	return vectorScale(normal, distanceToOrigin);
}

float planeDistance(tVec3 normal, tVec3 pointOnPlane) {
	return -vectorDot(normal, pointOnPlane);
}

tPlane planeFromPoints(tVec3 a, tVec3 b, tVec3 c) {
	tPlane p;
	p.normal = planeNormal(a, b, c);
	p.distance = planeDistance(p.normal, a);
	return p;
}

float planeHalfSpace(tPlane p, tVec3 point) {
	return (vectorDot(p.normal, point) + p.distance);
}

bool planePointBack(tPlane p, tVec3 point) {
	return (planeHalfSpace(p, point) < 0.f);
}

bool planePointFront(tPlane p, tVec3 point) {
	return (planeHalfSpace(p, point) > 0.f);
}

bool planePointOn(tPlane p, tVec3 point) {
	return (planeHalfSpace(p, point) == 0.f);
}

bool planesIntersectionPoint(tPlane p1, tPlane p2, tPlane p3, tVec3 *s) {
	tMatrix4x4 m = {
		p1.normal.x, p1.normal.y, p1.normal.z, -p1.distance,
		p2.normal.x, p2.normal.y, p2.normal.z, -p2.distance,
		p3.normal.x, p3.normal.y, p3.normal.z, -p3.distance,
		0.f, 0.f, 0.f, 1.f
	};
	float det = matrixDeterminant(m);
	if (fabsf(det) < 0.0001f) return false;
	tMatrix4x4 mi = matrixInverse(m);
	s->x = mi.row0.w;
	s->y = mi.row1.w;
	s->z = mi.row2.w;
	return true;
}

//-------------------------------------------
bool isClockwise(tTriangle t) {
	// the centroid / center of gravity
	tVec3 center = vectorAdd(t.point[0],t.point[1]);
	center = vectorAdd(center, t.point[2]);
	center = vectorScale(center, (1.f / 3.f));
	//
	int smallest = -1;
	float smallestAngle = -1.f;
	int i;
	//
	for (i = 0; i <= 1; i++) {
		smallest = -1;
		smallestAngle = -1.f;
		tVec3 N = planeNormal(t.point[0],t.point[1],t.point[2]);
		tVec3 chkN = planeNormal(t.point[i], center, vectorAdd(center, N));
		tPlane p;
		p.normal = chkN;
		p.distance = planeDistance(chkN, t.point[i]);
		tVec3 A = vectorNormalize(vectorSub(t.point[i], center));
		for (int j = i+1; j <= 2; j++) {
			if (vectorDot(p.normal,t.point[j])+p.distance >= 0) {
				tVec3 B = vectorNormalize(vectorSub(t.point[j], center));
				float Angle = vectorDot(A, B);
				if (Angle >= smallestAngle) {
					smallestAngle = Angle;
					smallest = j;
				}
			}

		}
	}
	return (smallest != -1 && smallest != i + 1);
}