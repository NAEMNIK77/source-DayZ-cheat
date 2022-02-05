#ifndef __GAIMBOT__
#define __GAIMBOT__

#include "GSDK.h"

class GAimbot {
public:
	GAimbot();
	void Reset();
	void Collect(CObject *obj);
	void Do();
private:
	bool locked;
	float distance;
	float buf_distance;
	CObject *nearest_object;

	GVector target_pos;
	GVector old_pos;
	GVector old_viewangles;
};

#endif