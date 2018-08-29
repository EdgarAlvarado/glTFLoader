#include "Geometry3D.h"
#include <cmath>
#include <cfloat>

#define CMP(x, y) \
		(fabsf((x)-(y)) <= FLT_EPSILON *	\
			fmaxf(1.0f,						\
			fmaxf(fabsf(x), fabsf(y)))		\
		)
#define CLAMP(number, minimum, maximum)		\
		number = (number < minimum) ?		\
		minimum : (number > maximum ?		\
		maximum : number)
#define OVERLAP(aMin, aMax, bMin, bMax) ((bMin <= aMax) && (aMin <= bMax))

float Lenght(const Line& line)
{
	return Magnitude(line.start - line.end);
}
float LenghtSq(const Line& line)
{
	return MagnitudeSq(line.start - line.end);
}

Ray FromPoints(const Point& from, const Point& to)
{
	return Ray(from, Normalized(to - from));
}

vec3 GetMin(const AABB& aabb)
{
	vec3 p1 = aabb.position + aabb.size;
	vec3 p2 = aabb.position - aabb.size;

	return vec3(fminf(p1.x, p2.x),
		fminf(p1.y, p2.y),
		fminf(p1.z, p2.z));
}
vec3 GetMax(const AABB& aabb)
{
	vec3 p1 = aabb.position + aabb.size;
	vec3 p2 = aabb.position - aabb.size;

	return vec3(fmaxf(p1.x, p2.x),
		fmaxf(p1.y, p2.y),
		fmaxf(p1.z, p2.z));
}
AABB FromMinMax(const vec3& min, const vec3& max)
{
	return AABB((min + max) * 0.5f, (max - min) * 0.5f);
}

float PlaneEquation(const Point& point, const Plane& plane)
{
	return Dot(point, plane.normal) - plane.distance;
}

bool PointInSphere(const Point& point, const Sphere& sphere)
{
	return MagnitudeSq(point - sphere.position) < sphere.radius * sphere.radius;
}
Point ClosestPoint(const Sphere& sphere, const Point& point)
{
	vec3 sphereToPoint = point - sphere.position;
	Normalize(sphereToPoint);
	sphereToPoint = sphereToPoint * sphere.radius;
	return sphereToPoint + sphere.position;
}

bool PointInAABB(const Point& point, const AABB& aabb)
{
	Point min = GetMin(aabb);
	Point max = GetMax(aabb);
	return point.x > min.x && point.y > min.y && point.z > min.z &&
		point.x < max.x && point.y < max.y && point.z < max.z;
}
Point ClosestPoint(const AABB& aabb, const Point& point)
{
	Point result = point;
	Point min = GetMin(aabb);
	Point max = GetMax(aabb);

	CLAMP(result.x, min.x, max.x);
	CLAMP(result.y, min.y, max.y);
	CLAMP(result.z, min.z, max.z);
	return result;
}

bool PointInOBB(const Point& point, const OBB& obb)
{
	vec3 dir = point - obb.position;
	for (int i = 0; i < 3; ++i)
	{
		const float* orientation = &obb.orientation.asArray[i * 3];
		vec3 axis(orientation[0], orientation[1], orientation[2]);
		float distance = Dot(dir, axis);
		if (distance > obb.size.asArray[i] ||
			distance < -obb.size.asArray[i])
			return false;
	}
	return true;
}
Point ClosestPoint(const OBB& obb, const Point& point)
{
	Point result = obb.position;
	vec3 dir = point - obb.position;
	for (int i = 0; i < 3; ++i)
	{
		const float* orientation = &obb.orientation.asArray[i * 3];
		vec3 axis(orientation[0], orientation[1], orientation[2]);
		float distance = Dot(dir, axis);
		CLAMP(distance, -obb.size.asArray[i], obb.size.asArray[i]);
		result = result + (axis * distance);
	}
	return result;
}

bool PointOnPlane(const Point& point, const Plane& plane)
{
	return CMP(Dot(point, plane.normal) - plane.distance, 0.0f);
}
Point ClosestPoint(const Plane& plane, const Point& point)
{
	return point - plane.normal * (Dot(plane.normal, point) - plane.distance);
}

bool PointOnLine(const Point& point, const Line& line)
{
	Point closest = ClosestPoint(line, point);
	float distanceSq = MagnitudeSq(closest - point);
	return CMP(distanceSq, 0.0f);
}
Point ClosestPoint(const Line& line, const Point& point)
{
	vec3 lVec = line.end - line.start;
	float t = Dot(point - line.start, lVec) / Dot(lVec, lVec);
	CLAMP(t, 0.0f, 1.0f);
	return line.start + lVec * t;
}

