
#pragma once
#include <cmath>
#include <vector>

const unsigned int MaxUInt32 = 0xFFFFFFFF;
const int MinInt32 = 0x80000000;
const int MaxInt32 = 0x7FFFFFFF;
const float Maxloat = 3.402823466e+38F;
const float MinPosFloat = 1.175494351e-38F;
const float MATH_FLOAT_SMALL = 1.0e-37f;
const float MATH_TOLERANCE = 2e-37f;
const float Pi = 3.141592654f;
const float TwoPi = 6.283185307f;
const float PiHalf = 1.570796327f;

const float dtor = 0.0174532925199432957692369076848861f;
const float rtod = 1 / dtor;

const float Epsilon = 0.000001f;
const float ZeroEpsilon = 32.0f * MinPosFloat; // Very small epsilon for checking against 0.0f

const float M_INFINITY = 1.0e30f;

template <typename T>
T Min(const T &a, const T &b) { return (a < b) ? a : b; }

template <typename T>
T Max(const T &a, const T &b) { return (a > b) ? a : b; }

#define powi(base, exp) (int)powf((float)(base), (float)(exp))

#define ToRadians(x) (float)(((x) * Pi / 180.0f))
#define ToDegrees(x) (float)(((x) * 180.0f / Pi))

inline float Sin(float a) { return sin(a * Pi / 180); }
inline float Cos(float a) { return cos(a * Pi / 180); }
inline float Tan(float a) { return tan(a * Pi / 180); }
inline float SinRad(float a) { return sin(a); }
inline float CosRad(float a) { return cos(a); }
inline float TanRad(float a) { return tan(a); }
inline float ASin(float a) { return asin(a) * 180 / Pi; }
inline float ACos(float a) { return acos(a) * 180 / Pi; }
inline float ATan(float a) { return atan(a) * 180 / Pi; }
inline float ATan2(float y, float x) { return atan2(y, x) * 180 / Pi; }
inline float ASinRad(float a) { return asin(a); }
inline float ACosRad(float a) { return acos(a); }
inline float ATanRad(float a) { return atan(a); }
inline float ATan2Rad(float y, float x) { return atan2(y, x); }
inline int Floor(float a) { return (int)(floor(a)); }
inline int Ceil(float a) { return (int)(ceil(a)); }
inline int Trunc(float a)
{
	if (a > 0)
		return Floor(a);
	else
		return Ceil(a);
}
inline int Round(float a)
{
	if (a < 0)
		return (int)(ceil(a - 0.5f));
	else
		return (int)(floor(a + 0.5f));
}
inline float Sqrt(float a)
{
	if (a > 0)
		return sqrt(a);
	else
		return 0;
}
inline float Abs(float a)
{
	if (a < 0)
		a = -a;
	return a;
}
inline int Mod(int a, int b)
{
	if (b == 0)
		return 0;
	return a % b;
}
inline float FMod(float a, float b)
{
	if (b == 0)
		return 0;
	return fmod(a, b);
}
inline float Pow(float a, float b) { return pow(a, b); }
inline int Sign(float a)
{
	if (a < 0)
		return -1;
	else if (a > 0)
		return 1;
	else
		return 0;
}
inline float Min(float a, float b) { return a < b ? a : b; }
inline float Max(float a, float b) { return a > b ? a : b; }
inline float Clamp(float a, float min, float max)
{
	if (a < min)
		a = min;
	else if (a > max)
		a = max;
	return a;
}
inline int Clamp(int a, int min, int max)
{
	if (a < min)
		a = min;
	else if (a > max)
		a = max;
	return a;
}

inline bool isnotzero(const float a, const float tolerance = Epsilon)
{
	return fabsf(a) > tolerance;
}

inline bool isequal(const float a, const float b, const float tolerance = Epsilon)
{
	if (fabs(b - a) <= tolerance)
		return true;
	else
		return false;
}

struct Vec2
{

	float x, y;

	Vec2();
	Vec2(float x, float y);

	// Getters/Setters
	float getX() const;
	float getY() const;
	void setX(float x);
	void setY(float y);

	static double Distance(const Vec2 &a, const Vec2 &b);
	static double DistanceSquared(const Vec2 &a, const Vec2 &b);
	void set(float x, float y) { this->x = x; this->y = y; }

	// Operations
	Vec2 operator+(const Vec2 &other) const;
	Vec2 operator-(const Vec2 &other) const;
	Vec2 operator*(float scalar) const;
	float dot(const Vec2 &other) const;
	float length() const;
	Vec2 normalize() const;
};

struct Vec3
{

	static constexpr float EPSILON = 1e-6f;

	float x, y, z;

