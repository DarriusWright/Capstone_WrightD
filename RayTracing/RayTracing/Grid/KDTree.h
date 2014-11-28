#include <vector>
#include <Objects\Object.h>
#include <Objects\Mesh.h>
#include <Helper.h>

static const int NUMBER_OF_OBJECTS = 5;
static const int DIMENSIONS = 3;
static const float COST_OF_TRAVERSAL = 1.0;
static const float COST_OF_INTERSECTION = 1.5;


struct KDTreeInfo
{
	std::vector<int> objects;
};

struct KDTreeNode
{
	BBox boundingBox;
	//int objects[NUMBER_OF_OBJECTS] ;
	int startingIndex; // if internal node then starting index == -1
	int numberOfObjects;
	int nodes[2];
	int right;
	int index;
	int depth;

	KDTreeNode(BBox boundingBox, int depth, int index) : numberOfObjects(0) , depth(depth) , boundingBox(boundingBox)
	{
		startingIndex = -1;
		nodes[0] = nodes[1] = -1;
	}

	void build(std::vector<KDTreeNode> & nodes,std::vector<KDTreeInfo> & nodeInfo, 
		const std::vector<Mesh> meshes,const std::vector<Object> & objects , std::vector<int> & leafIndices,
		int nodeIndex)
	{
		//if stop met done

		if(stopCreation())
		{
			startingIndex = leafIndices.size();
			for (int i = 0; i < nodeInfo[nodeIndex].objects.size(); i++)
			{
				leafIndices.push_back(nodeInfo[nodeIndex].objects[i]);
			}

			return;
		}
		//else

		float bestPosition = -1;
		float bestCost = 10000000.0f;
		for (int i = 0; i < nodeInfo[nodeIndex].objects.size(); i++)
		{
			float min = objects[nodeInfo[nodeIndex].objects[i]].box.min[depth % DIMENSIONS] - FLT_EPSILON;
			float max = objects[nodeInfo[nodeIndex].objects[i]].box.max[depth % DIMENSIONS] + FLT_EPSILON;

			float cost = findCost(min,objects, nodeInfo[nodeIndex].objects,meshes);
			if(cost < bestCost){bestCost = cost; bestPosition = min;}
			cost = findCost(max,objects, nodeInfo[nodeIndex].objects, meshes);
			if(cost < bestCost){bestCost = cost; bestPosition = max;}
		}

		glm::vec3 halfDimension;
		int axis = depth%DIMENSIONS;

		for (int i = 0; i < DIMENSIONS; i++)
		{
			if(axis != i)
				halfDimension[i] = boundingBox.max[i];
		}

		halfDimension[axis] = bestPosition;

		BBox left = {boundingBox.min , 0.0f , halfDimension};
		BBox right = {halfDimension , 0.0f , boundingBox.max};
		KDTreeNode leftNode(left,depth + 1, nodes.size() + 1); 
		KDTreeNode rightNode(right,depth + 1, nodes.size() + 2);

		KDTreeInfo leftInfo;
		KDTreeInfo rightInfo;

		leftInfo.objects.reserve(nodeInfo[nodeIndex].objects.size());
		rightInfo.objects.reserve(nodeInfo[nodeIndex].objects.size());

		for (int i = 0; i < nodeInfo[nodeIndex].objects.size(); i++)
		{
			if(objectBoxCollided(left,objects[nodeInfo[nodeIndex].objects[i]],meshes[objects[nodeInfo[nodeIndex].objects[i]].meshIndex]))
			{
				leftInfo.objects.push_back(i);
			}

			if(objectBoxCollided(right,objects[nodeInfo[nodeIndex].objects[i]],meshes[objects[nodeInfo[nodeIndex].objects[i]].meshIndex]))
			{
				rightInfo.objects.push_back(i);
			}
		}

		leftNode.numberOfObjects = leftInfo.objects.size();
		rightNode.numberOfObjects = rightInfo.objects.size();

		nodeInfo.push_back(leftInfo);
		nodeInfo.push_back(rightInfo);
		
		nodes.push_back(leftNode);
		nodes.push_back(rightNode);


		
		nodes[nodeIndex].nodes[0] =  nodes.size() - 2;
		nodes[nodeIndex].nodes[1] =  nodes.size() - 1;

		nodes[nodes.size() - 1].build(nodes,nodeInfo,meshes,objects, leafIndices,nodes.size() -1);///(objects,nodes,nodeInfo);
		nodes[nodes.size() - 2].build(nodes,nodeInfo,meshes,objects, leafIndices,nodes.size() -2);//.build(objects,nodes,nodeInfo);
	}