bool PointOnRay(const Point& point, const Ray& ray)
{
	if (point == ray.origin)
		return true;
	vec3 norm = point - ray.origin;
	Normalize(norm);
	return CMP(Dot(norm, ray.direction), 1.0f);
}
Point ClosestPoint(const Ray& ray, const Point& point)
{
	float t = Dot(point - ray.origin, ray.direction);
	t = fmaxf(t, 0.0f);
	return Point(ray.origin + ray.direction * t);
}

bool SphereSphere(const Sphere& s1, const Sphere& s2)
{
	float radiiSum = s1.radius + s2.radius;
	return MagnitudeSq(s1.position - s2.position) < radiiSum * radiiSum;
}
bool SphereAABB(const Sphere& sphere, const AABB& aabb)
{
	Point closestPoint = ClosestPoint(aabb, sphere.position);
	return MagnitudeSq(sphere.position - closestPoint) < sphere.radius * sphere.radius;
}
bool SphereOBB(const Sphere& sphere, const OBB& obb)
{
	Point closestPoint = ClosestPoint(obb, sphere.position);
	return MagnitudeSq(sphere.position - closestPoint) < sphere.radius * sphere.radius;
}
bool SpherePlane(const Sphere& sphere, const Plane& plane)
{
	Point closestPoint = ClosestPoint(plane, sphere.position);
	return MagnitudeSq(sphere.position - closestPoint) < sphere.radius * sphere.radius;
}

Interval GetInterval(const AABB& aabb, const vec3& axis)
{
	vec3 min = GetMin(aabb);
	vec3 max = GetMax(aabb);

	vec3 vertex[8] = {
		vec3(min.x, max.y, max.z),
		vec3(min.x, max.y, min.z),
		vec3(min.x, min.y, max.z),
		vec3(min.x, min.y, min.z),
		vec3(max.x, max.y, max.z),
		vec3(max.x, max.y, min.z),
		vec3(max.x, min.y, max.z),
		vec3(max.x, min.y, min.z)
	};

	Interval result;
	result.min = result.max = Dot(axis, vertex[0]);
	for (int i = 1; i < 8; ++i)
	{
		float projection = Dot(axis, vertex[i]);
		if (projection < result.min)
			result.min = projection;
		if (projection > result.max)
			result.max = projection;
	}
	return result;
}
Interval GetInterval(const OBB& obb, const vec3& axis)
{
	vec3 vertex[8];
	vec3 C = obb.position; // OBB center
	vec3 E = obb.size; // OBB extents
	const float* o = obb.orientation.asArray;
	vec3 A[] = {
		vec3(o[0], o[1], o[2]),
		vec3(o[3], o[4], o[5]),
		vec3(o[6], o[7], o[8])
	};

	vertex[0] = C + (A[0] * E[0]) + (A[1] * E[1]) + (A[2] * E[2]);
	vertex[1] = C - (A[0] * E[0]) + (A[1] * E[1]) + (A[2] * E[2]);
	vertex[2] = C + (A[0] * E[0]) - (A[1] * E[1]) + (A[2] * E[2]);
	vertex[3] = C + (A[0] * E[0]) + (A[1] * E[1]) - (A[2] * E[2]);
	vertex[4] = C - (A[0] * E[0]) - (A[1] * E[1]) - (A[2] * E[2]);
	vertex[5] = C + (A[0] * E[0]) - (A[1] * E[1]) - (A[2] * E[2]);
	vertex[6] = C - (A[0] * E[0]) + (A[1] * E[1]) - (A[2] * E[2]);
	vertex[7] = C - (A[0] * E[0]) - (A[1] * E[1]) + (A[2] * E[2]);

	Interval result;
	result.min = result.max = Dot(axis, vertex[0]);
	for (int i = 1; i < 8; ++i)
	{
		float projection = Dot(axis, vertex[i]);
		if (projection < result.min)
			result.min = projection;
		if (projection > result.max)
			result.max = projection;
	}
	return result;
}
bool OverlapOnAxis(const AABB& aabb, const OBB& obb, const vec3& axis)
{
	Interval a = GetInterval(aabb, axis);
	Interval b = GetInterval(obb, axis);
	return OVERLAP(a.min, a.max, b.min, b.max);
}
bool OverlapOnAxis(const OBB& obb1, const OBB& obb2, const vec3& axis)
{
	Interval a = GetInterval(obb1, axis);
	Interval b = GetInterval(obb2, axis);
	return OVERLAP(a.min, a.max, b.min, b.max);
}

