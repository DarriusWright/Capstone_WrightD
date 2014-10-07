__constant float epsilion = 0.00001;

typedef struct
{
	float3 min;
	float3 max;
}BBox;

typedef struct 
{
	float3 ambient;
	float3 diffuse;
	float3 specular;


}Material;


typedef struct
{
	float3 v0;
	float3 v1;
	float3 v2;
}Triangle;

typedef struct 
{
	float3 v0;
	float3 v1;
	float3 v2;
	float3 normal;
	bool hasIntersection;
	float distanceFromIntersection;

}TriangleInfo;

typedef struct
{
    Material material;
    BBox box;
    float4 position;
    int triangleIndex;
    int index;
    uint cellId;
    
    
}Object;

typedef struct
{
	float3 u;
	float3 v;
	float3 w;
	float3 up;
	float3 lookAt;
	float3 position;
	float distance;
}Camera;

typedef struct
{
	float3 direction;
	float3 origin;
}Ray;

typedef struct
{
	bool hasIntersection;
	float distanceToIntersection;
	float3 intersectionPoint;
	float3 normal;
}SphereInfo;

typedef struct
{
	Material material;
	float3 position;
}Light;

typedef struct
{
	Material material;
	float4 position;
	//float radius;
}Sphere;

typedef struct 
{
	float minValue;
	float maxValue;
	bool hit;
}HitReturn;
__constant int OCTREE_MAX_OBJECTS = 5;

typedef struct
{
	
	BBox boundingBox;
	int childrenIndices[8];
	int objectIndices[5];
	float3 origin;
	int numberOfObjects;
}Octree;




TriangleInfo triangleCollision(Ray ray, Triangle triangle)
{
	TriangleInfo tri;
	tri.v0 = triangle.v0;
	tri.v1 = triangle.v1;
	tri.v2 = triangle.v2;

	tri.hasIntersection = false;
	float3 V3 = tri.v2;
	float3 V1 = tri.v0;
	float3 V2 = tri.v1;



	float3 e1, e2;  //Edge1, Edge2
	float3 P, Q, T;
	float det, inv_det, u, v;
	float t;

	//Find vectors for two edges sharing V1
	e1=  V2 - V1;
	e2=  V3 - V1;
	//Begin calculating determinant - also used to calculate u parameter
	P = cross( ray.direction, e2);
	//if determinant is near zero, ray lies in plane of triangle
	det = dot(e1, P);
	//NOT CULLING
	if (det > -epsilion && det < epsilion) return tri;
	inv_det = 1.f / det;

	//calculate distance from V1 to ray origin
	T =  ray.origin - V1;

	//Calculate u parameter and test bound
	u = dot(T, P) * inv_det;
	//The intersection lies outside of the triangle
	if (u < 0.f || u > 1.f) return tri;

	//Prepare to test v parameter
	Q = cross( T, e1);

	//Calculate V parameter and test bound
	v = dot(ray.direction, Q) * inv_det;
	//The intersection lies outside of the triangle
	if (v < 0.f || u + v  > 1.f) return tri;

	t = dot(e2, Q) * inv_det;

	if (t > epsilion) { //ray intersection

		tri.hasIntersection = true;
		tri.distanceFromIntersection = t;

		return tri;
	}
	return tri;
}

uint4 getColor()
{
	return (uint4)(102,32,34,255);
}

bool insideBBox(float3 origin,float3 minPoint, float3 maxPoint)
{
	return (origin.x >= minPoint.x && origin.x <= maxPoint.x) &&
	(origin.y >= minPoint.y && origin.y <= maxPoint.y) &&
	(origin.z >= minPoint.z && origin.z <= maxPoint.z);
}



HitReturn hitBBox(Ray ray,float3 minPoint, float3 maxPoint)
{
	float a = 1.0 / ray.direction.x;
	float b = 1.0 / ray.direction.y;
	float c = 1.0 / ray.direction.z;

	float3 hitMin;
	float3 hitMax;



	if(a >= 0)
	{
		hitMin.x = (minPoint.x - ray.origin.x) * a; 
		hitMax.x = (maxPoint.x - ray.origin.x) * a; 
	}
	else
	{
		hitMin.x = (maxPoint.x - ray.origin.x) * a; 
		hitMax.x = (minPoint.x - ray.origin.x) * a; 
	}

	if(b >= 0)
	{
		hitMin.y = (minPoint.y - ray.origin.y) * b; 
		hitMax.y = (maxPoint.y - ray.origin.y) * b; 
	}
	else
	{
		hitMin.y = (maxPoint.y - ray.origin.y) * b; 
		hitMax.y = (minPoint.y - ray.origin.y) * b; 
	}

	if(c >= 0)
	{
		hitMin.z = (minPoint.z - ray.origin.z) * c; 
		hitMax.z = (maxPoint.z - ray.origin.z) * c; 
	}
	else
	{
		hitMin.z = (maxPoint.z - ray.origin.z) * c; 
		hitMax.z= (minPoint.z - ray.origin.z) * c; 
	}


	float minValue, maxValue;

	//largest entering value
	minValue = max(hitMin.x,hitMin.y);
	minValue = max(hitMin.z, minValue );

	// smallest exiting value
	maxValue = min(hitMax.x,hitMax.y);
	maxValue = min(hitMax.z, maxValue );

	HitReturn returnValue;
	returnValue.hit  =(minValue < maxValue && maxValue > epsilion);
	returnValue.minValue = minValue;
	returnValue.maxValue = maxValue;
	return returnValue;
}


