#pragma once

#include "BBox.h"
#include "Object.h"



bool inline bboxCollided(BBox b1 , BBox b2)
{



	return (b1.min[0] <= b2.min[0] + fabs(b2.min[0] - b2.max[0]) &&
		b2.min[0] <= b1.min[0] + fabs(b1.min[0] - b1.max[0]) &&

		b1.min[1] <= b2.min[1] + fabs(b2.min[1] - b2.max[1]) &&
		b2.min[1] <= b1.min[1] + fabs(b1.min[1] - b1.max[1]) &&

		b1.min[2] <= b2.min[2] + fabs(b2.min[2] - b2.max[2]) &&
		b2.min[2] <= b1.min[2] + fabs(b1.min[2] - b1.max[2])) 
		;
}

bool inline objectBoxCollided(BBox b1 , Object b2)
{
	b2.box.min[0] +=b2.position.s[0];
	b2.box.min[1] +=b2.position.s[1];
	b2.box.min[2] +=b2.position.s[2];

	b2.box.max[0] +=b2.position.s[0];
	b2.box.max[1] +=b2.position.s[1];
	b2.box.max[2] +=b2.position.s[2];


	return bboxCollided(b1, b2.box);
}