bool AABBAABB(const AABB& aabb1, const AABB& aabb2)
{
	Point aMin = GetMin(aabb1);
	Point aMax = GetMax(aabb1);
	Point bMin = GetMin(aabb2);
	Point bMax = GetMax(aabb2);

	return OVERLAP(aMin.x, aMax.x, bMin.x, bMax.x) &&
		OVERLAP(aMin.y, aMax.y, bMin.y, bMax.y) &&
		OVERLAP(aMin.z, aMax.z, bMin.z, bMax.z);
}
bool AABBOBB(const AABB& aabb, const OBB& obb)
{
	const float* o = obb.orientation.asArray;

	vec3 test[15] = {
		vec3(1, 0, 0),
		vec3(0, 1, 0),
		vec3(0, 0, 1),
		vec3(o[0], o[1], o[2]),
		vec3(o[3], o[4], o[5]),
		vec3(o[6], o[7], o[8])
	};

	for (int i = 0; i < 3; ++i)
	{
		test[6 + i * 3 + 0] = Cross(test[i], test[0]);
		test[6 + i * 3 + 1] = Cross(test[i], test[1]);
		test[6 + i * 3 + 2] = Cross(test[i], test[2]);
	}

	for (int i = 0; i < 15; ++i)
	{
		if (!OverlapOnAxis(aabb, obb, test[i]))
			return false;
	}
	return true;
}
bool AABBPlane(const AABB& aabb, const Plane& plane)
{
	float pLen = aabb.size.x * fabsf(plane.normal.x) +
				aabb.size.y * fabsf(plane.normal.y) +
				aabb.size.z * fabsf(plane.normal.z);
	return fabsf(Dot(plane.normal, aabb.position) - plane.distance) <= pLen;
}

bool OBBOBB(const OBB& obb1, const OBB& obb2)
{
	const float* o1 = obb1.orientation.asArray;
	const float* o2 = obb2.orientation.asArray;

	vec3 test[15] = {
		vec3(o1[0], o1[1], o1[2]),
		vec3(o1[3], o1[4], o1[5]),
		vec3(o1[6], o1[7], o1[8]),
		vec3(o2[0], o2[1], o2[2]),
		vec3(o2[3], o2[4], o2[5]),
		vec3(o2[6], o2[7], o2[8])
	};

	for (int i = 0; i < 3; ++i)
	{
		test[6 + i * 3 + 0] = Cross(test[i], test[0]);
		test[6 + i * 3 + 1] = Cross(test[i], test[1]);
		test[6 + i * 3 + 2] = Cross(test[i], test[2]);
	}

	for (int i = 0; i < 15; ++i)
	{
		if (!OverlapOnAxis(obb1, obb2, test[i]))
			return false;
	}
	return true;
}
bool OBBPlane(const OBB& obb, const Plane& plane)
{
	const float* o = obb.orientation.asArray;
	vec3 rot[] = {
		vec3(o[0], o[1], o[2]),
		vec3(o[3], o[4], o[5]),
		vec3(o[6], o[7], o[8])
	};
	float pLen = obb.size.x * fabsf(Dot(plane.normal, rot[0])) +
				obb.size.y * fabsf(Dot(plane.normal, rot[1])) +
				obb.size.z * fabsf(Dot(plane.normal, rot[2]));
	return fabsf(Dot(plane.normal, obb.position) - plane.distance) <= pLen;
}

bool PlanePlane(const Plane& plane1, const Plane& plane2)
{
	vec3 d = Cross(plane1.normal, plane2.normal);
	return CMP(Dot(d, d), 0.0f);
}