HitReturn hitBox(Ray ray, BBox box)
{
	return hitBBox(ray,box.min, box.max);
}

Ray generateRay(int2 pixelLocation, int width, int height, Camera camera, int2 dim, int sampleNumber)
{
	/*
	Ray ray;
	float2 pixelToRay = (float2)(pixelLocation.x - (0.5f * width -1.0f), pixelLocation.y - (0.5 * (height - 1.0f)));
	ray.origin = camera.position;
	ray.direction = normalize(pixelToRay.x * camera.u + pixelToRay.y * camera.v - camera.distance * camera.w);

	return ray;
	*/
	Ray ray;
	float2 pixelToRay = (float2)((pixelLocation.x - (0.5f * width)) + (dim.x + 0.5)/sampleNumber, (pixelLocation.y - (0.5 * height)) + (dim.y + 0.5)/sampleNumber);

	ray.origin = camera.position;
	ray.direction = normalize((pixelToRay.x * -camera.u) + (pixelToRay.y * camera.v) - (camera.distance * camera.w));


	return ray;
}

SphereInfo sphereIntersection(Ray ray, float3 position, float radius)
{
	float3 originToShape = ray.origin - position;
	float a = dot(ray.direction, ray.direction);
	float b = 2.0f * dot(originToShape, ray.direction);
	float c = dot(originToShape, originToShape) - radius * radius;
	float disc = b*b - 4.0f * a * c;
	float t ;
	SphereInfo s;
	s.hasIntersection = false;
	if(disc > 0.0f)
	{
		float e = sqrt(disc);
		float denom = 2.0f * a;
		t = (-b-e)/denom;
		if(t > epsilion)	
		{
			s.normal = (float3) (((ray.origin - position) + t * ray.direction)/ radius);
			s.intersectionPoint = (ray.origin - position) + t * ray.direction;
			s.hasIntersection = true;
			s.distanceToIntersection = t;
			return s;
		}

		t = (-b + e) / denom;
		if(t > epsilion)
		{
			s.normal = (float3)(((ray.origin - position) + t * ray.direction)/radius);
			s.hasIntersection = true;
			s.intersectionPoint = (ray.origin - position) + t * ray.direction;
			s.distanceToIntersection = t;
			return s;
		}
	}

	return s;
}

uint4 adsLightS(Object object,Light light,SphereInfo sphereInfo)
{
	float3 ambient = object.material.ambient.xyz * light.material.ambient.xyz;
	float3 lightVector = normalize( light.position.xyz - object.position.xyz);
	float lightDotNormal = max(dot(lightVector.xyz,sphereInfo.normal.xyz), 0.0);
	float3 diffuse = object.material.diffuse.xyz * light.material.diffuse.xyz * lightDotNormal;
	float3 finalColor =   ambient + diffuse;// + specular;
	return (uint4)((convert_uint3(finalColor * 255.0f)),255);
}


uint4 adsLightT(Object object,Light light,TriangleInfo triangleInfo)
{
	float3 ambient = object.material.ambient.xyz * light.material.ambient.xyz;
	float3 lightVector = normalize( light.position.xyz - object.position.xyz);
	float lightDotNormal = max(dot(lightVector.xyz,triangleInfo.normal.xyz), 0.0);
	float3 diffuse = object.material.diffuse.xyz * light.material.diffuse.xyz * lightDotNormal;
	float3 finalColor =   ambient + diffuse;// + specular;
	return (uint4)((convert_uint3(finalColor * 255.0f)),255);
}


bool bboxCollided(BBox b1 , BBox b2)
{
	return (b1.min.x <= b2.min.x + fabs(b2.min.x - b2.max.x) &&
		b2.min.x <= b1.min.x + fabs(b1.min.x - b1.max.x) &&

		b1.min.y <= b2.min.y + fabs(b2.min.y - b2.max.y) &&
		b2.min.y <= b1.min.y + fabs(b1.min.y - b1.max.y) &&

		b1.min.z <= b2.min.z + fabs(b2.min.z - b2.max.z) &&
		b2.min.z <= b1.min.z + fabs(b1.min.z - b1.max.z)) 
	;
}

bool bboxObjectCollided(BBox b1 , Object o1)
{

	BBox box;
	box.min = o1.box.min + o1.position;
	box.max = o1.box.max + o1.position;

	return bboxCollided(b1,box);

}