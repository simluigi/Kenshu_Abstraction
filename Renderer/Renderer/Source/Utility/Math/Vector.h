#pragma once

template<typename T>
struct Vector2
{
	Vector2() : x(), y() {};
	Vector2(T x, T y) : x(x), y(y) {};

	T x, y;
};

using Int2 = Vector2<int>;
using Float2 = Vector2<float>;

template<typename T>
struct Vector3
{
	Vector3() : x(), y(), z() {};
	Vector3(T x, T y, T z) : x(x), y(y), z(z) {};

	T x, y, z;

	Vector3<T> operator-(const Vector3<T>& vec) { return Vector3<T>(x - vec.x, y - vec.y, z - vec.z); }
	Vector3<T> operator*(float scale) { return Vector3<T>(x * scale, y * scale, z * scale); }
	void operator+=(const Vector3<T> vec) { x += vec.x; y += vec.y; z += vec.z; }
};

using Float3 = Vector3<float>;

template<typename T>
struct Vector4
{
	Vector4() : x(), y(), z(), w() {};
	Vector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {};

	T x, y, z, w;
};

using Float4 = Vector4<float>;

namespace Vector
{
	Float3 Normalize(const Float3& vec);
}