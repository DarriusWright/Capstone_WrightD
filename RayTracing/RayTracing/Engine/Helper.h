#pragma once

#include <BoundingBox\BBox.h>
#include <Objects\Object.h>
#include <Objects\Triangle.h>
#include <Objects\Mesh.h>

Object inline createObjectFromTriangle(Triangle triangle, glm::vec3 position)
{
	BBox box  = {glm::min(glm::min(triangle.v0, triangle.v1), triangle.v2),0.0f, glm::max(glm::max(triangle.v0, triangle.v1), triangle.v2), 0.0f};
	Object retObj;

	retObj.box = box;
	//retObj.position = glm::vec4(position,0.0f);


	return retObj;
}

bool inline bboxCollided(BBox b1 , BBox b2)
{
	/*bool xCollision = (b1.min[0] <= b2.min[0] + fabs(b2.min[0] - b2.max[0]) &&
		b2.min[0] <= b1.min[0] + fabs(b1.min[0] - b1.max[0]));
	bool yCollision = b1.min[1] <= b2.min[1] + fabs(b2.min[1] - b2.max[1]) &&
		b2.min[1] <= b1.min[1] + fabs(b1.min[1] - b1.max[1]);
	bool zCollision = b1.min[2] <= b2.min[2] + fabs(b2.min[2] - b2.max[2]) &&
		b2.min[2] <= b1.min[2] + fabs(b1.min[2] - b1.max[2]);
*/
	bool xCollision = !(b1.max[0] < b2.min[0] || b1.min[0] > b2.max[0] );
	bool yCollision = !(b1.max[1] < b2.min[1] || b1.min[1] > b2.max[1] );
	bool zCollision = !(b1.max[2] < b2.min[2] || b1.min[2] > b2.max[2] );

	return xCollision && yCollision && zCollision;
}

bool inline objectBoxCollided(BBox b1 , Object b2,const  Mesh m)
{
	b2.box.min[0] +=m.position.s[0];
	b2.box.min[1] +=m.position.s[1];
	b2.box.min[2] +=m.position.s[2];
							  
	b2.box.max[0] +=m.position.s[0];
	b2.box.max[1] +=m.position.s[1];
	b2.box.max[2] +=m.position.s[2];
							  

	return bboxCollided(b1, b2.box);
}




