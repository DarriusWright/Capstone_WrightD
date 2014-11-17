#pragma OPENCL EXTENSION cl_khr_fp64: enable

__constant float epsilion = 0.00001f;
__constant float airIndex = 1.000293f;
__constant uint DIRECTIONAL_LIGHT = 0;
__constant uint POINT_LIGHT = 1;
__constant int TRIANGLE = 0;
__constant int SPHERE = 0;
__constant uint PINHOLE_CAMERA = 0;
__constant uint THINLENS_CAMERA = 1;
__constant uint FISHEYE_CAMERA = 2;
__constant uint SPHERICAL_CAMERA = 3;
__constant uint STEREO_CAMERA = 4;
__constant uint DIFFUSE = 0;
__constant uint SPECULAR = 1;
__constant uint TRANS = 2;

__constant float PI = 3.1415926535f;//...
__constant float PI_1 = 1.0f/3.1415926535f;

typedef struct
{
	float3 color;
	float3 reflection;
}BRDFRet;

typedef struct BRDF
{
	float kd;
	float3 cd;
	uint type;
}BRDF;

typedef struct
{
	float3 min;
	float3 max;
}BBox;

typedef struct
{
	float3 nextCrossing;
	float3 deltaT;
	int3 nextStep;
	int3 out;
}StepInfo;

typedef struct
{
	StepInfo stepInfo;
	int3 currentCell;
	bool continueStep;

}StepReturn;

typedef struct 
{
	float4 color;
	float reflection;
	float refraction;
	uint type;
	float d[1];
}Material;

typedef struct
{
	float3 v0;
	float3 v1;
	float3 v2;
	float3 normal;
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
    BBox box;
    int triangleIndex;
    uint meshIndex;
}Object;

typedef struct
{
	Material material;
	float4 position;
	uint2 indices;
	float d[2];
}Mesh;

typedef struct
{
	float3 u;
	float3 v;
	float3 w;
	float3 up;
	float3 lookAt;
	float4 position;
	float distance;
	float zoom;
	uint type;
	float focalDistance;
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
	float4 direction;
	int type;
	float d[3]
}Light;