	bool stopCreation()
	{
		return numberOfObjects <= NUMBER_OF_OBJECTS;
	}

	float leftCost(float splitPosition, const std::vector<Object> & objects , 
		const std::vector<int> & objectIndices, const std::vector<Mesh> meshes)
	{
		glm::vec3 dimensions;
		int axis = depth%DIMENSIONS;

		for (int i = 0; i < DIMENSIONS; i++)
		{
			if(axis != i)
				dimensions[i] = boundingBox.max[i];
		}

		dimensions[axis] = splitPosition;

		glm::vec3 absDimensions = dimensions - boundingBox.min;

		float area = 2.0f * absDimensions.x * absDimensions.y * absDimensions.z;


		BBox box = {glm::min(dimensions, boundingBox.min), 0.0, glm::max(dimensions,boundingBox.min )};

		int nObjects = 0;
		for (int i = 0; i < objectIndices.size(); i++)
		{
			

			if(objectBoxCollided(box,objects[objectIndices[i]],meshes[objects[objectIndices[i]].meshIndex]))
			{
				nObjects++;
			}
		}


		return COST_OF_TRAVERSAL + area * nObjects * COST_OF_INTERSECTION;
	}

	float rightCost(float splitPosition, const std::vector<Object> & objects
		, const std::vector<int> & objectIndices, const std::vector<Mesh> meshes)
	{
		glm::vec3 dimensions;
		int axis = depth%DIMENSIONS;

		for (int i = 0; i < DIMENSIONS; i++)
		{
			if(axis != i)
				dimensions[i] = boundingBox.max[i];
		}

		dimensions[axis] = splitPosition;

		glm::vec3 absDimensions = glm::abs( boundingBox.max - dimensions ) ;

		float area = 2.0f * absDimensions.x * absDimensions.y * absDimensions.z;


		BBox box = {glm::min(dimensions, boundingBox.max), 0.0, glm::max(dimensions,boundingBox.max )};

		int nObjects = 0;
		for (int i = 0; i < objectIndices.size(); i++)
		{
			if(objectBoxCollided(box,objects[objectIndices[i]],meshes[objects[objectIndices[i]].meshIndex] ))
			{
				nObjects++;
			}
		}


		return COST_OF_TRAVERSAL + area * nObjects * COST_OF_INTERSECTION;
	}

	float findCost(float splitPosition , const std::vector<Object> & objects,
		const std::vector<int> & objectIndices, const std::vector<Mesh> meshes)
	{
		return leftCost(splitPosition, objects, objectIndices,meshes) + rightCost(splitPosition,objects, objectIndices, meshes);
	}

};



struct KDTreeManager
{
	KDTreeManager(KDTreeNode root)
	{
		root.index = 0;
		root.depth = 0;
		nodes.push_back(root);
	}

	KDTreeManager()
	{

	}


	std::vector<KDTreeNode> nodes;
	std::vector<KDTreeInfo> nodesInfo;
	std::vector<int> leafObjectIndices;

	void insert(const  std::vector<Object> & objects , const std::vector<Mesh> & mesh)
	{
		KDTreeInfo node;
		node.objects.reserve(objects.size());
		for (int i = 0; i < objects.size(); i++)
		{
			node.objects.push_back(i);
		}

		nodesInfo.push_back(node);

		nodes[0].numberOfObjects = objects.size();
		nodes[0].build(nodes,nodesInfo,mesh,objects,leafObjectIndices,0);
	}
};