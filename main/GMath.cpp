#include <math.h>

#include "GMath.h"

//CS 1.6 CalcAngle ftw
GVector GMath::CalcAngle(const GVector src, const GVector dst) 
{
	GVector angle, delta = MakeDelta(src, dst);

	float hyp = delta.Mag2D() + static_cast<float>(0.001);

	angle.x = atanf(delta.y / hyp);
	angle.y = atanf(delta.z / delta.x);

	if (delta.x >= 0 && delta.z >= 0)
		angle.y -= PI_2;
	if (delta.x < 0 && delta.z >= 0)
		angle.y += PI_2;
	if (delta.x < 0 && delta.z < 0)
		angle.y += PI_2;
	if (delta.x >= 0 && delta.z < 0)
		angle.y -= PI_2;

	return angle;
}


GVector GMath::MakeDelta(const GVector src, const GVector dst)
{
	GVector delta = src - dst;
	return delta;
}