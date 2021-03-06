#pragma once

#include "vectors.h"
#include <vector>

#define PointLine(point, line) PointOnLine(point, line)
#define LinePoint(line, point) PointOnLine(point, line)
#define CircleLine(circle, line) LineCircle(line, circle)
#define RectangleLine(rectangle, line) LineRectange(line, rectangle)
#define OrientedRectangleLine(rectangle, line) LineOrientedRectangle(line, rectangle)
#define RectangleCircle(rectangle, circle) CircleRectangle(circle, rectangle)
#define OrientedRectangleCircle(rectangle, circle) CircleOrientedRectangle(circle, rectangle)
#define OrientedRectangleRectangle(oriented, regular) RectangleOrientedRectangle(regular, oriented)

typedef vec2 Point2D;

typedef struct Line2D {
	Point2D start;
	Point2D end;

	inline Line2D() {}
	inline Line2D(const Point2D& s, const Point2D& e) : start(s), end(e) {}
} Line2D;
typedef struct Circle {
	Point2D position;
	float radius;

	inline Circle() : radius(1.0f) {}
	inline Circle(const Point2D& p, float r) : position(p), radius(r) {}
} Circle;
typedef struct Rectangle2D {
	Point2D origin;
	vec2 size;

	inline Rectangle2D() : size(1, 1) {}
	inline Rectangle2D(const Point2D& o, const vec2& s) : origin(o), size(s) {}
} Rectangle2D;
typedef struct OrientedRectangle {
	Point2D position;
	vec2 halfExtents;
	float rotation;

	inline OrientedRectangle() : halfExtents(1.0f, 1.0f), rotation(0.0f) {}
	inline OrientedRectangle(const Point2D& p, const vec2& e) : position(p), halfExtents(e), rotation(0.0f) {}
	inline OrientedRectangle(const Point2D& p, const vec2& e, float r) : position(p), halfExtents(e), rotation(r) {}
} OrientedRectangle;
typedef struct Interval2D {
	float min;
	float max;
} Interval2D;
typedef struct BoundingShape {
	std::vector<Circle> circles;
	std::vector<Rectangle2D> rectangles;
} BoundingShape;

float Lenght(const Line2D& line);
float LenghtSq(const Line2D& line);

vec2 GetMin(const Rectangle2D& rect);
vec2 GetMax(const Rectangle2D& rect);
Rectangle2D FromMinMax(const vec2& min, const vec2& max);

bool PointOnLine(const Point2D& point, const Line2D& line);
bool PointInCircle(const Point2D& point, const Circle& c);
bool PointInRectangle(const Point2D& point, const Rectangle2D& rectangle);
bool PointInOrientedRectangle(const Point2D& point, const OrientedRectangle& rectangle);

bool LineCircle(const Line2D& line, const Circle& circle);
bool LineRectangle(const Line2D& l, const Rectangle2D& r);
bool LineOrientedRectangle(const Line2D& line, const OrientedRectangle& rectangle);

bool CircleCircle(const Circle& c1, const Circle& c2);
bool CircleRectangle(const Circle& circle, const Rectangle2D& rectangle);
bool CircleOrientedRectangle(const Circle& circle, const OrientedRectangle& rectangle);
bool RectangleRectangle(const Rectangle2D& rect1, const Rectangle2D& rect2);

Interval2D GetInterval(const Rectangle2D& rect, const vec2& axis);
bool OverlapOnAxis(const Rectangle2D& rect1, const Rectangle2D& rect2, const vec2& axis);
bool RectangleRectangleSAT(const Rectangle2D& rect1, const Rectangle2D& rect2);

Interval2D GetInterval(const OrientedRectangle& rect, const vec2& axis);
bool OverlapOnAxis(const Rectangle2D& rect1, const OrientedRectangle& rect2, const vec2& axis);
bool RectangleOrientedRectangle(const Rectangle2D& rect1, const OrientedRectangle& rect2);

bool OrientedRectangleOrientedRectangle(const OrientedRectangle& r1, const OrientedRectangle& r2);

Circle ContainingCircle(std::vector<Point2D> points);

Rectangle2D ContainingRectangle(std::vector<Point2D> points);

bool PointInShape(const BoundingShape& shape, const Point2D& point);