	Vec3();
	Vec3(float x, float y, float z);

	float lengthSquared() const { return x * x + y * y + z * z; }


	// Getters/Setters
	float getX() const;
	float getY() const;
	float getZ() const;
	void setX(float x);
	void setY(float y);
	void setZ(float z);

	void set(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }

	// Operations
	Vec3 operator+(const Vec3 &other) const;
	Vec3 operator-(const Vec3 &other) const;
	Vec3 operator*(float scalar) const;
	float dot(const Vec3 &other) const;
	Vec3 cross(const Vec3 &other) const;
	float length() const;
	Vec3 normalize() const;
	static Vec3 Mult(const Vec3 &a, const Vec3 &b);
	static Vec3 Div(const Vec3 &a, const Vec3 &b);
	static Vec3 Min(const Vec3 &a, const Vec3 &b);
	static Vec3 Max(const Vec3 &a, const Vec3 &b);
	static double Distance(const Vec3 &a, const Vec3 &b);

	Vec3 operator-() const
	{ // negação
		return Vec3(-x, -y, -z);
	}

	Vec3 &operator+=(const Vec3 &other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	Vec3 &operator-=(const Vec3 &other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	Vec3 &operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	bool operator==(const Vec3 &other) const
	{
		return std::abs(x - other.x) < EPSILON &&
			   std::abs(y - other.y) < EPSILON &&
			   std::abs(z - other.z) < EPSILON;
	}
};

struct Vec4
{
	float x, y, z, w;

	Vec4() : x(0), y(0), z(0), w(0) {}
	Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	Vec4(const Vec3 &v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}

	Vec4 operator+(const Vec4 &other) const
	{
		return Vec4(x + other.x, y + other.y, z + other.z, w + other.w);
	}

	Vec4 operator-(const Vec4 &other) const
	{
		return Vec4(x - other.x, y - other.y, z - other.z, w - other.w);
	}

	Vec4 operator*(float scalar) const
	{
		return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
	}

	Vec4 &operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;
		return *this;
	}

	float dot(const Vec4 &other) const
	{
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}

	float dot(const Vec3 &v) const
	{
		return x * v.x + y * v.y + z * v.z + w;
	}

	float length() const
	{
		return sqrt(x * x + y * y + z * z + w * w);
	}

	Vec4 normalize() const
	{
		float len = length();
		if (len > 0)
		{
			float invLen = 1.0f / len;
			return Vec4(x * invLen, y * invLen, z * invLen, w * invLen);
		}
		return *this;
	}

	Vec3 toVec3() const
	{
		return Vec3(x, y, z);
	}

	static Vec4 Min(const Vec4 &a, const Vec4 &b)
	{
		return Vec4(
			std::min(a.x, b.x),
			std::min(a.y, b.y),
			std::min(a.z, b.z),
			std::min(a.w, b.w));
	}

	static Vec4 Max(const Vec4 &a, const Vec4 &b)
	{
		return Vec4(
			std::max(a.x, b.x),
			std::max(a.y, b.y),
			std::max(a.z, b.z),
			std::max(a.w, b.w));
	}

	bool operator==(const Vec4 &other) const
	{
		return std::abs(x - other.x) < Epsilon &&
			   std::abs(y - other.y) < Epsilon &&
			   std::abs(z - other.z) < Epsilon &&
			   std::abs(w - other.w) < Epsilon;
	}
};

struct Quat
{

	float w, x, y, z;

	Quat();
	Quat(float w, float x, float y, float z);
	Quat(float angle, const Vec3 &axis); // Rotation constructor

	// Getters/Setters
	float getW() const;
	float getX() const;
	float getY() const;
	float getZ() const;

	// Operations
	Quat operator*(const Quat &other) const;
	Quat conjugate() const;
	Quat normalize() const;
	float length() const;
	Vec3 rotate(const Vec3 &v) const;
};

struct Mat4
{
	float m[16]; // Column-major order

	Mat4(); // Identity matrix
	Mat4(const float *values);

	// Access
	float &at(int row, int col);
	float at(int row, int col) const;
	const float *data() const;

	// Basic operations
	Mat4 operator*(const Mat4 &other) const;
	Vec3 transform(const Vec3 &v) const;

	// Create transformation matrices
	static Mat4 Identity();
	static Mat4 Translate(const Vec3 &v);
	static Mat4 Rotate(const Quat &q);
	static Mat4 Scale(const Vec3 &v);
	static Mat4 Perspective(float fov, float aspect, float near, float far);
	static Mat4 LookAt(const Vec3 &eye, const Vec3 &center, const Vec3 &up);
	static Mat4 Ortho(float left, float right, float bottom, float top, float near, float far);
};

class BoundingBox
{
public:
	Vec3 min;
	Vec3 max;

