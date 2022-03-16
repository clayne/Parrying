#include <cassert>
#include <iostream>

#include "Geom.h"

const float FLOAT_DELTA = 0.00005f;

Point3 rotate(float r, const Point3& rotation)
{
	Point3 ans;

	float gamma = -rotation.z + 3.1415926f / 2, beta = rotation.x;
	float cos_g = cos(gamma);
	float sin_g = sin(gamma);
	float cos_b = cos(beta);
	float sin_b = sin(beta);

	ans.x = r * cos_g * cos_b;
	ans.y = r * sin_g * cos_b;
	ans.z = r * -sin_b;

	return ans;
}

Point3 rotateZ(float r, const Point3& rotation)
{
	Point3 ans;

	float gamma = -rotation.z + 3.1415926f / 2;
	float cos_g = cos(gamma);
	float sin_g = sin(gamma);

	ans.x = r * cos_g;
	ans.y = r * sin_g;
	ans.z = 0.0f;

	return ans;
}

static float Clamp01(float t)
{
	return std::max(0.0f, std::min(1.0f, t));
}

static Point3 Lerp(const Point3& A, const Point3& B, float k)
{
	return A + (B - A) * k;
}

static Point3 constrainToSegment(const Point3& position, const Point3& a, const Point3& b)
{
	auto ba = b - a;
	auto t = ba * (position - a) / (ba * ba);
	return Lerp(a, b, Clamp01(t));
}

float dist(const Point3& A, const Point3& B, const Point3& C)
{
	return constrainToSegment(C, A, B).GetDistance(C);
}

float dist(const Point3& A, const Point3& B, const Point3& C, const Point3& D)
{
	auto CD = D - C;
	float CD2 = CD * CD;
	auto inPlaneA = A - (CD * (CD * (A - C) / CD2));
	auto inPlaneB = B - (CD * (CD * (B - C) / CD2));
	auto inPlaneBA = inPlaneB - inPlaneA;
	float inPlaneBA2 = inPlaneBA * inPlaneBA;
	float t;
	if (inPlaneBA2 < FLOAT_DELTA)
		t = 0.0f;
	else
		t = inPlaneBA * (C - inPlaneA) / inPlaneBA2;
	auto segABtoLineCD = Lerp(A, B, Clamp01(t));

	return dist(A, B, constrainToSegment(segABtoLineCD, C, D));
}