typedef struct
{
	Material material;
	float4 position;
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



float clampFloat(float x)
{
	return clamp(x,0.0f,1.0f);
}

uint toUInt(float x)
{
	return clampFloat(x) * 255;//(uint)(pow(clampFloat(x),1.0f/2.2f) * 255 + .5);
}


uint4 toRGBA(float4 color)
{
	return (uint4)(toUInt(color.x),toUInt(color.y),toUInt(color.z),toUInt(color.w));
}

float3 reflect(float3 direction, float3 normal)
{
	return direction - normal * 2.0f * dot(normal, direction);
}


uint3 mixUIntColor(uint3 leftColor, uint3 rightColor)
{
	float3 colorf = convert_float3(leftColor.xyz) / 255.0f;
	float3 colorf2 = convert_float3(rightColor.xyz) / 255.0f;
	float3 finalColor = (colorf * colorf2) * 255.0f;

	return convert_uint3(finalColor);
}

int3 positionToVoxel(float3 position,  float3 invWidth, float3 numberOfVoxels , BBox sceneBox)
{
	float3 dif = (position - sceneBox.min) + 0.0001f;
	int3 voxelPosition = convert_int3(dif * invWidth);
	int3 nVoxels = convert_int3(numberOfVoxels);
	voxelPosition = clamp(voxelPosition,(int3)(0,0,0) , nVoxels);
	return voxelPosition;
}

float3 voxelToPosition(BBox sceneBox, int3 position, float3 width)
{
	return sceneBox.min + (convert_float3(position) * width);
}

int findVoxelIndex(int3 position , float3 cellDimensions)
{
	return position.x + position.y * (int)cellDimensions.x + position.z * (int)cellDimensions.x * (int)cellDimensions.y;
}

int3 findVoxelPosition(int index , float3 cellDimensions)
{
	int3 position;
	position.z = index / ((int)(cellDimensions.x* cellDimensions.y));
	position.y = (index % ((int)(cellDimensions.x* cellDimensions.y))) / ((int)cellDimensions.x);
	position.x = index - position.y * cellDimensions.x - position.z *cellDimensions.x *cellDimensions.y;
	return position;
}


StepInfo findStepInfo(int3 currentCell,Ray ray, float minVal, 
	float maxVal,float3 voxelWidth,BBox box, float3 numberOfVoxels)
{
	
	float3 nextCrossing;
	float3 deltaT;
	int3 nextStep;
	int3 out;

	float3 voxelPositionPos =  voxelToPosition(box, currentCell + 1,voxelWidth.xyz);
	float3 voxelPositionNeg =  voxelToPosition(box, currentCell,voxelWidth.xyz);
	float3 intersectionPoint = ray.origin + ray.direction * minVal;

	if(ray.direction.x >= 0)
	{
		nextCrossing.x = minVal + (voxelPositionPos.x - intersectionPoint.x) / ray.direction.x;
		deltaT.x = voxelWidth.x/ ray.direction.x;
		nextStep.x = 1;
		out.x = numberOfVoxels.x;

	}
	else
	{
		nextCrossing.x = minVal + (voxelPositionNeg.x - intersectionPoint.x) / ray.direction.x;
		deltaT.x= -voxelWidth.x/ ray.direction.x;
		nextStep.x = -1;
		out.x = -1;
	}


	if(ray.direction.y >= 0)
	{
		nextCrossing.y = minVal + (voxelPositionPos.y - intersectionPoint.y) / ray.direction.y;
		deltaT.y = voxelWidth.y/ ray.direction.y;
		nextStep.y = 1;
		out.y = numberOfVoxels.y;

	}
	else
	{
		nextCrossing.y = minVal + (voxelPositionNeg.y- intersectionPoint.y) / ray.direction.y;
		deltaT.y= -voxelWidth.y/ ray.direction.y;
		nextStep.y = -1;
		out.y= -1;
	}

	if(ray.direction.z >= 0)
	{
		nextCrossing.z = minVal + (voxelPositionPos.z- intersectionPoint.z) / ray.direction.z;
		deltaT.z = voxelWidth.z/ ray.direction.z;
		nextStep.z = 1;
		out.z = numberOfVoxels.z;

	}
	else
	{
		nextCrossing.z = minVal + (voxelPositionNeg.z - intersectionPoint.z) / ray.direction.z;
		deltaT.z= -voxelWidth.z/ ray.direction.z;
		nextStep.z = -1;
		out.z = -1;
	}

	StepInfo s;
	s.nextCrossing= nextCrossing;
	s.deltaT = deltaT;
	s.nextStep = nextStep;
	s.out = out;

	return s;	
}



StepReturn stepThroughGrid(StepInfo s, int3 currentCell, float maxVal)
{

	bool run = true;

	int stepAxis =  (s.nextCrossing.x  < s.nextCrossing.y && s.nextCrossing.x < s.nextCrossing.z) ? 0 :(s.nextCrossing.y < s.nextCrossing.z) ? 1 : 2;



	if(stepAxis == 0)
	{


			if(maxVal < s.nextCrossing.x) 
			{
				run = false;
			}
			currentCell.x += s.nextStep.x;

	

			//if((ray.direction.x >= 0) ?  out.x <= currentCell.x : out.x >= currentCell.x)
			if( s.out.x == currentCell.x)
			{
				run = false;

			}
		
			s.nextCrossing.x += s.deltaT.x;

		
	
	}


	if(stepAxis == 1)
	{

			if(maxVal < s.nextCrossing.y)
			{
				run = false;
			}

			currentCell.y += s.nextStep.y;
			//if((ray.direction.y >= 0) ?  out.y <= currentCell.y : out.y >= currentCell.y)
			if( s.out.y == currentCell.y)
			{
				run = false;
			}
			s.nextCrossing.y += s.deltaT.y;

	}


	if(stepAxis == 2)
	{

			if(maxVal < s.nextCrossing.z)
			{
				run = false;
			}

			currentCell.z += s.nextStep.z;
			
			//if(out.z >= currentCell.z)
			if(s.out.z == currentCell.z)
			{
				run = false;
			}
			s.nextCrossing.z += s.deltaT.z;
		
	}


	StepReturn retS;
	retS.stepInfo = s;
	retS.continueStep = run;
	retS.currentCell = currentCell;

	return retS;
}

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
	P = cross( ray.direction.xyz, e2);
	//if determinant is near zero, ray lies in plane of triangle
	det = dot(e1, P);
	//NOT CULLING
	if (det > -epsilion && det < epsilion) return tri;
	inv_det = 1.f / det;

	//calculate distance from V1 to ray origin
	T =  ray.origin.xyz - V1;

	//Calculate u parameter and test bound
	u = dot(T, P) * inv_det;
	//The intersection lies outside of the triangle
	if (u < 0.f || u > 1.f) return tri;

	//Prepare to test v parameter
	Q = cross( T, e1);

	//Calculate V parameter and test bound
	v = dot(ray.direction.xyz, Q) * inv_det;
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

float surfaceArea(BBox box)
{
	float3 d = box.max - box.min;
	return 2.0f * (d.x * d.y + d.x * d.z + d.y * d.z);
}

float volumne(BBox box)
{
	float3 d = box.max - box.min;
	return d.x * d.y * d.z;
}

int maxExtent(BBox box)
{
	float3 d = box.max - box.min;
	return (d.x > d.y && d.x > d.z) ? 0 : (d.y > d.z) ? 1 : 2;
}


BBox createCellBox(int index, float3 numberOfVoxels, float3 delta, BBox box)
{
	BBox retBox;
	retBox.min = voxelToPosition(box,findVoxelPosition(index, numberOfVoxels),delta / numberOfVoxels );
	retBox.max = retBox.min  + (delta / numberOfVoxels);
	return retBox;
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

	if(returnValue.hit)
	{
		returnValue.minValue = minValue;
		returnValue.maxValue = maxValue;
	}
	else
	{
		returnValue.minValue = 0.0f;
		returnValue.maxValue = 0.0f;	
	}


	return returnValue;
}
HitReturn hitBox(Ray ray, BBox box)
{
	return hitBBox(ray,box.min, box.max);
}
long inline nextPowerOfTwo(long number)
{
	return pow(2.0, ceil(log((double)number)/log(2.0)));
}
float absF(float x)
{
	return x < 0 ? x * -1.0f : x;
}
float randFloat(long seed)
{
	//Java's implementation
	long randomNumber = ((seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1)) ;
	long randomNumber2 = (((seed + seed) * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1)) ;
	
	long minNum = min(randomNumber, randomNumber2);
	long maxNum = max(randomNumber, randomNumber2);


	float result =  (float)minNum/ (float)maxNum;//nextPowerOfTwo(randomNumber) ;
	return result;
}
float3 mapToHemiSphere(float2 samplePoint, float e)
{
	float cosPhi = cos(2.0f * PI * samplePoint.x);
	float sinPhi = sin(2.0f * PI * samplePoint.x);
	float cosTheata = pow((1.0f - samplePoint.y), 1.0f / (e + 1.0f)) ;
	float sinTheata = sqrt(1.0f - cosTheata * cosTheata) ;

	return (float3)(sinTheata * cosPhi, sinTheata * sinPhi, cosTheata);

}

float2 mapToDisk(float2 samplePoint)
{
	float r,phi;

	if(samplePoint.x > -samplePoint.y)
	{
		if(samplePoint.x > samplePoint.y)
		{
			r = samplePoint.x;
			phi = samplePoint.y/ samplePoint.x;
		}
		else
		{
			r = samplePoint.y;
			phi = 2 - samplePoint.x/ samplePoint.y;
		}
	}
	else
	{
		if(samplePoint.x < samplePoint.y)
		{
			r = -samplePoint.x;
			phi = 4 + samplePoint.y/ samplePoint.x;
		}
		else
		{
			r = -samplePoint.y;
			if(samplePoint.y != 0.0f)
				phi = 6 - samplePoint.x/samplePoint.y;
			else
				phi = 0.0f;
		}
	}
	phi *= PI/4.0f;

	return (float2)(r* cos(phi),r * sin(phi));
}
Ray generateRay(int2 pixelLocation, int width, int height, Camera camera, int2 dim, int sampleNumber , long seed)
{
	Ray ray;

	long xSeed = pixelLocation.x + dim.x + seed;
	long ySeed = pixelLocation.y + dim.y + seed * xSeed;
	float r1  = 2 * randFloat(xSeed) , dx = r1 < 1 ? sqrt(r1)-1 : 1 - sqrt(2 -r1);
	float r2  = 2 * randFloat(ySeed) , dy = r2 < 1 ? sqrt(r2)-1 : 1 - sqrt(2 -r2);


	//ray.direction = camera.u * (((dim.x + 0.5f +dx )/2.0f + pixelLocation.x)/width - 0.5f) + 
	//camera.v * (((dim.y + 0.5f +dy )/2.0f + pixelLocation.y)/height - 0.5f) + 
	//(camera.distance * camera.w);
	ray.origin.xyz =camera.position.xyz;
	float2 samplePoint = (float2)( (dim.x + r1) , (dim.y + r2));
	
	float3 direction;
	float3 origin = camera.position.xyz;
	float2 pp =  (float2)((pixelLocation.x - (0.5f * width)) + samplePoint.x/sampleNumber, (pixelLocation.y - (0.5 * height)) + samplePoint.y/sampleNumber);
	float2 pixelToRay = pp;


	ray.direction = normalize((pixelToRay.x * -camera.u) + (pixelToRay.y * -camera.v) - (camera.distance * camera.w));


	return ray;
}
SphereInfo sphereIntersection(Ray ray, float3 position, float radius)
{
	float3 originToShape = ray.origin.xyz - position;
	float a = dot(ray.direction.xyz, ray.direction.xyz);
	float b = 2.0f * dot(originToShape, ray.direction.xyz);
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
			s.normal = (float3) (((ray.origin.xyz - position) + t * ray.direction.xyz)/ radius);
			s.intersectionPoint = (ray.origin.xyz - position) + t * ray.direction.xyz;
			s.hasIntersection = true;
			s.distanceToIntersection = t;
			return s;
		}

		t = (-b + e) / denom;
		if(t > epsilion)
		{
			s.normal = (float3)(((ray.origin.xyz - position) + t * ray.direction.xyz)/radius);
			s.hasIntersection = true;
			s.intersectionPoint = (ray.origin.xyz - position) + t * ray.direction.xyz;
			s.distanceToIntersection = t;
			return s;
		}
	}

	return s;
}
uint4 adsLight(Light light, Material material, Camera camera, float3 intersectionPoint,  float3 normal)
{
	float3 lightVector ;//=normalize(light.position - intersectionPoint);// normalize( light.position.xyz - object.position.xyz);
	lightVector = (light.type == DIRECTIONAL_LIGHT) ? normalize(light.direction.xyz) : normalize(light.position - intersectionPoint);
	float lightDotNormal = max(dot(lightVector.xyz,normal.xyz), 0.0f);
	float diffuseFactor = dot(normal,lightVector);
	float3 diffuse ;//= light.material.diffuse.xyz * max(diffuseFactor, 0.0f) * material.diffuse.xyz;// * (light.type == POINT_LIGHT) ? attenuation : 1.0f;
	float3 ambient ;//= material.ambient.xyz * light.material.ambient.xyz;

	//float3 s = dot(normal, lightVector) * normal - lightVector;
	//float3 r =  (2.0f*(dot(normal, lightVector) * normal)) - lightVector;
	//float3 v = intersectionPoint - camera.position;
	//r = normalize(r);
	//v = normalize(v);
	//s = normalize(s);

	//if(dot(r,v) < 0)
	//{
	//	r = (float3)(0.0f,0.0f,0.0f);
	//	v = (float3)(0.0f,0.0f,0.0f);
	//}
	//float specularPower = pow(dot(v,r), material.specular.w);

	//float3 specular = light.material.specular.xyz * specularPower * material.specular;// * attenuation;//* (light.type == POINT_LIGHT) ? attenuation : 1.0f;
	//if(light.type == POINT_LIGHT)
	//{
	//	float attenuation = clamp((1.0f - length(lightVector)/ light.direction.w) , 0.0f, 1.0f);
	//	diffuse *= attenuation;
	//	specular *= attenuation;
	//}
	
	float3 finalColor =   ambient + diffuse;// + specular;
	
	return (uint4)((convert_uint3(finalColor * 255.0f)),255);	
}
uint3 convertColor(float4 color)
{
	return convert_uint3(color.xyz * 255.0f);
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
bool bboxObjectCollided(BBox b1 , Mesh m, Object o1)
{

	BBox box;
	box.min = o1.box.min + m.position;
	box.max = o1.box.max + m.position;

	return bboxCollided(b1,box);

}
typedef struct
{
	float3 direction;
	float3 position;
	float3 color;
	float distance;
	int objectIndex;
	int type;
}IntersectionInfo;

IntersectionInfo rayTrace(Ray ray,Camera camera, 
	__global int * cellIndices, __global int * objectIndices,
	__global Light * light, __global Object * objects,
	__global Triangle * triangles, int cellIndex, int3 currentCell,
	 BBox box,float3 voxelWidth,  float3 cellDimensions, __global Mesh * meshes)
{
	bool run = true;
	IntersectionInfo intersect;
	intersect.distance = -1;
	float lastDistance = 100000.0f;
	HitReturn hitCheck = hitBBox(ray,box.min,box.max);
	float maxVal=hitCheck.maxValue;
	StepInfo s=  findStepInfo(currentCell, ray,hitCheck.minValue, hitCheck.maxValue, voxelWidth, box, cellDimensions );

	while(run)
	{
		int cellObjectNumber = (cellIndex > 0) ? cellIndices[cellIndex]- cellIndices[cellIndex-1]  : cellIndices[0];
		int objectColorIndex = 0;
		float3 normal;
		for(int i = 0; i <  cellObjectNumber; i++ )
		{
			int objectIndex = objectIndices[cellIndices[ (cellIndex > 0) ? cellIndex-1 : 0] + i]-1;
			if(meshes[objects[objectIndex].meshIndex].position.w == 0.0)
			{
				TriangleInfo triInfo = triangleCollision(ray,triangles[objects[objectIndex].triangleIndex]);
				triInfo.normal = triangles[objects[objectIndex].triangleIndex].normal;
				if(triInfo.hasIntersection && lastDistance  > triInfo.distanceFromIntersection)
				{
					//outColor = (uint4)adsLight(objects[objectIndex], light[0], triInfo);
					lastDistance = triInfo.distanceFromIntersection;
					normal = triInfo.normal;
					objectColorIndex = objectIndex;
					run = false;
				}
			}
		}
		if(!run)
		{
			intersect.direction = ray.direction;
			intersect.position = ray.direction * lastDistance + ray.origin;
			float3 intersectionPoint = ray.direction.xyz * lastDistance;
			//intersect.color.xyz = convert_float3(adsLight(light[0], objects[objectColorIndex].material, camera, intersectionPoint, normal).xyz);
			//intersect.color.xyz /= 255.0f;
			intersect.color.xyz = (float3)(0,0,0);
			intersect.distance = lastDistance;
			intersect.objectIndex = objectColorIndex;
			intersect.type = TRIANGLE;
		}
		if(run)
		{
			StepReturn stepReturned =  stepThroughGrid(s,currentCell,maxVal);
			currentCell = stepReturned.currentCell;
			run = stepReturned.continueStep;
			s = stepReturned.stepInfo;
		}
		cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
	}
	return intersect;
}
