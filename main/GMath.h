#ifndef __GMATH__
#define __GMATH__

class GVector;


const float PI = static_cast<float>(3.14159265358979323846);
const float PI_2 = static_cast<float>(1.57079632679489661923);

class GMath {
public:
	GMath();

	static GVector CalcAngle(const GVector src, const GVector dst);
	static GVector MakeDelta(const GVector src, const GVector dst);
private:
};

class GVector
{
public:
	GVector(float X = 0, float Y = 0, float Z = 0) { x = X; y = Y; z = Z; }

	bool operator ==(GVector &v) { return (x == v.x && y == v.y && z == v.z); }
	bool operator !=(GVector &v) { return !(*this == v); }
	GVector operator +(float v) const { return GVector(x + v, y + v, z + v); };
	GVector operator +(const GVector &v) const { return GVector(x + v.x, y + v.y, z + v.z); };
	GVector operator -(float v) const { return GVector(x - v, y - v, z - v); };
	GVector operator -(const GVector &v) const { return GVector(x - v.x, y - v.y, z - v.z); };
	GVector operator *(float v) const { return GVector(x * v, y * v, z * v); };
	GVector operator *(const GVector &v) const { return GVector(x * v.x, y * v.y, z * v.z); };
	GVector operator /(float v) const { return GVector(x / v, y / v, z / v); };
	GVector operator /(const GVector &v) const { return GVector(x / v.x, y / v.y, z / v.z); };
	GVector &operator =(const GVector &v) { x = v.x; y = v.y; z = v.z; return *this; }
	GVector &operator =(float v) { x = v; y = v; z = v; return *this; }
	GVector &operator -() { x = -x; y = -y; z = -z; return *this; }
	GVector &operator +=(const GVector &v) { x += v.x; y += v.y; z += v.z; return *this; };
	GVector &operator +=(float v) { x += v; y += v; z += v; return *this; };
	GVector &operator -=(const GVector &v) { x -= v.x; y -= v.y; z -= v.z; return *this; };
	GVector &operator -=(float v) { x -= v; y -= v; z -= v; return *this; };
	GVector &operator *=(const GVector &v) { x *= v.x; y *= v.y; z *= v.z; return *this; };
	GVector &operator *=(float v) { x *= v; y *= v; z *= v; return *this; };
	GVector &operator /=(const GVector &v) { x /= v.x; y /= v.y; z /= v.z; return *this; };
	GVector &operator /=(float v) { x /= v; y /= v; z /= v; return *this; };
	float MagSqr() { return x*x + y*y + z*z; }
	float Mag2D() { return sqrtf(x*x + z*z); }
	float Mag() { return sqrtf(x*x + y*y + z*z); }
	float Dist(const GVector &v) { return (*this - v).Mag(); }
	float DistSqr(const GVector &v) { return (*this - v).MagSqr(); }
	float Dot(const GVector &v) { return x*v.x + y*v.y + z*v.z; }
	GVector Cross(const GVector &v) { return GVector(v.y*z - v.z*y, v.z*x - v.x*z, v.x*y - v.y*x); };
	GVector GetUnit() { return (*this) / Mag(); }
	void Normalize() { (*this) /= Mag(); }
	void SetMagnitude(float v) { (*this) *= v / Mag(); }
	GVector Lerp(const GVector &v, float t) { GVector(x + (v.x - x) * t, y + (v.y - y) * t, z + (v.z - z) * t); };

	float x, y, z;
};

#endif