	BoundingBox()
	{
		min = Vec3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		max = Vec3(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
	}
	BoundingBox(float _x0, float _y0, float _z0, float _x1, float _y1, float _z1)
	{
		min.set(_x0, _y0, _z0);
		max.set(_x1, _y1, _z1);
	}

	BoundingBox(const Vec3 &min, const Vec3 &max) : min(min), max(max) {}

	bool intersects(const Vec3 &rayOrigin, const Vec3 &rayDir, float &tMin) const
	{

		Vec3 invDir = Vec3(1.0f / rayDir.x, 1.0f / rayDir.y, 1.0f / rayDir.z);
		Vec3 t0 = Vec3::Mult((min - rayOrigin), invDir);
		Vec3 t1 = Vec3::Mult((max - rayOrigin), invDir);

		float tMinX = std::min(t0.x, t1.x);
		float tMaxX = std::max(t0.x, t1.x);

		float tMinY = std::min(t0.y, t1.y);
		float tMaxY = std::max(t0.y, t1.y);

		float tMinZ = std::min(t0.z, t1.z);
		float tMaxZ = std::max(t0.z, t1.z);

		tMin = std::max(std::max(tMinX, tMinY), tMinZ);
		float tMax = std::min(std::min(tMaxX, tMaxY), tMaxZ);

		return tMax >= tMin && tMax > 0;
	}
	void set(const Vec3 &min, const Vec3 &max)
	{
		this->min = min;
		this->max = max;
	}
	void merge(const BoundingBox &other)
	{
		min = Vec3::Min(min, other.min);
		max = Vec3::Max(max, other.max);
	}
	void merge(const Vec3 &point)
	{
		min = Vec3::Min(min, point);
		max = Vec3::Max(max, point);
	}
	void transform(const Mat4 &matrix);

	bool intersects(const BoundingBox &other) const
	{
		return (min.x <= other.max.x && max.x >= other.min.x) &&
			   (min.y <= other.max.y && max.y >= other.min.y) &&
			   (min.z <= other.max.z && max.z >= other.min.z);
	}
};

class Frustum
{
public:
	void update(const Mat4 &matrix);

	bool isPointInFrustum(float x, float y, float z) const;
	bool isSphereInFrustum(float x, float y, float z, float radius) const;
	bool isBoxInFrustum(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) const;
	bool isBoxInFrustum(const BoundingBox &box) const { return isBoxInFrustum(box.min.x, box.min.y, box.min.z, box.max.x, box.max.y, box.max.z); }

private:
	float frustum[6][4]; // Cada plano é representado por A, B, C, D

	void normalizePlane(int plane);
};

template <typename T>
struct Rect
{

	T x;
	T y;
	T width;
	T height;

	Rect() : x(0), y(0), width(0), height(0) {}
	Rect(T x, T y, T width, T height) : x(x), y(y), width(width), height(height) {}
	Rect(const Rect &rect) : x(rect.x), y(rect.y), width(rect.width), height(rect.height) {}

	void Set(T x, T y, T width, T height)
	{
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
	}

	void Merge(const Rect &rect)
	{
		T right = x + width;
		T bottom = y + height;
		T rectRight = rect.x + rect.width;
		T rectBottom = rect.y + rect.height;
		x = Min(x, rect.x);
		y = Min(y, rect.y);
		right = Max(right, rectRight);
		bottom = Max(bottom, rectBottom);
		width = right - x;
		height = bottom - y;
	}

	void Merge(const Vec2 &point)
	{
		T right = x + width;
		T bottom = y + height;
		x = Min(x, point.x);
		y = Min(y, point.y);
		right = Max(right, point.x);
		bottom = Max(bottom, point.y);
		width = right - x;
		height = bottom - y;
	}

	void Clear()
	{
		x = 0;
		y = 0;
		width = 0;
		height = 0;
	}

	Rect &operator=(const Rect &rect)
	{
		if (this == &rect)
			return *this;
		x = rect.x;
		y = rect.y;
		width = rect.width;
		height = rect.height;
		return *this;
	}
};

template <typename T>
struct Size
{
	T width;
	T height;

	Size() : width(0), height(0) {}
	Size(T w, T h) : width(w), height(h) {}
	Size(const Size &size) : width(size.width), height(size.height) {}

	Size &operator=(const Size &size)
	{
		if (this == &size)
			return *this;
		width = size.width;
		height = size.height;
		return *this;
	}
};

typedef Rect<int> IntRect;
typedef Rect<float> FloatRect;
typedef Size<int> IntSize;
typedef Size<float> FloatSize;