float Raycast(const Sphere& sphere, const Ray& ray)
{
	vec3 e = sphere.position - ray.origin;
	float rSq = sphere.radius * sphere.radius;
	float eSq = MagnitudeSq(e);
	float a = Dot(e, ray.direction);
	float bSq = eSq - (a * a);
	
	if (rSq - bSq < 0.0f)
		return -1;
	
	float f = sqrt(rSq - bSq);
	return eSq < rSq ? a + f : a - f; //inside the spehre : outside
}
float Raycast(const AABB& aabb, const Ray& ray)
{
	vec3 min = GetMin(aabb);
	vec3 max = GetMax(aabb);

	float idx = 1 / (CMP(ray.direction.x, 0.0f) ? 0.00001f : ray.direction.x);
	float idy = 1 / (CMP(ray.direction.y, 0.0f) ? 0.00001f : ray.direction.y);
	float idz = 1 / (CMP(ray.direction.z, 0.0f) ? 0.00001f : ray.direction.z);
	float t1 = (min.x - ray.origin.x) * idx;
	float t2 = (max.x - ray.origin.x) * idx;
	float t3 = (min.y - ray.origin.y) * idy;
	float t4 = (max.y - ray.origin.y) * idy;
	float t5 = (min.z - ray.origin.z) * idz;
	float t6 = (max.z - ray.origin.z) * idz;

	float tmin = fmaxf(
		fmaxf(
			fminf(t1, t2),
			fminf(t3, t4)
		),
		fminf(t5, t6)
	);
	float tmax = fminf(
		fminf(
			fmaxf(t1, t2),
			fmaxf(t3, t4)
		),
		fmaxf(t5, t6)
	);

	if (tmax < 0.0f)
		return -1;

	if (tmin > tmax)
		return -1;

	if (tmin < 0.0f)
		return tmax;

	return tmin;
}
float Raycast(const OBB& obb, const Ray& ray)
{
	const float* o = obb.orientation.asArray;
	const float* size = obb.size.asArray;
	vec3 X(o[0], o[1], o[2]);
	vec3 Y(o[3], o[4], o[5]);
	vec3 Z(o[6], o[7], o[8]);

	vec3 p = obb.position - ray.origin;

	vec3 f(
		Dot(X, ray.direction),
		Dot(Y, ray.direction),
		Dot(Z, ray.direction)
	);

	vec3 e(
		Dot(X, p),
		Dot(Y, p),
		Dot(Z, p)
	);

	float t[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	for (int i = 0; i < 3; ++i)
	{
		if (CMP(f[i], 0.0f))
		{
			if (-e[i] - size[i] > 0.0f || -e[i] + size[i] < 0.0f)
				return -1;
			f[i] = 0.00001f;
		}
		float inverse = 1.0f / f[i];
		t[i * 2 + 0] = (e[i] + size[i]) * inverse;
		t[i * 2 + 0] = (e[i] - size[i]) * inverse;
	}

	float tmin = fmaxf(
		fmaxf(
			fminf(t[1], t[2]),
			fminf(t[3], t[4])
		),
		fminf(t[5], t[6])
	);
	float tmax = fminf(
		fminf(
			fmaxf(t[1], t[2]),
			fmaxf(t[3], t[4])
		),
		fmaxf(t[5], t[6])
	);

	if (tmax < 0.0f)
		return -1;

	if (tmin > tmax)
		return -1;

	if (tmin < 0.0f)
		return tmax;

	return tmin;
}
float Raycast(const Plane& plane, const Ray& ray)
{
	float nd = Dot(ray.direction, plane.normal);
	if (nd >= 0.0f)
		return -1;
	float pn = Dot(ray.origin, plane.normal);
	float t = (plane.distance - pn) / nd;

	if (t >= 0.0f)
		return t;
	return -1;
}

bool Linetest(const Sphere& sphere, const Line& line)
{
	Point closest = ClosestPoint(line, sphere.position);
	return MagnitudeSq(sphere.position - closest) <= sphere.radius * sphere.radius;
}
bool Linetest(const AABB& aabb, const Line& line)
{
	Ray ray;
	ray.origin = line.start;
	ray.direction = Normalized(line.end - line.start);
	float t = Raycast(aabb, ray);
	return t >= 0.0f && t * t <= LenghtSq(line);
}
bool Linetest(const OBB& obb, const Line& line)
{
	Ray ray;
	ray.origin = line.start;
	ray.direction = Normalized(line.end - line.start);
	float t = Raycast(obb, ray);
	return t >= 0.0f && t * t <= LenghtSq(line);
}
bool Linetest(const Plane& plane, const Line& line)
{
	vec3 ab = line.end - line.start;

	float nA = Dot(plane.normal, line.start);
	float nAB = Dot(plane.normal, ab);

	if (CMP(nAB, 0.0f))
		return false;

	float t = (plane.distance - nA) / nAB;
	return t >= 0.0f && t <= 1.0f;
}